#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

#define MINIAUDIO_IMPLEMENTATION
#define CPPHTTPLIB_OPENSSL_SUPPORT
#define _WIN32_WINNT 0x0A00  // Target Windows 10/11

#include "miniaudio.h"
#include "json.hpp"
#include "httplib.h"

using json = nlohmann::json;

// - Lowercase a string
std::string toLower(const std::string& s) {
    std::string out = s;
    std::transform(out.begin(), out.end(), out.begin(), ::tolower);
    return out;
}

int main() {
    std::string songName, artistName;
    std::cout << "Enter song name: ";
    std::getline(std::cin, songName);
    std::cout << "Enter artist name: ";
    std::getline(std::cin, artistName);

    httplib::Client cli("https://api.audius.co");

    std::string query = songName + " " + artistName;
    std::string path = "/v1/tracks/search?query=" + query + "&app_name=Phonon";

    auto res = cli.Get(path.c_str());
    if (!res || res->status != 200) {
        std::cerr << "Search request failed....\n";
        return 1;
    }

    auto searchJson = json::parse(res->body);
    if (!searchJson.contains("data") || searchJson["data"].empty()) {
        std::cout << "No Tracks Found\n";
        return 1;
    }

    // Top Matches
    std::vector<json> filteredTracks;
    std::string lowerSong = toLower(songName);
    std::string lowerArtist = toLower(artistName);

    for (auto &track : searchJson["data"]) {
        std::string tName = toLower(track["title"]);
        std::string tArtist = toLower(track["user"]["name"]);
        if (tName.find(lowerSong) != std::string::npos && tArtist.find(lowerArtist) != std::string::npos) {
            filteredTracks.push_back(track);
        }
    }

    // show top 5
    if (filteredTracks.empty()) {
        int count = std::min(5, (int)searchJson["data"].size());
        std::cout << "No exact matches. Top " << count << " results:\n";
        for (int i = 0; i < count; ++i) {
            auto t = searchJson["data"][i];
            std::cout << i << ": " << t["title"] << " by " << t["user"]["name"] << "\n";
        }
        std::cout << "Pick a track number: ";
        int choice; 
        std::cin >> choice;
        if (choice < 0 || choice >= count) {
            std::cerr << "Invalid choice.\n"; 
            return 1;
        }
        filteredTracks.push_back(searchJson["data"][choice]);
    }

    // pick the first track boi
    auto track = filteredTracks[0];
    std::string trackId = track["id"];
    std::cout << "Now Playing: " << track["title"] << " by " << track["user"]["name"] << "\n";

    // get stream url
    path = "/v1/tracks/" + trackId + "/stream?app_name=Phonon&no_redirect=true";
    res = cli.Get(path.c_str());
    if (!res || res->status != 200) {
        std::cerr << "Failed to get stream metadata...\n";
        return 1;
    }

    auto streamJson = json::parse(res->body);
    if (!streamJson.contains("data")) {
        std::cerr << "No stream URL found in response...\n";
        return 1;
    }

    std::string streamUrl = streamJson["data"];

    std::string host, path_to_file;
    size_t protocol_end = streamUrl.find("://");
    size_t host_start = (protocol_end == std::string::npos) ? 0 : protocol_end + 3;
    size_t path_start = streamUrl.find("/", host_start);

    if (path_start != std::string::npos) {
        host = streamUrl.substr(0, path_start);
        path_to_file = streamUrl.substr(path_start);
    } else {
        host = streamUrl;
        path_to_file = "/";
    }

    httplib::Client audioCli(host); 
    audioCli.enable_server_certificate_verification(false);
    audioCli.set_follow_location(true); 
    audioCli.set_read_timeout(20); 

    auto audioRes = audioCli.Get(path_to_file.c_str()); 
    if (!audioRes || audioRes->status != 200) {
        std::cerr << "Download failed. Status: " << (audioRes ? audioRes->status : 0) << "\n";
        if (audioRes && audioRes->status == 302) std::cerr << "Stuck on redirect.\n";
        return 1;
    }
    
    std::vector<uint8_t> mp3Buffer(audioRes->body.begin(), audioRes->body.end());

    // init miniaudio
    ma_result result;
    ma_decoder decoder;
    result = ma_decoder_init_memory(mp3Buffer.data(), mp3Buffer.size(), NULL, &decoder);
    if (result != MA_SUCCESS) {
        std::cerr << "Decoder init failed...\n";
        return 1;
    }

    ma_device_config deviceConfig = ma_device_config_init(ma_device_type_playback);
    deviceConfig.playback.format = decoder.outputFormat;
    deviceConfig.playback.channels = decoder.outputChannels;
    deviceConfig.sampleRate = decoder.outputSampleRate;
    deviceConfig.dataCallback = [](ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount) {
        ma_decoder* pDecoder = (ma_decoder*)pDevice->pUserData;
        ma_decoder_read_pcm_frames(pDecoder, pOutput, frameCount, NULL); 
    };
    deviceConfig.pUserData = &decoder;

    ma_device device;
    if (ma_device_init(NULL, &deviceConfig, &device) != MA_SUCCESS) {
        std::cerr << "Device init failed...\n";
        ma_decoder_uninit(&decoder);
        return 1;
    }
    if (ma_device_start(&device) != MA_SUCCESS) {
        std::cerr << "Device start failed...\n";
        ma_device_uninit(&device);
        ma_decoder_uninit(&decoder);
        return 1;
    }

    std::cout << "Playing...Press Enter to Stop\n";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get();
    
    ma_device_uninit(&device);
    ma_decoder_uninit(&decoder);

    return 0;
}
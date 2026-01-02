# Phonon
Clean and Minimal CLI Music Player

## Table of Contents
- [Features](#features)
- [Usage](#usage)
- [Contributing](#contributing)
- [Licence](#licence)

## Features
- Search and play songs. 
- MP3 Decoding and Playback using Miniaudio. 
- Track selection from search results. 
- Handles Stream Redirects.
- Cross Platform CLI. 

## Installation
Clone the repository and run ./build.sh for Linux Users or ./build.bat for Windows Users.
**Ensure the Project structure looks like this:** 
- include
L httplib.h
L json_fwd.hpp
L json.hpp
L miniaudio.h
- src
L main.cpp

## Usage
After building Phonon, run the player from the terminal: 

```bash
./phonon
```
**Step by Step**
1 - Enter song name when prompted.

2 - Enter artist name when prompted.

3 - Select a track if multiple results appear. 

4 - Playback starts and you can now listen to the tunes.

**Notes**
- The player will fetch the track from Audius and play it. 
- Currently, playback stops when you hit Enter. 
- Future updates may include pause/resume/skip controls and more.

**Tips**: 
- Internet Connection is obviously required, I believe this is pretty obvious, but for those people who didn't know here you go.
- Playback is smooth, but it may take some time initially on longer tracks (until the streaming refactor is done).

## Contribution

Thanks for considering to contribute to the project! Here's how you can help: 

1. **Report bugs or suggest features**
    - Open an issue on Github describing the problem or feature clearly. 

2. **Fork the Repository**
    - Make your changes in a branch seperate from `main`.

3. **Submit a Pull Request**
    - PRs should include: 
        - Clear description of the changes
        - Any new dependancies or steps required
        - Screenshots and examples if applicable

4. **Code Style**
    - Follow similar code style. 
    - Keep the code organized. 
    - Comment your changes where needed.

5. **Testing**
    - Make sure your changes compile and run.
    - Don't break existing functionality.

---

> Any contribution is welcome, big or small! Improving the README counts 😎.

## Licence

Phonon is released under [GNU GENERAL PUBLIC LICENSE v3](LICENCE).
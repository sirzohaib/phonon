#!/bin/bash
# Build RockPlayer

clang++ src/main.cpp -std=c++17 -Isrc -lssl -lcrypto -lpthread -lws2_32 -lcrypt32 -o player

# Check if build succeeded
if [ $? -eq 0 ]; then
    echo "Build successful! Run ./player to begin."
else
    echo "Build failed."
fi

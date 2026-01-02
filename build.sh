#!/bin/bash
# Build Phonon

clang++ src/main.cpp -std=c++17 -Iinclude -lssl -lcrypto -lpthread -lws2_32 -lcrypt32 -o phonon

# Check if build succeeded
if [ $? -eq 0 ]; then
    echo "Build successful! Run ./phonon to begin."
else
    echo "Build failed."
fi

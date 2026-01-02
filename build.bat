@echo off
REM Build Phonon

clang++ src/main.cpp -std=c++17 -Iinclude -lssl -lcrypto -lpthread -lws2_32 -lcrypt32 -o phonon
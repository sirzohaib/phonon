@echo off
clang++ src/main.cpp -std=c++17 -Isrc -lssl -lcrypto -lpthread -lws2_32 -lcrypt32 -o player
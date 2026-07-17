# Sebox Locker

## About
- Sebox Locker is a desktop application that lets you securely store and encrypt files and folders with a password. Everything is stored locally on your device, with a data backup option.
- It functions as a local digital vault. Your data remains cryptographically shredded—intact, inaccessible, and locked away. All files are heavily protected using libsodium, utilizing advanced hashing for key derivation and authenticated encryption (AEAD) to ensure maximum security.

## Features
* File locker functionality
* File encryption
* Password-protected access
* File renaming
* File deletion
* Backup and export features
* Local-only storage; no cloud

## Tech Stack
* C++
* CMake - package tool
* Slint - gui
* NFD - File Dialogs
* libsodium - encryption

## Sebox Demo
<video src="./assets/sebox_demo_video.mp4" controls>
  Your browser does not support the video tag.
</video>
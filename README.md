# CIV-IT-Final-Project

## Inventory Management System (C CLI App)

This is a modular C language CLI application for inventory management. It uses:

* SQLite for inventory records (items, stock additions, issues)
* Binary file handling for login system (configs.dat)
* Role-based access control for secure menu access
* FIFO inventory logic for issuing stock

## Features

* SQLite-backed structured data (data.db)
* Binary-based login (configs.dat) with access levels
* Modular code (menu, item, and inventory management)
* FIFO logic for stock consumption and costing
* Works on Windows, Linux, and macOS
* Easy compilation using CMake

## Project Structure

```
project-root/
├── main.c                 # Entry point
├── CMakeLists.txt         # CMake build script
├── src/                   # Source files
├── include/               # Header files
├── external/              # SQLite amalgamation source
├── configs.dat            # Login file (generated at runtime)
├── data.db                # SQLite DB (generated at runtime)
└── open-Mysys2.bat        # Shortcut for MSYS2 shell
```

## Requirements

### On Windows (via MSYS2)

1. Download and install MSYS2 from: [https://www.msys2.org](https://www.msys2.org)

2. Launch MSYS2 MSYS shell (from Start menu).

3. Update base system:

   ```
   pacman -Syu
   ```

   If prompted, restart the shell and re-run the same command.

4. Switch to MinGW 64-bit shell:

   * Either launch "MSYS2 MinGW 64-bit" from Start Menu
   * Or run:

     ```
     C:\msys64\mingw64.exe
     ```

5. Install build tools:

   ```
   pacman -S --needed mingw-w64-x86_64-gcc mingw-w64-x86_64-cmake
   ```

   This installs:

   * GCC (C compiler)
   * CMake (build system)

6. To open the correct shell in project folder, run in PowerShell:

   ```
   .\open-Mysys2.bat
   ```

   This opens MSYS2 MinGW 64-bit terminal directly in current folder.

7. Verify setup:

   ```
   which gcc
   gcc --version
   which cmake
   cmake --version
   ```

### On Linux/macOS

Most required tools are pre-installed on modern systems.

To install missing build tools:

#### Debian/Ubuntu:

```
sudo apt update
sudo apt install build-essential cmake
```

#### Arch:

```
sudo pacman -S base-devel cmake
```

#### Fedora:

```
sudo dnf install gcc gcc-c++ make cmake
```

## Build Instructions

```
mkdir build
cd build
cmake ..
cmake --build .
```

The output will be located at: build/dist/program.exe (Windows) or build/dist/program (Linux/macOS)

## Running the Program

### On Windows (in MSYS2 MinGW64 shell):

```
./dist/program.exe
```

### On Linux/macOS:

```
./dist/program
```

## First Run

When you run the app:

* data.db is created and initialized
* configs.dat is generated with 3 sample users:

| Username | Password | Role   | Access       |
| -------- | -------- | ------ | ------------ |
| admin    | 123      | Admin  | Full         |
| user     | 123      | Staff  | Limited edit |
| read     | 123      | Viewer | Read-only    |

## Warnings

* Do not manually edit data.db or configs.dat
* Deleting stock additions that were already consumed breaks FIFO logic
* Deleting stock issues will reverse inventory changes and may break costing integrity

## Detailed Windows (MSYS2) Setup Guide

1. Download the latest MSYS2 installer from [https://www.msys2.org](https://www.msys2.org)

2. Install MSYS2 to a directory like C:\msys64 (avoid spaces or special characters)

3. Launch "MSYS2 MSYS" from Start Menu and run:

   ```
   pacman -Syu
   ```

   Restart and repeat until all packages are up-to-date.

4. Open the MSYS2 MinGW 64-bit shell (Start Menu > MSYS2 MinGW 64-bit), or run:

   ```
   C:\msys64\mingw64.exe
   ```

5. In this shell, install the required development tools:

   ```
   pacman -S --needed mingw-w64-x86_64-gcc mingw-w64-x86_64-cmake
   ```

6. To launch MSYS2 directly in the project directory from PowerShell, run:

   ```
   .\open-Mysys2.bat
   ```

   This batch file will start MSYS2 MinGW64 in the current working directory.
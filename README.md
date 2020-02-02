# GMod HTML
This is pretty much just an abstraction layer around The Chromium Embedded Framework (https://bitbucket.org/chromiumembedded/cef)
Note: None of this works with the current release of GMod. You'll have to wait for the full 64-bit release before using these with a live build of GMod.

## Chromium Embedded Framework Binary Distribution
To work with this project you will need a build of the Chromium Embedded Framework. You can download the builds used by Garry's Mod here if you don't want to compile your own:

| Platform | URL |
| -------- | --- |
| Windows x86 | https://files.facepunch.com/willox/46c89e07-a257-4988-96e8-81e945284469/cef_binary_79.1.31%2Bgfc9ef34%2Bchromium-79.0.3945.117_windows32.tar.bz2 |
| Windows x64 | https://files.facepunch.com/willox/8750c07c-4888-4a79-8449-a01bd6e851f7/cef_binary_79.1.31%2Bgfc9ef34%2Bchromium-79.0.3945.117_windows64.tar.bz2 |
| Linux x64 | https://files.facepunch.com/willox/3004a977-b227-4a20-bbfb-e698913d8a71/cef_binary_79.1.31%2Bgfc9ef34%2Bchromium-79.0.3945.117_linux64.tar.bz2 |
| macOS x64 | https://files.facepunch.com/willox/7fd05979-e9ad-4b94-8d85-0014bacb170c/cef_binary_79.1.35%2Bgfebbb4a%2Bchromium-79.0.3945.130_macosx64.tar.bz2 |

These belong in the `./thirdparty/cef3/` directory (after extraction.) The paths are currently hardcoded into the root `CMakelists.txt` file.

## Getting started
### Windows
#### Requirements
- Visual Studio 2019
- CMake 3.15 or newer
#### Generating Visual Studio Solution
##### x86
```bat
mkdir build_x86
cd build_x86
cmake -G "Visual Studio 16 2019" -A Win32 ..
```
#### x86-64
```bat
mkdir build_x64
cd build_x64
cmake -G "Visual Studio 16 2019" -A x64 ..
```

After running either of these sets of commands, you can enter your created directory and open the `gmod-html.sln` solution in Visual Studio. Compiling the `INSTALL` project will place a complete build into `<your_build_dir>/bin` by default.

### Linux
#### Requirements
- A version of GCC/G++ or Clang/Clang++ with C++11 support
- CMake 2.8.7 or newer

#### Compiling
```sh
mkdir build
cd build
cmake -G "Unix Makefiles" ..
make && make install
```

This will place a complete build into `build/bin` by default.

### macOS
Todo

## TODO
- Improve the CMake files. We don't use them for GMod builds so they're a bit wonky.
- Get the example_host into workable condition. It's disabled at the moment
- Cleanup. Everything is quite messy.
- macOS. These builds require quite a few unique things so they're not handled by the CMake scripts at all at the moment. It's still technically possible to get the builds working, though.
- Dynamic loading of the HTML implementation. Atm we just use dylib() or LoadLibrary() in each host which is kind of lame. It'd be nice to simplify it.

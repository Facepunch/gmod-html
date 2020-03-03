# GMod HTML
This is pretty much just an abstraction layer around The Chromium Embedded Framework (https://bitbucket.org/chromiumembedded/cef)
Note: None of this works with the current release of GMod. You'll have to wait for the full 64-bit release before using these with a live build of GMod.

## Chromium Embedded Framework Binary Distribution
To work with this project you will need a build of the Chromium Embedded Framework. You can download the builds used by Garry's Mod here if you don't want to compile your own:

| Platform | URL |
| -------- | --- |
| Windows x86 | https://files.facepunch.com/willox/7bb55f57-7c70-4140-83d7-2d61d8b57816/cef_binary_80.0.4%2Bg74f7b0c%2Bchromium-80.0.3987.122_windows32.tar.bz2 |
| Windows x64 | https://files.facepunch.com/willox/14ceed65-8809-4fba-97a7-2d524b6d45ec/cef_binary_80.0.4%2Bg74f7b0c%2Bchromium-80.0.3987.122_windows64.tar.bz2 |
| Linux x64 | https://files.facepunch.com/willox/dfd5d7fe-2184-40a9-90b0-49f9eef9a9b5/cef_binary_80.0.4%2Bg74f7b0c%2Bchromium-80.0.3987.122_linux64.tar.bz2 |
| macOS x64 | https://files.facepunch.com/willox/17e19274-4112-4734-ac20-22cd8644bcb4/cef_binary_80.0.4%2Bg74f7b0c%2Bchromium-80.0.3987.122_macosx64.tar.bz2 |

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

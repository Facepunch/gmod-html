# GMod HTML
This is pretty much just an abstraction layer around The Chromium Embedded Framework (https://bitbucket.org/chromiumembedded/cef)

## Chromium Embedded Framework Binary Distribution
To work with this project you will need a build of the Chromium Embedded Framework. You can download prebuilt versions of CEF from Spotify's Automated Builder if you don't want to compile your own:

https://cef-builds.spotifycdn.com/index.html

You'll probably want the **Minimal Distribution** unless you need to debug with symbols or something. These belong in the `./thirdparty/cef3/` directory (after extraction.) The paths are currently hardcoded into the root `CMakelists.txt` file.

## Currently supported CEF version
The current version of CEF that's supported by this library is:

- **95.7.18+g0d6005e+chromium-95.0.4638.69**

This is not the only version that could be supported, but it's the version that's currently configured and tested to work.

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

# GMod HTML
This is pretty much just an abstraction layer around The Chromium Embedded Framework (https://bitbucket.org/chromiumembedded/cef)

## Chromium Embedded Framework Binary Distribution
To work with this project you will need a build of the Chromium Embedded Framework. You can download prebuilt versions of CEF from Spotify's Automated Builder if you don't want to compile your own:

https://cef-builds.spotifycdn.com/index.html

Everything you need is in the **Standard Distribution**. If you need to debug the CEF part, grab the Symbols too. The extracted binary folder belongs in the `./thirdparty/cef3/` directory. The paths are currently hardcoded into the root `CMakelists.txt` file.

## Currently supported CEF version
The current version of CEF that's supported by this library is:

- **136.1.6+g1ac1b14+chromium-136.0.7103.114**

This is not the only version that could be supported, but it's the version that's currently configured and tested to work.

## Getting started
### Windows
#### Requirements
- Visual Studio 2022
- CMake 3.19 or newer
#### Generating Visual Studio Solution
##### x86
```bat
mkdir build_x86
cd build_x86
cmake -G "Visual Studio 17 2022" -A Win32 ..
```
#### x86-64
```bat
mkdir build_x64
cd build_x64
cmake -G "Visual Studio 17 2022" -A x64 ..
```

After running either of these sets of commands, you can enter your created directory and open the `gmod-html.sln` solution in Visual Studio. Compiling the `INSTALL` project will place a complete build into the `dist/` folder by default.

If you only have VS Build Tools, use this command in "Developer Command Prompt for VS 2022":
```
msbuild /p:Configuration=Release INSTALL.vcxproj
```


### Linux
#### Requirements
- A version of GCC/G++ or Clang/Clang++ with C++11 support
- CMake 3.19 or newer

#### Compiling
```sh
mkdir build
cd build
cmake -G "Unix Makefiles" -D CMAKE_BUILD_TYPE=Release -D CMAKE_POLICY_VERSION_MINIMUM=3.5 ..
make && make install
```

This will place a complete build into the `dist` folder by default.

### macOS
#### Requirements
- Ninja
- CMake 3.19 or newer

#### Compiling
```sh
mkdir build
cd build
cmake -G Ninja -D CMAKE_BUILD_TYPE=Release -D CMAKE_POLICY_VERSION_MINIMUM=3.5 ..
ninja && ninja install
```

This will place a complete build into the `dist` folder by default.

## TODO
- Dynamic loading of the HTML implementation. Atm we just use dylib() or LoadLibrary() in each host which is kind of lame. It'd be nice to simplify it.

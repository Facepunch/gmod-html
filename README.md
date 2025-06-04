# GMod HTML
This is pretty much just an abstraction layer around The Chromium Embedded Framework (https://bitbucket.org/chromiumembedded/cef)

## Chromium Embedded Framework Binary Distribution
To work with this project you will need a build of the Chromium Embedded Framework. You can download prebuilt versions of CEF from Spotify's Automated Builder if you don't want to compile your own:

https://cef-builds.spotifycdn.com/index.html

Everything you need is in the **Standard Distribution**. If you need to debug the CEF part, grab the Symbols too. The extracted binary folder belongs in the `./thirdparty/cef3/` directory. The paths are currently hardcoded into the root `CMakelists.txt` file.

## Currently supported CEF version
The current version of CEF that's supported by this library is:

- **137.0.10+g7e14fe1+chromium-137.0.7151.69**

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

#### Converting debug symbols to Breakpad symbols
```powershell
cd ../dist
dir . -Filter *.pdb -Recurse | %{ dump_syms -o "$($_.FullName).sym" $_.FullName }
dir . -Filter *.pdb.sym -Recurse | Rename-Item -NewName { $_.FullName -replace '\.pdb.sym$','.sym' }
```
Optionally cleanup platform symbols:
```powershell
dir . -Filter *.pdb -Recurse | Remove-Item -Path {$_.FullName}
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

#### Stripping / Separating / Converting debug symbols to Breakpad symbols
```bash
cd ../dist/linux64-Release/GarrysMod/bin/linux64
find *.so *.so.1 gmod chrome-sandbox chromium_process -type f -exec objcopy --only-keep-debug {} {}.debug \; -exec strip --strip-debug --strip-unneeded {} \;
find *.debug -type f -exec dump_syms -o {}.sym {} \;
for file in *.debug.sym; do
	mv "$file" "$(basename "$file" .debug.sym).sym"
done
```
Optionally cleanup platform symbols:
```bash
rm *.debug
```

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

##### Apple Silicon
```sh
cmake -G Ninja -D CMAKE_BUILD_TYPE=Release -D CMAKE_POLICY_VERSION_MINIMUM=3.5 -D CMAKE_APPLE_SILICON_PROCESSOR=x86_64 ..
```

This will place a complete build into the `dist` folder by default.

#### Stripping / Separating debug symbols
```
TODO
```

## TODO
- Dynamic loading of the HTML implementation. Atm we just use dylib() or LoadLibrary() in each host which is kind of lame. It'd be nice to simplify it.

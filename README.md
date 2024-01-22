# OpenTTD's Discord Integration Plugin

This plugin allows OpenTTD to announce what the player is doing on Discord.

## Supported OpenTTD

- OpenTTD 14+

## Supported Platforms

- Windows (32bit, 64bit)
- Linux (x86_64)
- MacOS (x86_64, arm64)

## Preparation

- Lookup the latest SDK version [here](https://discord.com/developers/docs/game-sdk/sdk-starter-guide#step-1-get-the-thing).
- Run `./download-sdk.sh <SDK version>`.

In case you can't execute `download-sdk.sh`, you have to do some actions manually:

- Download the latest Discord Game SDK from [here](https://discord.com/developers/docs/game-sdk/sdk-starter-guide#step-1-get-the-thing).
- Unpack the zip-file.
- Copy the `cpp` folder to `src/discord_sdk` (so there now is a `src/discord_sdk` folder with include-files).
- Copy the `lib` folder to `lib` (so there are now folders like `x86` in `lib/`).
- Rename all files that end with `.so` and `.dylib` from `discord_game_sdk` to `libdiscord_game_sdk`.
  Do not do this for `.dll` files.
- Rename all `.dll.lib` files to `.lib`.
- Open up `src/discord_sdk/ffi.h` and replace `#include <stdint.h>` with `#include <cstdint>`

## Building

This is a default CMake Project.

```bash
mkdir build
cd build
cmake -DCMAKE_INSTALL_PREFIX=$(pwd)/install -DDISCORD_APPLICATION_ID=1 ..
make
```

On MacOS, you need to add `-DCMAKE_OSX_ARCHITECTURES=[arm64|x86_64]`, to select which of the targets you want to build for.

Without an actual Discord Application ID, it will not show the correct game when trying; please register your own application for testing.

## Installation

```bash
make install
```

This creates a `lib` folder in the `install` folder of your build-dir.

In your OpenTTD folder, for example in your Documents folder, there should be a `social_integration` folder (if not, start OpenTTD once to create that folder).
Copy the content of the `lib` folder into `social_integration/discord`.

Now start OpenTTD to try out the plugin.
Start OpenTTD with `-dmisc=1` to get a bit more information about what plugins it sees and how they are loading.

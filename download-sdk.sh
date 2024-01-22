#!/bin/sh

set -e

if [ -z "${1}" ]; then
    echo "Usage: ${0} <sdk-version>"
    exit 1
fi

if [ -e "sdk" ]; then
    echo "ERROR - 'sdk' directory already exists; please remove that directory before running this script"
    exit 1
fi

SDK_VERSION="${1}"

mkdir sdk
(
    cd sdk
    curl -O https://dl-game-sdk.discordapp.net/${SDK_VERSION}/discord_game_sdk.zip
    unzip -q discord_game_sdk.zip
)

rm -rf src/discord_sdk
cp -R sdk/cpp src/discord_sdk

rm -rf lib
cp -R sdk/lib .

# Linux / MacOS expect libraries to start with `lib`.
find lib -name "*.so" | sed -e 'p;s/discord_game_sdk/libdiscord_game_sdk/' | xargs -n 2 mv
find lib -name "*.dylib" | sed -e 'p;s/discord_game_sdk/libdiscord_game_sdk/' | xargs -n 2 mv
# MSVC expects libraries to end with `.lib`, not `.dll.lib`.
find lib -name "*.dll.lib" | sed -e 'p;s/.dll.lib/.lib/' | xargs -n 2 mv

# Fix up the header file to use C++-style includes.
sed 's/#include <stdint.h>/#include <cstdint>/' < src/discord_sdk/ffi.h > src/discord_sdk/ffi.h.tmp
mv src/discord_sdk/ffi.h.tmp src/discord_sdk/ffi.h

rm -rf sdk

echo "OK - SDK downloaded."

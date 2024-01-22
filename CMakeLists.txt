cmake_minimum_required(VERSION 3.16)

project(discord-social
    VERSION 1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")
set(CMAKE_OSX_DEPLOYMENT_TARGET 11.0)

include(RPath)
include(CompileFlags)

if(NOT DISCORD_APPLICATION_ID)
    message(FATAL_ERROR "Please use -DDISCORD_APPLICATION_ID=<id> to tell the plugin what Discord application ID to use.")
endif()

add_compile_definitions(DISCORD_APPLICATION_ID=${DISCORD_APPLICATION_ID})

add_library(${PROJECT_NAME} SHARED
    src/plugin.cpp
    src/discord_sdk/achievement_manager.cpp
    src/discord_sdk/activity_manager.cpp
    src/discord_sdk/application_manager.cpp
    src/discord_sdk/core.cpp
    src/discord_sdk/image_manager.cpp
    src/discord_sdk/lobby_manager.cpp
    src/discord_sdk/network_manager.cpp
    src/discord_sdk/overlay_manager.cpp
    src/discord_sdk/relationship_manager.cpp
    src/discord_sdk/storage_manager.cpp
    src/discord_sdk/store_manager.cpp
    src/discord_sdk/types.cpp
    src/discord_sdk/user_manager.cpp
    src/discord_sdk/voice_manager.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE discord_game_sdk)
install(TARGETS ${PROJECT_NAME} DESTINATION .)

if(WIN32)
    if ("$ENV{Platform}" STREQUAL "arm64")
        message(FATAL_ERROR "Windows ARM64 is (currently) not supported by the Discord Game SDK")
    else()
        target_sources(${PROJECT_NAME} PRIVATE os/windows/library.manifest)

        if(CMAKE_SIZEOF_VOID_P EQUAL 8)
            target_link_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/lib/x86_64)
            set(LIBRARY_DEPENDENCY ${CMAKE_SOURCE_DIR}/lib/x86_64/discord_game_sdk.dll)
            install(FILES ${LIBRARY_DEPENDENCY} DESTINATION .)
        else()
            target_link_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/lib/x86)
            set(LIBRARY_DEPENDENCY ${CMAKE_SOURCE_DIR}/lib/x86/discord_game_sdk.dll)
            install(FILES ${LIBRARY_DEPENDENCY} DESTINATION .)
        endif()
    endif()
elseif(APPLE)
    if (CMAKE_OSX_ARCHITECTURES STREQUAL "arm64")
        target_link_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/lib/aarch64)
        set(LIBRARY_DEPENDENCY ${CMAKE_SOURCE_DIR}/lib/aarch64/libdiscord_game_sdk.dylib)
        install(FILES ${LIBRARY_DEPENDENCY} RENAME discord_game_sdk.dylib DESTINATION .)
    else()
        target_link_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/lib/x86_64)
        set(LIBRARY_DEPENDENCY ${CMAKE_SOURCE_DIR}/lib/x86_64/libdiscord_game_sdk.dylib)
        install(FILES ${LIBRARY_DEPENDENCY} RENAME discord_game_sdk.dylib DESTINATION .)
    endif()
else()
    target_link_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/lib/x86_64)
    set(LIBRARY_DEPENDENCY ${CMAKE_SOURCE_DIR}/lib/x86_64/libdiscord_game_sdk.so)
    install(FILES ${LIBRARY_DEPENDENCY} DESTINATION .)
endif()

include(Version)
include(InstallAndPackage)

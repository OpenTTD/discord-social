/*
 * Copyright 2023 OpenTTD project
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#ifdef _WIN32
#   define WIN32_LEAN_AND_MEAN
#   include <windows.h>
#   define DLL_EXPORT __declspec(dllexport)
#else
#   define DLL_EXPORT
#endif /* WIN32 */

#include <chrono>
#include <time.h>
#include <string>

#include "discord_sdk/discord.h"
#include "openttd_social_integration_api/openttd_social_integration_api.h"

static const std::string PLUGIN_SOCIAL_PLATFORM = "Discord";
static const std::string PLUGIN_NAME = "Discord Integration";
static const std::string PLUGIN_VERSION = "dev";

static discord::Core *_discord = nullptr;
static discord::Activity _activity = {};

static bool _activity_needs_update = false;
static std::chrono::time_point<std::chrono::system_clock> _activity_last_update = {};

void DiscordCallbackIgnore(discord::Result)
{
}

bool run_callbacks()
{
    if (_discord == nullptr) {
        return false;
    }

    if (_discord->RunCallbacks() != discord::Result::Ok) {
        return false;
    }

    /* Update the activity never more than once every 5 seconds. */
    if (_activity_needs_update && std::chrono::system_clock::now() - _activity_last_update > std::chrono::seconds(5)) {
        _activity_needs_update = false;
        _activity_last_update = std::chrono::system_clock::now();
        _discord->ActivityManager().UpdateActivity(_activity, DiscordCallbackIgnore);
    }

    return true;
}

void shutdown()
{
    _discord->ActivityManager().ClearActivity(DiscordCallbackIgnore);
    _discord->RunCallbacks();

    delete _discord;
    _discord = nullptr;
}

void event_enter_main_menu()
{
    _activity.SetState("Main Menu");
    _activity.GetTimestamps().SetStart(0);
    _activity_needs_update = true;
}

void event_enter_scenario_editor(unsigned int map_width, unsigned int map_height)
{
    std::string state = "Scenario Editor - " + std::to_string(map_width) + "x" + std::to_string(map_height);
    _activity.SetState(state.c_str());
    _activity.GetTimestamps().SetStart(time(nullptr));
    _activity_needs_update = true;
}

void event_enter_singleplayer(unsigned int map_width, unsigned int map_height)
{
    std::string state = "Singleplayer - " + std::to_string(map_width) + "x" + std::to_string(map_height);
    _activity.SetState(state.c_str());
    _activity.GetTimestamps().SetStart(time(nullptr));
    _activity_needs_update = true;
}

void event_enter_multiplayer(unsigned int map_width, unsigned int map_height)
{
    std::string state = "Multiplayer - " + std::to_string(map_width) + "x" + std::to_string(map_height);
    _activity.SetState(state.c_str());
    _activity.GetTimestamps().SetStart(time(nullptr));
    _activity_needs_update = true;
}

void event_joining_multiplayer()
{
    _activity.SetState("Joining Multiplayer");
    _activity.GetTimestamps().SetStart(0);
    _activity_needs_update = true;
}

bool initialize(const char *openttd_version)
{
    auto res = discord::Core::Create(DISCORD_APPLICATION_ID, DiscordCreateFlags_NoRequireDiscord, &_discord);
    if (res != discord::Result::Ok) {
        return false;
    }

    std::string version = "OpenTTD " + std::string(openttd_version);

    _activity.GetAssets().SetLargeImage("openttd_1024");
    _activity.GetAssets().SetLargeText(version.c_str());
    _activity.SetType(discord::ActivityType::Playing);
    _activity_needs_update = true;

    return true;
}

extern "C" DLL_EXPORT void SocialIntegration_v1_GetInfo(OpenTTD_SocialIntegration_v1_PluginInfo *plugin_info)
{
    static OpenTTD_SocialIntegration_v1_PluginInfo info {
        .social_platform = PLUGIN_SOCIAL_PLATFORM.c_str(),
        .name = PLUGIN_NAME.c_str(),
        .version = PLUGIN_VERSION.c_str(),
    };

    *plugin_info = info;
}

extern "C" DLL_EXPORT int SocialIntegration_v1_Init(OpenTTD_SocialIntegration_v1_PluginApi *plugin_api, const OpenTTD_SocialIntegration_v1_OpenTTDInfo *openttd_info)
{
    static OpenTTD_SocialIntegration_v1_PluginApi api {
        .shutdown = shutdown,
        .run_callbacks = run_callbacks,

        .event_enter_main_menu = event_enter_main_menu,
        .event_enter_scenario_editor = event_enter_scenario_editor,
        .event_enter_singleplayer = event_enter_singleplayer,
        .event_enter_multiplayer = event_enter_multiplayer,
        .event_joining_multiplayer = event_joining_multiplayer,
    };

    *plugin_api = api;

    if (!initialize(openttd_info->openttd_version)) {
        return OTTD_SOCIAL_INTEGRATION_V1_INIT_PLATFORM_NOT_RUNNING;
    }

    return OTTD_SOCIAL_INTEGRATION_V1_INIT_SUCCESS;
}

#ifdef _WIN32
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    /* Ensure our library remains loaded. */
    return TRUE;
}
#endif /* WIN32 */

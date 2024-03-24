#include "libretro.h"
#include <cmath>
#include <cstring>
#include <iostream>
#include <stdarg.h>

#include "../ONesSamaCore.h"

static retro_video_refresh_t video_cb;
static retro_audio_sample_t audio_cb;
static retro_environment_t environ_cb;
static retro_log_printf_t log_cb;
static retro_input_poll_t input_poll_cb;
static retro_input_state_t input_state_cb;

uint32_t framebuffer[256 * 240];
ONesSamaCore oNesSamaCore;
uint32_t colorPalette[64];

static void fallback_log(enum retro_log_level level, const char* fmt, ...)
{
    (void)level;
    va_list va;
    va_start(va, fmt);
    vfprintf(stdout, fmt, va);
    va_end(va);
}

void retro_init()
{
    std::cout << "ONESSAMA: " << __FUNCTION__ << std::endl;

    static struct retro_log_callback logging;
    if (environ_cb(RETRO_ENVIRONMENT_GET_LOG_INTERFACE, &logging))
        log_cb = logging.log;
    else
        log_cb = fallback_log;
}

void retro_get_system_info(struct retro_system_info* info)
{
    std::cout << "ONESSAMA: " << __FUNCTION__ << std::endl;

    memset(info, 0, sizeof(*info));
    info->library_name = "O-Nes-Sama";
    info->library_version = "0.8.0";
    info->need_fullpath = true;
    info->valid_extensions = "nes";
}

void retro_get_system_av_info(struct retro_system_av_info* info)
{
    std::cout << "ONESSAMA: " << __FUNCTION__ << std::endl;

    memset(info, 0, sizeof(*info));
    info->timing.fps = oNesSamaCore.getPPUFPS();
    info->timing.sample_rate = oNesSamaCore.cpufreq;
    constexpr unsigned ppuWidth = oNesSamaCore.getPPUInteralWidth();
    constexpr unsigned ppuHeight = oNesSamaCore.getPPUInteralHeight();
    info->geometry.base_width = ppuWidth;
    info->geometry.base_height = ppuHeight;
    info->geometry.max_width = ppuWidth;
    info->geometry.max_height = ppuHeight;
    info->geometry.aspect_ratio = float(ppuWidth) / ppuHeight;

    enum retro_pixel_format pixel_format = RETRO_PIXEL_FORMAT_XRGB8888;
    if (!environ_cb(RETRO_ENVIRONMENT_SET_PIXEL_FORMAT, &pixel_format)) {
        std::cout << "ONESSAMA: "
                  << "FAILED ALV!" << std::endl;
    }

    unsigned char* palette = oNesSamaCore.getDefaultPalette();
    for (int pos = 0; pos < 64; pos++) {
        unsigned char r = palette[pos * 3 + 0];
        unsigned char g = palette[pos * 3 + 1];
        unsigned char b = palette[pos * 3 + 2];
        colorPalette[pos] = b | (g << 8) | (r << 16);
    }
}

bool retro_load_game(const struct retro_game_info* game)
{
    std::cout << "ONESSAMA: " << __FUNCTION__ << std::endl;

    // clang-format off
    struct retro_input_descriptor desc[] = {
            { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT,   "Left" },
            { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_UP,     "Up" },
            { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_DOWN,   "Down" },
            { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT,  "Right" },
            { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_A,      "A" },
            { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_B,      "B" },
            { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_START,  "Start" },
            { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_SELECT, "Select" },

            { 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT,   "Left" },
            { 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_UP,     "Up" },
            { 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_DOWN,   "Down" },
            { 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT,  "Right" },
            { 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_A,      "A" },
            { 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_B,      "B" },
            { 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_START,  "Start" },
            { 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_SELECT, "Select" }
    };
    // clang-format on
    environ_cb(RETRO_ENVIRONMENT_SET_INPUT_DESCRIPTORS, desc);

    if (game && game->path) {
        oNesSamaCore.loadCartridge(game->path);
        oNesSamaCore.reset();
    }

    return true;
}

void retro_unload_game()
{
    std::cout << "ONESSAMA: " << __FUNCTION__ << std::endl;
    oNesSamaCore.unloadCartridge();
}

void retro_reset()
{
    std::cout << "ONESSAMA: " << __FUNCTION__ << std::endl;
    oNesSamaCore.reset();
}

void retro_run()
{
    // std::cout << "ONESSAMA: " << __FUNCTION__ << std::endl;

    // int debugLine = 0;

    // INPUT
    bool inputs[2][8];
    memset(inputs, 0, sizeof(inputs));
    enum Buttons {
        A,
        B,
        SELECT,
        START,
        UP,
        DOWN,
        LEFT,
        RIGHT
    };
    for (unsigned player = 0; player < 2; player++) {
        // clang-format off
        inputs[player][A] =      input_state_cb(player, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_A);
        inputs[player][B] =      input_state_cb(player, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_B);
        inputs[player][SELECT] = input_state_cb(player, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_SELECT);
        inputs[player][START] =  input_state_cb(player, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_START);
        inputs[player][UP] =     input_state_cb(player, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_UP);
        inputs[player][DOWN] =   input_state_cb(player, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_DOWN);
        inputs[player][LEFT] =   input_state_cb(player, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT);
        inputs[player][RIGHT] =  input_state_cb(player, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT);
        // clang-format on
    }

    oNesSamaCore.setControllersMatrix(inputs);
    // std::cout << debugLine++ << std::endl;

    oNesSamaCore.runOneFrame();
    // std::cout << debugLine++ << std::endl;

    unsigned char* palettedFrameBuffer = oNesSamaCore.getPalettedFrameBuffer();
    constexpr unsigned ppuWidth = oNesSamaCore.getPPUInteralWidth();
    constexpr unsigned ppuHeight = oNesSamaCore.getPPUInteralHeight();
    for (unsigned pos = 0; pos < ppuWidth * ppuHeight; pos++) {
        framebuffer[pos] = colorPalette[palettedFrameBuffer[pos]];
    }
    video_cb(framebuffer, ppuWidth, ppuHeight, sizeof(uint32_t) * ppuWidth);
    // std::cout << debugLine++ << std::endl;
}
/////////////////////////SET CALLBACKS/////////////////////////////////////////
void retro_set_environment(retro_environment_t cb)
{
    std::cout << "ONESSAMA: " << __FUNCTION__ << std::endl;
    environ_cb = cb;
}
void retro_set_audio_sample(retro_audio_sample_t cb)
{
    std::cout << "ONESSAMA: " << __FUNCTION__ << std::endl;
    audio_cb = cb;
    oNesSamaCore.setPushAudioSampleCallback(audio_cb);
}
void retro_set_video_refresh(retro_video_refresh_t cb)
{
    video_cb = cb;
}
void retro_set_input_poll(retro_input_poll_t cb) { input_poll_cb = cb; }
void retro_set_input_state(retro_input_state_t cb) { input_state_cb = cb; }

///////////////////////////////////DEFAULTS////////////////////////////////////
unsigned retro_api_version(void) { return RETRO_API_VERSION; }
unsigned retro_get_region(void) { return RETRO_REGION_NTSC; }

//////////////////////////////////EMPTY////////////////////////////////////////

void retro_cheat_reset(void) { }
void retro_cheat_set(unsigned index, bool enabled, const char* code) { }
bool retro_load_game_special(unsigned game_type, const struct retro_game_info* info, size_t num_info) { return false; }
void retro_set_controller_port_device(unsigned port, unsigned device) { }
void* retro_get_memory_data(unsigned id) { return NULL; }
size_t retro_get_memory_size(unsigned id) { return 0; }
size_t retro_serialize_size(void) { return 0; }
bool retro_serialize(void* data, size_t size) { return false; }
bool retro_unserialize(const void* data, size_t size) { return false; }
void retro_deinit(void) { }
void retro_set_audio_sample_batch(retro_audio_sample_batch_t cb) { }

/*
int main()
{
    //struct retro_system_info rsi;
    //retro_get_system_info(&rsi);
}
*/

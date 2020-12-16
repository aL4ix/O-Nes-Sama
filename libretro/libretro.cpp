#include "libretro.h"
#include <cstring>
#include <iostream>
#include <stdarg.h>
#include <cmath>


static retro_video_refresh_t video_cb;
static retro_audio_sample_t audio_cb;
static retro_environment_t environ_cb;
static retro_log_printf_t log_cb;
static retro_input_poll_t input_poll_cb;
static retro_input_state_t input_state_cb;

uint32_t framebuffer[256*240];
static unsigned phase;

static void fallback_log(enum retro_log_level level, const char *fmt, ...)
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
    info->need_fullpath = false;
    info->valid_extensions = "nes";
}

void retro_get_system_av_info(struct retro_system_av_info* info)
{
    std::cout << "ONESSAMA: " << __FUNCTION__ << std::endl;

    memset(info, 0, sizeof(*info));
    info->timing.fps = 60;
    info->timing.sample_rate = 44100;
    info->geometry.base_width = 256;
    info->geometry.base_height = 240;
    info->geometry.max_width = 256;
    info->geometry.max_height = 240;
    info->geometry.aspect_ratio = 256.f / 240.f;

    enum retro_pixel_format pixel_format = RETRO_PIXEL_FORMAT_XRGB8888;
    if(!environ_cb(RETRO_ENVIRONMENT_SET_PIXEL_FORMAT, &pixel_format))
    {
        std::cout << "ONESSAMA: " << "FALLO ALV!" << std::endl;
    }
}

bool retro_load_game(const struct retro_game_info* game)
{
    std::cout << "ONESSAMA: " << __FUNCTION__ << std::endl;

    struct retro_input_descriptor desc[] = {
            { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT,   "Left" },
            { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_UP,     "Up" },
            { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_DOWN,   "Down" },
            { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT,  "Right" },
            { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_A,      "A" },
            { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_B,      "B" },
            { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_START,  "Start" },
            { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_SELECT, "Select" }
    };
    environ_cb(RETRO_ENVIRONMENT_SET_INPUT_DESCRIPTORS, desc);

    if(game && game->data)
    {
        (void)game->data;
        (void)game->size;
    }
    //reset

    return true;
}

void retro_unload_game()
{
    std::cout << "ONESSAMA: " << __FUNCTION__ << std::endl;
    //Do we need this?
}

void retro_reset()
{
    std::cout << "ONESSAMA: " << __FUNCTION__ << std::endl;
    //reset
}

void retro_run()
{
    std::cout << "ONESSAMA: " << __FUNCTION__ << std::endl;

    input_poll_cb();
    int16_t a = input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_A);
    std::cout << a << std::endl;

    for (unsigned i = 0; i < 44100 / 60; i++, phase++)
    {
       int16_t val = 0x800 * sinf(2.0f * M_PI * phase * 300.0f / 30000.0f);
       audio_cb(val, val);
    }
    phase %= 100;

    for(int y=0; y<240; y++)
    {
        for(int x=0; x<256; x++)
        {
            framebuffer[(y*256)+x] = x+0xFFFF00*a;
        }
    }
    video_cb(framebuffer, 256, 240, sizeof(uint32_t)*256);
}
/////////////////////////SET CALLBACKS/////////////////////////////////////////
void retro_set_environment(retro_environment_t cb)
{
    std::cout << "ONESSAMA: " << __FUNCTION__ << std::endl;
    environ_cb = cb;
}
void retro_set_audio_sample(retro_audio_sample_t cb)
{
    audio_cb = cb;
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

void retro_cheat_reset(void) {}
void retro_cheat_set(unsigned index, bool enabled, const char *code) {}
bool retro_load_game_special(unsigned game_type, const struct retro_game_info *info, size_t num_info) { return false; }
void retro_set_controller_port_device(unsigned port, unsigned device) {}
void *retro_get_memory_data(unsigned id) { return NULL; }
size_t retro_get_memory_size(unsigned id){ return 0; }
size_t retro_serialize_size(void) { return 0; }
bool retro_serialize(void *data, size_t size) { return false; }
bool retro_unserialize(const void *data, size_t size) { return false; }
void retro_deinit(void) {}
void retro_set_audio_sample_batch(retro_audio_sample_batch_t cb) {}

//int main() {}

#include "../ONesSamaCore.h"
#include "../RetroEmu/RetroGraphics.hpp"
#include "libs/lodepng.h"
#include <dirent.h>
#include <stdlib.h>
#include <regex>
#include <algorithm>
#include <filesystem>


void runTest(const char* filepath)
{
    ONesSamaCore oNesSamaCore;
    oNesSamaCore.loadCartridge(filepath);
    oNesSamaCore.reset();
    for(int i=0; i<60; i++)
    {
        oNesSamaCore.runOneFrame();
    }

    unsigned char* palettedFrameBuffer = oNesSamaCore.getPalettedFrameBuffer();

    /*
    RetroGraphics retroGraphics(oNesSamaCore.getPPUInteralWidth(), oNesSamaCore.getPPUInteralHeight(), 3);
    retroGraphics.loadColorPaletteFromArray(oNesSamaCore.getDefaultPalette());
    retroGraphics.DrawBegin();
    retroGraphics.DrawPaletted(palettedFrameBuffer, oNesSamaCore.getPPUInteralWidth()*oNesSamaCore.getPPUInteralHeight());
    retroGraphics.DrawEnd();
    */

    lodepng::State state;
    unsigned char* palette = oNesSamaCore.getDefaultPalette();
    for(int pos=0; pos<64; pos++)
    {
        unsigned char r = palette[pos*3+0];
        unsigned char g = palette[pos*3+1];
        unsigned char b = palette[pos*3+2];
        lodepng_palette_add(&state.info_png.color, r, g, b, 255);
        lodepng_palette_add(&state.info_raw, r, g, b, 255);
    }

    state.info_png.color.colortype = LCT_PALETTE;
    state.info_png.color.bitdepth = 8;
    state.info_raw.colortype = LCT_PALETTE;
    state.info_raw.bitdepth = 8;
    state.encoder.auto_convert = 0;

    std::vector<unsigned char> buffer;
    unsigned error = lodepng::encode(buffer, palettedFrameBuffer, oNesSamaCore.getPPUInteralWidth(), oNesSamaCore.getPPUInteralHeight(), state);
    if(error)
    {
        std::cout << "encoder error " << error << ": "<< lodepng_error_text(error) << std::endl;
    }
    else
    {
        std::filesystem::path p(filepath);
        p.replace_extension(".png");
        std::filesystem::path o;
        {//Replace parent folder (nes-test-roms) to "expectedResults", so screenshots have the same tree structure as the roms
            int i = 0;
            for(auto it = p.begin(); it != p.end(); ++it, i++)
            {
                const char *s;
                if(i == 0)
                {
                    s = "expectedResults";
                }
                else
                {
                    s = it->c_str();
                }
                o /= s;
            }
        }
        std::filesystem::create_directories(o.parent_path());
        error = lodepng::save_file(buffer, o);
        if(error)
        {
            std::cout << "encoder error " << error << ": "<< lodepng_error_text(error) << std::endl;
        }
    }
}

std::vector<std::string> scanDir(std::string path, std::regex re)
{
    //TODO: Change this to use c++17's filesystem lib
    DIR *dir;
    struct dirent *ent;
    std::vector<std::string> result;
    if((dir = opendir(path.c_str())) != NULL)
    {
        while((ent = readdir(dir)) != NULL)
        {
            char* file = ent->d_name;
            if(strcmp(file, ".") != 0 && strcmp(file, "..") != 0)
            {
                std::string pathToFile = path+"/"+file;
                std::vector<std::string> a = scanDir(pathToFile, re);
                result.insert(result.end(), a.begin(), a.end());
                std::string lower = file;
                transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
                if(std::regex_match(lower, re))
                {
                    result.push_back(pathToFile);
                }
            }
        }
        closedir(dir);
    }
    return result;
}

int main()
{
    runTest("nes-test-roms/mmc3_irq_tests/4.Scanline_timing.nes");

    const char* testRomsFolder = "nes-test-roms";

    std::vector<std::string> results = scanDir(testRomsFolder, std::regex(".*\\.nes$"));
    if(results.size() > 0)
    {
        for(std::string& nesRom : results)
        {
            if( nesRom == "nes-test-roms/mmc3_irq_tests/4.Scanline_timing.nes")
            {
                printf("IT'S GONNA FAIL!\n");
            }
            runTest(nesRom.c_str());
        }
    }
    else
    {
        printf("Nothing found at %s\n", testRomsFolder);
    }
}

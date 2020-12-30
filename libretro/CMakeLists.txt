cmake_minimum_required(VERSION 3.10)
project(O_NES_Sama)

include_directories(.)
set(LINKER_SCRIPT "${CMAKE_SOURCE_DIR}/link.T")

set(O_NES_SAMA_SOURCE libretro.cpp
    libretro.h
    ../APU.cpp
    ../APU.h
    ../CPU.cpp
    ../CPU.h
    ../Cartridge/Cartridge.cpp
    ../Cartridge/Cartridge.hpp
    ../Cartridge/Mappers/AxROM.cpp
    ../Cartridge/Mappers/AxROM.hpp
    ../Cartridge/Mappers/BasicMapper.cpp
    ../Cartridge/Mappers/BasicMapper.hpp
    ../Cartridge/Mappers/CNROM.cpp
    ../Cartridge/Mappers/CNROM.hpp
    ../Cartridge/Mappers/GxROM.cpp
    ../Cartridge/Mappers/GxROM.hpp
    ../Cartridge/Mappers/MMC1.cpp
    ../Cartridge/Mappers/MMC1.hpp
    ../Cartridge/Mappers/MMC2_4.cpp
    ../Cartridge/Mappers/MMC2_4.hpp
    ../Cartridge/Mappers/MMC3.cpp
    ../Cartridge/Mappers/MMC3.hpp
    ../Cartridge/Mappers/MemoryMapper.cpp
    ../Cartridge/Mappers/MemoryMapper.h
    ../Cartridge/Mappers/UxROM.cpp
    ../Cartridge/Mappers/UxROM.hpp
    ../Input.cpp
    ../Input.h
    ../Logging/Logger.cpp
    ../Logging/Logger.h
    ../ONesSamaCore.cpp
    ../ONesSamaCore.h
    ../PPU.cpp
    ../PPU.h
    ../RetroEmu/RetroFraction.cpp
    ../RetroEmu/RetroFraction.hpp)

add_library(o_nes_sama_libretro SHARED ${O_NES_SAMA_SOURCE})

set_target_properties(o_nes_sama_libretro PROPERTIES PREFIX "")
set(CMAKE_EXE_LINKER_FLAGS "-T ${LINKER_SCRIPT}")

set_source_files_properties(libretro/libretro.cpp
        PROPERTIES OBJECT_DEPENDS ${LINKER_SCRIPT})
        
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

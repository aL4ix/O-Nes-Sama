#ifndef ROM_LOADER_HPP_INCLUDED
#define ROM_LOADER_HPP_INCLUDED

#include "Mappers/MemoryMapper.hpp"

class ROMLoader {
public:
    virtual ~ROMLoader();
    MemoryMapper* mapper = nullptr;
    virtual void saveState(FILE* file) = 0;
    virtual bool loadState(FILE* file) = 0;
};

#endif // ROM_LOADER_HPP_INCLUDED

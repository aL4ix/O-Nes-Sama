# How to build the libretro shared library
1. Go to the `libretro` folder and run:
```
cmake -DCMAKE_BUILD_TYPE=Release .
make
```

2. Then copy `o_nes_sama_libretro.info` to `/usr/share/libretro/info/`
and `o_nes_sama_libretro.so` to `/usr/lib/x86_64-linux-gnu/libretro/`

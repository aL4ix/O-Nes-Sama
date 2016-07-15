#ifndef MEMORYMODULE_H_INCLUDED
#define MEMORYMODULE_H_INCLUDED

class RAM{

    public:
        RAM(int size, unsigned char init=0xFF);

        inline unsigned char read(int address){
            return memoryArray[address];
        }
        inline void write(int address, unsigned char data){
            memoryArray[address] = data;
        }

        ~RAM();
    private:
        int size;
        unsigned char * memoryArray;
};


#endif // MEMORYMODULE_H_INCLUDED

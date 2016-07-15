#include "RAM.h"
#include <stdio.h>

RAM::RAM(int size, unsigned char init){
    this->size = size;
    memoryArray = new unsigned char[size];
    /*Initialize memory array to 0xFF*/
    for (int i=0; i < size; i++){ memoryArray[i] = init;}
}

/*unsigned char RAM::read(int address){
    return memoryArray[address];
}

void RAM::write(int address, unsigned char data){
    memoryArray[address] = data;
}*/

RAM::~RAM(){
    delete memoryArray;
}

#include <stdio.h>
#include <stdlib.h>
int lsLittleEndian(){
    unsigned short i=1;
    return (1 == *((char *)&i));
}
int main(int argc, const char *argv[])
{
    if(lsLittleEndian()){
        printf("Little Endian.\n");
    }else{
        printf("Big Endian.\n");
    }
    return 0;
}


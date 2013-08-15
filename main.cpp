#include "virtualringbuffer.h"
#include <string.h>
#include <stdio.h>

int main()
{
    VirtualRingBuffer vbrb(8000);

    char *indata = new char[2000];
    char *outdata = new char[2000];

    memset(indata, '1', 2000);

    //simple test
    memcpy(vbrb.reserve(), indata, 2000);
    vbrb.commit(2000);

    memcpy(outdata, vbrb.take(), 2000);
    vbrb.decommit(2000);

    if (memcmp(outdata, indata, 2000) != 0)
        printf("failed on simple test\n");


    //loop test
    for (int i = 0; i < 100; ++i) {
        memset(indata, '0' + i, 2000);

        memcpy(vbrb.reserve(), indata, 2000);
        vbrb.commit(2000);

        memcpy(outdata, vbrb.take(), 2000);
        vbrb.decommit(2000);

        if (memcmp(outdata, indata, 2000) != 0)
            printf("failed on loop test\n");
    }

    //wrapped test
    //assuming page is 4096 size then vbrb will be 8192
    //if we do 2000 * 5 commits when reading from rb 1808 will be from last write next 192 will be from first

    for (int i = 0; i < 5; ++i) {
        memset(indata, '0' + i, 2000);
        memcpy(vbrb.reserve(), indata, 2000);
        vbrb.commit(2000);
    }

    memcpy(outdata, vbrb.take(), 1808);
    vbrb.decommit(1808);

    for (int i = 0; i < 1808; ++i) {
        if (outdata[i] != '4')
            printf("failed on wrapped test\n");
    }

    memcpy(outdata, vbrb.take(), 192);
    vbrb.decommit(192);

    for (int i = 0; i < 192; ++i) {
        if (outdata[i] != '0')
            printf("failed on wrapped test\n");
    }

    return 0;
}


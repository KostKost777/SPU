#include <TXLib.h>

#include "common_functions.h"
#include "dump_functions.h"
#include "stack_functions.h"
#include "SPU_functions.h"

int main()
{
    struct SPU spu = {};

    SPUCtor(&spu);

    if (SPUReadCmdFromFile(&spu)){
        SPUDtor(&spu);
        return 1;
    }

    if (SPUVerifier(&spu)){
        SPUDtor(&spu);
        return 1;
    }

    if (SPURunCmdFromBuffer(&spu)){
        SPUDtor(&spu);
        return 1;
    }

    SPUDtor(&spu);

    return 0;
}

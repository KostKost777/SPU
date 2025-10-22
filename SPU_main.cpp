#include <TXLib.h>

#include "COMMON\common_functions.h"
#include "STACK\dump_functions.h"
#include "STACK\stack_functions.h"
#include "SPU_functions.h"

int main()
{
    atexit(CloseLogFile);

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

    printf("End SPU\n");

    return 0;
}

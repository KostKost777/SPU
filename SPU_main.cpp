#include <TXLib.h>

#include "common_functions.h"
#include "dump_functions.h"
#include "stack_functions.h"
#include "SPU_functions.h"

const char* log_file_name = "logfile.txt";

FILE* log_file = fopen(log_file_name, "w");

int main()
{
    struct SPU spu = {};

    SPUCtor(&spu);

    if (ReadCmdFromBinFile(&(spu.buffer)) == 1)
        return 0;

    if (SPURunCmdFromBuffer(&spu) == 1)
        return 0;

    SPUDtor(&spu);

    return 0;
}

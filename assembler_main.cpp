#include<TXLib.h>

#include "dump_functions.h"
#include "stack_functions.h"
#include "common_functions.h"

#include "MC_Onegin\read_poem_from_file_functions.h"

#include "assembler_functions.h"

int main()
{
    struct Buffer buffer = {};

    if (BufferCtor(&buffer)) {
        AsmEndProcessing(&buffer);
        return 1;
    }

    if (AsmReadCmdFromFile(&buffer)) {
        AsmEndProcessing(&buffer);
        return 1;
    }

    if (AsmWriteCmdInFile(&buffer)) {
        AsmEndProcessing(&buffer);
        return 1;
    }

    if (AsmWriteCmdInBinFile(&buffer)) {
        AsmEndProcessing(&buffer);
        return 1;
    }

    AsmEndProcessing(&buffer);

    printf("Asm OK\n");

    return 0;
}

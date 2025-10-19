#include<TXLib.h>

#include "disassembler_funcs.h"
#include "dump_functions.h"
#include "stack_functions.h"
#include "common_functions.h"

int main()
{
    struct Buffer buffer = {};

    if (BufferCtor(&buffer) == 1) {
        DisAsmEndProcessing(&buffer);
        return 0;
    }

    if (ReadCmdFromBinFile(&buffer) == 1) {
        DisAsmEndProcessing(&buffer);
        return 0;
    }

    if (DisAsmWriteCmdInFile(&buffer) == 1) {
        DisAsmEndProcessing(&buffer);
        return 0;
    }

    DisAsmEndProcessing(&buffer);

    printf("DisAsm OK\n");
}

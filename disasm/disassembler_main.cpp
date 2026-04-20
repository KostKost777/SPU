#include<TXLib.h>

#include "disassembler_funcs.h"
#include "STACK\dump_functions.h"
#include "STACK\stack_functions.h"
#include "COMMON\common_functions.h"

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

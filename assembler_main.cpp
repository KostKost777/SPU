#include<TXLib.h>

#include "assembler_functions.h"
#include "dump_functions.h"
#include "stack_functions.h"
#include "common_functions.h"

const char* log_file_name = "logfile.txt";

FILE* log_file = fopen(log_file_name, "w");

int main()
{
    struct Buffer buffer = {};

    buffer.last_index = 0;

    if (AssemReadCmdFromFile(&buffer) == 1) {
        AssemEndProcessing();
        return 0;
    }

    if (AssemWriteCmdInFile(&buffer) == 1) {
        AssemEndProcessing();
        return 0;
    }

    if (AssemWriteCmdInBinFile(&buffer) == 1) {
        AssemEndProcessing();
        return 0;
    }

    AssemEndProcessing();

    printf("Assem OK\n");
}

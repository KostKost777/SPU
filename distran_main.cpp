#include<TXLib.h>

#include "distranslator_funcs.h"
#include "dump_functions.h"
#include "stack_functions.h"
#include "common_functions.h"

const char* log_file_name = "logfile.txt";

FILE* log_file = fopen(log_file_name, "w");

int main()
{
    struct Buffer buffer = {};

    buffer.last_index = 0;

    if (ReadCmdFromBinFile(&buffer) == 1) {
        DistranEndProcessing();
        return 0;
    }

    if (DistranWriteCmdInFile(&buffer) == 1) {
        DistranEndProcessing();
        return 0;
    }

    DistranEndProcessing();

    printf("Distran OK\n");
}

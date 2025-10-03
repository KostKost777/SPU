#include<TXLib.h>

#include "translator_functions.h"
#include "dump_functions.h"
#include "stack_functions.h"
#include "common_functions.h"

const char* log_file_name = "logfile.txt";

FILE* log_file = fopen(log_file_name, "w");

int main()
{
    struct Buffer buffer = {};

    buffer.last_index = 0;

    if (TranReadCmdFromFile(&buffer) == 1) {
        TranEndProcessing();
        return 0;
    }

    if (TranWriteCmdInFile(&buffer) == 1) {
        TranEndProcessing();
        return 0;
    }

    if (TranWriteCmdInBinFile(&buffer) == 1) {
        TranEndProcessing();
        return 0;
    }

    TranEndProcessing();

    printf("Tran OK\n");
}

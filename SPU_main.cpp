#include <TXLib.h>

#include "dump_functions.h"
#include "stack_functions.h"
#include "SPU_functions.h"
#include "common_functions.h"

const char* log_file_name = "logfile.txt";

FILE* log_file = fopen(log_file_name, "w");

int main()
{
    struct Buffer buffer = {};

    buffer.last_index = 0;

    if (ReadCmdFromBinFile(&buffer) == 1)
        return 0;

    if (SPURunCmdFromBuffer(&buffer) == 1)
        return 0;

}

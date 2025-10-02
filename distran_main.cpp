#include<TXLib.h>

#include "distranslator_funcs.h"
#include "dump_functions.h"
#include "stack_functions.h"
#include "common_functions.h"

const char* log_file_name = "logfile.txt";

FILE* log_file = fopen(log_file_name, "w");

int main()
{
    const size_t CAPACITY = 1000;

    int code_arr[CAPACITY] = {};

    size_t last_index = 0;

    if (DistranReadCmdFromBinFile(code_arr, CAPACITY, &last_index) == 1) {
        DistranEndProcessing();
        return 0;
    }

    if (DistranWriteCmdInFile(code_arr, last_index) == 1) {
        DistranEndProcessing();
        return 0;
    }

    DistranEndProcessing();

    printf("Distran OK\n");
}

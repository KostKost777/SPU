#include<TXLib.h>

#include "translator_functions.h"
#include "dump_functions.h"
#include "stack_functions.h"

const char* log_file_name = "logfile.txt";

FILE* log_file = fopen(log_file_name, "w");

int main()
{
    const size_t CAPACITY = 1000;

    int code_arr[CAPACITY] = {};

    size_t last_index = 0;

    if (TranReadCmdFromFile(code_arr, CAPACITY, &last_index) == 1) {
        TranEndProcessing();
        return 0;
    }

    if (TranWriteCmdInFile(code_arr, last_index) == 1) {
        TranEndProcessing();
        return 0;
    }

    if (TranWriteCmdInBinFile(code_arr, last_index) == 1) {
        TranEndProcessing();
        return 0;
    }

    TranEndProcessing();

    printf("Tran OK\n");
}

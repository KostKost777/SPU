#include <TXLib.h>

#include "dump_functions.h"
#include "stack_functions.h"
#include "SPU_functions.h"
#include "translator_functions.h"

const char* log_file_name = "logfile.txt";
const char* bin_file_name = "binfile.txt";

FILE* log_file = fopen(log_file_name, "w");

int main()
{
    const size_t CAPACITY = 1000;

    int code_arr[CAPACITY] = {};

    size_t last_index = 0;

    if (SPUReadCmdFromFile(code_arr, CAPACITY, &last_index) == 1)
        return 0;

    if (SPURunCmdFromBuffer(code_arr, last_index) == 1)
        return 0;

}

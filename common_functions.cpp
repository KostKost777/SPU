#include <TXLib.h>

#include "dump_functions.h"
#include "stack_functions.h"
#include "SPU_functions.h"
#include "common_functions.h"

extern const char* log_file_name;

extern FILE* log_file;

int ReadCmdFromBinFile(struct Buffer* buffer)
{
    if (buffer == NULL){
        PRINT_LOGS("Buffer have NULL ptr");
        return 1;
    }

    if (buffer->code_arr == NULL){
        PRINT_LOGS("Array with commands have NULL ptr");
        return 1;
    }

    FILE* bin_file = fopen("binfile.bin", "rb");

    if (bin_file == NULL) {
        PRINT_LOGS("The bin file did not open");
        return 1;
    }

    buffer->last_index = fread(buffer->code_arr,
                               sizeof(int), CAPACITY,
                               bin_file);

    return 0;
}

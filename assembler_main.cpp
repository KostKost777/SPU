#include<TXLib.h>

#include "dump_functions.h"
#include "stack_functions.h"
#include "common_functions.h"

#include "MC_Onegin\read_poem_from_file_functions.h"

#include "assembler_functions.h"

const char* log_file_name = "logfile.txt";
const char* listing_file_name = "listing.lst";
const char* source_file_name = "source.asm";

FILE* log_file = fopen(log_file_name, "w");
FILE* lst_file = fopen(listing_file_name, "w");

int main()
{
    struct Buffer buffer = {};

    buffer.code_arr = (int*)calloc(CAPACITY, sizeof(int));

    buffer.size = 2;

    if (AssemReadCmdFromFile(&buffer) == 1) {
        AssemEndProcessing(&buffer);
        return 0;
    }

    if (AssemWriteCmdInFile(&buffer) == 1) {
        AssemEndProcessing(&buffer);
        return 0;
    }

    if (AssemWriteCmdInBinFile(&buffer) == 1) {
        AssemEndProcessing(&buffer);
        return 0;
    }

    AssemEndProcessing(&buffer);

    printf("Assem OK\n");
}

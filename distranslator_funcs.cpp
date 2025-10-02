#include<TXLib.h>

#include "distranslator_funcs.h"
#include "dump_functions.h"
#include "stack_functions.h"
#include "common_functions.h"

extern FILE* log_file;

extern const char* log_file_name;

int DistranReadCmdFromBinFile(int* code_arr, const size_t CAPACITY,
                              size_t* last_index)
{
    if (code_arr == NULL){
        PRINT_LOGS("Buffer have NULL ptr");
        return 1;
    }

    if (last_index == NULL){
        PRINT_LOGS("Last index have NULL ptr");
        return 1;
    }

    FILE* bin_file = fopen("binfile.bin", "rb");

    if (bin_file == NULL) {
        PRINT_LOGS("The bin file did not open");
        return 1;
    }

    *last_index = fread(code_arr, sizeof(int), CAPACITY, bin_file);

    return 0;
}

int DistranWriteCmdInFile(int* code_arr, int last_index)
{
    FILE* out_file = fopen("distran_out.txt", "w");

    // char* cmd_names[MAXCMD] = {"HLT", "PUSH", "ADD",
    //                            "SUB", "DIV", "OUT",
    //                            "MUL", "SQVRT"};

    for (int i = 0; i < last_index; ++i){
        switch(code_arr[i])
        {
            case cmdHLT: fprintf(out_file, "HLT\n");
                         break;

            case cmdPUSH: fprintf(out_file, "PUSH");
                          fprintf(out_file, " %d\n", code_arr[++i]);
                          break;

            case cmdSQVRT: fprintf(out_file, "SQVRT\n");
                         break;

            case cmdADD: fprintf(out_file, "ADD\n");
                         break;

            case cmdSUB: fprintf(out_file, "SUB\n");
                         break;

            case cmdDIV: fprintf(out_file, "DIV\n");
                         break;

            case cmdOUT: fprintf(out_file, "OUT\n");
                         break;

            case cmdMUL: fprintf(out_file, "MUL\n");
                         break;

            default: PRINT_LOGS("Invalid command");
                     return 1;
        }
    }
    return 0;
    fclose(out_file);
}

void DistranEndProcessing()
{
    fclose(log_file);
}

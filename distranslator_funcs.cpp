#include<TXLib.h>

#include "distranslator_funcs.h"
#include "dump_functions.h"
#include "stack_functions.h"
#include "common_functions.h"

extern FILE* log_file;

extern const char* log_file_name;

int DistranWriteCmdInFile(struct Buffer* buffer)
{
    FILE* out_file = fopen("distran_out.txt", "w");

    // char* cmd_names[MAXCMD] = {"HLT", "PUSH", "ADD",
    //                            "SUB", "DIV", "OUT",
    //                            "MUL", "SQVRT"};

    for (size_t i = 0; i < buffer->last_index; ++i){
        switch(buffer->code_arr[i])
        {
            case cmdHLT: fprintf(out_file, "HLT\n");
                         break;

            case cmdPUSH: fprintf(out_file, "PUSH");
                          fprintf(out_file, " %d\n", buffer->code_arr[++i]);
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

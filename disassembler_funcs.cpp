#include<TXLib.h>

#include "STACK\dump_functions.h"
#include "STACK\stack_functions.h"
#include "COMMON\common_functions.h"
#include "disassembler_funcs.h"

#include "MC_Onegin\read_poem_from_file_functions.h"
#include "MC_Onegin\text_functions.h"

FILE* out_file = fopen("disass_out.txt", "w");

int DisAsmWriteCmdInFile(struct Buffer* buffer)
{
    for (size_t i = HEADER_OFFSET; i < buffer->size; ++i){

        int now_cmd_code = buffer->code_arr[i];

        int index = 0;

        bool check_correct_cmd = false;

        for (; index < NUM_OF_CMDS; ++index) {

            if (all_cmd[index].cmd == now_cmd_code) {
                check_correct_cmd = true;
                break;
            }
        }

        if (check_correct_cmd == false) {
            PRINT_LOGS("Invalid command");
            return 1;
        }

        if (DisAsmProcessingCmd(buffer, all_cmd[index], &i)) {
            PRINT_LOGS("Invalid command");
            return 1;
        }



    }
    return 0;
}

int DisAsmProcessingCmd(struct Buffer* buffer, StructCmd command,
                       size_t* index)
{
    assert(buffer != NULL);
    assert(index != NULL);

    fprintf(out_file, "%s ", command.name);

    switch(command.arg)
    {
        case no_arg: fprintf(out_file, "\n");
                        break;

        case numeric_arg: fprintf(out_file, " %d\n", buffer->code_arr[++(*index)]);
                            break;

        case registr_arg:
            if (strcmp(command.name, "PUSHM") == 0
                || strcmp(command.name, "POPM") == 0) {

                fprintf(out_file, "[%cX]\n", buffer->code_arr[++(*index)] + 'A');
                break;
            }

            fprintf(out_file, "%cX\n", buffer->code_arr[++(*index)] + 'A');
                break;

        case jmp_arg: fprintf(out_file, ":%d\n", buffer->code_arr[++(*index)]);
                        break;

        default: PRINT_LOGS("Invalid command");

                    return 1;
    }
    return 0;
}

void DisAsmEndProcessing(struct Buffer* buffer)
{
    BufferDtor(buffer);
    fclose(log_file);
    fclose(out_file);
}

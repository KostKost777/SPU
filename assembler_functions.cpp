#include <TXLIB.h>

#include "assembler_functions.h"
#include "common_functions.h"
#include "dump_functions.h"

#include "MC_Onegin\read_poem_from_file_functions.h"
#include "MC_Onegin\text_functions.h"

extern FILE* log_file;

extern const char* log_file_name;

extern StructCmdWithName no_arg_funcs[NUMOFNOARGFUNCS];
extern StructCmdWithName single_arg_funcs[NUMOFSINGLEARGFUNCS];
extern const char* reg_name_arr[NUMBEROFREGS];

int AssemPrintLogs(const char* message,
                   size_t line, const char* source_file_name)
{
    if (log_file == NULL) {
        PRINT_LOGS("The log file did not open");
        return 1;
    }

    if (message == NULL) {
        PRINT_LOGS("NULL pointer to messages");
        return 1;
    }

    if (source_file_name == NULL) {
        PRINT_LOGS("NULL pointer to source file name");
        return 1;
    }

    fprintf(log_file, "ERROR: %s in %s:%u\n", message,
                                              source_file_name,
                                              line);
    fflush(log_file);

    return 0;
}

int AssemWriteCmdInFile(struct Buffer* buffer)
{

    if (buffer == NULL){
        PRINT_LOGS("Buffer have NULL ptr");
        return 1;
    }

    if (buffer->code_arr == NULL){
        PRINT_LOGS("Array with commands have NULL ptr");
        return 1;
    }

    FILE* bin_file = fopen("binfile.txt", "w");

    if (bin_file == NULL) {
        PRINT_LOGS("The bin file did not open");
        return 1;
    }

    for (size_t i = 0; i < buffer->size; ++i)
        fprintf(bin_file, "%d ", buffer->code_arr[i]);

    fclose(bin_file);

    return 0;
}

int AssemWriteCmdInBinFile(struct Buffer* buffer)
{
    if (buffer == NULL) {
        PRINT_LOGS("Buffer have NULL ptr");
        return 1;
    }

    if (buffer->code_arr == NULL) {
        PRINT_LOGS("Array with commands have NULL ptr");
        return 1;
    }

    FILE* bin_file = fopen("binfile.bin", "wb");

    if (bin_file == NULL) {
        PRINT_LOGS("The bin file did not open");
        return 1;
    }

    fwrite(buffer->code_arr, sizeof(int), buffer->size, bin_file);

    fclose(bin_file);

    return 0;
}

void AssemEndProcessing(struct Buffer* buffer)
{
    free(buffer->code_arr);
    buffer->code_arr = NULL;
    fclose(log_file);
}

int AssemReadCmdFromFile(struct Buffer* buffer)
{

    if (buffer == NULL){
        PRINT_LOGS("Buffer have NULL ptr");
        return 1;
    }

    if (buffer->code_arr == NULL){
        PRINT_LOGS("Array with commands have NULL ptr");
        return 1;
    }

    const char* source_file_name = "source.asm";

    Struct_Poem Asmtext = {};

    ReadPoemStructFromFile(&Asmtext, source_file_name);

    //printf("ll: %d\n", lines);

    const int MAXCMDLEN = 10;
    char cmdStr[MAXCMDLEN] = "";
    int arg = 0;
    size_t source_file_line_now = 1;

   // printf("%u\n", buffer->code_arr[0]);

    buffer->code_arr[0] = MASK;
    buffer->code_arr[1] = VERSION;

    int reg_index = 0;

    //printf("%d", lines);

    for (size_t i = 0; i < Asmtext.number_of_lines; ++i) {

        bool check_correct_cmd = false;

        int status = sscanf(Asmtext.poem_ptr_array[i].line_ptr,
                            "%s", cmdStr);

        //printf("NOW FUNC: %s\n", cmdStr);

        if (status == EOF){
            PRINT_LOGS("END OF FILE");
            break;
        }

        else if (status == 0) {
            AssemPrintLogs("Invalid command", source_file_line_now,
                                              source_file_name);
            return 1;
        }

        else if (status == 1) {

            for (int index = 0; index < NUMOFNOARGFUNCS; ++index) {
                if (strcmp(no_arg_funcs[index].name, cmdStr) == 0){
                    //printf("cmd0: %s\n", no_arg_funcs[index].name);
                    EmitInArr(buffer, no_arg_funcs[index].cmd);
                    check_correct_cmd = true;
                    break;
                }
            }

            for (int index = 0; index < NUMOFSINGLEARGFUNCS; ++index) {
                if (strcmp(single_arg_funcs[index].name, cmdStr) == 0 &&
                    sscanf(Asmtext.poem_ptr_array[i].line_ptr,
                    "%*s %d", &arg) == 1){

                    //printf("cmd1: %s\n", single_arg_funcs[index].name);
                    EmitInArr(buffer, single_arg_funcs[index].cmd);
                    EmitInArr(buffer, arg);
                    check_correct_cmd = true;
                    break;
                }
            }

            if (strcmp(cmdStr, "PUSHREG") == 0 &&
                    (reg_index = GetRegIndex(Asmtext.poem_ptr_array[i].line_ptr))
                     != -1) {

                EmitInArr(buffer, cmdPUSHREG);
                EmitInArr(buffer, reg_index);

                check_correct_cmd = true;
            }

            else if (strcmp(cmdStr, "POPREG") == 0 &&
                    (reg_index = GetRegIndex(Asmtext.poem_ptr_array[i].line_ptr))
                     != -1) {

                EmitInArr(buffer, cmdPOPREG);
                EmitInArr(buffer, reg_index);

                check_correct_cmd = true;
            }

            if (check_correct_cmd == false){
                AssemPrintLogs("Invalid command", source_file_line_now,
                                                  source_file_name);
                return 1;
            }
        }

        source_file_line_now++;
    }

    return 0;
}

int EmitInArr(struct Buffer* buffer, int value)
{
    if (buffer == NULL){
        PRINT_LOGS("Buffer have NULL ptr");
        return 1;
    }

    if (buffer->code_arr == NULL){
        PRINT_LOGS("Array with commands have NULL ptr");
        return 1;
    }

    buffer->code_arr[(buffer->size)++] = value;
    return 0;
}

int GetRegIndex(char* str_with_reg)
{
    assert(str_with_reg != NULL);

    const int MAXREGNAMELEN = 3;

    char regname[MAXREGNAMELEN] = {};

    int status = sscanf(str_with_reg, "%*s %s", regname);

    if (status == 0)
        return -1;

    for (size_t i = 0; i < NUMBEROFREGS; ++i) {
        if (strcmp(reg_name_arr[i], regname) == 0) {
            return reg_name_arr[i][0] - 'A';
        }
    }

    return -1;
}

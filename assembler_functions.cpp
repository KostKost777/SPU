#include <TXLIB.h>

#include "common_functions.h"
#include "dump_functions.h"

#include "MC_Onegin\read_poem_from_file_functions.h"
#include "MC_Onegin\text_functions.h"

#include "assembler_functions.h"

extern FILE* log_file;
extern FILE* lst_file;

extern const char* log_file_name;
extern const char* source_file_name;

extern AsmStructCmd all_cmd[NUM_OF_CMDS];
extern const char* reg_name_arr[NUMBER_OF_REGS];

int AssemPrintLogs(const char* message, size_t line)
{
    assert(message != NULL);

    if (log_file == NULL) {
        PRINT_LOGS("The log file did not open");
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

    assert(buffer != NULL);
    assert(buffer->code_arr != NULL);

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
    assert(buffer != NULL);
    assert(buffer->code_arr != NULL);

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
    fclose(lst_file);
}

int AssemReadCmdFromFile(struct Buffer* buffer)
{

    assert(buffer != NULL);
    assert(buffer->code_arr != NULL);

    Struct_Poem Asmtext = {};

    ReadPoemStructFromFile(&Asmtext, source_file_name);

    buffer->code_arr[0] = SIGNATURE;
    buffer->code_arr[1] = VERSION;

    //printf("%d", lines);

    int labels[CAPACITY] = {};

    const int NOLABLE = -1;
    memset(labels, NOLABLE, CAPACITY * sizeof(int));

    for (int compile_iterator = 1; compile_iterator <= 2; ++compile_iterator) {
        if (ProcessingAsmCommands(buffer, Asmtext, labels) == 1)
        return 1;
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

int TryRegFunctions(char* str_with_arg, struct Buffer* buffer,
                      int* pc, AsmStructCmd cmd_struct)
{
    assert(buffer != NULL);
    assert(str_with_arg != NULL);
    assert(pc != NULL);

    int registr = GetRegIndex(str_with_arg);

    //printf("REGISTR: %d   NAME: %s\n", registr,  cmd_struct.name);

    if (registr == -1) return 0;

    EmitInArr(buffer, cmd_struct.cmd);
    EmitInArr(buffer, registr);
    *pc+=2;

    fprintf(lst_file, "[%3u]   |  %3d %3d  |  %s %cX\n",
                       buffer->size - HEADER_OFFSET,
                       cmd_struct.cmd, registr,
                       cmd_struct.name, registr + 'A');

    return 1;
}

int GetRegIndex(char* str_with_reg)
{
    assert(str_with_reg != NULL);

    const int MAX_REG_NAME_LEN = 3;

    char regname[MAX_REG_NAME_LEN] = {};

    int status1 = sscanf(str_with_reg, "%*s %s", regname);

    // printf("\nSTR: %s\n\n", str_with_reg);

    int status2 = sscanf(str_with_reg, "%*s [%2[^]]", regname);

//     printf("\nSTR: %s\n\n", str_with_reg);
//
//     printf("\n\nABOBA: %s\n\n", regname);
//
//     printf("STATUS: %d %d\n", status1, status2);

    if ((status1 + status2) == 0)
        return -1;

    for (size_t i = 0; i < NUMBER_OF_REGS; ++i) {
        if (strcmp(reg_name_arr[i], regname) == 0) {
            return reg_name_arr[i][0] - 'A';
        }
    }

    return -1;
}

int TryJumpFunctions(char* str_with_arg, struct Buffer* buffer,
                      int* pc, int labels[], AsmStructCmd cmd_struct)
{
    assert(buffer != NULL);
    assert(str_with_arg != NULL);
    assert(labels != NULL);
    assert(pc != NULL);

    int arg = 0;

    if (sscanf(str_with_arg, "%*s :%d", &arg) == 0)
        return 0;

    *pc+=2;

    fprintf(lst_file, "[%3u]   |  %3d %3d  |  %s\n",
                       buffer->size - HEADER_OFFSET,
                       cmd_struct.cmd, labels[arg],
                       cmd_struct.name);

    EmitInArr(buffer, cmd_struct.cmd);
    EmitInArr(buffer, labels[arg]);

    return 1;
}

int TryPushFunction(char* str_with_arg, struct Buffer* buffer,
                      int* pc, AsmStructCmd cmd_struct)
{
    assert(buffer != NULL);
    assert(str_with_arg != NULL);
    assert(pc != NULL);

    int arg = 0;

    if (sscanf(str_with_arg, "%*s %d", &arg) == 0)
        return 0;

    fprintf(lst_file, "[%3u]   |  %3d %3d  |  %s\n",
                       buffer->size - HEADER_OFFSET,
                       cmd_struct.cmd, arg, cmd_struct.name);

    EmitInArr(buffer, cmd_struct.cmd);
    EmitInArr(buffer, arg);

    *pc+=2;

    return 1;
}

int TryNoArgFunctions(struct Buffer* buffer,
                      int* pc, AsmStructCmd cmd_struct)
{
    assert(buffer != NULL);
    assert(pc != NULL);

    if (cmd_struct.have_arg == true)
        return 0;

    EmitInArr(buffer, cmd_struct.cmd);

    fprintf(lst_file, "[%3u]   |  %3d      |  %s\n",
                    buffer->size - HEADER_OFFSET,
                    cmd_struct.cmd, cmd_struct.name);

    *pc+=1;

    return 1;
}

int ProcessingAsmCommands(struct Buffer* buffer, Struct_Poem Asmtext,
                          int labels[CAPACITY])
{
    assert(buffer != NULL);
    assert(labels != NULL);

    const int MAXCMDLEN = 10;
    char cmdStr[MAXCMDLEN] = "";

    int arg = 0;
    int pc = 0;
    int source_file_line_now = 1;

    buffer->size = HEADER_OFFSET;

    for (size_t i = 0; i < Asmtext.number_of_lines; ++i) {

        bool check_correct_cmd = false;

        if (sscanf(Asmtext.poem_ptr_array[i].line_ptr, " :%d", &arg) == 1){

            // printf("ARG: :%d\n", arg);
            labels[arg] = pc;
            source_file_line_now++;
            continue;
        }

        int status = sscanf(Asmtext.poem_ptr_array[i].line_ptr,
                            "%s", cmdStr);

        //   printf("NOW FUNC: %s\n", cmdStr);
        //   printf("STATUS: %d\n\n", status);

        if (status == EOF){
            PRINT_LOGS("END OF FILE");
            break;
        }

        else if (status == 0) {
            AssemPrintLogs("Invalid command", source_file_line_now);
            return 1;
        }

        for (int index = 0; index < NUM_OF_CMDS; ++index) {

            if (strcmp(all_cmd[index].name, cmdStr) == 0) {
                //printf("cmdStr: %s\n", cmdStr);
                if (TryNoArgFunctions(buffer, &pc, all_cmd[index]))
                    check_correct_cmd = true;

                else if (TryRegFunctions(Asmtext.poem_ptr_array[i].line_ptr,
                                        buffer, &pc, all_cmd[index]))
                    check_correct_cmd = true;

                else if (TryPushFunction(Asmtext.poem_ptr_array[i].line_ptr,
                                            buffer, &pc,
                                            all_cmd[index]))
                    check_correct_cmd = true;

                else if (TryJumpFunctions(Asmtext.poem_ptr_array[i].line_ptr,
                                            buffer, &pc, labels,
                                            all_cmd[index]))
                        check_correct_cmd = true;

                if (check_correct_cmd) break;
            }
        }

        if (check_correct_cmd == false){
            AssemPrintLogs("Invalid command", source_file_line_now);
            return 1;
        }

        source_file_line_now++;
    }

    fprintf(lst_file, "\n");

    return 0;
}


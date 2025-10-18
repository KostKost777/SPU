#include <TXLIB.h>

#include "common_functions.h"
#include "dump_functions.h"

#include "MC_Onegin\read_poem_from_file_functions.h"
#include "MC_Onegin\text_functions.h"

#include "assembler_functions.h"

FILE* lst_file = fopen("listing.lst", "w");

const char* source_file_name = "source.asm";

int AsmPrintLogs(const char* message, size_t line)
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

int BufferCtor(struct Buffer* buffer)
{
    assert(buffer != NULL);

    buffer->code_arr = (int*)calloc(BUFFER_CAPACITY, sizeof(int));

    if (buffer->code_arr == NULL) {
        PRINT_LOGS("Didn`t allocate memory for buffer");
        return 1;
    }

    buffer->size = HEADER_OFFSET;
    return 0;
}


int AsmWriteCmdInFile(struct Buffer* buffer)
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

int AsmWriteCmdInBinFile(struct Buffer* buffer)
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

void AsmEndProcessing(struct Buffer* buffer)
{
    assert(buffer != NULL);

    BufferDtor(buffer);

    fclose(log_file);
    fclose(lst_file);
}

void BufferDtor(struct Buffer* buffer)
{
    assert(buffer != NULL);

    free(buffer->code_arr);
    buffer->code_arr = NULL;

    buffer->size = 0;
}

int AsmReadCmdFromFile(struct Buffer* buffer)
{
    assert(buffer != NULL);
    assert(buffer->code_arr != NULL);

    Struct_Poem Asmtext = {};

    ReadPoemStructFromFile(&Asmtext, source_file_name);

    buffer->code_arr[0] = SIGNATURE;
    buffer->code_arr[1] = VERSION;

    int labels[NUM_OF_LABELS] = {};

    SetDefaultLabels(labels);

    for (int compile_iterator = 1; compile_iterator <= 2; ++compile_iterator) {
        if (FillAsmBuffer(buffer, Asmtext, labels) == 1)
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

int InsertRegFuncInBuffer(const char* str_with_arg, struct Buffer* buffer,
                          int* pc, StructCmd cmd_struct)
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

    fprintf(lst_file, "[%3d]   |  %3d %3d  |  %s %cX\n",
                       *pc, cmd_struct.cmd, registr,
                       cmd_struct.name, registr + 'A');

    return 1;
}

int GetRegIndex(const char* str_with_reg)
{
    assert(str_with_reg != NULL);

    const int MAX_REG_NAME_LEN = 3;

    char regname[MAX_REG_NAME_LEN] = {};

    int status1 = sscanf(str_with_reg, "%*s %s", regname);

    // printf("\nSTR: %s\n\n", str_with_reg);

    int status2 = sscanf(str_with_reg, "%*s [%[^]]", regname);

//     printf("\nSTR: %s\n\n", str_with_reg);
//
//     printf("STATUS: %d %d\n", status1, status2);

    if (status1 == 0 && status2 == 0)
        return -1;

    for (size_t i = 0; i < NUMBER_OF_REGS; ++i) {
        if (strcmp(reg_name_arr[i], regname) == 0) {
            return reg_name_arr[i][0] - 'A';
        }
    }

    return -1;
}

int InsertJumpFuncInBuffer(const char* str_with_arg, struct Buffer* buffer,
                           int* pc, int labels[], StructCmd cmd_struct)
{
    assert(buffer != NULL);
    assert(str_with_arg != NULL);
    assert(labels != NULL);
    assert(pc != NULL);

    int arg = 0;

    if (sscanf(str_with_arg, "%*s :%d", &arg) == 0)
        return 0;

    *pc+=2;

    fprintf(lst_file, "[%3d]   |  %3d %3d  |  %s\n",
                       *pc, cmd_struct.cmd, labels[arg],
                       cmd_struct.name);

    EmitInArr(buffer, cmd_struct.cmd);
    EmitInArr(buffer, labels[arg]);

    return 1;
}

int InsertPushFuncInBuffer(const char* str_with_arg, struct Buffer* buffer,
                           int* pc, StructCmd cmd_struct)
{
    assert(buffer != NULL);
    assert(str_with_arg != NULL);
    assert(pc != NULL);

    int arg = 0;

    if (sscanf(str_with_arg, "%*s %d", &arg) == 0)
        return 0;

    fprintf(lst_file, "[%3d]   |  %3d %3d  |  %s\n",
                       *pc, cmd_struct.cmd,
                       arg, cmd_struct.name);

    EmitInArr(buffer, cmd_struct.cmd);
    EmitInArr(buffer, arg);

    *pc+=2;

    return 1;
}

int InsertNoArgFuncInBuffer(struct Buffer* buffer,
                            int* pc, StructCmd cmd_struct)
{
    assert(buffer != NULL);
    assert(pc != NULL);

    EmitInArr(buffer, cmd_struct.cmd);

    fprintf(lst_file, "[%3d]   |  %3d      |  %s\n",
                       *pc, cmd_struct.cmd, cmd_struct.name);

    *pc+=1;

    return 1;
}

int FillAsmBuffer(struct Buffer* buffer, Struct_Poem Asmtext,
                  int labels[])
{
    assert(buffer != NULL);
    assert(labels != NULL);

    const int MAXCMDLEN = 10;
    char cmdStr[MAXCMDLEN] = "";

    int arg = 0;
    int pc = 0;
    int source_line_count = 1;

    buffer->size = HEADER_OFFSET;

    for (size_t i = 0; i < Asmtext.number_of_lines; ++i) {

        bool check_correct_cmd = false;

        const char* now_line = Asmtext.poem_ptr_array[i].line_ptr;

        if (sscanf(now_line, " :%d", &arg) == 1) {

            // printf("ARG: :%d\n", arg);
            labels[arg] = pc;
            source_line_count++;
            continue;
        }

        int status = sscanf(now_line, "%s", cmdStr);

        //    printf("NOW FUNC: %s\n", cmdStr);
        //    printf("STATUS: %d\n\n", status);

        if (status == EOF){
            PRINT_LOGS("END OF FILE");
            break;
        }

        if (status == 0) {
            AsmPrintLogs("Invalid command", source_line_count);
            return 1;
        }

        check_correct_cmd = ProcessingAsmCmd(buffer, now_line,
                                             cmdStr, labels, &pc);

        if (check_correct_cmd == false) {
            AsmPrintLogs("Invalid command", source_line_count);
            return 1;
        }

        source_line_count++;
    }

    fprintf(lst_file, "\n");

    return 0;
}

bool ProcessingAsmCmd(struct Buffer* buffer,
                      const char* line, char cmdStr[],
                      int labels[], int* pc)
{
    int index = 0;

    bool check_correct_cmd = false;

    for (; index < NUM_OF_CMDS; ++index) {

        if (strcmp(all_cmd[index].name, cmdStr) == 0) {
            //printf("cmdStr: %s\n", cmdStr);
            check_correct_cmd = true;
            break;
        }
    }

    if (check_correct_cmd == false)
        return check_correct_cmd;

    else if (all_cmd[index].arg == no_arg) {

        check_correct_cmd = InsertNoArgFuncInBuffer(buffer,
                                                    pc, all_cmd[index]);

    }

    else if (all_cmd[index].arg == registr_arg) {

        check_correct_cmd = InsertRegFuncInBuffer(line, buffer,
                                                  pc, all_cmd[index]);

    }

    else if (all_cmd[index].arg == numeric_arg) {

        check_correct_cmd = InsertPushFuncInBuffer(line, buffer,
                                                   pc, all_cmd[index]);

    }

    else if (all_cmd[index].arg == jmp_arg) {

        check_correct_cmd = InsertJumpFuncInBuffer(line, buffer,
                                                   pc, labels, all_cmd[index]);
    }

    return check_correct_cmd;
}


void SetDefaultLabels(int labels[])
{
    assert(labels != NULL);

    const int DEFAULT_LABEL = -1;

    for (int i = 0; i < NUM_OF_LABELS; ++i) {
        labels[i] = DEFAULT_LABEL;
    }
}

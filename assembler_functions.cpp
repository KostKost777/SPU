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

    fclose(lst_file);
}

//ATEXIT

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

    int pc = 0;
    int source_line_count = 1;

    buffer->size = HEADER_OFFSET;

    for (size_t i = 0; i < Asmtext.number_of_lines; ++i) {

        bool check_correct_cmd = false;

        const char* now_line = Asmtext.poem_ptr_array[i].line_ptr;
         //printf("%s\n", now_line);

        int status = sscanf(now_line, "%s", cmdStr);

        // printf("NOW FUNC: (%s)\n", cmdStr);
        // printf("STATUS: %d\n\n", status);

        if (DetectLabel(cmdStr, labels, pc)) {
            //printf("LINE: %d\n", source_line_count);
            source_line_count++;
            continue;
        }

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
    assert(buffer != NULL);
    assert(line != NULL);
    assert(pc != NULL);
    assert(cmdStr != NULL);

    unsigned long long cmdStr_hash = GetHash(cmdStr);

    //printf("Hash: %s - %u\n", cmdStr, cmdStr_hash);

    bool check_correct_cmd = true;

    StructCmd* current_cmd = (StructCmd*)bsearch(&cmdStr_hash, all_cmd,
                                                 NUM_OF_CMDS, sizeof(StructCmd),
                                                 BinSearchComparator);

    if (current_cmd == NULL) {
        check_correct_cmd = false;
        return check_correct_cmd;
    }

    switch(current_cmd->arg)
    {
        case no_arg:

            check_correct_cmd = InsertNoArgFuncInBuffer(buffer, pc,
                                                        *current_cmd);
            break;

        case registr_arg:

            check_correct_cmd = InsertRegFuncInBuffer(line, buffer,
                                                      pc, *current_cmd);
            break;

        case numeric_arg:

            check_correct_cmd = InsertPushFuncInBuffer(line, buffer,
                                                       pc, *current_cmd);
            break;

        case jmp_arg:

            check_correct_cmd = InsertJumpFuncInBuffer(line, buffer,
                                                       pc, labels, *current_cmd);
            break;

        default:
             check_correct_cmd = false;
             break;

    }

    return check_correct_cmd;
}

int DetectLabel(const char* cmdStr, int labels[], int pc)
{
    assert(cmdStr != NULL);
    assert(labels != NULL);

    if (cmdStr[0] == ':') {
        int arg = 0;

        sscanf(cmdStr, ":%d", &arg);

        labels[arg] = pc;

        return 1;
    }

    return 0;
}

unsigned long long GetHash(const char* cmd_name)
{
    unsigned long long hash = 5381;
    int sym = 0;

    while ((sym = *cmd_name++)) {
        hash = ((hash << 5) + hash) + sym;
    }

    return hash;
}

void SetDefaultLabels(int labels[])
{
    assert(labels != NULL);

    const int DEFAULT_LABEL = -1;

    for (int i = 0; i < NUM_OF_LABELS; ++i) {
        labels[i] = DEFAULT_LABEL;
    }
}

int StructCmdComparator(const void* param1, const void* param2)
{
    assert(param1 != NULL);
    assert(param2 != NULL);

    const StructCmd* cmd1 = (const StructCmd*)param1;
    const StructCmd* cmd2 = (const StructCmd*)param2;

    if (cmd1->hash < cmd2->hash)
        return -1;

    else if (cmd1->hash == cmd2->hash)
        return 0;

    return 1;
}

int BinSearchComparator(const void* param1, const void* param2)
{
    assert(param1 != NULL);
    assert(param2 != NULL);

    const unsigned long long* value = (const unsigned long long* )param1;
    const StructCmd* cmd = (const StructCmd*)param2;

    if (*value < cmd->hash)
        return -1;

    else if (*value == cmd->hash)
        return 0;

    return 1;
}

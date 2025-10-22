#include <TXLIB.h>

#include "..\COMMON\common_functions.h"
#include "..\STACK\dump_functions.h"

#include "..\MC_Onegin\read_poem_from_file_functions.h"
#include "..\MC_Onegin\text_functions.h"

#include "assembler_functions.h"

const char* source_file_name;

FILE* lst_file = fopen("listing.lst", "w");

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

    FILE* bin_file = fopen(txt_bin_file_name, "w");

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

    FILE* bin_file = fopen(bin_file_name, "wb");

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

    struct StructLabelsArr labels = {};

    labels.size = 0;

    SetDefaultLabels(labels.arr);

    for (int compile_iterator = 1; compile_iterator <= 2; ++compile_iterator) {
        if (FillAsmBuffer(buffer, Asmtext, &labels) == 1)
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
                           int* pc, struct StructLabelsArr* labels,
                           StructCmd cmd_struct)
{
    assert(buffer != NULL);
    assert(str_with_arg != NULL);
    assert(labels != NULL);
    assert(pc != NULL);

    EmitInArr(buffer, cmd_struct.cmd);

    const int MAX_LABEL_LEN = 100;

    char str_label[MAX_LABEL_LEN]= "";

    printf("aboba\n");

    if (sscanf(str_with_arg, "%*s :%s", str_label) != 1)
        return 0;

    //printf("STR: |%s| \n", str_label);

    size_t str_label_hash = GetHash(str_label);

    qsort(labels->arr, labels->size,
          sizeof(StructLabel), StructLabelComparator);

    // for (size_t i = 0; i < labels->size; ++i) {
    //     printf("LABEL_HASH: %u ---- PC: %d\n", labels->arr[i].label_hash,
    //                                            labels->arr[i].label_pc);
    // }

    StructLabel* current_label = (StructLabel*)bsearch(
                                                &str_label_hash, labels->arr,
                                                labels->size,
                                                sizeof(StructLabel),
                                                BinSearchComparatorForLabel);

    int label_pc_to_insert = -1;

    if (current_label == NULL) {

        //printf("Label_new_ne_metka: |%s|   HASH: %u\n  ", str_label, str_label_hash);

        labels->arr[labels->size].label_hash = str_label_hash;
        labels->arr[labels->size].label_pc = -1;
        labels->size++;

    }

    else {
        //printf("label_allready_exist_in_JMP_func: |%s|\n", str_label);
        label_pc_to_insert = current_label->label_pc;

    }

    EmitInArr(buffer, label_pc_to_insert);

    *pc+=2;

    fprintf(lst_file, "[%3d]   |  %3d %3d  |  %s %s\n",
                        *pc, cmd_struct.cmd, label_pc_to_insert,
                        cmd_struct.name, str_label);
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
                  struct StructLabelsArr* labels)
{
    assert(buffer != NULL);
    assert(labels != NULL);

    const int MAXCMDLEN = 100;
    char cmdStr[MAXCMDLEN] = "";

    int pc = 0;
    int source_line_count = 1;

    buffer->size = HEADER_OFFSET;

    for (size_t i = 0; i < Asmtext.number_of_lines; ++i) {

        bool check_correct_cmd = false;

        const char* now_line = Asmtext.poem_ptr_array[i].line_ptr;
         //printf("%s\n", now_line);

        int status = sscanf(now_line, "%s", cmdStr);

         //printf("NOW FUNC: (%s)\n", cmdStr);
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
                      struct StructLabelsArr* labels, int* pc)
{
    assert(buffer != NULL);
    assert(line != NULL);
    assert(pc != NULL);
    assert(cmdStr != NULL);

    size_t cmdStr_hash = GetHash(cmdStr);

    //printf("Hash: %s - %u\n", cmdStr, cmdStr_hash);

    bool check_correct_cmd = true;

    StructCmd* current_cmd = (StructCmd*)bsearch(&cmdStr_hash, all_cmd,
                                                 NUM_OF_CMDS, sizeof(StructCmd),
                                                 BinSearchComparatorForCmd);

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

int DetectLabel(const char* cmdStr, struct StructLabelsArr* labels, int pc)
{
    assert(cmdStr != NULL);
    assert(labels != NULL);

    if (cmdStr[0] != ':')
        return 0;

    const int MAX_LABEL_LEN = 100;

    char str_label[MAX_LABEL_LEN] = "";

    int status = sscanf(cmdStr, ":%s", str_label);

    size_t str_label_hash = GetHash(str_label);

    if (status != 1)
        return 0;

    qsort(labels->arr, labels->size,
          sizeof(StructLabel), StructLabelComparator);

    StructLabel* current_label = (StructLabel*)bsearch(
                                                &str_label_hash, labels->arr,
                                                labels->size,
                                                sizeof(StructLabel),
                                                BinSearchComparatorForLabel);

    if (current_label != NULL) {

        //printf("LABEL_EXISTE_v_metke: |%s|  HASH:  %u\n", str_label, str_label_hash);
        current_label->label_pc = pc;
        return 1;

    }

    labels->arr[labels->size].label_hash = str_label_hash;
    labels->arr[labels->size].label_pc = pc;

    //printf("LABEL_NEW_v_metke: |%s|  HASH:  %u\n", str_label, str_label_hash);

    labels->size++;

    return 1;
}

void SetDefaultLabels(struct StructLabel arr[])
{
    assert(arr != NULL);

    const int DEFAULT_LABEL_PC = -1;

    for (size_t i = 0; i < LABELS_ARR_CAPACITY; ++i) {
        arr[i].label_pc = DEFAULT_LABEL_PC;
    }
}

int StructCmdComparatorByCmdEnum(const void* param1, const void* param2)
{
    assert(param1 != NULL);
    assert(param2 != NULL);

    const StructCmd* cmd1 = (const StructCmd*)param1;
    const StructCmd* cmd2 = (const StructCmd*)param2;

    if (cmd1->cmd < cmd2->cmd) return -1;

    else if (cmd1->cmd == cmd2->cmd) return 0;

    return 1;
}

int StructCmdComparatorByHash(const void* param1, const void* param2)
{
    assert(param1 != NULL);
    assert(param2 != NULL);

    const StructCmd* cmd1 = (const StructCmd*)param1;
    const StructCmd* cmd2 = (const StructCmd*)param2;

    if (cmd1->hash < cmd2->hash) return -1;

    else if (cmd1->hash == cmd2->hash ) return 0;

    return 1;
}

int BinSearchComparatorForCmd(const void* param1, const void* param2)
{
    assert(param1 != NULL);
    assert(param2 != NULL);

    const size_t* value = (const size_t* )param1;
    const StructCmd* cmd = (const StructCmd*)param2;

    if (*value < cmd->hash) return -1;

    else if (*value == cmd->hash) return 0;

    return 1;
}

int BinSearchComparatorForLabel(const void* param1, const void* param2)
{
    assert(param1 != NULL);
    assert(param2 != NULL);

    const size_t* value = (const size_t* )param1;
    const StructLabel* label_2 = (const StructLabel*)param2;

    if (*value < label_2->label_hash) return -1;

    else if (*value == label_2->label_hash) return 0;

    return 1;
}

int StructLabelComparator(const void* param1, const void* param2)
{
    assert(param1 != NULL);
    assert(param2 != NULL);

    const StructLabel* label_1 = (const StructLabel*)param1;
    const StructLabel* label_2 = (const StructLabel*)param2;

    if (label_1->label_hash < label_2->label_hash) return -1;

    else if (label_1->label_hash == label_2->label_hash) return 0;

    return 1;
}

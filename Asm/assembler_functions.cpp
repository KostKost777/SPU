#include <TXLIB.h>

#include "COMMON\common_functions.h"
#include "STACK\dump_functions.h"

#include "MC_Onegin\read_poem_from_file_functions.h"
#include "MC_Onegin\clean_memory_functions.h"
#include "MC_Onegin\text_functions.h"

#include "assembler_functions.h"

const char* source_file_name = "";

const char* lst_file_name = "listing.lst";
FILE* lst_file = fopen(lst_file_name, "w");

static int IsLabel(const char* cmdStr, struct StructLabelsArr* labels, int pc);

static int EmitInArr(struct Buffer* buffer, int value);

static void LabelsDtor(struct StructLabelsArr* labels);

static int InsertPushFuncInBuffer(const char* str_with_arg,
                                  struct CurrentStateAsm cur_state_asm);

static int InsertJumpFuncInBuffer(const char* str_with_arg,
                                  struct CurrentStateAsm cur_state_asm,
                                  struct StructLabelsArr* labels);

static int InsertRegFuncInBuffer(const char* str_with_arg,
                          struct CurrentStateAsm cur_state_asm);

static int InsertNoArgFuncInBuffer(struct CurrentStateAsm cur_state_asm);

static int GetRegIndex(const char* str_with_reg);

static int FillAsmBuffer(struct Buffer* buffer, Struct_Poem Asmtext,
                         struct StructLabelsArr* labels);

static bool ProcessingAsmCmd(struct Buffer* buffer,
                             const char* line, char cmdStr[],
                             struct StructLabelsArr* labels, int* pc);

static struct StructLabel* FindLabelByHash(struct StructLabelsArr* labels,
                                           size_t cur_hash);

static int BinSearchComparatorForCmd(const void* param1, const void* param2);

static int BinSearchComparatorForLabel(const void* param1, const void* param2);

int StructLabelComparator(const void* param1, const void* param2);

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

int AsmReadCmdFromFile(struct Buffer* buffer)
{
    assert(buffer != NULL);
    assert(buffer->code_arr != NULL);

    Struct_Poem Asmtext = {};

    ReadPoemStructFromFile(&Asmtext, source_file_name);

    buffer->code_arr[0] = SIGNATURE;
    buffer->code_arr[1] = VERSION;

    struct StructLabelsArr labels = {};

    labels.compile_stage = 1;

    if (FillAsmBuffer(buffer, Asmtext, &labels)) {
        LabelsDtor(&labels);
        FreeDataPoem(&Asmtext);    //Vadik не забывать чистить
        return 1;                   //DED сделать goto
    }

    qsort(labels.arr, labels.size, sizeof(StructLabel), StructLabelComparator);

    labels.compile_stage = 2;

    if (FillAsmBuffer(buffer, Asmtext, &labels)) {
        LabelsDtor(&labels);
        FreeDataPoem(&Asmtext);
        return 1;
    }

    LabelsDtor(&labels);
    FreeDataPoem(&Asmtext);

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

int InsertRegFuncInBuffer(const char* str_with_arg,
                          struct CurrentStateAsm cur_state_asm)
{
    assert(cur_state_asm.buffer != NULL);
    assert(str_with_arg != NULL);
    assert(cur_state_asm.pc != NULL);

    int registr = GetRegIndex(str_with_arg);

    //printf("REGISTR: %d   NAME: %s\n", registr,  cmd_struct.name);

    if (registr == -1) return 0;

    EmitInArr(cur_state_asm.buffer, cur_state_asm.cmd_struct.cmd);
    EmitInArr(cur_state_asm.buffer, registr);

    fprintf(lst_file, "[%3d]   |  %3d %3d  |  %s %cX\n",
                       *cur_state_asm.pc, cur_state_asm.cmd_struct.cmd,
                       registr, cur_state_asm.cmd_struct.name, registr + 'A');

    *cur_state_asm.pc += 2;

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

    if (status1 != 1 && status2 != 1)
        return -1;

    for (size_t i = 0; i < NUMBER_OF_REGS; ++i) {
        if (strcmp(reg_name_arr[i], regname) == 0) {
            return reg_name_arr[i][0] - 'A';
        }
    }

    return -1;
}

int InsertJumpFuncInBuffer(const char* str_with_arg,
                           struct CurrentStateAsm cur_state_asm,
                           struct StructLabelsArr* labels)
{
    assert(cur_state_asm.buffer != NULL);
    assert(str_with_arg != NULL);
    assert(labels != NULL);
    assert(cur_state_asm.pc != NULL);

    const int MAX_LABEL_LEN = 100;
    char str_label[MAX_LABEL_LEN]= "";

    if (sscanf(str_with_arg, "%*s :%s", str_label) != 1)
        return 0;

    //printf("STR: |%s| \n", str_label);

    size_t str_label_hash = GetHash(str_label);

    struct StructLabel* current_label = FindLabelByHash(labels, str_label_hash);

    int label_pc_to_insert = -1;

    if (current_label != NULL
        && strcmp(current_label->label_name, str_label) == 0) {

        //printf("label_allready_exist_in_JMP_func: |%s|\n", str_label);
        label_pc_to_insert = current_label->label_pc;

    }

    else {

        //printf("Label_new_ne_metka: |%s|   HASH: %u\n  ", str_label, str_label_hash);

        labels->arr[labels->size].label_name = strdup(str_label);

        labels->arr[labels->size].label_hash = str_label_hash;
        labels->arr[labels->size].label_pc = -1;
        labels->size++;

    }

    EmitInArr(cur_state_asm.buffer, cur_state_asm.cmd_struct.cmd);
    EmitInArr(cur_state_asm.buffer, label_pc_to_insert);

    *cur_state_asm.pc += 2;

    fprintf(lst_file, "[%3d]   |  %3d %3d  |  %s %s\n",
                        *cur_state_asm.pc, cur_state_asm.cmd_struct.cmd,
                        label_pc_to_insert, cur_state_asm.cmd_struct.name,
                        str_label);
    return 1;
}

int InsertPushFuncInBuffer(const char* str_with_arg,
                           struct CurrentStateAsm cur_state_asm)
{
    assert(cur_state_asm.buffer != NULL);
    assert(str_with_arg != NULL);
    assert(cur_state_asm.pc != NULL);

    int arg = 0;

    if (sscanf(str_with_arg, "%*s %d", &arg) != 1)
        return 0;

    fprintf(lst_file, "[%3d]   |  %3d %3d  |  %s\n",
                       *cur_state_asm.pc, cur_state_asm.cmd_struct.cmd,
                       arg, cur_state_asm.cmd_struct.name);

    EmitInArr(cur_state_asm.buffer, cur_state_asm.cmd_struct.cmd);
    EmitInArr(cur_state_asm.buffer, arg);

    *cur_state_asm.pc += 2;

    return 1;
}

int InsertNoArgFuncInBuffer(struct CurrentStateAsm cur_state_asm)
{
    assert(cur_state_asm.buffer != NULL);
    assert(cur_state_asm.pc != NULL);

    EmitInArr(cur_state_asm.buffer, cur_state_asm.cmd_struct.cmd);

    fprintf(lst_file, "[%3d]   |  %3d      |  %s\n",
                       *cur_state_asm.pc, cur_state_asm.cmd_struct.cmd,
                       cur_state_asm.cmd_struct.name);

    *cur_state_asm.pc += 1;

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

        bool is_correct_cmd = false;

        const char* now_line = Asmtext.poem_ptr_array[i].line_ptr;
         //printf("%s\n", now_line);

        int status = sscanf(now_line, "%s", cmdStr);

         //printf("NOW FUNC: (%s)\n", cmdStr);
        // printf("STATUS: %d\n\n", status);

        if (IsLabel(cmdStr, labels, pc)) {
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

        is_correct_cmd = ProcessingAsmCmd(buffer, now_line,
                                             cmdStr, labels, &pc);

        if (!is_correct_cmd) {
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

    bool is_correct_cmd = true;  //переименовать Vadik

    StructCmd* current_cmd = (StructCmd*)bsearch(&cmdStr_hash, all_cmd,
                                                 NUM_OF_CMDS, sizeof(StructCmd),
                                                 BinSearchComparatorForCmd);

    if (current_cmd == NULL || strcmp(current_cmd->name, cmdStr)) {
        is_correct_cmd = false;
        return is_correct_cmd;
    }

    struct CurrentStateAsm cur_state_asm = {buffer, pc, *current_cmd};
    // сделать эту структуру Арман

    switch(current_cmd->arg)
    {
        case no_arg:

            is_correct_cmd = InsertNoArgFuncInBuffer(cur_state_asm);
            break;

        case registr_arg:

            is_correct_cmd = InsertRegFuncInBuffer(line, cur_state_asm);
            break;

        case numeric_arg:

            is_correct_cmd = InsertPushFuncInBuffer(line, cur_state_asm);
            break;

        case jmp_arg:

            is_correct_cmd = InsertJumpFuncInBuffer(line, cur_state_asm,
                                                       labels);
            break;

        default:
             is_correct_cmd = false;
             break;

    }

    return is_correct_cmd;
}

//переименовать Арман

int IsLabel(const char* cmdStr, struct StructLabelsArr* labels, int pc)
{
    assert(cmdStr != NULL);
    assert(labels != NULL);

    if (cmdStr[0] != ':')
        return 0;

    const int MAX_LABEL_LEN = 100;
    char str_label[MAX_LABEL_LEN] = "";

    int status = sscanf(cmdStr, ":%s", str_label);

    if (status != 1)
        return 0;

    size_t str_label_hash = GetHash(str_label);

    struct StructLabel* current_label = FindLabelByHash(labels, str_label_hash);

    if (current_label != NULL
        && strcmp(current_label->label_name, str_label) == 0) {

        //printf("LABEL_EXISTE_v_metke: |%s|  HASH:  %u\n", str_label, str_label_hash);
        current_label->label_pc = pc;

        return 1;
    }

    labels->arr[labels->size].label_name = strdup(str_label);

    labels->arr[labels->size].label_hash = str_label_hash;
    labels->arr[labels->size].label_pc = pc;
    labels->size++;

    //printf("LABEL_NEW_v_metke: |%s|  HASH:  %u\n", str_label, str_label_hash);

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

    else if (*value == cmd->hash ) return 0;

    return 1;
}


struct StructLabel* FindLabelByHash(struct StructLabelsArr* labels,
                                    size_t cur_hash)
{
    assert(labels != NULL);

    if(labels->compile_stage == 1) {
        for (size_t i = 0; i < labels->size; ++i) {
            if (cur_hash == labels->arr[i].label_hash)
                return &labels->arr[i];
        }

        return NULL;
    }

    return (StructLabel*)bsearch(&cur_hash, labels->arr,
                                 labels->size,
                                 sizeof(StructLabel),
                                 BinSearchComparatorForLabel);

}

void LabelsDtor(struct StructLabelsArr* labels)
{
    for (size_t i = 0;i < labels->size; ++i) {
        free(labels->arr[i].label_name);
        labels->arr[i].label_name = NULL;
    }
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

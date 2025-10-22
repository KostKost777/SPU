#ifndef TRANSLATOR
#define TRANSLATOR

const size_t LABELS_ARR_CAPACITY = 100;

struct StructLabel
{
    size_t label_hash;
    char* label_name;
    int label_pc;
};

struct StructLabelsArr
{
    struct StructLabel arr[LABELS_ARR_CAPACITY];
    size_t size;
};

extern const char* source_file_name;

extern const char* reg_name_arr[NUMBER_OF_REGS];

int DetectLabel(const char* cmdStr, struct StructLabelsArr* labels, int pc);

int AsmPrintLogs(const char* message, size_t line);

int AsmWriteCmdInFile(struct Buffer* buffer);

int AsmWriteCmdInBinFile(struct Buffer* buffer);

int AsmReadCmdFromFile(struct Buffer* buffer);

void AsmEndProcessing(struct Buffer* buffer);

int EmitInArr(struct Buffer* buffer, int value);

int InsertPushFuncInBuffer(const char* str_with_arg, struct Buffer* buffer,
                           int* pc, StructCmd cmd_struct);

int InsertJumpFuncInBuffer(const char* str_with_arg, struct Buffer* buffer,
                           int* pc, struct StructLabelsArr*,
                           StructCmd cmd_struct);

int InsertRegFuncInBuffer(const char* str_with_arg, struct Buffer* buffer,
                          int* pc, StructCmd cmd_struct);

int InsertNoArgFuncInBuffer(struct Buffer* buffer,
                            int* pc, StructCmd cmd_struct);

int GetRegIndex(const char* str_with_reg);

int FillAsmBuffer(struct Buffer* buffer, Struct_Poem Asmtext,
                  struct StructLabelsArr*);

bool ProcessingAsmCmd(struct Buffer* buffer,
                      const char* line, char cmdStr[],
                      struct StructLabelsArr*, int* pc);

struct StructLabel* FindLabelByHash(struct StructLabelsArr* labels,
                                    size_t cur_hash);

int StructCmdComparatorByHash(const void* param1, const void* param2);

int StructCmdComparatorByCmdEnum(const void* param1, const void* param2);

int BinSearchComparatorForCmd(const void* param1, const void* param2);

int BinSearchComparatorForLabel(const void* param1, const void* param2);

int StructLabelComparator(const void* param1, const void* param2);

#endif

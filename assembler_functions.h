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

struct CurrentStateAsm
{
    struct Buffer* buffer;
    int* pc;
    struct StructCmd cmd_struct;
};

extern const char* source_file_name;

extern const char* const reg_name_arr[NUMBER_OF_REGS];

int AsmPrintLogs(const char* message, size_t line);

int AsmWriteCmdInFile(struct Buffer* buffer);

int AsmWriteCmdInBinFile(struct Buffer* buffer);

int AsmReadCmdFromFile(struct Buffer* buffer);

void AsmEndProcessing(struct Buffer* buffer);

int StructCmdComparatorByHash(const void* param1, const void* param2);

#endif

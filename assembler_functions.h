#ifndef TRANSLATOR
#define TRANSLATOR

struct AsmStructCmd {
    const char* name;
    int cmd;
    bool have_arg;
};

int AssemPrintLogs(const char* message, size_t line);

int AssemWriteCmdInFile(struct Buffer* buffer);

int AssemWriteCmdInBinFile(struct Buffer* buffer);

int AssemReadCmdFromFile(struct Buffer* buffer);

void AssemEndProcessing(struct Buffer* buffer);

int EmitInArr(struct Buffer* buffer, int value);

int TryPushFunction(char* str_with_arg, struct Buffer* buffer,
                      int* pc, AsmStructCmd cmd_struct);

int TryJumpFunctions(char* str_with_arg, struct Buffer* buffer,
                   int* pc, int labels[], AsmStructCmd cmd_struct);

int TryRegFunctions(char* str_with_arg, struct Buffer* buffer,
                      int* pc, AsmStructCmd cmd_struct);

int TryNoArgFunctions(struct Buffer* buffer,
                      int* pc, AsmStructCmd cmd_struct);

int GetRegIndex(char* str_with_reg);

int ProcessingAsmCommands(struct Buffer* buffer, Struct_Poem Asmtext,
                          int labels[CAPACITY]);

#endif

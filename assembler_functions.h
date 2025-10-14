#ifndef TRANSLATOR
#define TRANSLATOR

int AssemPrintLogs(const char* message, size_t line);

int AssemWriteCmdInFile(struct Buffer* buffer);

int AssemWriteCmdInBinFile(struct Buffer* buffer);

int AssemReadCmdFromFile(struct Buffer* buffer);

void AssemEndProcessing(struct Buffer* buffer);

int EmitInArr(struct Buffer* buffer, int value);

int GetRegIndex(char* str_with_reg);

int ProcessingAsmCommands(struct Buffer* buffer, Struct_Poem Asmtext,
                          int labels[CAPACITY]);

#endif

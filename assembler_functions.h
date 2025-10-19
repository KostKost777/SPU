#ifndef TRANSLATOR
#define TRANSLATOR

const int NUM_OF_LABELS = 100;

extern const char* reg_name_arr[NUMBER_OF_REGS];

int DetectLabel(const char* cmdStr, int labels[], int pc);

int AsmPrintLogs(const char* message, size_t line);

int AsmWriteCmdInFile(struct Buffer* buffer);

int AsmWriteCmdInBinFile(struct Buffer* buffer);

int AsmReadCmdFromFile(struct Buffer* buffer);

void AsmEndProcessing(struct Buffer* buffer);

int EmitInArr(struct Buffer* buffer, int value);

int InsertPushFuncInBuffer(const char* str_with_arg, struct Buffer* buffer,
                           int* pc, StructCmd cmd_struct);

int InsertJumpFuncInBuffer(const char* str_with_arg, struct Buffer* buffer,
                           int* pc, int labels[], StructCmd cmd_struct);

int InsertRegFuncInBuffer(const char* str_with_arg, struct Buffer* buffer,
                          int* pc, StructCmd cmd_struct);

int InsertNoArgFuncInBuffer(struct Buffer* buffer,
                            int* pc, StructCmd cmd_struct);

int GetRegIndex(const char* str_with_reg);

int FillAsmBuffer(struct Buffer* buffer, Struct_Poem Asmtext,
                  int labels[]);

bool ProcessingAsmCmd(struct Buffer* buffer,
                      const char* line, char cmdStr[],
                      int labels[], int* pc);

void SetDefaultLabels(int labels[]);

#endif

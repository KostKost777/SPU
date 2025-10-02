#ifndef TRANSLATOR
#define TRANSLATOR

int TranPrintLogs(const char* message, size_t line,
                  const char* source_file_name);

int TranWriteCmdInFile(int* code_arr, size_t last_index);

int TranWriteCmdInBinFile(int* code_arr, size_t last_index);

int TranReadCmdFromFile(int* code_arr, size_t capacity, size_t* last_index);

void TranEndProcessing();

int EmitInArr(int* code_arr, size_t* last_index, int value);

#endif

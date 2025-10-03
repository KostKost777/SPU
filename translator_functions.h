#ifndef TRANSLATOR
#define TRANSLATOR

int TranPrintLogs(const char* message, size_t line,
                  const char* source_file_name);

int TranWriteCmdInFile(struct Buffer* buffer);

int TranWriteCmdInBinFile(struct Buffer* buffer);

int TranReadCmdFromFile(struct Buffer* buffer);

void TranEndProcessing();

int EmitInArr(struct Buffer* buffer, int value);

#endif

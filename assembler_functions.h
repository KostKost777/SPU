#ifndef TRANSLATOR
#define TRANSLATOR

int AssemPrintLogs(const char* message, size_t line,
                  const char* source_file_name);

int AssemWriteCmdInFile(struct Buffer* buffer);

int AssemWriteCmdInBinFile(struct Buffer* buffer);

int AssemReadCmdFromFile(struct Buffer* buffer);

void AssemEndProcessing();

int EmitInArr(struct Buffer* buffer, int value);

#endif

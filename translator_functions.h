#ifndef TRANSLATOR
#define TRANSLATOR

enum CmdCodes
{
    cmdHLT = 0,
    cmdPUSH = 1,
    cmdADD = 2,
    cmdSUB = 3,
    cmdDIV = 4,
    cmdOUT = 5,
    cmdMUL = 6
};

int TranPrintLogs(const char* message, size_t line,
                  const char* source_file_name);

int TranWriteCmdInFile(int* code_arr, size_t last_index);

int TranReadCmdFromFile(int* code_arr, size_t capacity, size_t* last_index);

void TranEndProcessing();

int EmitInArr(int* code_arr, size_t* last_index, int value);

#endif

#ifndef TRANSLATOR
#define TRANSLATOR

enum CmdCodes
{
    cmdHLT = 0,
    cmdPUSH = 1,
    cmdADD = 2,
    cmdSUB = 3,
    cmdDIV = 4,
    cmdOUT = 5
};

void PrintTranslatorLogs(const char* message, size_t line);

void WriteCmdInFile(int* code_arr, size_t last_index);

int SetNewArrSize(int** code_arr, size_t* capacity);

void EndProcessing(int** arr);

#endif

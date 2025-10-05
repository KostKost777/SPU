#ifndef COMMON
#define COMMON

const int MAXCMD = 8;

enum CmdCodes
{
    cmdHLT = 0,
    cmdPUSH = 1,
    cmdADD = 2,
    cmdSUB = 3,
    cmdDIV = 4,
    cmdOUT = 5,
    cmdMUL = 6,
    cmdSQVRT = 7
};

const size_t NUMBEROFREGS = 5; //AX, BX, CX, DX, EX

const size_t CAPACITY = 500;

struct Buffer
{
    int code_arr[CAPACITY];
    size_t size;
};

int ReadCmdFromBinFile(struct Buffer* buffer);

#endif

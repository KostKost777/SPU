#ifndef COMMON
#define COMMON

const int VERSION = 2;

const int MASK = 1380077378; //BOBR

enum CmdCodes
{
    cmdHLT = 0,
    cmdPUSH = 1,
    cmdADD = 2,
    cmdSUB = 3,
    cmdDIV = 4,
    cmdOUT = 5,
    cmdMUL = 6,
    cmdSQVRT = 7,
    cmdJMP = 8,
    cmdJB = 9,
    cmdJBE = 10,
    cmdJA = 11,
    cmdJAE = 12,
    cmdJE = 13,
    cmdJNE = 14,
    cmdIN = 15,
    cmdPUSHREG = 33,
    cmdPOPREG = 52
};

const size_t NUMBEROFREGS = 7; //AX, BX, CX, DX, EX, FX, GX

const size_t CAPACITY = 500;

struct Buffer
{
    int code_arr[CAPACITY];
    size_t size;
};

int ReadCmdFromBinFile(struct Buffer* buffer);

#endif

#ifndef COMMON
#define COMMON

const int VERSION = 3;

const int SIGNATURE = 1380077378; //BOBR

const int TITLE_OFFSET = 2;

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
    cmdCALL = 16,
    cmdRET = 17,
    cmdPUSHREG = 33,
    cmdPOPREG = 52
};

struct StructCmdWithName {
    const char* name;
    int cmd;
};

const int NUM_OF_JUMP_FUNCS = 8;

const int NUM_OF_NO_ARG_FUNCS = 9;

const size_t NUMBER_OF_REGS = 7; //AX, BX, CX, DX, EX, FX, GX

const int CAPACITY = 1500;

struct Buffer
{
    int* code_arr;
    size_t size;
};

int ReadCmdFromBinFile(struct Buffer* buffer);

#endif

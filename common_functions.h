#ifndef COMMON
#define COMMON

const int VERSION = 3;

const int SIGNATURE = 1380077378; //BOBR

const int HEADER_OFFSET = 2;

extern FILE* log_file;
extern const char* log_file_name;

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
    cmdPUSHM = 18,
    cmdPOPM = 19,
    cmdDRAW = 20,
    cmdPUSHREG = 33,
    cmdPOPREG = 52
};

enum ArgType
{
    no_arg = 0,
    registr_arg = 1,
    jmp_arg = 2,
    numeric_arg = 3
};

typedef int (*SPU_func_ptr)(struct SPU* spu, int cmd);

struct StructCmd {
    const char* name;
    const int cmd;
    const ArgType arg;
    const SPU_func_ptr cmd_function;
    unsigned long long hash;
};

const int NUM_OF_CMDS = 23;

const size_t NUMBER_OF_REGS = 8; //AX, BX, CX, DX, EX, FX, GX, HX

const int BUFFER_CAPACITY = 1500;

struct Buffer
{
    int* code_arr;
    size_t size;
};

void BufferDtor(struct Buffer* buffer);

int BufferCtor(struct Buffer* buffer);

int ReadCmdFromBinFile(struct Buffer* buffer);

void CloseLogFile();

extern StructCmd all_cmd[NUM_OF_CMDS];

#endif

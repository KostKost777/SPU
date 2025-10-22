#ifndef COMMON
#define COMMON

const int VERSION = 3;

const int SIGNATURE = 1380077378; //BOBR

const int HEADER_OFFSET = 2;

extern FILE* log_file;

extern const char* log_file_name;
extern const char* txt_bin_file_name;
extern const char* bin_file_name;

enum CmdCodes
{
    cmdHLT = 5,
    cmdPUSH = 18,
    cmdADD = 0,
    cmdSUB = 1,
    cmdDIV = 2,
    cmdOUT = 6,
    cmdMUL = 3,
    cmdSQVRT = 7,
    cmdJMP = 10,
    cmdJB = 11,
    cmdJBE = 12,
    cmdJA = 13,
    cmdJAE = 14,
    cmdJE = 15,
    cmdJNE = 16,
    cmdIN = 4,
    cmdCALL = 17,
    cmdRET = 8,
    cmdPUSHM = 21,
    cmdPOPM = 22,
    cmdDRAW = 9,
    cmdPUSHREG = 19,
    cmdPOPREG = 20
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
    size_t hash;
};

const int NUM_OF_CMDS = 23;

const size_t NUMBER_OF_REGS = 8; //AX, BX, CX, DX, EX, FX, GX, HX

const int BUFFER_CAPACITY = 1500;

extern StructCmd all_cmd[NUM_OF_CMDS];

struct Buffer
{
    int* code_arr;
    size_t size;
};

void BufferDtor(struct Buffer* buffer);

int BufferCtor(struct Buffer* buffer);

int ReadCmdFromBinFile(struct Buffer* buffer);

void CloseLogFile();

size_t GetHash(const char* cmd_name);

extern StructCmd all_cmd[NUM_OF_CMDS];

#endif

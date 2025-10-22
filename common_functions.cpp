#include <TXLib.h>

#include "..\COMMON\common_functions.h"
#include "..\STACK\dump_functions.h"
#include "..\STACK\stack_functions.h"
#include "..\SPU\SPU_functions.h"

#include "..\MC_Onegin\read_poem_from_file_functions.h"
#include "..\MC_Onegin\text_functions.h"

#include "..\ASM\assembler_functions.h"

const char* log_file_name = "../LOGS/logfile.txt";
const char* txt_bin_file_name = "../COMMON/binfile.txt";
const char* bin_file_name = "../COMMON/binfile.bin";

FILE* log_file = fopen(log_file_name, "w");

StructCmd all_cmd[NUM_OF_CMDS] =
{
{"ADD",      cmdADD,       no_arg,      BinaryArifmeticFuncs,     GetHash("ADD")},
{"SUB",      cmdSUB,       no_arg,      BinaryArifmeticFuncs,     GetHash("SUB")},
{"DIV",      cmdDIV,       no_arg,      BinaryArifmeticFuncs,     GetHash("DIV")},
{"MUL",      cmdMUL,       no_arg,      BinaryArifmeticFuncs,     GetHash("MUL")},
{"IN",       cmdIN,        no_arg,      In,                       GetHash("IN")},
{"HLT",      cmdHLT,       no_arg,      Hlt,                      GetHash("HLT")},
{"OUT",      cmdOUT,       no_arg,      Out,                      GetHash("OUT")},
{"SQVRT",    cmdSQVRT,     no_arg,      Sqvrt,                    GetHash("SQVRT")},
{"RET",      cmdRET,       no_arg,      Ret,                      GetHash("RET")},
{"DRAW",     cmdDRAW,      no_arg,      Draw,                     GetHash("DRAW")},
{"JMP",      cmdJMP,       jmp_arg,     Jmp,                      GetHash("JMP")},
{"JB",       cmdJB,        jmp_arg,     ConditionalJumps,         GetHash("JB")},
{"JBE",      cmdJBE,       jmp_arg,     ConditionalJumps,         GetHash("JBE")},
{"JA",       cmdJA,        jmp_arg,     ConditionalJumps,         GetHash("JA")},
{"JAE",      cmdJAE,       jmp_arg,     ConditionalJumps,         GetHash("JAE")},
{"JE",       cmdJE,        jmp_arg,     ConditionalJumps,         GetHash("JE")},
{"JNE",      cmdJNE,       jmp_arg,     ConditionalJumps,         GetHash("JNE")},
{"CALL",     cmdCALL,      jmp_arg,     Call,                     GetHash("CALL")},
{"PUSH",     cmdPUSH,      numeric_arg,    Push,                  GetHash("PUSH")},
{"PUSHREG",  cmdPUSHREG,   registr_arg,    RegFuncs,              GetHash("PUSHREG")},
{"POPREG",   cmdPOPREG,    registr_arg,    RegFuncs,              GetHash("POPREG")},
{"PUSHM",    cmdPUSHM,     registr_arg,    MemFuncs,              GetHash("PUSHM")},
{"POPM",     cmdPOPM,      registr_arg,    MemFuncs,              GetHash("POPM")}
};
int a;
const char* reg_name_arr[NUMBER_OF_REGS] = {"AX", "BX", "CX",
                                            "DX", "EX", "FX",
                                            "GX", "HX"};

int ReadCmdFromBinFile(struct Buffer* buffer)
{
    if (buffer == NULL){
        PRINT_LOGS("Buffer have NULL ptr");
        return 1;
    }

    if (buffer->code_arr == NULL){
        PRINT_LOGS("Array with commands have NULL ptr");
        return 1;
    }

    FILE* bin_file = fopen(bin_file_name, "rb");

    if (bin_file == NULL) {
        PRINT_LOGS("The bin file did not open");
        return 1;
    }

    buffer->size = fread(buffer->code_arr,
                         sizeof(int), BUFFER_CAPACITY,
                         bin_file);

    fclose(bin_file);

    return 0;
}

int BufferCtor(struct Buffer* buffer)
{
    assert(buffer != NULL);

    buffer->code_arr = (int*)calloc(BUFFER_CAPACITY, sizeof(int));

    if (buffer->code_arr == NULL) {
        PRINT_LOGS("Didn`t allocate memory for buffer");
        return 1;
    }

    buffer->size = HEADER_OFFSET;
    return 0;
}

void BufferDtor(struct Buffer* buffer)
{
    assert(buffer != NULL);

    free(buffer->code_arr);
    buffer->code_arr = NULL;

    buffer->size = 0;
}

size_t GetHash(const char* cmd_name)
{
    size_t hash = 5381;
    int sym = 0;

    while ((sym = *cmd_name++)) {
        hash = (((hash << 5) + hash) + sym);
    }

    return hash;
}

void CloseLogFile()
{
    fclose(log_file);
    printf("Logfile close\n");
}



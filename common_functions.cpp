#include <TXLib.h>

#include "dump_functions.h"
#include "stack_functions.h"
#include "common_functions.h"
#include "SPU_functions.h"

#include "MC_Onegin\read_poem_from_file_functions.h"
#include "MC_Onegin\text_functions.h"

#include "assembler_functions.h"

const char* log_file_name = "logfile.txt";
FILE* log_file = fopen(log_file_name, "w");

StructCmd all_cmd[NUM_OF_CMDS] =
{
{"ADD",         cmdADD,          no_arg,        BinaryArifmeticFuncs},
{"SUB",         cmdSUB,          no_arg,        BinaryArifmeticFuncs},
{"DIV",         cmdDIV,          no_arg,        BinaryArifmeticFuncs},
{"MUL",         cmdMUL,          no_arg,        BinaryArifmeticFuncs},
{"IN",         cmdIN,           no_arg,         In},
{"HLT",         cmdHLT,          no_arg,        Hlt},
{"OUT",         cmdOUT,          no_arg,        Out},
{"SQVRT",       cmdSQVRT,        no_arg,        Sqvrt},
{"RET",         cmdRET,          no_arg,        Ret},
{"DRAW",        cmdDRAW,         no_arg,        Draw},
{"JMP",         cmdJMP,          jmp_arg,       Jmp},
{"JB",          cmdJB,           jmp_arg,       ConditionalJumps},
{"JBE",         cmdJBE,          jmp_arg,       ConditionalJumps},
{"JA",          cmdJA,           jmp_arg,       ConditionalJumps},
{"JAE",         cmdJAE,          jmp_arg,       ConditionalJumps},
{"JE",          cmdJE,           jmp_arg,       ConditionalJumps},
{"JNE",         cmdJNE,          jmp_arg,       ConditionalJumps},
{"CALL",        cmdCALL,         jmp_arg,       Call},
{"PUSH",        cmdPUSH,         numeric_arg,   Push},
{"PUSHREG",     cmdPUSHREG,      registr_arg,   RegFuncs},
{"POPREG",      cmdPOPREG,       registr_arg,   RegFuncs},
{"PUSHM",       cmdPUSHM,        registr_arg,   MemFuncs},
{"POPM",        cmdPOPM,         registr_arg,   MemFuncs}
};

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

    FILE* bin_file = fopen("binfile.bin", "rb");

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

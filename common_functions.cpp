#include <TXLib.h>

#include "dump_functions.h"
#include "stack_functions.h"
#include "common_functions.h"
#include "SPU_functions.h"

#include "MC_Onegin\read_poem_from_file_functions.h"
#include "MC_Onegin\text_functions.h"

#include "assembler_functions.h"

extern const char* log_file_name;

extern FILE* log_file;

StructCmd all_cmd[NUM_OF_CMDS] =

{{"IN",          cmdIN,          no_arg,        In},
{"HLT",         cmdHLT,          no_arg,        Hlt},
{"ADD",         cmdADD,          no_arg,        Add},
{"SUB",         cmdSUB,          no_arg,        Sub},
{"DIV",         cmdDIV,          no_arg,        Div},
{"OUT",         cmdOUT,          no_arg,        Out},
{"MUL",         cmdMUL,          no_arg,        Mul},
{"SQVRT",       cmdSQVRT,        no_arg,        Sqvrt},
{"RET",         cmdRET,          no_arg,        Ret},
{"DRAW",        cmdDRAW,         no_arg,        Draw},
{"JMP",         cmdJMP,          jmp_arg,       Jmp},
{"JB",          cmdJB,           jmp_arg,       Jb},
{"JBE",         cmdJBE,          jmp_arg,       Jbe},
{"JA",          cmdJA,           jmp_arg,       Ja},
{"JAE",         cmdJAE,          jmp_arg,       Jae},
{"JE",          cmdJE,           jmp_arg,       Je},
{"JNE",         cmdJNE,          jmp_arg,       Jne},
{"CALL",        cmdCALL,         jmp_arg,       Call},
{"PUSH",        cmdPUSH,         digit_arg,     Push},
{"PUSHREG",     cmdPUSHREG,      registr_arg,   PushReg},
{"POPREG",      cmdPOPREG,       registr_arg,   PopReg},
{"PUSHM",       cmdPUSHM,        registr_arg,   PushM},
{"POPM",        cmdPOPM,         registr_arg,   PopM}};

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
                         sizeof(int), CAPACITY,
                         bin_file);

    fclose(bin_file);

    return 0;
}

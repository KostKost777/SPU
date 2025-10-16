#include <TXLib.h>

#include "dump_functions.h"
#include "stack_functions.h"
#include "common_functions.h"
#include "SPU_functions.h"

extern const char* log_file_name;

extern FILE* log_file;

StructCmdWithName no_arg_funcs[NUM_OF_NO_ARG_FUNCS] = { {"IN",    cmdIN},
                                                    {"HLT",   cmdHLT},
                                                    {"ADD",   cmdADD},
                                                    {"SUB",   cmdSUB},
                                                    {"DIV",   cmdDIV},
                                                    {"OUT",   cmdOUT},
                                                    {"MUL",   cmdMUL},
                                                    {"SQVRT", cmdSQVRT},
                                                    {"RET",   cmdRET}};

StructCmdWithName single_arg_funcs[NUM_OF_JUMP_FUNCS] = { {"JMP",  cmdJMP},
                                                       {"JB",   cmdJB},
                                                       {"JBE",  cmdJBE},
                                                       {"JA",   cmdJA},
                                                       {"JAE",  cmdJAE},
                                                       {"JE",   cmdJE},
                                                       {"JNE",  cmdJNE},
                                                       {"CALL", cmdCALL}};

const char* reg_name_arr[NUMBER_OF_REGS] = {"AX", "BX", "CX",
                                          "DX", "EX", "FX",
                                          "GX"};

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

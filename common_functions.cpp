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

AsmStructCmd all_cmd[NUM_OF_CMDS] = {{"IN",          cmdIN,           0},
                                    {"HLT",         cmdHLT,          0},
                                    {"ADD",         cmdADD,          0},
                                    {"SUB",         cmdSUB,          0},
                                    {"DIV",         cmdDIV,          0},
                                    {"OUT",         cmdOUT,          0},
                                    {"MUL",         cmdMUL,          0},
                                    {"SQVRT",       cmdSQVRT,        0},
                                    {"RET",         cmdRET,          0},
                                    {"JMP",         cmdJMP,          1},
                                    {"JB",          cmdJB,           1},
                                    {"JBE",         cmdJBE,          1},
                                    {"JA",          cmdJA,           1},
                                    {"JAE",         cmdJAE,          1},
                                    {"JE",          cmdJE,           1},
                                    {"JNE",         cmdJNE,          1},
                                    {"CALL",        cmdCALL,         1},
                                    {"PUSH",        cmdPUSH,         1},
                                    {"PUSHREG",     cmdPUSHREG,      1},
                                    {"POPREG",      cmdPOPREG,       1},
                                    {"PUSHM",       cmdPUSHM,        1},
                                    {"POPM",        cmdPOPM,         1}};

SPUStructCmd all_cmd_funcs[NUM_OF_CMDS] = { {cmdIN,           In},
                                            {cmdADD,          Add},
                                            {cmdSUB,          Sub},
                                            {cmdDIV,          Div},
                                            {cmdOUT,          Out},
                                            {cmdMUL,          Mul},
                                            {cmdSQVRT,        Sqvrt},
                                            {cmdRET,          Ret},
                                            {cmdJMP,          Jmp},
                                            {cmdJB,           Jb},
                                            {cmdJBE,          Jbe},
                                            {cmdJA,           Ja},
                                            {cmdJAE,          Jae},
                                            {cmdJE,           Je},
                                            {cmdJNE,          Jne},
                                            {cmdCALL,         Call},
                                            {cmdPUSH,         Push},
                                            {cmdPUSHREG,      PushReg},
                                            {cmdPOPREG,       PopReg},
                                            {cmdPOPM,         PopM},
                                            {cmdPUSHM,        PushM}};

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

#include <TXLib.h>
#include <windows.h>

#include "..\COMMON\common_functions.h"
#include "..\STACK\dump_functions.h"
#include "..\STACK\stack_functions.h"
#include "SPU_functions.h"

#include "..\MC_Onegin\read_poem_from_file_functions.h"

int SPUCtor(SPU* spu)
{
    assert(spu != NULL);

    const int STACK_CAPACITY = 1000;

    StackCtor(&spu->stk, STACK_CAPACITY);
    StackCtor(&spu->ret_stk, STACK_CAPACITY);

    spu->open_window = false;

    spu->RAM = (int* )calloc(RAM_SIZE,sizeof(int));

    assert(spu->RAM != NULL);

    spu->pc = 0;
    spu->buffer.size = 0;

    return 0;
}

int SPUVerifier(SPU* spu)
{
    assert(spu != NULL);

    if (spu->buffer.size >= BUFFER_CAPACITY) {
        spu->err_code |= code_arr_size_err;
        return code_arr_size_err;
    }

    if (spu->pc >= BUFFER_CAPACITY) {
        spu->err_code |= pc_err;
        return pc_err;
    }

    if (spu->buffer.code_arr == NULL) {
        spu->err_code |= code_arr_ptr_err;
        return code_arr_ptr_err;
    }

    return 0;
}

void SPUDtor(SPU* spu)
{
    free(spu->buffer.code_arr - HEADER_OFFSET);
    StackDtor(&spu->stk);
    StackDtor(&spu->ret_stk);
    free(spu->RAM);
    spu->RAM = NULL;
    fclose(log_file);
}

int SPUReadCmdFromFile(struct SPU* spu)
{
    assert(spu != NULL);

    FILE* bin_file = fopen(bin_file_name, "rb");

    if (bin_file == NULL) {
        PRINT_LOGS("The bin file did not open");
        return 1;
    }

    int* cmd_buffer = (int*)calloc(BUFFER_CAPACITY, sizeof(int));

    assert(cmd_buffer != NULL);

    spu->buffer.size = fread(cmd_buffer, sizeof(int), BUFFER_CAPACITY, bin_file);

    spu->buffer.size -= HEADER_OFFSET;

    spu->buffer.code_arr = cmd_buffer + HEADER_OFFSET;

    if (spu->buffer.code_arr[-2] != SIGNATURE) {
        PRINT_LOGS("This byte code is not for this processor.");
        return 1;
    }

    if (spu->buffer.code_arr[-1] != VERSION) {
        PRINT_LOGS("This processor version is outdated, please recompile it.");
        return 1;
    }

    fclose(bin_file);

    return 0;
}

int SPURunCmdFromBuffer(struct SPU* spu)
{
    SPUVerifier(spu);
    for (; spu->pc < spu->buffer.size; ++(spu->pc)) {

        #ifdef DUMP
        SPUdump(spu);
        getchar();
        #endif

        int current_cmd = spu->buffer.code_arr[spu->pc];

        //printf("CUR_CMD: %d   NAME: %s\n", current_cmd, all_cmd[current_cmd].name);

        if (all_cmd[current_cmd].cmd_function(spu, all_cmd[current_cmd].cmd)) {
            PRINT_LOGS("Invalid command");
            return 1;
        }

        SPUVerifier(spu);
    }

    return 0;
}

int BinaryArifmeticFuncs(struct SPU* spu, int cmd)
{
    assert(spu != NULL);

    GET_TWO_ELEM(&spu->stk);

    switch(cmd)
    {
        case cmdADD: StackPush(&spu->stk, elem1 + elem2);
                     break;

        case cmdSUB: StackPush(&spu->stk, elem2 - elem1);
                     break;

        case cmdMUL: StackPush(&spu->stk, elem1 * elem2);
                     break;

        case cmdDIV:
            if (elem1 == 0) {
                PRINT_LOGS("You can`t divide by zero");
                return 1;
            }
            else
                StackPush(&spu->stk, elem2 / elem1);

            break;

        default: PRINT_LOGS("Unknown command");
                 return 1;
    }
    return 0;
}

int ConditionalJumps(struct SPU* spu, int cmd)
{
    assert(spu != NULL);

    int arg = spu->buffer.code_arr[++spu->pc] - 1;

    GET_TWO_ELEM(&spu->stk);

    switch(cmd)
    {
        case cmdJB:
            if (elem2 < elem1)
                spu->pc = arg;
            break;

        case cmdJBE:
            if (elem2 <= elem1)
                spu->pc = arg;
            break;

        case cmdJA:
            if (elem2 > elem1)
                spu->pc =  arg;
            break;

        case cmdJAE:
            if (elem2 >= elem1)
                spu->pc =  arg;
            break;

        case cmdJE:
            if (elem2 == elem1)
                spu->pc =  arg;
            break;

        case cmdJNE:
            if (elem2 != elem1)
                spu->pc =  arg;
            break;

        default: PRINT_LOGS("Unknown command");
                 return 1;
    }

    return 0;
}

int RegFuncs(struct SPU* spu, int cmd)
{
    assert(spu != NULL);

    int* registr_cell = &spu->regs[spu->buffer.code_arr[++spu->pc]];

    switch(cmd)
    {
        case cmdPOPREG: StackPop(&spu->stk, registr_cell);
                        break;

        case cmdPUSHREG: StackPush(&spu->stk, *registr_cell);
                         break;

        default: PRINT_LOGS("Unknown command");
                 return 1;
    }

    return 0;
}

int MemFuncs(struct SPU* spu, int cmd)
{
    assert(spu != NULL);

    int last_el = 0;

    int* registr_cell = &spu->regs[spu->buffer.code_arr[++spu->pc]];

    switch(cmd)
    {
        case cmdPOPM: StackPop(&spu->stk, &last_el);
                      spu->RAM[*registr_cell] = last_el;
                      //printf("REG: %d  COLOR: %d\n", *registr_cell, last_el);
                      break;

        case cmdPUSHM: StackPush(&spu->stk, spu->RAM[*registr_cell]);
                       break;

        default: PRINT_LOGS("Unknown command");
                 return 1;
    }

    return 0;
}

int Push(struct SPU* spu, int cmd)
{
    assert(spu != NULL);

    (void)cmd;

    int arg = spu->buffer.code_arr[++spu->pc];

    StackPush(&spu->stk, arg);

    return 0;
}

int Jmp(struct SPU* spu, int cmd)
{
    assert(spu != NULL);

    (void)cmd;

    int arg = spu->buffer.code_arr[++spu->pc] - 1;

    spu->pc = arg;

    return 0;
}

int Call(struct SPU* spu, int cmd)
{
    assert(spu != NULL);

    (void)cmd;

    int arg = spu->buffer.code_arr[++spu->pc] - 1;

    StackPush(&spu->ret_stk, spu->pc);

    spu->pc =  arg;

    return 0;
}

int Hlt(struct SPU* spu, int cmd)
{
    assert(spu != NULL);

    (void)cmd;

    printf("End of programm\n");

    return 1;
}

int Ret(struct SPU* spu, int cmd)
{
    assert(spu != NULL);

    (void)cmd;

    int last_ret = 0;

    StackPop(&spu->ret_stk, &last_ret);

    spu->pc = last_ret;

    return 0;
}

int Draw(struct SPU* spu, int cmd)
{
    assert(spu != NULL);

    (void)cmd;

    const int SCALE = 10;

    const int COLOR_OFFSET = 3;

    if(!spu->open_window)
        txCreateWindow(100 * SCALE, 100 * SCALE);

    txSetColor(TX_BLACK);

    for (int i = 0; i < RAM_SIZE; i += COLOR_OFFSET) {

        int X_coord = (i / COLOR_OFFSET) % 100 * SCALE;
        int Y_coord = (i / COLOR_OFFSET) / 100 * SCALE;

        txSetFillColor(RGB(spu->RAM[i], spu->RAM[i + 1], spu->RAM[i + 2]));

        txRectangle(X_coord, Y_coord,
                    X_coord + SCALE, Y_coord + SCALE);
    }

    return 0;
}

int In(struct SPU* spu, int cmd)
{
    assert(spu != NULL);
    (void)cmd;

    int value = 0;

    printf("Enter number: ");

    while (scanf("%d", &value) != 1) {
        SkipLine();
        printf("\nWrong input\n");
        printf("Enter number: ");
    }

    StackPush(&spu->stk, value);

    return 0;
}

int Sqvrt(struct SPU* spu, int cmd)
{
    assert(spu != NULL);

    (void)cmd;

    StackValueType elem = 0;

    StackPop(&spu->stk, &elem);

    if (elem < 0) {
        PRINT_LOGS("You can`t take sqrt from negative number");
        return 1;
    }

    StackPush(&spu->stk,(int)sqrt(elem));

    return 0;
}

int Out(struct SPU* spu, int cmd)
{
    assert(spu != NULL);

    (void)cmd;

    StackValueType elem = 0;

    StackPop(&spu->stk, &elem);

    printf("Answer is: %d\n", elem);

    return 0;
}

void SPUdump(struct SPU* spu)
{
    printf("\n\n&&&&&&&&&&&&&&&&&&&&&&&&-DUMP-&&&&&&&&&&&&&&&&&&&&&&&&&&\n");

    if (spu->err_code != 0)
        PrintSPUErrors(spu->err_code);

    printf("STACK: \n");
    printf("SIZE: %d \n", spu->stk.size);
    for (int i = CANARY_CONST; i < spu->stk.size; ++i) {
        printf("    *[%d] - %d\n", i, spu->stk.data[i]);
    }
    printf("------------------------------------------------------\n");

    printf("CODE_ARR: \n");

    for (size_t i = 0; i < spu->buffer.size; ++i) {
        if (i == spu->pc) {
            printf("(%d) ", spu->buffer.code_arr[i]);
            continue;
        }
        printf("%d ", spu->buffer.code_arr[i]);
    }

    printf("\n");

    printf("------------------------------------------------------\n");

    printf("REGISTERS:\n");
    char reg_name = 'A';
    for (size_t i = 0; i < NUMBER_OF_REGS; ++i, reg_name++) {
        printf("[%cX]  -  [%d]\n", reg_name, spu->regs[i]);
    }

    printf("------------------------------------------------------\n");

    printf("RET_STACK: \n");
    printf("SIZE: %d \n", spu->ret_stk.size);
    for (int i = CANARY_CONST; i < spu->ret_stk.size; ++i) {
        printf("    *[%d] - %d\n", i, spu->ret_stk.data[i]);
    }
    printf("------------------------------------------------------\n");

    printf("PC: %u\n", spu->pc);

    printf("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&\n\n");
}

void PrintSPUErrors(int err_code)
{
    if (err_code & mask_err)
        PRINT_LOGS("This byte code is not for this processor.");

    if (err_code & version_err)
        PRINT_LOGS("This processor version is outdated, please recompile it.");

    if (err_code & code_arr_size_err)
        PRINT_LOGS("Invalid array size with codes");

    if (err_code & pc_err)
        PRINT_LOGS("Invalid current command index");

    if (err_code & code_arr_ptr_err)
        PRINT_LOGS("Null pointer to an array of commands");

}




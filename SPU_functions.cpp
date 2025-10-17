#include <TXLib.h>
#include <windows.h>

#include "common_functions.h"
#include "dump_functions.h"
#include "stack_functions.h"
#include "SPU_functions.h"

extern const char* log_file_name;

extern FILE* log_file;

extern SPUStructCmd all_cmd_funcs[NUM_OF_CMDS];

int SPUCtor(SPU* spu)
{
    assert(spu != NULL);

    StackCtor(&spu->stk, CAPACITY);
    StackCtor(&spu->ret_stk, CAPACITY);

    spu->RAM = (int* )calloc(RAM_SIDE_LEN * RAM_SIDE_LEN,sizeof(int));

    spu->pc = 0;
    spu->buffer.size = 0;

    return 0;
}

int SPUVerifier(SPU* spu)
{
    assert(spu != NULL);

    //printf("%d\n", spu->buffer.code_arr[0]);

    if (spu->buffer.code_arr[-2] != SIGNATURE) {
        PRINT_LOGS("This byte code is not for this processor.");
        spu->err_code |= mask_err;
        return mask_err;
    }


    if (spu->buffer.code_arr[-1] != VERSION) {
        PRINT_LOGS("This processor version is outdated, please recompile it.");
        spu->err_code |= version_err;
        return version_err;
    }

    if (spu->buffer.size >= CAPACITY) {
        spu->err_code |= code_arr_size_err;
        return code_arr_size_err;
    }

    if (spu->pc >= CAPACITY) {
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
    if (spu == NULL){
        PRINT_LOGS("Buffer have NULL ptr");
        return 1;
    }

    FILE* bin_file = fopen("binfile.bin", "rb");

    if (bin_file == NULL) {
        PRINT_LOGS("The bin file did not open");
        return 1;
    }

    int* cmd_buffer = (int*)calloc(CAPACITY, sizeof(int));

    assert(cmd_buffer != NULL);

    spu->buffer.size = fread(cmd_buffer, sizeof(int), CAPACITY, bin_file);

    spu->buffer.size -= HEADER_OFFSET;

    spu->buffer.code_arr = cmd_buffer + HEADER_OFFSET;

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

        if (spu->buffer.code_arr[spu->pc] == cmdHLT){
            //SPUdump(spu);
            return 0;
        }

        bool check_correct_cmd = false;

        for (int index = 0; index < NUM_OF_CMDS; ++index) {
            if (all_cmd_funcs[index].cmd == spu->buffer.code_arr[spu->pc]) {

                all_cmd_funcs[index].cmd_function(spu);
                check_correct_cmd = true;
                break;
            }
        }

        if (check_correct_cmd == false) {
            PRINT_LOGS("Invalid command");
            return 1;
        }

        SPUVerifier(spu);
    }

    return 0;
}

void PopReg(struct SPU* spu)
{
    assert(spu != NULL);

    StackPop(&spu->stk, &spu->regs[spu->buffer.code_arr[++spu->pc]]);
}

void PushReg(struct SPU* spu)
{
    assert(spu != NULL);

    StackPush(&spu->stk, spu->regs[spu->buffer.code_arr[++spu->pc]]);
}

void PopM(struct SPU* spu)
{
    assert(spu != NULL);

    int last_el = 0;

    StackPop(&spu->stk, &last_el);

    spu->RAM[spu->regs[spu->buffer.code_arr[++spu->pc]]] = last_el;
}

void PushM(struct SPU* spu)
{
    assert(spu != NULL);

    StackPush(&spu->stk, spu->RAM[spu->buffer.code_arr[++spu->pc]]);
}

void Push(struct SPU* spu)
{
    assert(spu != NULL);

    int arg = spu->buffer.code_arr[++spu->pc];

    StackPush(&spu->stk, arg);
}

void Jmp(struct SPU* spu)
{
    assert(spu != NULL);

    int arg = spu->buffer.code_arr[++spu->pc];

    spu->pc = arg - 1;
}

void Jb(struct SPU* spu)
{
    assert(spu != NULL);

    int arg = spu->buffer.code_arr[++spu->pc];

    GET_TWO_ELEM(&spu->stk);

    if (elem2 < elem1)
        spu->pc = arg - 1;
}

void Jbe(struct SPU* spu)
{
    assert(spu != NULL);

    int arg = spu->buffer.code_arr[++spu->pc];

    GET_TWO_ELEM(&spu->stk);

    if (elem2 <= elem1)
        spu->pc = arg - 1;
}

void Ja(struct SPU* spu)
{
    assert(spu != NULL);

    int arg = spu->buffer.code_arr[++spu->pc];

    GET_TWO_ELEM(&spu->stk);

    if (elem2 > elem1)
        spu->pc =  arg - 1;
}

void Jae(struct SPU* spu)
{
    assert(spu != NULL);

    int arg = spu->buffer.code_arr[++spu->pc];

    GET_TWO_ELEM(&spu->stk);

    if (elem2 >= elem1)
        spu->pc =  arg - 1;
}

void Je(struct SPU* spu)
{
    assert(spu != NULL);

    int arg = spu->buffer.code_arr[++spu->pc];

    GET_TWO_ELEM(&spu->stk);

    if (elem2 == elem1)
        spu->pc =  arg - 1;
}

void Jne(struct SPU* spu)
{
    assert(spu != NULL);

    int arg = spu->buffer.code_arr[++spu->pc];

    GET_TWO_ELEM(&spu->stk);

    if (elem2 != elem1)
        spu->pc =  arg - 1;
}

void Call(struct SPU* spu)
{
    assert(spu != NULL);

    int arg = spu->buffer.code_arr[++spu->pc];

    StackPush(&spu->ret_stk, spu->pc);

    spu->pc =  arg - 1;
}

void Ret(struct SPU* spu)
{
    assert(spu != NULL);

    int last_ret = 0;

    StackPop(&spu->ret_stk, &last_ret);

    spu->pc = last_ret;
}

void Draw(struct SPU* spu)
{
    assert(spu != NULL);

    system("cls");

    printf("RAM: \n");
    for (int i = 0; i < RAM_SIDE_LEN * RAM_SIDE_LEN; ++i) {
        printf("%c", spu->RAM[i] + 95);
        if ((i + 1) % RAM_SIDE_LEN == 0) printf("\n");
    }
}

void In(struct SPU* spu)
{
    assert(spu != NULL);

    int value = 0;
    printf("Enter number: ");
    while (scanf("%d", &value) != 1) {
        SkipLine();
        printf("\nWrong input\n");
        printf("Enter number: ");
    }

    StackPush(&spu->stk, value);
}

void Add(struct SPU* spu)
{
    assert(spu != NULL);

    GET_TWO_ELEM(&spu->stk);

    StackPush(&spu->stk, elem1 + elem2);
}

void Sqvrt(struct SPU* spu)
{
    assert(spu != NULL);

    StackValueType elem = 0;

    StackPop(&spu->stk, &elem);

    StackPush(&spu->stk,(int)sqrt(elem));
}

void Sub(struct SPU* spu)
{
    assert(spu != NULL);

    GET_TWO_ELEM(&spu->stk);

    StackPush(&spu->stk, elem2 - elem1);
}

void Mul(struct SPU* spu)
{
    assert(spu != NULL);

    GET_TWO_ELEM(&spu->stk);

    StackPush(&spu->stk, elem1 * elem2);
}

void Div(struct SPU* spu)
{
    assert(spu != NULL);

    GET_TWO_ELEM(&spu->stk);

    if (elem1 == 0){
        PRINT_LOGS("You can`t divide by zero");
    }
    else
        StackPush(&spu->stk, elem2 / elem1);
}

void Out(struct SPU* spu)
{
    assert(spu != NULL);

    StackValueType elem = 0;

    StackPop(&spu->stk, &elem);

    printf("Answer is: %d\n", elem);
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

void SkipLine(void)
{
    int ch = 0;
    while ((ch = getchar()) != '\n')
        continue;
}




#include <TXLib.h>

#include "common_functions.h"
#include "dump_functions.h"
#include "stack_functions.h"
#include "SPU_functions.h"

extern const char* log_file_name;

extern FILE* log_file;

int SPUCtor(SPU* spu)
{
    assert(spu != NULL);

    StackCtor(&spu->stk, CAPACITY);
    StackCtor(&spu->ret_stk, CAPACITY);

    spu->pc = 0;
    spu->buffer.size = 0;

    return 0;
}

int SPUVerifier(SPU* spu)
{
    assert(spu != NULL);

    //printf("%d\n", spu->buffer.code_arr[0]);

    if (spu->buffer.code_arr[-2] != MASK) {
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
    free(spu->buffer.code_arr - TITLEOFFSET);
    StackDtor(&spu->stk);
    StackDtor(&spu->ret_stk);
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

    spu->buffer.size -= TITLEOFFSET;

    spu->buffer.code_arr = cmd_buffer + TITLEOFFSET;

    fclose(bin_file);

    return 0;
}

int SPURunCmdFromBuffer(struct SPU* spu)
{
    for (; spu->pc < spu->buffer.size; ++(spu->pc)) {

         SPUdump(spu);
         getchar();

        switch(spu->buffer.code_arr[spu->pc])
        {
            case cmdHLT: return 0;

            case cmdSQVRT: Sqvrt(&spu->stk);
                           break;

            case cmdADD: Add(&spu->stk);
                         break;

            case cmdSUB: Sub(&spu->stk);
                         break;

            case cmdDIV: Div(&spu->stk);
                         break;

            case cmdOUT: Out(&spu->stk);
                         break;

            case cmdMUL: Mul(&spu->stk);
                         break;

            case cmdIN: In(&spu->stk);
                        break;

            case cmdCALL: Call(spu, spu->buffer.code_arr[++spu->pc]);
                          break;

            case cmdRET: Ret(spu);
                         break;

            case cmdPUSH: Push(&spu->stk, spu->buffer.code_arr[++spu->pc]);
                          break;

            case cmdJMP: Jmp(spu, spu->buffer.code_arr[++spu->pc]);
                         break;

            case cmdJB: Jb(spu, spu->buffer.code_arr[++spu->pc]);
                        break;

            case cmdJBE: Jbe(spu, spu->buffer.code_arr[++spu->pc]);
                        break;

            case cmdJA: Ja(spu, spu->buffer.code_arr[++spu->pc]);
                        break;

            case cmdJAE: Jae(spu, spu->buffer.code_arr[++spu->pc]);
                        break;

            case cmdJE: Je(spu, spu->buffer.code_arr[++spu->pc]);
                        break;

            case cmdJNE: Jne(spu, spu->buffer.code_arr[++spu->pc]);
                        break;

            case cmdPOPREG: PopReg(spu, spu->buffer.code_arr[++spu->pc]);
                            break;

            case cmdPUSHREG: PushReg(spu, spu->buffer.code_arr[++spu->pc]);
                             break;

            default: PRINT_LOGS("Invalid command");
                     return 1;
        }
    }
    return 0;
}

void PopReg(struct SPU* spu, int reg_index)
{
    assert(spu != NULL);

    StackPop(&spu->stk, &spu->regs[reg_index]);
}

void PushReg(struct SPU* spu, int reg_index)
{
    assert(spu != NULL);

    StackPush(&spu->stk, spu->regs[reg_index]);
}

void Push(Stack* stk, int arg)
{
    assert(stk != NULL);

    StackPush(stk, arg);
}

void Jmp(struct SPU* spu, int arg)
{
    assert(spu != NULL);

    spu->pc = arg - 1;
}

void Jb(struct SPU* spu, int arg)
{
    assert(spu != NULL);

    GET_TWO_ELEM(&spu->stk);

    if (elem2 < elem1)
        spu->pc = arg - 1;
}

void Jbe(struct SPU* spu, int arg)
{
    assert(spu != NULL);

    GET_TWO_ELEM(&spu->stk);

    if (elem2 <= elem1)
        spu->pc = arg - 1;
}

void Ja(struct SPU* spu, int arg)
{
    assert(spu != NULL);

    GET_TWO_ELEM(&spu->stk);

    if (elem2 > elem1)
        spu->pc =  arg - 1;
}

void Jae(struct SPU* spu, int arg)
{
    assert(spu != NULL);

    GET_TWO_ELEM(&spu->stk);

    if (elem2 >= elem1)
        spu->pc =  arg - 1;
}

void Je(struct SPU* spu, int arg)
{
    assert(spu != NULL);

    GET_TWO_ELEM(&spu->stk);

    if (elem2 == elem1)
        spu->pc =  arg - 1;
}

void Jne(struct SPU* spu, int arg)
{
    assert(spu != NULL);

    GET_TWO_ELEM(&spu->stk);

    if (elem2 != elem1)
        spu->pc =  arg - 1;
}

void Call(struct SPU* spu, int arg)
{
    assert(spu != NULL);

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

void In(struct Stack* stk)
{
    assert(stk != NULL);

    int value = 0;
    printf("Enter number: ");
    while (scanf("%d", &value) != 1) {
        SkipLine();
        printf("\nWrong input\n");
        printf("Enter number: ");
    }

    StackPush(stk, value);
}

void Add(Stack* stk)
{
    assert(stk != NULL);

    GET_TWO_ELEM(stk);

    StackPush(stk, elem1 + elem2);
}

void Sqvrt(Stack* stk)
{
    assert(stk != NULL);

    StackValueType elem = 0;

    StackPop(stk, &elem);

    StackPush(stk,(int)sqrt(elem));
}

void Sub(Stack* stk)
{
    assert(stk != NULL);

    GET_TWO_ELEM(stk);

    StackPush(stk, elem2 - elem1);
}

void Mul(Stack* stk)
{
    assert(stk != NULL);

    GET_TWO_ELEM(stk);

    StackPush(stk, elem1 * elem2);
}

void Div(Stack* stk)
{
    assert(stk != NULL);

    GET_TWO_ELEM(stk);

    if (elem1 == 0){
        PRINT_LOGS("You can`t divide by zero");
    }
    else
        StackPush(stk, elem2 / elem1);
}

void Out(Stack* stk)
{
    assert(stk != NULL);

    StackValueType elem = 0;

    StackPop(stk, &elem);

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
    for (size_t i = 0; i < NUMBEROFREGS; ++i, reg_name++) {
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




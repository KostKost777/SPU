#include <TXLib.h>

#include "common_functions.h"
#include "dump_functions.h"
#include "stack_functions.h"
#include "SPU_functions.h"

extern const char* log_file_name;

extern FILE* log_file;

int SPUCtor(SPU* spu)
{
    StackCtor(&spu->stk, CAPACITY);
    spu->cp = CODEARROFFSET;
    return 0;
}

int SPUVerifier(SPU* spu)
{
    assert(spu != NULL);

    if (spu->buffer.code_arr[0] != MASK) {
        PRINT_LOGS("This byte code is not for this processor.");
        return 1;
    }

    if (spu->buffer.code_arr[1] != VERSION) {
        PRINT_LOGS("This processor version is outdated, please recompile it.");
        return 1;
    }

    return 0;
}

void SPUDtor(SPU* spu)
{
    StackDtor(&spu->stk);
    fclose(log_file);
}

int SPUReadCmdFromFile(struct Buffer* buffer)
{
    if (buffer == NULL){
        PRINT_LOGS("Buffer have NULL ptr");
        return 1;
    }

    if (buffer->code_arr == NULL){
        PRINT_LOGS("Array with commands have NULL ptr");
        return 1;
    }

    FILE* bin_file = fopen("binfile.txt", "r");

    if (bin_file == NULL) {
        PRINT_LOGS("The bin file did not open");
        return 1;
    }

    while (true) {

        if (buffer->size >= CAPACITY){
            PRINT_LOGS("Not enought memory in buffer, please increase capacity");
            fclose(bin_file);
            return 1;
        }

        //printf("%d\n", (*code_arr)[*size]);

        if (fscanf(bin_file, "%d",
                   &(buffer->code_arr[buffer->size])) == EOF)
            break;

        buffer->size++;
    }

    fclose(bin_file);
    return 0;
}

int SPURunCmdFromBuffer(struct SPU* spu)
{
    for (; spu->cp < spu->buffer.size; ++(spu->cp)) {

        // SPUdump(spu);
        // getchar();

        switch(spu->buffer.code_arr[spu->cp])
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

            case cmdPUSH: Push(&spu->stk, spu->buffer.code_arr[++spu->cp]);
                          break;

            case cmdJMP: Jmp(spu, spu->buffer.code_arr[++spu->cp]);
                         break;

            case cmdJB: Jb(spu, spu->buffer.code_arr[++spu->cp]);
                        break;

            case cmdJBE: Jbe(spu, spu->buffer.code_arr[++spu->cp]);
                        break;

            case cmdJA: Ja(spu, spu->buffer.code_arr[++spu->cp]);
                        break;

            case cmdJAE: Jae(spu, spu->buffer.code_arr[++spu->cp]);
                        break;

            case cmdJE: Je(spu, spu->buffer.code_arr[++spu->cp]);
                        break;

            case cmdJNE: Jne(spu, spu->buffer.code_arr[++spu->cp]);
                        break;

            case cmdPOPREG: PopReg(spu, spu->buffer.code_arr[++spu->cp]);
                            break;

            case cmdPUSHREG: PushReg(spu, spu->buffer.code_arr[++spu->cp]);
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

    spu->cp = CODEARROFFSET + arg - 1;
}

void Jb(struct SPU* spu, int arg)
{
    assert(spu != NULL);

    GET_TWO_ELEM(&spu->stk);

    if (elem2 < elem1)
        spu->cp = CODEARROFFSET + arg - 1;
}

void Jbe(struct SPU* spu, int arg)
{
    assert(spu != NULL);

    GET_TWO_ELEM(&spu->stk);

    if (elem2 <= elem1)
        spu->cp = CODEARROFFSET + arg - 1;
}

void Ja(struct SPU* spu, int arg)
{
    assert(spu != NULL);

    GET_TWO_ELEM(&spu->stk);

    if (elem2 > elem1)
        spu->cp = CODEARROFFSET + arg - 1;
}

void Jae(struct SPU* spu, int arg)
{
    assert(spu != NULL);

    GET_TWO_ELEM(&spu->stk);

    if (elem2 >= elem1)
        spu->cp = CODEARROFFSET + arg - 1;
}

void Je(struct SPU* spu, int arg)
{
    assert(spu != NULL);

    GET_TWO_ELEM(&spu->stk);

    if (elem2 == elem1)
        spu->cp = CODEARROFFSET + arg - 1;
}

void Jne(struct SPU* spu, int arg)
{
    assert(spu != NULL);

    GET_TWO_ELEM(&spu->stk);

    if (elem2 != elem1)
        spu->cp = CODEARROFFSET + arg - 1;
}

void In(struct Stack* stk)
{
    assert(stk != NULL);

    int value = 0;
    printf("Enter number: ");
    while (scanf("%d", &value) != 1) {
        SkipLine();
        printf("Wrong input\n");
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
    printf("--------------------------------------------------------\n");
    printf("STACK: \n");
    printf("SIZE: %d \n", spu->stk.size);
    for (int i = CANARY_CONST; i < spu->stk.size; ++i) {
        printf("    *[%d] - %d\n", i, spu->stk.data[i]);
    }

    printf("CODE_ARR: \n");

    for (size_t i = 0; i < spu->buffer.size; ++i) {
        if (i == spu->cp) {
            printf("(%d) ", spu->buffer.code_arr[i]);
            continue;
        }
        printf("%d ", spu->buffer.code_arr[i]);
    }

    printf("\n");

    printf("REGISTERS:\n");
    char reg_name = 'A';
    for (size_t i = 0; i < NUMBEROFREGS; ++i, reg_name++) {
        printf("[%cX]  -  [%d]\n", reg_name, spu->regs[i]);
    }

    printf("CP: %u\n", spu->cp);
}

void SkipLine(void)
{
    int ch = 0;
    while ((ch = getchar()) != '\n')
        continue;
}




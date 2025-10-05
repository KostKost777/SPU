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
    return 0;
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
            return 1;
        }

        //printf("%d\n", (*code_arr)[*size]);

        if (fscanf(bin_file, "%d",
                   &(buffer->code_arr[buffer->size])) == EOF)
            break;

        buffer->size++;
    }
    return 0;
}

int SPURunCmdFromBuffer(struct SPU* spu)
{
    for (size_t i = 0; i < spu->buffer.size; ++i){
        switch(spu->buffer.code_arr[i])
        {
            case cmdHLT: return 0;

            case cmdPUSH: Push(&spu->stk, spu->buffer.code_arr[++i]);
                          break;

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

            default: PRINT_LOGS("Invalid command");
                     return 1;
        }
    }
    return 0;
}

void Push(Stack* stk, int arg)
{
    StackPush(stk, arg);
}

void Add(Stack* stk)
{
    GET_TWO_ELEM(stk);

    StackPush(stk, elem1 + elem2);
}

void Sqvrt(Stack* stk)
{
    StackValueType elem = 0;

    StackPop(stk, &elem);

    StackPush(stk,(int)sqrt(elem));
}

void Sub(Stack* stk)
{
    GET_TWO_ELEM(stk);

    StackPush(stk, elem2 - elem1);
}

void Mul(Stack* stk)
{
    GET_TWO_ELEM(stk);

    StackPush(stk, elem1 * elem2);
}

void Div(Stack* stk)
{

    GET_TWO_ELEM(stk);

    if (elem1 == 0){
        PRINT_LOGS("You can`t divide by zero");
    }
    else
        StackPush(stk, elem2 / elem1);
}

void Out(Stack* stk)
{
    StackValueType elem = 0;

    StackPop(stk, &elem);

    printf("Answer is: %d", elem);
}




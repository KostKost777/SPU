#ifndef SPU
#define SPU

int SPUReadCmdFromFile(int* code_arr, const size_t CAPACITY,
                                      size_t* last_index);

int SPUReadCmdFromBinFile(int* code_arr, const size_t CAPACITY,
                                         size_t* last_index);

int SPURunCmdFromBuffer(int* code_arr, int last_index);

void Push(Stack* stk, int arg);

void Add(Stack* stk);

void Sub(Stack* stk);

void Mul(Stack* stk);

void Div(Stack* stk);

void Out(Stack* stk);

void Sqvrt(Stack* stk);

#define GET_TWO_ELEM(stk)           \
    StackValueType elem1 = 0;       \
    StackValueType elem2 = 0;       \
                                    \
    StackPop(stk, &elem1);          \
    StackPop(stk, &elem2);          \

#endif

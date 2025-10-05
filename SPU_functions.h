#ifndef SPU_FUNCS
#define SPU_FUNCS

struct SPU
{
    size_t cp;
    struct Stack stk;
    struct Buffer buffer;
    int regs[NUMBEROFREGS];
};

int SPUCtor(struct SPU* spu);

int SPUReadCmdFromFile(struct Buffer* buffer);

int SPURunCmdFromBuffer(struct SPU* spu);

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

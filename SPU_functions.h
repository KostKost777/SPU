#ifndef SPU_FUNCS
#define SPU_FUNCS

const int CODEARROFFSET = 2;

struct SPU
{
    size_t cp;
    struct Stack stk;
    struct Buffer buffer;
    int regs[NUMBEROFREGS];
};

void SkipLine(void);

void In(struct Stack* stk);

void SPUDtor(SPU* spu);

void SPUdump(struct SPU* spu);

int SPUCtor(struct SPU* spu);

int SPUVerifier(SPU* spu);

int SPUReadCmdFromFile(struct Buffer* buffer);

int SPURunCmdFromBuffer(struct SPU* spu);

void Push(Stack* stk, int arg);

void Jmp(struct SPU* spu, int arg);

void Jb(struct SPU* spu, int arg);

void Jbe(struct SPU* spu, int arg);

void Ja(struct SPU* spu, int arg);

void Jae(struct SPU* spu, int arg);

void Je(struct SPU* spu, int arg);

void Jne(struct SPU* spu, int arg);

void PopReg(struct SPU* spu, int reg_index);

void PushReg(struct SPU* spu, int reg_index);

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

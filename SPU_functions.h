#ifndef SPU_FUNCS
#define SPU_FUNCS

const int CODEARROFFSET = 2;

struct SPU
{
    int pc;
    struct Stack stk;
    struct Buffer buffer;
    int regs[NUMBEROFREGS];
    int err_code;
};

enum SPUErr
{
    none_err = 0,
    mask_err = 1,
    version_err = 2,
    code_arr_size_err = 4,
    code_arr_ptr_err = 8,
    pc_err = 16
};

void SkipLine(void);

void PrintSPUErrors(int err_code);

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

#ifndef SPU_FUNCS
#define SPU_FUNCS

const int RAM_CAPACITY = 2500;

struct SPU
{
    size_t pc;
    struct Stack stk;
    struct Stack ret_stk;
    struct Buffer buffer;
    int regs[NUMBER_OF_REGS];
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

typedef void (*func_ptr_t)(struct SPU* spu);

struct SPUStructCmd
{
    int cmd;
    func_ptr_t cmd_function;
};

void SkipLine(void);

void PrintSPUErrors(int err_code);

void In(struct SPU* spu);

void SPUDtor(SPU* spu);

void SPUdump(struct SPU* spu);

int SPUCtor(struct SPU* spu);

int SPUVerifier(SPU* spu);

int SPUReadCmdFromFile(struct SPU* spu);

int SPURunCmdFromBuffer(struct SPU* spu);

void Push(struct SPU* spu);

void Jmp(struct SPU* spu);

void Jb(struct SPU* spu);

void Jbe(struct SPU* spu);

void Ja(struct SPU* spu);

void Jae(struct SPU* spu);

void Je(struct SPU* spu);

void Jne(struct SPU* spu);

void Call(struct SPU* spu);

void Ret(struct SPU* spu);

void PopReg(struct SPU* spu);

void PushReg(struct SPU* spu);

void PopM(struct SPU* spu);

void PushM(struct SPU* spu);

void Add(struct SPU* spu);

void Sub(struct SPU* spu);

void Mul(struct SPU* spu);

void Div(struct SPU* spu);

void Out(struct SPU* spu);

void Sqvrt(struct SPU* spu);

#define GET_TWO_ELEM(stk)           \
    StackValueType elem1 = 0;       \
    StackValueType elem2 = 0;       \
                                    \
    StackPop(stk, &elem1);          \
    StackPop(stk, &elem2);          \

#endif

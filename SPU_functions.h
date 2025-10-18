#ifndef SPU_FUNCS
#define SPU_FUNCS

const int RAM_SIDE_LEN = 50;

struct SPU
{
    size_t pc;
    int* RAM;
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

void PrintSPUErrors(int err_code);

void SPUDtor(SPU* spu);

void SPUdump(struct SPU* spu);

int SPUCtor(struct SPU* spu);

int SPUVerifier(struct SPU* spu);

void SetDefaultRAMValue(int* RAM, char value);

int SPUReadCmdFromFile(struct SPU* spu);

int SPURunCmdFromBuffer(struct SPU* spu);

void Push(struct SPU* spu, int cmd);

void Jmp(struct SPU* spu, int cmd);

void ConditionalJumps(struct SPU* spu, int cmd);

void In(struct SPU* spu, int cmd);

void Draw(struct SPU* spu, int cmd);

void Hlt(struct SPU* spu, int cmd);

void Call(struct SPU* spu, int cmd);

void Ret(struct SPU* spu, int cmd);

void RegFuncs(struct SPU* spu, int cmd);

void MemFuncs(struct SPU* spu, int cmd);

void BinaryArifmeticFuncs(struct SPU* spu, int cmd);

void Out(struct SPU* spu, int cmd);

void Sqvrt(struct SPU* spu, int cmd);

#define GET_TWO_ELEM(stk)           \
    StackValueType elem1 = 0;       \
    StackValueType elem2 = 0;       \
                                    \
    StackPop(stk, &elem1);          \
    StackPop(stk, &elem2);          \

#endif

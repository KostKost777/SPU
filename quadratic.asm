IN
POPREG AX
IN
POPREG BX
IN
POPREG CX
PUSHREG AX
PUSH 0

JE :1
    PUSH 0          ;если А != 0
    PUSHREG BX

JE :2
    PUSHREG CX      ;если В != 0 и А != 0
    PUSH 0

JE :3
    PUSHREG BX      ;если С != 0 и В != 0 и А != 0
    PUSHREG BX
    MUL
    PUSHREG AX
    PUSHREG CX
    PUSH 4
    MUL
    MUL
    SUB
    POPREG DX
    PUSHREG DX
    PUSH 0

JB :4
    PUSHREG DX          ;дискр >= 0
    PUSH 0

JE :5
    PUSHREG BX          ;дискр > 0
    PUSH -1
    MUL
    PUSHREG DX
    SQVRT
    ADD
    PUSH 2
    DIV
    PUSHREG AX
    DIV
    POPREG EX
    PUSHREG BX
    PUSH -1
    MUL
    PUSHREG DX
    SQVRT
    SUB
    PUSH 2
    DIV
    PUSHREG AX
    DIV
    POPREG FX
    PUSH 2
    POPREG GX
    PUSHREG GX
    OUT
    PUSHREG EX
    OUT
    PUSHREG FX
    OUT
    HLT

:5              ;дискр == 0
    PUSHREG BX
    PUSHREG AX
    DIV
    PUSH -1
    MUL
    PUSH 2
    DIV
    POPREG EX
    PUSH 1
    POPREG GX
    PUSHREG GX
    OUT
    PUSHREG EX
    OUT
    HLT

:4               ;дискр < 0
    PUSH 0
    OUT
    HLT

:3               ;если С == 0 и В != 0 и А != 0
    PUSH 0
    POPREG EX
    PUSH 2
    POPREG GX
    PUSHREG BX
    PUSHREG AX
    DIV
    PUSH -1
    MUL
    POPREG FX
    PUSHREG GX
    OUT
    PUSHREG EX
    OUT
    PUSHREG FX
    OUT
    HLT

:1                      ;если А == 0
    PUSHREG BX
    PUSH 0

    JE :7
    PUSHREG CX          ;если А == 0 и В != 0
    PUSH 0

    JE :6
    PUSHREG CX       ;если А == 0 и В != 0 и С != 0
    PUSH -1
    MUL
    PUSHREG BX
    DIV
    POPREG EX
    PUSH 1
    POPREG GX
    PUSHREG GX
    OUT
    PUSHREG EX
    OUT
    HLT

:2                          ;если А != 0 и В == 0
    PUSHREG CX
    PUSH 0

    JE :6
    PUSHREG CX      ;если А != 0 и В == 0 и С != 0
    PUSHREG AX
    DIV
    PUSH -1
    MUL
    POPREG DX
    PUSHREG DX
    PUSH 0

    JB :9
    PUSHREG DX     ;если X^2 = -C / A имеет решения
    SQVRT
    POPREG EX
    PUSHREG EX
    PUSH -1
    MUL
    POPREG FX
    PUSHREG FX
    PUSHREG EX
    PUSH 2
    POPREG GX
    PUSHREG GX
    OUT
    PUSHREG EX
    OUT
    PUSHREG FX
    OUT
    HLT

:7              ;если А == 0 и В == 0
    PUSHREG CX
    PUSH 0
    JE :8
    PUSH 0        ;если А == 0 и В == 0 и С != 0
    OUT
    HLT

:6          ;если А == 0 и В != 0 и С == 0
    PUSH 1
    OUT
    PUSH 0
    OUT
    HLT

:8         ;если А == 0 и В == 0 и С == 0
    PUSH -1
    OUT
    HLT

:9         ;если X^2 = -C / A не имеет решений
    PUSH 0
    OUT
    HLT


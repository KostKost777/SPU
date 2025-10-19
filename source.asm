PUSH 50
POPREG DX                       ;в DX храниться размер стороны квадрата
PUSH -1
POPREG AX
PUSH 11
POPREG GX                       ;РАДИУС КРУГА
CALL :CountRadius               ;СЧИТАЕМ КООРДИНАТЫ ЦЕНТРА КРУГА

:While_Ax_<_100

    PUSHREG AX
    PUSH 1
    ADD
    POPREG AX
    CALL :CheckDot                  ;ПРОВЕРКА ПРИНАДЛЕЖИТ ЛИ ТОЧКА ОКРУЖНОСТИ
    PUSH 1
    JNE :5
        CALL :GetDotCoord            ;РАССЧЕТ ПОЗИЦИИ КУДА НAДО ПОСТАВИТЬ ТОЧКУ
        PUSH 46
        POPM [HX]
    :5                               ;ЕСЛИ ТОЧКУ НЕ НАДО ПЕЧАТАТЬ
    CALL :SetNextDot                 ;ОБНОВЛЕНИЕ ЗНАЧЕНИЙ CX И BX
    PUSHREG AX
    PUSHREG DX
    PUSHREG DX
    MUL

JB :While_Ax_<_100
DRAW
HLT                             ;КОНЕЦ

:CheckDot                       ;ПРОВЕРКА ПРИНАДЛЕЖИТ ЛИ ТОЧКА ОКРУЖНОСТИ
    PUSHREG BX
    PUSHREG EX
    SUB
    PUSHREG BX
    PUSHREG EX
    SUB
    MUL
    PUSHREG CX
    PUSH 2
    MUL
    PUSHREG FX
    SUB
    PUSHREG CX
    PUSH 2
    MUL
    PUSHREG FX
    SUB
    MUL
    ADD
    PUSHREG GX
    PUSHREG GX
    MUL
    JBE :4
    PUSH 0
    RET

:4
    PUSH 1
    RET

:CountRadius                ; СЧИТАЕМ КООРДИНАТЫ ЦЕНТРА КРУГА
    PUSHREG DX
    PUSH 2
    DIV
    POPREG EX
    PUSHREG DX
    PUSH 2
    DIV
    POPREG FX
    RET

:GetDotCoord                ;РАССЧЕТ ПОЗИЦИИ КУДА НАДО ПОСТАВИТЬ ТОЧКУ
    PUSHREG CX
    PUSHREG DX
    MUL
    PUSHREG BX
    ADD
    POPREG HX
    RET

:SetNextDot
    PUSHREG BX
    PUSH 1
    ADD
    POPREG BX
    CALL :Check_BX             ;ПРОВЕРКА ЧТО BX МЕНЬШЕ 10
    RET

:Check_BX
    PUSHREG BX
    PUSHREG DX
    JE :SetNew_BX_CX
    RET

:SetNew_BX_CX
    PUSH 0
    POPREG BX
    PUSH 1
    PUSHREG CX
    ADD
    POPREG CX
    RET

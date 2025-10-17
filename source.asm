PUSH 50
POPREG DX     ;в DX храниться размер стороны квадрата
PUSH -1
POPREG AX
PUSH 10
POPREG GX    ;РАДИУС КРУГА
CALL :2       ;СЧИТАЕМ КООРДИНАТЫ ЦЕНТРА КРУГА
:1
PUSHREG AX
PUSH 1
ADD
POPREG AX
CALL :3    ;ПРОВЕРКА ПРИНАДЛЕЖИТ ЛИ ТОЧКА ОКРУЖНОСТИ
PUSH 1
JNE :5
CALL :6    ;РАССЧЕТ ПОЗИЦИИ КУДА НAДО ПОСТАВИТЬ ТОЧКУ
PUSH 11
POPM [HX]
:5              ;ЕСЛИ ТОЧКУ НЕ НАДО ПЕЧАТАТЬ
CALL :7            ;ОБНОВЛЕНИЕ ЗНАЧЕНИЙ CX И BX
PUSHREG AX
PUSHREG DX
PUSHREG DX
MUL
JB :1
HLT   ;КОНЕЦ
:3                  ;ПРОВЕРКА ПРИНАДЛЕЖИТ ЛИ ТОЧКА ОКРУЖНОСТИ
PUSHREG BX
PUSHREG EX
SUB
PUSHREG BX
PUSHREG EX
SUB
MUL
PUSHREG CX
PUSHREG FX
SUB
PUSHREG CX
PUSHREG FX
SUB
MUL
ADD
SQVRT
PUSHREG GX
JBE :4
PUSH 0
RET
:4
PUSH 1
RET
:2     ; СЧИТАЕМ КООРДИНАТЫ ЦЕНТРА КРУГА
PUSHREG DX
PUSH 2
DIV
POPREG EX
PUSHREG DX
PUSH 2
DIV
POPREG FX
RET
:6              ;РАССЧЕТ ПОЗИЦИИ КУДА НАДО ПОСТАВИТЬ ТОЧКУ
PUSHREG CX
PUSHREG DX
MUL
PUSHREG BX
ADD
POPREG HX
RET
:7
PUSHREG BX
PUSH 1
ADD
POPREG BX
CALL :8      ;ПРОВЕРКА ЧТО BX МЕНЬШЕ 10
RET
:8
PUSHREG BX
PUSHREG DX
JE :9
RET
:9
PUSH 0
POPREG BX
PUSH 1
PUSHREG CX
ADD
POPREG CX
RET

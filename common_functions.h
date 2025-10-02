#ifndef COMMON
#define COMMON

const int MAXCMD = 8;

enum CmdCodes
{
    cmdHLT = 0,
    cmdPUSH = 1,
    cmdADD = 2,
    cmdSUB = 3,
    cmdDIV = 4,
    cmdOUT = 5,
    cmdMUL = 6,
    cmdSQVRT = 7
};

#endif

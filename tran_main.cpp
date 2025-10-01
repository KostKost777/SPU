#include<TXLib.h>

#include "translator_functions.h"
#include "dump_functions.h"
#include "stack_functions.h"

const char* source_file_name = "source.asm";
const char* log_file_name = "logfile.txt";
const char* bin_file_name = "binfile.txt";

FILE* source_file = fopen(source_file_name, "r");
FILE* log_file = fopen(log_file_name, "w");
FILE* bin_file = fopen(bin_file_name, "w");

int main()
{
    size_t capacity = 8;
    const int MAXLINELEN = 10;

    int* code_arr = (int*)calloc(capacity, sizeof(int));

    if (code_arr == NULL) {
        PRINT_LOGS("Invalid command");
        EndProcessing(&code_arr);
        return 0;
    }

    char cmdStr[MAXLINELEN] = "";
    int arg = 0;
    size_t last_index = 0;
    size_t line_now = 1;

    while (true) {

        fscanf(source_file, "%s", cmdStr);

        //printf("%s\n", cmdStr);

        if (last_index >= capacity)
            if(SetNewArrSize(&code_arr, &capacity) == 1){
                EndProcessing(&code_arr);
                return 0;
            }

        if (strcmp(cmdStr, "PUSH") == 0) {
            code_arr[last_index++] = cmdPUSH;

            fscanf(source_file, "%d", &arg);
            code_arr[last_index++] = arg;
        }

        else if (strcmp(cmdStr, "ADD") == 0)
            code_arr[last_index++] = cmdADD;

        else if (strcmp(cmdStr, "SUB") == 0)
            code_arr[last_index++] = cmdSUB;

        else if (strcmp(cmdStr, "OUT") == 0)
            code_arr[last_index++] = cmdOUT;

        else if (strcmp(cmdStr, "DIV") == 0)
            code_arr[last_index++] = cmdDIV;

        else if (strcmp(cmdStr, "HLT") == 0){
            code_arr[last_index++] = cmdHLT;
            break;
        }

        else{
            PrintTranslatorLogs("Invalid command", line_now);
            EndProcessing(&code_arr);

            return 0;
        }

        line_now++;
    }

    //printf("%d", capacity);

    //printf("%u\n", last_index);

    WriteCmdInFile(code_arr, last_index);

    EndProcessing(&code_arr);

    printf("OK\n");
}

#include <TXLIB.h>

#include "translator_functions.h"
#include "dump_functions.h"

extern FILE* log_file;

extern const char* log_file_name;

int TranPrintLogs(const char* message,
                  size_t line, const char* source_file_name)
{

    if (log_file == NULL) {
        PRINT_LOGS("The log file did not open");
        return 1;
    }

    if (message == NULL) {
        PRINT_LOGS("NULL pointer to messages");
        return 1;
    }

    if (source_file_name == NULL) {
        PRINT_LOGS("NULL pointer to source file name");
        return 1;
    }

    fprintf(log_file, "ERROR: %s in %s:%u\n", message,
                                              source_file_name,
                                              line);
    fflush(log_file);

    return 0;
}

int TranWriteCmdInFile(int* code_arr, size_t last_index)
{
    if (code_arr == NULL){
        PRINT_LOGS("Buffer have NULL ptr");
        return 1;
    }

    FILE* bin_file = fopen("binfile.txt", "w");

    if (bin_file == NULL) {
        PRINT_LOGS("The bin file did not open");
        return 1;
    }

    for (size_t i = 0; i < last_index; ++i) {

        if (code_arr[i] == cmdPUSH) {

            if (i + 1 >= last_index) {
                PRINT_LOGS("Incorrect syntax of the PUSH operation");
                return 1;
            }

            fprintf(bin_file, "%d %d\n", code_arr[i], code_arr[i + 1]);
            i++;
        }
        else
            fprintf(bin_file, "%d\n", code_arr[i]);
    }

    fclose(bin_file);

    return 0;
}

void TranEndProcessing()
{
    fclose(log_file);
}

int TranReadCmdFromFile(int* code_arr,
                        const size_t CAPASITY,
                        size_t* last_index)
{
    const char* source_file_name = "source.asm";

    if (code_arr == NULL){
        PRINT_LOGS("Buffer have NULL ptr");
        return 1;
    }

    FILE* source_file = fopen(source_file_name, "r");

    if (source_file == NULL) {
        PRINT_LOGS("The source file did not open");
        return 1;
    }

    const int MAXLINELEN = 10;
    char cmdStr[MAXLINELEN] = "";
    int arg = 0;
    size_t line_now = 1;

    while (true) {

        if (fscanf(source_file, "%s", cmdStr) == EOF)
            break;

        if (*last_index >= CAPASITY){
            PRINT_LOGS("Not enought memory in buffer, please increase capacity");
            return 1;
        }

        if (strcmp(cmdStr, "PUSH") == 0) {
            EmitInArr(code_arr, last_index, cmdPUSH);

            fscanf(source_file, "%d", &arg);
            EmitInArr(code_arr, last_index, arg);
        }

        else if (strcmp(cmdStr, "ADD") == 0)
            EmitInArr(code_arr, last_index, cmdADD);

        else if (strcmp(cmdStr, "SUB") == 0)
            EmitInArr(code_arr, last_index, cmdSUB);

        else if (strcmp(cmdStr, "OUT") == 0)
            EmitInArr(code_arr, last_index, cmdOUT);

        else if (strcmp(cmdStr, "DIV") == 0)
            EmitInArr(code_arr, last_index, cmdDIV);

        else if (strcmp(cmdStr, "HLT") == 0)
            EmitInArr(code_arr, last_index, cmdHLT);

        else{
            TranPrintLogs("Invalid command", line_now, source_file_name);
            return 1;
        }

        line_now++;
    }

    //printf("%d", capacity);

    //printf("%u\n", last_index);

    fclose(source_file);
    return 0;
}

int EmitInArr(int* code_arr, size_t* last_index, int value)
{
    if (code_arr == NULL){
        PRINT_LOGS("Buffer have NULL ptr");
        return 1;
    }

    if (last_index == NULL){
        PRINT_LOGS("Last index have NULL ptr");
        return 1;
    }

    code_arr[(*last_index)++] = value;
    return 0;
}






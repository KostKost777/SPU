#include <TXLIB.h>

#include "translator_functions.h"
#include "common_functions.h"
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

int TranWriteCmdInFile(struct Buffer* buffer)
{

    if (buffer == NULL){
        PRINT_LOGS("Buffer have NULL ptr");
        return 1;
    }

    if (buffer->code_arr == NULL){
        PRINT_LOGS("Array with commands have NULL ptr");
        return 1;
    }

    FILE* bin_file = fopen("binfile.txt", "w");

    if (bin_file == NULL) {
        PRINT_LOGS("The bin file did not open");
        return 1;
    }

    for (size_t i = 0; i < buffer->last_index; ++i) {
            fprintf(bin_file, "%d", buffer->code_arr[i]);
            if (i != buffer->last_index - 1)
                fprintf(bin_file, " ");
    }

    fclose(bin_file);

    return 0;
}

int TranWriteCmdInBinFile(struct Buffer* buffer)
{
    if (buffer == NULL){
        PRINT_LOGS("Buffer have NULL ptr");
        return 1;
    }

    if (buffer->code_arr == NULL){
        PRINT_LOGS("Array with commands have NULL ptr");
        return 1;
    }

    FILE* bin_file = fopen("binfile.bin", "wb");

    if (bin_file == NULL) {
        PRINT_LOGS("The bin file did not open");
        return 1;
    }

    fwrite(buffer->code_arr, sizeof(int), buffer->last_index, bin_file);

    fclose(bin_file);

    return 0;
}

void TranEndProcessing()
{
    fclose(log_file);
}

int TranReadCmdFromFile(struct Buffer* buffer)
{
    const char* source_file_name = "source.asm";

    if (buffer == NULL){
        PRINT_LOGS("Buffer have NULL ptr");
        return 1;
    }

    if (buffer->code_arr == NULL){
        PRINT_LOGS("Array with commands have NULL ptr");
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

        if (buffer->last_index >= CAPACITY){
            PRINT_LOGS("Not enought memory in buffer, please increase capacity");
            return 1;
        }

        if (strcmp(cmdStr, "PUSH") == 0) {
            EmitInArr(buffer, cmdPUSH);

            int status = fscanf(source_file, "%d", &arg);

            if (status == 0) {
                TranPrintLogs("Incorrect syntax of the PUSH operation",
                              line_now, source_file_name);
                return 1;
            }

            EmitInArr(buffer, arg);
        }

        else if (strcmp(cmdStr, "ADD") == 0)
            EmitInArr(buffer, cmdADD);

        else if (strcmp(cmdStr, "SQVRT") == 0)
            EmitInArr(buffer, cmdSQVRT);

        else if (strcmp(cmdStr, "SUB") == 0)
            EmitInArr(buffer, cmdSUB);

        else if (strcmp(cmdStr, "OUT") == 0)
            EmitInArr(buffer, cmdOUT);

        else if (strcmp(cmdStr, "DIV") == 0)
            EmitInArr(buffer, cmdDIV);

        else if (strcmp(cmdStr, "HLT") == 0)
            EmitInArr(buffer, cmdHLT);

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

int EmitInArr(struct Buffer* buffer, int value)
{
    if (buffer == NULL){
        PRINT_LOGS("Buffer have NULL ptr");
        return 1;
    }

    if (buffer->code_arr == NULL){
        PRINT_LOGS("Array with commands have NULL ptr");
        return 1;
    }

    buffer->code_arr[(buffer->last_index)++] = value;
    return 0;
}






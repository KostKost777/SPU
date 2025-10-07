#include <TXLIB.h>

#include "assembler_functions.h"
#include "common_functions.h"
#include "dump_functions.h"

extern FILE* log_file;

extern const char* log_file_name;

int AssemPrintLogs(const char* message,
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

int AssemWriteCmdInFile(struct Buffer* buffer)
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

    for (size_t i = 0; i < buffer->size; ++i)
            fprintf(bin_file, "%d ", buffer->code_arr[i]);

    fclose(bin_file);

    return 0;
}

int AssemWriteCmdInBinFile(struct Buffer* buffer)
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

    fwrite(buffer->code_arr, sizeof(int), buffer->size, bin_file);

    fclose(bin_file);

    return 0;
}

void AssemEndProcessing()
{
    fclose(log_file);
}

int AssemReadCmdFromFile(struct Buffer* buffer)
{

    if (buffer == NULL){
        PRINT_LOGS("Buffer have NULL ptr");
        return 1;
    }

    if (buffer->code_arr == NULL){
        PRINT_LOGS("Array with commands have NULL ptr");
        return 1;
    }

    const char* source_file_name = "source.asm";

    char str_buffer[CAPACITY];

    AssemReadInStringBuffer(str_buffer);

    char** str_ptr_arr = (char**)calloc(sizeof(char*), CAPACITY);

    size_t lines = 0;

    CopyFromBufferToStrPtrArr(str_buffer, str_ptr_arr, &lines);

    const int MAXLINELEN = 10;
    char cmdStr[MAXLINELEN] = "";
    int arg = 0;
    size_t source_file_line_now = 1;

    char reg_name = '\0';

    //printf("%d", lines);

    for (size_t i = 0; i < lines; ++i) {

        int status = sscanf(str_ptr_arr[i], "%s", cmdStr);

        //printf("%s\n", cmdStr);

        if (status == EOF)
            break;

        else if (status == 0) {
            AssemPrintLogs("Invalid command", source_file_line_now,
                                              source_file_name);
            return 1;
        }

        else if (status == 1) {
            if (strcmp(cmdStr, "ADD") == 0)
                EmitInArr(buffer, cmdADD);

            else if (strcmp(cmdStr, "SQVRT") == 0)
                EmitInArr(buffer, cmdSQVRT);

            else if (strcmp(cmdStr, "SUB") == 0)
                EmitInArr(buffer, cmdSUB);

            else if (strcmp(cmdStr, "OUT") == 0)
                EmitInArr(buffer, cmdOUT);

            else if (strcmp(cmdStr, "MUL") == 0)
                EmitInArr(buffer, cmdMUL);

            else if (strcmp(cmdStr, "DIV") == 0)
                EmitInArr(buffer, cmdDIV);

            else if (strcmp(cmdStr, "HLT") == 0)
                EmitInArr(buffer, cmdHLT);

            else if (strcmp(cmdStr, "PUSH") == 0 &&
                     sscanf(str_ptr_arr[i], "%*s %d", &arg) == 1) {

                    EmitInArr(buffer, cmdPUSH);
                    EmitInArr(buffer, arg);
            }

            else if (strcmp(cmdStr, "JMP") == 0 &&
                     sscanf(str_ptr_arr[i], "%*s %d", &arg) == 1) {

                EmitInArr(buffer, cmdJMP);
                EmitInArr(buffer, arg);
            }

            else if (strcmp(cmdStr, "JB") == 0 &&
                     sscanf(str_ptr_arr[i], "%*s %d", &arg) == 1) {

                EmitInArr(buffer, cmdJB);
                EmitInArr(buffer, arg);
            }

            else if (strcmp(cmdStr, "PUSHREG") == 0 &&
                     sscanf(str_ptr_arr[i], "%*s %c", &reg_name) == 1) {

                EmitInArr(buffer, cmdPUSHREG);
                EmitInArr(buffer, reg_name - 'A');
            }

            else if (strcmp(cmdStr, "POPREG") == 0 &&
                     sscanf(str_ptr_arr[i], "%*s %c", &reg_name) == 1) {

                EmitInArr(buffer, cmdPOPREG);
                EmitInArr(buffer, reg_name - 'A');
            }

            else{
                AssemPrintLogs("Invalid command", source_file_line_now,
                                                  source_file_name);
                return 1;
            }
        }

        source_file_line_now++;
    }

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

    buffer->code_arr[(buffer->size)++] = value;
    return 0;
}

int AssemReadInStringBuffer(char* str_buffer)
{
    const char* source_file_name = "source.asm";

    FILE* source_file = fopen(source_file_name, "r");

    if (source_file == NULL) {
        PRINT_LOGS("The source file did not open");
        return 1;
    }

    int str_arr_capacity = fread(str_buffer, sizeof(char), CAPACITY, source_file);

    str_buffer[str_arr_capacity] = '\0';

    fclose(source_file);

    return 0;
}

int CopyFromBufferToStrPtrArr(char* str_buffer,
                              char** str_ptr_arr, size_t* lines)
{
    assert(str_buffer != NULL);
    assert(str_ptr_arr != NULL);

    char* line_begin_ptr = str_buffer;
    char* now_ptr = NULL;
    size_t buffer_index = 0;
    size_t str_arr_index = 0;

    for (;; ++buffer_index) {
        if (str_buffer[buffer_index] == '\n' ||
            str_buffer[buffer_index] == '\0') {

            now_ptr = &str_buffer[buffer_index];
            str_ptr_arr[str_arr_index] = line_begin_ptr;

            str_arr_index++;
            line_begin_ptr = now_ptr + 1;

            (*lines)++;

            if (str_buffer[buffer_index] == '\0')
                break;

            str_buffer[buffer_index] = '\0';

            if (CAPACITY <= str_arr_index){
                PRINT_LOGS("Make CAPACITY bigger");
                return 1;
            }
        }
    }

    return 0;
}






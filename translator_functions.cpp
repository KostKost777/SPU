#include <TXLIB.h>

#include "translator_functions.h"
#include "dump_functions.h"

extern FILE* log_file;
extern FILE* bin_file;
extern FILE* source_file;

extern const char* source_file_name;

void PrintTranslatorLogs(const char* message, size_t line)
{
    assert(log_file != NULL);
    fprintf(log_file, "ERROR: %s in %s:%u\n", message,
                                              source_file_name,
                                              line);
    fflush(log_file);
}

void WriteCmdInFile(int* code_arr, size_t last_index)
{
    for (size_t i = 0;i < last_index; ++i) {
        if (code_arr[i] == cmdPUSH){
            fprintf(bin_file, "%d %d\n", code_arr[i], code_arr[i + 1]);
            i++;
        }

        else
            fprintf(bin_file, "%d\n", code_arr[i]);
    }
}

void EndProcessing(int** arr){
    fclose(log_file);
    fclose(source_file);
    fclose(bin_file);
    free(*arr);
}

int SetNewArrSize(int** code_arr, size_t* capacity) {
    assert(code_arr);

    size_t new_capacity = *capacity * 2;

    int* twin_ptr = (int*)realloc(*code_arr, new_capacity * sizeof(int));


    if (twin_ptr == NULL) {
        PRINT_LOGS("Memory allocation faild");
        return 1;
    }

    *code_arr = twin_ptr;
    *capacity = new_capacity;

    return 0;
}


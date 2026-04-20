#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <assert.h>
#include <ctype.h>
#include <sys/stat.h>
#include <errno.h>

#include "sort_functions.h"
#include "read_poem_from_file_functions.h"
#include "print_poem_in_file_functions.h"
#include "clean_memory_functions.h"
#include "text_functions.h"

int ReadPoemStructFromFile(struct Struct_Poem* Poem,
                           const char* input_filename)
{
    assert(Poem != NULL);
    assert(input_filename != NULL);

    int file_descriptor = open(input_filename, O_RDONLY);

    if (file_descriptor == -1) {
        fprintf(stderr, "Error opening file |%s|", input_filename);
        close(file_descriptor);
        perror("");
        return -1;
    }

    Poem->size_of_file = GetSizeOfFile(input_filename);

    if (Poem->size_of_file == -1)
        return -1;

    Poem->buffer = (char* )calloc(Poem->size_of_file + 1, sizeof(char));
    assert(Poem->buffer != NULL);

    Poem->size_of_file = read(file_descriptor, Poem->buffer, Poem->size_of_file);

    if (Poem->size_of_file == -1) {
        fprintf(stderr, "Error reading file |%s|", input_filename);
        close(file_descriptor);
        perror("");
        return -1;
    }
//    printf("%d\n", Poem->size_of_file);

    Poem->number_of_lines = CountSymbol(Poem, '\n');

    //printf("LINES: %d\n", Poem->number_of_lines);

    Poem->poem_ptr_array = (struct Struct_Line*)calloc(Poem->number_of_lines + 1,
                                                       sizeof(struct Struct_Line));
    assert(Poem->poem_ptr_array != NULL);

    //ReplaceSymbolInBuffer(Poem, '\n', '\0');
    CopyFromBufferInPtrArray(Poem);
    /*
    for (int i = 0;i < Poem->number_of_lines; ++i) {
        printf("%s\n", Poem->poem_ptr_array[i].line_ptr);
    }
    */

    close(file_descriptor);
    return 0;
}

void CopyFromBufferInPtrArray(struct Struct_Poem* Poem)
{
    assert(Poem != NULL);
    assert(Poem->buffer != NULL);
    assert(Poem->poem_ptr_array != NULL);

    char* line_begin_ptr = Poem->buffer;
    char* now_ptr = NULL;
    size_t buffer_index = 0;
    size_t poem_index = 0;

    for (; poem_index < Poem->number_of_lines; ++buffer_index) {

        //printf("%d\n", Poem->buffer[buffer_index]);

        if (*line_begin_ptr == '\n') {
            Poem->number_of_lines--;
            line_begin_ptr++;
            now_ptr = line_begin_ptr;
            continue;
        }

        if (Poem->buffer[buffer_index] == '\n') {

            now_ptr = &Poem->buffer[buffer_index];

            (Poem->poem_ptr_array[poem_index].line_ptr) = line_begin_ptr;

            (Poem->poem_ptr_array[poem_index].str_len) =
            now_ptr - line_begin_ptr;

            //printf("%s\n", line_begin_ptr);
            poem_index++;
            line_begin_ptr = now_ptr + 1;

        }
    }

    *line_begin_ptr = '\0';
}

ssize_t GetSizeOfFile(const char* filename)
{
    assert(filename != NULL);

    struct stat file_info = {};

    if (stat(filename, &file_info) == -1) {
        fprintf(stderr, "������ ��������� ���������� �� ����� |%s|", filename);
        perror("");
        return -1;
    }

    return file_info.st_size;
}

void SkipLine(void)
{
    int ch = 0;
    while ((ch = getchar()) != '\n')
        continue;
}

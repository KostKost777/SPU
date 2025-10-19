#ifndef read_poem_from_file_functions
#define read_poem_from_file_functions

///@file

/**
    @brief String structure
    @param line_ptr pointer to the beginning of the line
    @param str_len string length
**/

struct Struct_Line
{
    char* line_ptr;
    size_t str_len;
};

/**
    @brief Poem structure
    @param buffer whole file buffer
    @param poem_ptr_array array of string structures
    @param size_of_file size of file
    @param number_of_lines number of lines
**/

struct Struct_Poem
{
    char* buffer;
    struct Struct_Line* poem_ptr_array;
    ssize_t size_of_file;
    size_t number_of_lines;
};

/**
    @brief Function for reading text from a file
    @param Poem struct of poem
    @param input_filename input file name
**/

int ReadPoemStructFromFile(struct Struct_Poem* Poem,
                           const char* input_filename);

/**
    @brief Function to copy from a buffer to an array of string structures
    @param Poem struct of poem
**/

void CopyFromBufferInPtrArray(struct Struct_Poem* Poem);

/**
    @brief Function to get file size
    @param filename file name
**/

ssize_t GetSizeOfFile(const char* filename);

void SkipLine(void);

#endif

#ifndef DISTRAN
#define DISTRAN

int DistranWriteCmdInFile(int* code_arr, int last_index);

int DistranReadCmdFromBinFile(int* code_arr, const size_t CAPACITY,
                              size_t* last_index);

void DistranEndProcessing();

#endif

#ifndef DisAsm
#define DisAsm

int DisAsmWriteCmdInFile(struct Buffer* buffer);

int DisAsmProcessingCmd(struct Buffer* buffer, struct StructCmd command,
                        size_t* index);

void DisAsmEndProcessing(struct Buffer* buffer);

#endif

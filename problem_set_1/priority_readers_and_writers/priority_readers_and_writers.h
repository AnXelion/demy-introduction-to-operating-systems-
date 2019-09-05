#ifndef PS1_PRIORITY_READERS_AND_WRITERS_H_
#define PS1_PRIORITY_READERS_AND_WRITERS_H_

void *writerThreadFunc(void *pArg);
void *readerThreadFunc(void *pArg);
int main(int argc, char *argv[]);

#endif

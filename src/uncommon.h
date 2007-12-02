/* uncommon.h -- declarations for uncommon.c and C files linked to it */

extern FILE* debfopen(char*, char*);
extern FILE* findandfopen(char*, char*, char*, char*);
extern char* findandtestopen(char*, char*, char*, char*);
extern FILE* findandfreopen(char*, char*, char*, char*, FILE*);

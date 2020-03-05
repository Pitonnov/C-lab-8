#include <stdlib.h>
#include <stdio.h>

struct SYM 
{
	unsigned char ch;
	int freq=0;
	char code[256];
	struct SYM *left;
	struct SYM *right;
};
union CODE 
{
	unsigned char ch;
	struct 
    {
		unsigned short b1 : 1;
		unsigned short b2 : 1;
		unsigned short b3 : 1;
		unsigned short b4 : 1;
		unsigned short b5 : 1;
		unsigned short b6 : 1;
		unsigned short b7 : 1;
		unsigned short b8 : 1;
	}byte;
};
int compareByFreq(const void *_a, const void *_b);
struct SYM *makeStructure(char *filename,struct SYM syms[],struct SYM *psyms[]);
struct SYM *buildTree(struct SYM*[], int N, int totalSymbols);
void makeCodes(struct SYM *root);
int fillCodeFile(char *filename, char *file101, struct SYM syms[]);
void makeFiles(char *filename, char *file101, char *file_out, char *fileExt);
void writeSignature(char *file_out, int countSymbols, struct SYM syms[],int tail, long int fileSize, char *fileExt);
void writeContent(char *file101, char *file_out);
unsigned char pack(unsigned char buf[]);
int makeStructureFromTable(char *filename, struct SYM syms[], struct SYM *psyms[]);
void getASCII(unsigned int ch, char *code);
int convertCompToBin(char *filename, int countSymbols,char *ext);
void unPack(unsigned char buf[], unsigned char ch);
void makeOriginalFile(char *filename, struct SYM syms[],char *ext);
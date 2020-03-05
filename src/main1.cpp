#define _CRT_SECURE_NO_WARNINGS
#include "task1.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

int main(int argc, char *argv[])
{
	int i;
	float j = 0;
	int tail;
	struct SYM syms[257];
	struct SYM *psyms[512];
	char file101[30], file_out[30];
	struct stat st;
	char fileExt[5];
	char *file;
	FILE *fp101, *fp_out;
	unsigned char buf[8];
	
	if (strcmp(argv[1], "c") == 0)
	{
		char filename[30];
		strcpy(filename, argv[2]);
		filename[strlen(filename)] = 0;
		stat(filename, &st);
		long fileSize = st.st_size;
		*psyms = makeStructure(filename, syms, psyms);
		if (*psyms == NULL)
		{
			printf("File open error");
			return 1;
		}
		qsort(psyms, 256, sizeof(struct SYM*), compareByFreq);
		for (i = 0; i < 256; i++)
		{
			j += psyms[i]->freq;
			if (psyms[i]->freq == 0)
				break;
		}
		struct SYM *root = buildTree(psyms, i, i);
		if (!root->left && !root->right)
			strcat(root->code, "0");
		else
			makeCodes(root);
		makeFiles(filename, file101, file_out, fileExt);
		tail = fillCodeFile(filename, file101, syms);
		writeSignature(file_out, i, syms, tail, fileSize, fileExt);
		writeContent(file101, file_out);
		return 0;
	}
	else if (strcmp(argv[1], "d") == 0)
	{
		char filename[30];
		strcpy(filename, argv[2]);
		char ext[5];
		stat(filename, &st);
		int countSymbols;
		int sizeOfOriginalFile;
		countSymbols = makeStructureFromTable(filename, syms, psyms);
		if (countSymbols == -1)
		{
			printf("File %s is not MDY-file",filename);
			return 1;
		}
		if (countSymbols == 0)
		{
			printf("Cannot open file");
			return 1;
		}
		qsort(psyms, countSymbols, sizeof(struct SYM*), compareByFreq);
		struct SYM *root = buildTree(psyms, countSymbols, countSymbols);
		if (!root->left && !root->right)
			strcat(root->code,"0");
		else
			makeCodes(root);
		sizeOfOriginalFile = convertCompToBin(filename, countSymbols,ext);
		makeOriginalFile(filename, syms,ext);
		return 0;
	}
	else
		{
			printf("Use compressor path task1.cpp for compress\n");
			printf("Use decompressor path task1.cpp for decompress\n");
			return 0;
		}
}


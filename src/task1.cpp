#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include "task1.h"
#include <string.h>
#include <sys/stat.h>

//COMPRESSOR PATH!

int compareByFreq(const void *_a, const void *_b)
{
	struct SYM *a = *(struct SYM**)_a, *b = *(struct SYM**)_b;
	if ((a)->freq < (b)->freq)
		return 1;
	else if ((a)->freq > (b)->freq)
		return -1;
	else if ((a)->ch < (b)->ch)
		return 1;
	else
		return -1;
}
struct SYM *makeStructure(char *filename,struct SYM syms[],struct SYM *psyms[])
{
	FILE *fp;
	int ch;
	int countChar[256] = { 0 };
	int count=0;

	fp = fopen(filename, "rb");
	if (fp == NULL)
		return NULL;
	while ((ch = fgetc(fp)) != -1)
	{
		countChar[ch]++;
		count++;
	}
	for (ch = 0; ch < 256; ch++)
	{
		psyms[ch] = (struct SYM*)malloc(sizeof(struct SYM));
		syms[ch].freq = countChar[ch]; 
		syms[ch].ch = ch;
		strcpy(syms[ch].code, "");
		syms[ch].left = NULL;
		syms[ch].right = NULL;
		psyms[ch] = &syms[ch];
	}
	return psyms[0];
}
struct SYM *buildTree(struct SYM *psyms[], int N, int totalSymbols)
{
	struct SYM *temp = (struct SYM*)malloc(sizeof(struct SYM));
	if (N == 1)
	{
		return psyms[0];
	}
	temp->freq = psyms[N - 1]->freq + psyms[N - 2]->freq;
	temp->left = psyms[N - 1];
	temp->right = psyms[N - 2];
	if (N == 2)
	{
		temp->ch = 0;
		strcpy(temp->code, "");
		return temp;
	}
	psyms[totalSymbols*2 - N] = temp;
	struct SYM *buf1;
	for (int i = totalSymbols * 2 - N - 1; i >= 0; i--)
		if (psyms[i+1]->freq > psyms[i]->freq)
		{
			buf1 = psyms[i+1];
			psyms[i+1] = psyms[i];
			psyms[i] = buf1;
		}
		else
			break;
	return buildTree(psyms, N - 1,totalSymbols);
}
void makeCodes(struct SYM *root)
{
	if (root->left)
	{
		strcpy(root->left->code, root->code);
		strcat(root->left->code, "0");
		makeCodes(root->left);
	}
	if (root->right)
	{
		strcpy(root->right->code, root->code);
		strcat(root->right->code, "1");
		makeCodes(root->right);
	}
}
int fillCodeFile(char *filename, char *file101, struct SYM syms[])
{
	FILE *fp_in, *fp_out;
	unsigned char ch;
	int j;
	int count=0;
	fp_in = fopen(filename, "rb");
	fp_out = fopen(file101, "w+");
	while ((j=fread(&ch,sizeof(unsigned char),1,fp_in))==1)
		for	(int i=0;i<256;i++)
			if (syms[i].ch == ch)
			{
				fputs(syms[i].code, fp_out);
				count += strlen(syms[i].code);
				break;
			}
	int tail = count % 8;
	for (int i = 8 - tail; i > 0; i--)
		fputs("0", fp_out);
	fclose(fp_in);
	fclose(fp_out);
	if (tail == 0)
		tail = 8;
	return 8 - tail;
}
void makeFiles(char *filename, char *file101, char *file_out, char *fileExt)
{
	int i = 0;
	while (filename[i] && filename[i] != '.')
	{
		file101[i] = filename[i];
		file_out[i] = filename[i];
		i++;
	}
	file101[i] = '\0';
	file_out[i] = '\0';
	strcat(file101, ".101");
	strcat(file_out, ".mdy");
	i++;
	int j = 0;
	while (filename[i])
		fileExt[j++] = filename[i++];
	fileExt[j] = '\0';
	return;
}
void writeSignature(char *file_out, int countSymbols, struct SYM syms[], int tail, long int fileSize, char *fileExt)
{
	char buf[30];
	FILE *fp_out;

	fp_out = fopen(file_out, "wb");
	fwrite("MDYLL", sizeof(char),5 , fp_out);
	fwrite(&countSymbols, sizeof(int), 1, fp_out);
	for (int i = 0; i < 256; i++)
		if (syms[i].freq > 0)
		{
			fwrite(&syms[i].ch, sizeof(char), 1, fp_out);
			fwrite(&syms[i].freq, sizeof(int), 1, fp_out);
		}
	fwrite(&tail, sizeof(int), 1, fp_out);
	fwrite(&fileSize, sizeof(int), 1, fp_out);
	fwrite(fileExt, sizeof(char), 3, fp_out);
	fclose(fp_out);
	return;
}
unsigned char pack(unsigned char buf[])
{
	union CODE code;
	code.byte.b1 = buf[0] - '0';
	code.byte.b2 = buf[1] - '0';
	code.byte.b3 = buf[2] - '0';
	code.byte.b4 = buf[3] - '0';
	code.byte.b5 = buf[4] - '0';
	code.byte.b6 = buf[5] - '0';
	code.byte.b7 = buf[6] - '0';
	code.byte.b8 = buf[7] - '0';
	return code.ch;
}
void writeContent(char *file101, char *file_out)
{
	FILE *fp_in;
	FILE *fp_out;
	unsigned char buf[8];
	unsigned char ch;
	int j = 0;
	fp_in = fopen(file101, "r");
	fp_out = fopen(file_out, "ab");
	struct stat st;
	stat(file101, &st);
	long fileSize = st.st_size;
	fwrite(&fileSize, sizeof(long), 1, fp_out);
	while ((buf[0] = fgetc(fp_in))=='0'||buf[0]=='1')
	{
		for (int i = 1; i <= 7; i++)
			buf[i] = fgetc(fp_in);
		ch = pack(buf);
		fwrite(&ch, sizeof(unsigned char), 1, fp_out);
		j++;
	}
	fclose(fp_in);
	fclose(fp_out);
}


//DECOMPRESSOR PATH!

void unPack(unsigned char buf[], unsigned char ch)
{
	union CODE code;
	code.ch = ch;
	buf[0] = code.byte.b1 + '0';
	buf[1] = code.byte.b2 + '0';
	buf[2] = code.byte.b3 + '0';
	buf[3] = code.byte.b4 + '0';
	buf[4] = code.byte.b5 + '0';
	buf[5] = code.byte.b6 + '0';
	buf[6] = code.byte.b7 + '0';
	buf[7] = code.byte.b8 + '0';
	buf[8] = 0;
}
void getASCII(unsigned int ch, char *code)
{
	unsigned int n = ch;
	_itoa(n, code, 2);
	char buf;
	for (int i = 0; i < strlen(code) / 2; i++)
	{
		buf = code[i];
		code[i] = code[strlen(code) - 1 - i];
		code[strlen(code) - 1 - i] = buf;
	}
	while (strlen(code) < 8)
		strcat(code, "0");
	return;
}
int makeStructureFromTable(char *filename, struct SYM syms[], struct SYM *psyms[])
{
	FILE *fp;
	fp = fopen(filename, "rb");
	char buf[30];
	unsigned char a;
	int b;
	int countSymbols;
	if (fp == NULL)
		return 0;
	fread(buf, sizeof(char), 5, fp);
	buf[5] = 0;
	if (strcmp(buf, "MDYLL") != 0)
		return -1;
	fread(&countSymbols, sizeof(int), 1, fp);
	if (countSymbols < 1 || countSymbols>256)
		return NULL;
	for (int i = 0; i < countSymbols; i++)
	{
		fread(&a, sizeof(unsigned char), 1, fp);
		fread(&b, sizeof(int), 1, fp);
		syms[i].ch = a;
		syms[i].freq = b;
		strcpy(syms[i].code, "");
		syms[i].left = NULL;
		syms[i].right = NULL;
		psyms[i] = (struct SYM*)malloc(sizeof(struct SYM));
		psyms[i] = &syms[i];
	}
	return countSymbols;
}
int convertCompToBin(char *filename, int countSymbols, char *ext)
{
	char a, c;
	int b;
	char buf[255], fileExt[5];
	unsigned char code[9];
	char *file101;
	unsigned char ch;
	int j = 0;
	int fileSize;
	int sizeOfOriginalFile;
	int tail;
	FILE *fp_in = fopen(filename, "rb");
	file101 = strtok(filename, ".");
	strcat(file101, ".101");
	FILE *fp_out = fopen(file101, "w");
	fread(buf, sizeof(char),5, fp_in);
	fread(&b, sizeof(int), 1, fp_in);
	int aa;
	for (int i = 0; i < countSymbols; i++)
	{
		fread(&a, sizeof(char), 1, fp_in);
		fread(&b, sizeof(int), 1, fp_in);
	}
	fread(&tail, sizeof(int), 1, fp_in);
	fread(&sizeOfOriginalFile, sizeof(int), 1, fp_in);
	fread(ext, sizeof(char), 3, fp_in);
	ext[3] = 0;
	fread(&fileSize, sizeof(long), 1, fp_in);
	for (int i = 0; i < fileSize / 8 - 1; i++)
	{
		fread(&ch,sizeof(unsigned char),1,fp_in);
		unPack(code, ch);
		fputs((char*)code, fp_out);
	}
	ch = fgetc(fp_in);
	unPack(code, ch);
	for (int i = 0; i < tail; i++)
		code[7 - i] = 0;

	fputs((char*)code, fp_out);
	fclose(fp_in);
	fclose(fp_out);
	return sizeOfOriginalFile;
}
void makeOriginalFile(char *filename, struct SYM syms[],char *ext)
{
	char file101[50];
	char file_out[50];
	char fileExt[10];
	FILE *fp101;
	FILE *fp_out;
	char sequence[256];
	unsigned int ch;
	int i = 0, j;
	while (filename[i] && filename[i] != '.')
	{
		file101[i] = filename[i];
		file_out[i] = filename[i];
		i++;
	}
	file101[i] = '\0';
	file_out[i] = '\0';
	strcat(file_out, ".");
	strcat(file_out, ext);
	strcat(file101, ".101");
	fp101 = fopen(file101, "r");
	fp_out = fopen(file_out, "wb");
	i = 0;
	bool flag;
	while ((ch = fgetc(fp101)) != EOF)
	{
		sequence[i++] = ch;
		sequence[i] = 0;
		j = 0;
		flag = false;
		while (syms[j].freq > 0)
		{
			if (strcmp(sequence, syms[j].code) == 0)
			{
				flag = true;
				break;
			}
			else
				j++;
		}
		if (flag==false)
			continue;
		else
		{
			fwrite(&syms[j].ch, sizeof(unsigned char), 1, fp_out);
			i = 0;
		}
	}
	fclose(fp101);
	fclose(fp_out);
}

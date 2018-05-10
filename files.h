#pragma once
#include <stdio.h>
#include <io.h>
#include <string>
#include <fstream>
#include<Windows.h>
#include "SM4.h"
#define ENCSIZE 1024
#define ENC 1
#define DNC 2
using namespace std;
typedef unsigned long ulong;
typedef unsigned char uchar;

class files {
public:
	char name[200];
	char tmpname[230];
	char tername[230];
	files(int mod, char * Name);
	void Enc(SM4 sm4);
	void Dnc(SM4 sm4);
private:
	bool encable = true;
	long length;
	uchar toFill;
};

files::files(int mod, char * Name)
{
	FILE *fread = fopen(Name, "r");
	if (fread == NULL)
	{
		warn("Can't open the file");
		encable = false;
		return;
	}
	if (mod == ENC)
	{
		strncpy(name, Name, 195);
		sprintf(tmpname, "%s.sm4.tmp", name);
		sprintf(tername, "%s.sm4", name);
	}
	else if (mod == DNC)
	{
		strncpy(name, Name, 195);
		sprintf(tmpname, "%s.tmp", name);
		int len = strlen(name);
		sprintf(tername, "%s", name);
		tername[len - 4] = '\0';
	}
}

void files::Enc(SM4 sm4)
{
	if (!encable)return;
	/*加密逻辑*/
	uint toEnc[ENCSIZE];
	uint cipher[ENCSIZE];
	FILE *Filein = fopen(name, "rb");
	FILE *Fileout = fopen(tername, "wb+");
	if (Fileout == NULL)
	{
		printf("Can't Open the File\n");
		return;
	}
	length = _filelength(fileno(Filein));
	toFill = 16 - length % 16;
	long pres = length % 4;
	length /= 4;
	long total = length;
	while (length >= ENCSIZE)
	{
		fread(toEnc, sizeof(int), ENCSIZE, Filein);
		sm4.enc(toEnc, cipher, ENCSIZE);
		fwrite(cipher, sizeof(int), ENCSIZE, Fileout);
		length -= ENCSIZE;
		printf("\33[2K\r");
		printf("Complete: %2.2f%%", 100 * (1.0 - float(float(length) / float(total))));
	}
	FILE* Tmpout = fopen(tmpname, "wb+");
	length *= 4;
	length += pres;
	char tmpbuff[ENCSIZE * 5];
	fread(tmpbuff, sizeof(char), length, Filein);
	memset(&tmpbuff[length], toFill, toFill);
	fwrite(tmpbuff, sizeof(char), length + toFill, Tmpout);
	fclose(Tmpout);

	FILE *Tmpin = fopen(tmpname, "rb");
	length = (length + toFill) / 4;
	while (length >= ENCSIZE)
	{
		fread(toEnc, sizeof(int), ENCSIZE, Tmpin);
		sm4.enc(toEnc, cipher, ENCSIZE);
		fwrite(cipher, sizeof(int), ENCSIZE, Fileout);
		length -= ENCSIZE;
	}
	fread(toEnc, sizeof(int), length, Tmpin);
	sm4.enc(toEnc, cipher, length);
	fwrite(cipher, sizeof(int), length, Fileout);
	fclose(Tmpin);

	fclose(Filein);
	fclose(Fileout);
	DeleteFile(tmpname);
	char s[300];
	sprintf(s, "Finished!\nThe cryptographic file has been saved in:\n%s", tername);
	warn(s);
}

void files::Dnc(SM4 sm4) {
	if (!encable)return;
	uint toDnc[ENCSIZE];
	uint cipher[ENCSIZE];
	int pos = 0;
	FILE *filein = fopen(name, "rb");
	FILE *fileout = fopen(tername, "wb+");
	if (fileout == NULL)
	{
		printf("Can't Open the File\n");
		return;
	}

	length = _filelength(fileno(filein)) / 4;
	long total = length;
	fseek(filein, 0, SEEK_SET);
	while (length > ENCSIZE)
	{
		fread(toDnc, sizeof(int), ENCSIZE, filein);
		sm4.dec(toDnc, cipher, ENCSIZE);
		fwrite(cipher, sizeof(int), ENCSIZE, fileout);
		length -= ENCSIZE;
		printf("\33[2K\r");
		printf("Complete:%2.2f%%", 100 * (1.0 - float(float(length) / float(total))));
	}

	FILE *Tmpout = fopen(tmpname, "wb+");
	fread(toDnc, sizeof(int), length, filein);
	sm4.dec(toDnc, cipher, length);
	fwrite(cipher, sizeof(int), length, Tmpout);
	fclose(Tmpout);

	FILE *terin = fopen(tmpname, "rb");
	if (terin == NULL)
	{
		printf("Can't Open the File\n");
		return;
	}
	length = _filelength(fileno(terin));
	fseek(terin, -1, SEEK_END);
	uchar End;
	fread(&End, sizeof(uchar), 1, terin);
	fseek(terin, 0, SEEK_SET);


	long copylength = length - long(End);
	char buff[ENCSIZE * 4];
	while (copylength >= ENCSIZE * 4)
	{
		fread(buff, sizeof(char), ENCSIZE * 4, terin);
		fwrite(buff, sizeof(char), ENCSIZE * 4, fileout);
		copylength -= ENCSIZE * 4;
	}
	fread(buff, sizeof(char), copylength, terin);
	fwrite(buff, sizeof(char), copylength, fileout);

	fclose(filein);
	fclose(fileout);
	fclose(terin);
	DeleteFile(tmpname);
	char s[300];
	sprintf(s, "Finished!\nThe cryptographic file has been saved in:\n%s", tername);
	warn(s);
}
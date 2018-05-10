#include <iostream>
#include "files.h"
using namespace std;
int main()
{
	SM4 sm4;
	int mod;
	char cmd;
	bool exit = false;
	string rubbish;
	char name[200];
	files* newFile;
	while (!exit)
	{
		//printf("Please input command:\n1:encrypt the file\n2:Decrypt the file\n3:exit\n>");
		cin >> cmd;
		if (cmd<'1' || cmd>'3')
		{
			warn("Invalid command!!");
			while ((cmd = getchar()) != '\n'&&cmd != EOF);
			continue;
		}
		mod = cmd - '0';
		while ((cmd = getchar()) != '\n'&&cmd != EOF);
		switch (mod)
		{
		case ENC:
			printf("Input file name:");
			scanf_s("%s", &name, 199);
			newFile = new files(mod, name);
			newFile->Enc(sm4);
			delete newFile;
			break;
		case DNC:
			printf("Input file name:");
			scanf_s("%s", &name, 199);
			newFile = new files(mod, name);
			newFile->Dnc(sm4);
			delete newFile;
			break;
		case 3:
			exit = true;
			break;
		default:
			printf("Invalid command!!\n");
			break;

		}
	}
	printf("Thanks for using!\n");
	system("pause");
	return 0;
}

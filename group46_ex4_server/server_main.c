/* ==============================================
Introduction to Systems Programming
Winter 2017-2018
TEL-AVIV UNIVERSITY
Exercise 4
Uri Cohen                 302825807
Anton Chaplianka          310224209
============================================== */

//===================================================================================//
#include "MainServer.h"
#include <stdio.h>
#include <string.h>
#include <winsock2.h>
#include <stdlib.h>
//===================================================================================//
int main(int argc, char *argv[])
{
	if (argc != 4) {
		printf("not enough arguments\n");
		return -1;
	}
	errno_t retval;
	FILE *p_stream;
	retval = fopen_s(&p_stream, argv[1], "w");
	if (0 != retval)
	{
		return -1;
	}
	retval = fputs("\0", p_stream);
	retval = fclose(p_stream);
	if (0 != retval)
	{
		return -1;
	}
	int Res = MainServer(argv[1], argv[2], argv[3]);
	return Res;
}




//===================================================================================//
#include "SendRecvTools.h"
#include "SocketShared.h"
#include "BoardTools.h"
#include <stdio.h>
#include <string.h>
//===================================================================================//
int mark_board(char *board, char x, char y, char mark, char **p_new_board)
{
	if (strlen(board) == 9)
	{
		char *new_board = (char*)calloc(10, sizeof(char));
		if (new_board == NULL)
		{
			return -1;
		}
		int i;
		for (i = 0; i < 10;++i)
		{
			new_board[i] = board[i];
		}
		if (x == '1' && y == '1' && new_board[0] == '0')
		{
			new_board[0] = mark;
		}
		else if (x == '2' && y == '1' && new_board[1] == '0')
		{
			new_board[1] = mark;
		}
		else if (x == '3' && y == '1' && new_board[2] == '0')
		{
			new_board[2] = mark;
		}
		else if (x == '1' && y == '2' && new_board[3] == '0')
		{
			new_board[3] = mark;
		}
		else if (x == '2' && y == '2' && new_board[4] == '0')
		{
			new_board[4] = mark;
		}
		else if (x == '3' && y == '2' && new_board[5] == '0')
		{
			new_board[5] = mark;
		}
		else if (x == '1' && y == '3' && new_board[6] == '0')
		{
			new_board[6] = mark;
		}
		else if (x == '2' && y == '3' && new_board[7] == '0')
		{
			new_board[7] = mark;
		}
		else if (x == '3' && y == '3' && new_board[8] == '0')
		{
			new_board[8] = mark;
		}
		else
		{
			return -1;
		}
		*p_new_board = new_board;
		return 0;
	}
	else {
		return -1;
	}
	
}
//===================================================================================//
int he_won(char *board, char mark)
{
	if (strlen(board) == 9)
	{
		if (board[0] == mark && board[1] == mark && board[2] == mark)
		{
			return 1;
		}
		else if (board[3] == mark && board[4] == mark && board[5] == mark)
		{
			return 1;
		}
		else if (board[6] == mark && board[7] == mark && board[8] == mark)
		{
			return 1;
		}
		else if (board[0] == mark && board[3] == mark && board[6] == mark)
		{
			return 1;
		}
		else if (board[1] == mark && board[4] == mark && board[7] == mark)
		{
			return 1;
		}
		else if (board[2] == mark && board[5] == mark && board[8] == mark)
		{
			return 1;
		}
		else if (board[2] == mark && board[4] == mark && board[6] == mark)
		{
			return 1;
		}
		else if (board[0] == mark && board[4] == mark && board[8] == mark)
		{
			return 1;
		}
		else {
			return 0;
		}
	}
	else {
		return -1;
	}
	
}
//===================================================================================//
int print_board(char *board)
{
	if (strlen(board) == 9)
	{
		// ---- print upper row ------
		printf("|");
		if (board[6] == '0')
		{
			printf(" ");
		}
		else {
			printf("%c", board[6]);
		}
		printf("|");
		if (board[7] == '0')
		{
			printf(" ");
		}
		else {
			printf("%c", board[7]);
		}
		printf("|");
		if (board[8] == '0')
		{
			printf(" ");
		}
		else {
			printf("%c", board[8]);
		}
		printf("|\n");
		// ----------------------------

		// ----- print middle row -----
		printf("|");
		if (board[3] == '0')
		{
			printf(" ");
		}
		else {
			printf("%c", board[3]);
		}
		printf("|");
		if (board[4] == '0')
		{
			printf(" ");
		}
		else {
			printf("%c", board[4]);
		}
		printf("|");
		if (board[5] == '0')
		{
			printf(" ");
		}
		else {
			printf("%c", board[5]);
		}
		printf("|\n");
		// ----------------------------
		// ----- print bottom row -----
		printf("|");
		if (board[0] == '0')
		{
			printf(" ");
		}
		else {
			printf("%c", board[0]);
		}
		printf("|");
		if (board[1] == '0')
		{
			printf(" ");
		}
		else {
			printf("%c", board[1]);
		}
		printf("|");
		if (board[2] == '0')
		{
			printf(" ");
		}
		else {
			printf("%c", board[2]);
		}
		printf("|\n");
		// ----------------------------
		return 0;
	}
	else
	{
		return -1;
	}
}
//===================================================================================//
int board_full(char *board)
{
	int i=0;
	int full = 1;
	if (strlen(board) == 9)
	{
		while (full != 0 && i < 10)
		{
			if (board[i] == '0')
			{
				full = 0;
			}
			else
			{
				i++;
			}
		}
		if (full == 0)
		{
			return 0;
		}
		else
		{
			return 1;
		}
	}
	else
	{
		return -1;
	}

}

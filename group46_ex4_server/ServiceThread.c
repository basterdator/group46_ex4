/* ==============================================
Introduction to Systems Programming
Winter 2017-2018
TEL-AVIV UNIVERSITY
Exercise 4
Uri Cohen                 302825807
Anton Chaplianka          310224209
============================================== */
//===================================================================================//
#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
//===================================================================================//
#include <stdio.h>
#include <string.h>
#include <winsock2.h>
#include "MainServer.h"
#include "SendRecvTools.h"
#include "ServiceThread.h"
#include "BoardTools.h"
//===================================================================================//
#define SEND_STR_SIZE 70
//===================================================================================//
extern char user_name1[USER_NAME_SIZE];
extern char user_name2[USER_NAME_SIZE];
extern char board[9];
extern HANDLE board_semaphore;
extern HANDLE write_to_file;
extern SOCKET ThreadHandles[NUM_OF_WORKER_THREADS];
TransferResult_t board_view_and_turn_switch(SOCKET *t_socket, int last_player_num, int only_board);
DWORD ServiceThreadSenRec(player_params *p_player_params);
int RecieveStringTop(char **AcceptedStr, SOCKET *t_socket, int player_num);
int state1 = 0;
int state2 = 0;
HANDLE ThreadSenRecHandles[2];
int user1_disconected;
int user2_disconected;

//===================================================================================//

DWORD ServiceThread(player_params *p_player_params)
{


	//	strcpy(board, "000000000");
	DWORD wait_res;
	DWORD release_res;
	SOCKET *t_socket = (*p_player_params).player_soket;
	int player_num = (*p_player_params).player_num;
	if (player_num == 0)
	{
		strcpy(board, "000000000");
		user1_disconected = 0;
		state1 = 0;
		state2 = 0;
		strcpy(user_name1, "0000");
		user2_disconected = 0;
		strcpy(user_name2, "0000");
	}
	//printf("state1 is %d\n state2 is %d\n player is %d\n", state1, state2,player_num);

	char SendStr[SEND_STR_SIZE];
	BOOL Done = FALSE;
	BOOL RegisteredToGame = FALSE;
	BOOL user_name_exists;
	TransferResult_t SendRes;
	//	TransferResult_t RecvRes;
	char *AcceptedStr = NULL;
	int RecieveRes;
	if (player_num == 0) {
		user1_disconected = 0;
		state1 = 0;
	}
	else
	{
		state2 = 0;
		user2_disconected = 0;
	}
	//printf("state1 is %d\n state2 is %d\n player ",state1,state2);
	while (!RegisteredToGame) {

		AcceptedStr = NULL;
		RecieveRes = RecieveStringTop(&AcceptedStr, t_socket, player_num);
		if (RecieveRes == 1)
		{
			return 1;
		}

		char *MessageType = NULL;
		char *param1 = NULL;
		char *param2 = NULL;
		char *param3 = NULL;
		ParseMessage(AcceptedStr, &MessageType, &param1, &param2, &param3);
		//check if the message is of type: NEW_USER_REQUEST
		//printf("msg typ is: %s\n", MessageType);
		if (strcmp(MessageType, "NEW_USER_REQUEST") == 0) {//if it is
			user_name_exists = STRINGS_ARE_EQUAL(param1, user_name1) || STRINGS_ARE_EQUAL(param1, user_name2);
			if (user_name_exists) {
				printf("Requset to join was refused\n");
				closesocket(*t_socket);
				return 1;
			}

			if (player_num == 0) {
				strcpy(user_name1, param1);
				strcpy(SendStr, "NEW_USER_ACCEPTED:x;1\n");
				RegisteredToGame = TRUE;
			}
			else {
				strcpy(user_name2, param1);
				strcpy(SendStr, "NEW_USER_ACCEPTED:o;2\n");
			}
			SendRes = SendString(SendStr, *t_socket);
			RegisteredToGame = TRUE;
		}
		else//if no: error
		{
			strcpy(SendStr, "NEW_USER_DECLINED\n");
			SendRes = SendString(SendStr, *t_socket);
			printf("Request to join was refused\n");
			closesocket(*t_socket);
			return 1;
		}
	}//while (!RegisteredToGame)

	if (player_num == 0) {
		state1 = 1;
	}
	else
	{
		state2 = 1;
	}

	player_params player_params_for_thread;
	player_params_for_thread.player_num = player_num;
	player_params_for_thread.player_soket = t_socket;
	ThreadSenRecHandles[player_num] = CreateThread(
		NULL,
		0,
		(LPTHREAD_START_ROUTINE)ServiceThreadSenRec,
		&player_params_for_thread,
		0,
		NULL
	);


	//printf("before while, player is %d\n", player_num);
	while (!((state1 >= 1) && (state2 >= 1))) {
		//waiting for the other player 
		
		if ((player_num == 0)&& (user1_disconected) ){
			printf("Player disconnected. Exiting\n");

			wait_res = WaitForSingleObject(write_to_file, INFINITE);
			if (wait_res != WAIT_OBJECT_0)
			{
				return -1;
			}
			PrintToLogFile("Player disconnected. Exiting\n");
			release_res = ReleaseMutex(write_to_file);
			if (release_res == FALSE)
			{
				return -1;
			}
			return 0;
		}
		if ((player_num == 1) && (user2_disconected)) {
			printf("Player disconnected. Exiting\n");

			wait_res = WaitForSingleObject(write_to_file, INFINITE);
			if (wait_res != WAIT_OBJECT_0)
			{
				return -1;
			}
			PrintToLogFile("Player disconnected. Exiting\n");
			release_res = ReleaseMutex(write_to_file);
			if (release_res == FALSE)
			{
				return -1;
			}
			return 0;
		}

	
	}

	//printf("state1 is %d\n state2 is %d\n player is %d ########\n", state1, state2, player_num);


	strcpy(SendStr, "GAME_STARTED\n");
	//printf("player %d, sending %s\n",player_num,AcceptedStr);
	SendRes = SendString(SendStr, *t_socket);

	if (SendRes == TRNS_FAILED)
	{
		printf("Service socket error while writing, closing thread.\n");
		closesocket(*t_socket);
		return 1;
	}

	//--------------------game started-----------------//

	SendRes = board_view_and_turn_switch(t_socket, 1, 0);
	if (SendRes == TRNS_FAILED)
	{
		return 1;
	}

	if (player_num == 0) {
		state1 = 2;
	}
	else {
		state2 = 2;
	}

	int turn = 0;
	while (!((state1 == 2) && (state2 == 2))) {
		//waiting for both players to know the game started
	}

	turn = 0;
	while (turn < 9) {
		if ((turn % 2) == 0) {// player 1 turn
			while (!(state1 == (state2 + 1))) {
			//	printf("user1_disconected: %s, user2_disconected: %s", user1_disconected)
				if ((user1_disconected == 1) || (user2_disconected == 1))
				{
					printf("Player disconnected. Exiting\n");

					wait_res = WaitForSingleObject(write_to_file, INFINITE);
					if (wait_res != WAIT_OBJECT_0)
					{
						return -1;
					}
					PrintToLogFile("Player disconnected. Exiting\n");
					release_res = ReleaseMutex(write_to_file);
					if (release_res == FALSE)
					{
						return -1;
					}


					if (((player_num == 1) && (user1_disconected == 1)) || ((player_num == 0) && (user2_disconected == 1)));
					{
						closesocket(*t_socket);
					}
					//TerminateThread(ThreadSenRecHandles[0], 0x555);
					//TerminateThread(ThreadSenRecHandles[1], 0x555);
					return 0;
				}
				//waiting for player 1 to play
			}
			if (he_won(board, 'x')) {
				SendRes = board_view_and_turn_switch(t_socket, 1, 1);
				if (SendRes == TRNS_FAILED)
				{
					return 1;
				}
				strcpy(SendStr, "GAME_ENDED:0;");
				strcat(SendStr, user_name1);
				strcat(SendStr, "\n");
				SendRes = SendString(SendStr, *t_socket);
				if (SendRes == TRNS_FAILED)
				{
					printf("Service socket error while writing, closing thread.\n");
					closesocket(*t_socket);
					return 1;
				}
				break;
			}
			if (turn == 8)
			{
				SendRes = board_view_and_turn_switch(t_socket, 0, 1);
			}
			else
			{
				SendRes = board_view_and_turn_switch(t_socket, 0, 0);
			}
			if (SendRes == TRNS_FAILED)
			{
				return 1;
			}
		}
		else //player 2 turn
		{
			while (!(state1 == state2)) {
				//waiting for player 1 to play
				if ((user1_disconected == 1) || (user2_disconected == 1))
				{
					printf("other user disconected exiting\n");
					if (((player_num == 1) && (user1_disconected == 1)) || ((player_num == 0) && (user2_disconected == 1)));
					{
						closesocket(*t_socket);
					}
					return 0;
				}
			}

			if (he_won(board, 'o')) {
				SendRes = board_view_and_turn_switch(t_socket, 1, 1);
				if (SendRes == TRNS_FAILED)
				{
					return 1;
				}
				strcpy(SendStr, "GAME_ENDED:0;");
				strcat(SendStr, user_name2);
				strcat(SendStr, "\n");
				SendRes = SendString(SendStr, *t_socket);
				if (SendRes == TRNS_FAILED)
				{
					printf("Service socket error while writing, closing thread.\n");
					closesocket(*t_socket);
					return 1;
				}
				break;
			}
			SendRes = board_view_and_turn_switch(t_socket, 1, 0);
			if (SendRes == TRNS_FAILED)
			{
				return 1;
			}
		}
		turn += 1;
		//printf("turn is %d\n",turn);
	}
	if (turn == 9)
	{
		strcpy(SendStr, "GAME_ENDED:1");
		strcat(SendStr, "\n");
		SendRes = SendString(SendStr, *t_socket);
		if (SendRes == TRNS_FAILED)
		{
			printf("Service socket error while writing, closing thread.\n");
			closesocket(*t_socket);
			return 1;
		}
	}
	return 0;
}
//===================================================================================//
int RecieveStringTop(char **AcceptedStr, SOCKET *t_socket, int player_num) {
	TransferResult_t RecvRes;
	RecvRes = ReceiveString(AcceptedStr, *t_socket);
	if (RecvRes == TRNS_FAILED)
	{
		if (player_num == 0) {
			user1_disconected = 1;
		}
		else
		{
			user2_disconected = 1;
		}
		//	ThreadHandles[player_num] = NULL;
		printf("Service socket error while reading, closing thread.\n");
		closesocket(*t_socket);
		return 1;
	}
	else if (RecvRes == TRNS_DISCONNECTED)
	{
		if (player_num == 0) {
			user1_disconected = 1;
		}
		else
		{
			user2_disconected = 1;
		}
		//	ThreadHandles[player_num] = NULL;
		printf("Connection closed while reading, closing thread.\n");
		closesocket(*t_socket);
		return 1;
	}
	else //got string
	{
		printf("Got string : %s\n", *AcceptedStr); //for debug
		return 0;
	}
}
//===================================================================================//
DWORD ServiceThreadSenRec(player_params *p_player_params) {
	DWORD wait_res;
	DWORD release_res;
	SOCKET *t_socket = (*p_player_params).player_soket;
	int player_num = (*p_player_params).player_num;
	char SendStr[SEND_STR_SIZE];
	BOOL Done = FALSE;
	BOOL RegisteredToGame = FALSE;
	TransferResult_t SendRes;
	//	TransferResult_t RecvRes;
	char *AcceptedStr = NULL;
	char *MessageType = NULL;
	char *param1;
	char *param2;
	char *param3;
	int RecieveRes;
	int MarkBoardRes;
	char *p_new_board = NULL;
	int state1_d;
	int state2_d;
	while (!Done) {
		AcceptedStr = NULL;
		MessageType = NULL;
		param1 = NULL;
		param2 = NULL;
		param3 = NULL;
		MarkBoardRes = 0;
		int parse_check = 0;
		RecieveRes = RecieveStringTop(&AcceptedStr, t_socket, player_num);
		if (RecieveRes == 1)
		{

			printf("player exiting: %d\n", player_num);
			return 0;
		}
		state1_d = state1;
		state2_d = state2;
		parse_check = ParseMessage(AcceptedStr, &MessageType, &param1, &param2, &param3);
		if (parse_check == -1) {
			if (player_num == 0) {
				user1_disconected = 1;
			}
			else
			{
				user2_disconected = 1;
			}
			//	ThreadHandles[player_num] = NULL;
			printf("Connection closed while reading, closing thread.\n");
			closesocket(*t_socket);
			return 1;
		}
		if (!((state1_d >= 1) && (state2_d >= 1)))
		{
			if (STRINGS_ARE_EQUAL(MessageType, "PLAY_REQUEST")) {
				strcpy(SendStr, "PLAY_DECLINED:Game has not started\n");
				SendRes = SendString(SendStr, *t_socket);

				if (SendRes == TRNS_FAILED)
				{
					printf("Service socket error while writing, closing thread.\n");
					closesocket(*t_socket);
					return 1;
				}
			}
		}

		if ((state1_d == state2_d) && ((state1_d > 1) && (state2_d > 1))) {
			//			ParseMessage(AcceptedStr, &MessageType, &param1, &param2, &param3);
			if (player_num == 0) {//its your turn
				if (STRINGS_ARE_EQUAL(MessageType, "PLAY_REQUEST")) {
					p_new_board = NULL;
					MarkBoardRes = mark_board(board, param1[0], param2[0], 'x', &p_new_board);
					if (MarkBoardRes != 0) {//illegal move
						strcpy(SendStr, "PLAY_DECLINED:Illegal move\n");
						SendRes = SendString(SendStr, *t_socket);
						if (SendRes == TRNS_FAILED)
						{
							printf("Service socket error while writing, closing thread.\n");
							closesocket(*t_socket);
							return 1;
						}
					}
					else
					{
						wait_res = WaitForSingleObject(board_semaphore, INFINITE);
						if (wait_res != WAIT_OBJECT_0)
						{

							return -1;
						}

						strcpy(board, p_new_board);

						release_res = ReleaseSemaphore(
							board_semaphore,
							1,
							NULL);
						if (release_res == FALSE)
						{
							return -1;
						}
						strcpy(SendStr, "PLAY_ACCEPTED\n");
						SendRes = SendString(SendStr, *t_socket);
						if (SendRes == TRNS_FAILED)
						{
							printf("Service socket error while writing, closing thread.\n");
							closesocket(*t_socket);
							return 1;
						}
						//					state1 = 3;
						state1 += 1;
					}
				}

			}
			else
			{//not yout turn
				if (STRINGS_ARE_EQUAL(MessageType, "PLAY_REQUEST")) {
					strcpy(SendStr, "PLAY_DECLINED:Not your turn\n");
					SendRes = SendString(SendStr, *t_socket);
					if (SendRes == TRNS_FAILED)
					{
						printf("Service socket error while writing, closing thread.\n");
						closesocket(*t_socket);
						return 1;
					}
				}

			}
		}
		if (((state1_d - 1) == state2_d) && ((state1_d > 1) && (state2_d > 1))) {
			//			ParseMessage(AcceptedStr, &MessageType, &param1, &param2, &param3);
			if (player_num == 1) {//its your turn
				if (STRINGS_ARE_EQUAL(MessageType, "PLAY_REQUEST")) {

					p_new_board = NULL;
					MarkBoardRes = mark_board(board, param1[0], param2[0], 'o', &p_new_board);
					if (MarkBoardRes != 0) {//illegal move
						strcpy(SendStr, "PLAY_DECLINED:Illegal move\n");
						SendRes = SendString(SendStr, *t_socket);
						if (SendRes == TRNS_FAILED)
						{
							printf("Service socket error while writing, closing thread.\n");
							closesocket(*t_socket);
							return 1;
						}
					}
					else
					{
						wait_res = WaitForSingleObject(board_semaphore, INFINITE);
						if (wait_res != WAIT_OBJECT_0)
						{

							return -1;
						}

						strcpy(board, p_new_board);

						release_res = ReleaseSemaphore(
							board_semaphore,
							1,
							NULL);
						if (release_res == FALSE)
						{
							return -1;
						}

						strcpy(SendStr, "PLAY_ACCEPTED\n");
						SendRes = SendString(SendStr, *t_socket);
						if (SendRes == TRNS_FAILED)
						{
							printf("Service socket error while writing, closing thread.\n");
							closesocket(*t_socket);
							return 1;
						}
						//					state2 = 3;
						state2 += 1;
					}
				}

			}
			else
			{//not your turn
				if (STRINGS_ARE_EQUAL(MessageType, "PLAY_REQUEST")) {
					strcpy(SendStr, "PLAY_DECLINED:Not your turn\n");
					SendRes = SendString(SendStr, *t_socket);
					if (SendRes == TRNS_FAILED)
					{
						printf("Service socket error while writing, closing thread.\n");
						closesocket(*t_socket);
						return 1;
					}
				}

			}

		}//if player 2 turn

		if (STRINGS_ARE_EQUAL(MessageType, "USER_LIST_QUERY")) {
			strcpy(SendStr, "USER_LIST_REPLY:");
			if ((state1_d > 0) && (state2_d > 0)) {
				strcat(SendStr, user_name1);
				strcat(SendStr, ";");
				strcat(SendStr, user_name2);
				strcat(SendStr, "\n");
			}
			else if (state1_d > 0)
			{
				strcat(SendStr, user_name1);
				strcat(SendStr, "\n");
			}
			else {
				strcat(SendStr, user_name2);
				strcat(SendStr, "\n");
			}
			SendRes = SendString(SendStr, *t_socket);
			if (SendRes == TRNS_FAILED)
			{
				printf("Service socket error while writing, closing thread.\n");
				closesocket(*t_socket);
				return 1;
			}

		}//USER_LIST

		else if (STRINGS_ARE_EQUAL(MessageType, "GAME_STATE_QUERY")) {
			strcpy(SendStr, "GAME_STATE_REPLY:");
			if ((state1_d > 0) && (state2_d > 0)) {//game started
				strcat(SendStr, "1;");
				if (state1_d == state2_d) {
					strcat(SendStr, user_name1);
					strcat(SendStr, ";x\n");
				}
				else
				{
					strcat(SendStr, user_name2);
					strcat(SendStr, ";o\n");
				}
			}
			else
			{
				strcat(SendStr, "0\n");
			}
			SendRes = SendString(SendStr, *t_socket);
			if (SendRes == TRNS_FAILED)
			{
				printf("Service socket error while writing, closing thread.\n");
				closesocket(*t_socket);
				return 1;
			}
		}//STATE

		else if (STRINGS_ARE_EQUAL(MessageType, "BOARD_VIEW_QUERY")) {
			strcpy(SendStr, "BOARD_VIEW_REPLY:");

			wait_res = WaitForSingleObject(board_semaphore, INFINITE);
			if (wait_res != WAIT_OBJECT_0)
			{

				return -1;
			}

			strcat(SendStr, board);
			strcat(SendStr, "\n");
			SendRes = SendString(SendStr, *t_socket);

			release_res = ReleaseSemaphore(
				board_semaphore,
				1,
				NULL);
			if (release_res == FALSE)
			{
				return -1;
			}

			if (SendRes == TRNS_FAILED)
			{
				printf("Service socket error while writing, closing thread.\n");
				closesocket(*t_socket);
				return 1;
			}
		}//BOARD_VIEW_QUERY
		else if (STRINGS_ARE_EQUAL(MessageType, "PLAY_REQUEST")) {}
		else
		{
			if (player_num == 0) {
				user1_disconected = 1;
			}
			else
			{
				user2_disconected = 1;
			}
			//	ThreadHandles[player_num] = NULL;
			printf("Connection closed while reading, closing thread.\n");
			closesocket(*t_socket);
			return 1;
		}

	}//while
	return 0;
}
//===================================================================================//
TransferResult_t board_view_and_turn_switch(SOCKET *t_socket, int last_player_num, int only_board) {
	TransferResult_t SendRes;
	char SendStr[SEND_STR_SIZE];
	DWORD wait_res;
	DWORD release_res;

	wait_res = WaitForSingleObject(board_semaphore, INFINITE);
	if (wait_res != WAIT_OBJECT_0)
	{
		return -1;
	}

	strcpy(SendStr, "BOARD_VIEW:");
	strcat(SendStr, board);
	strcat(SendStr, "\n");
	SendRes = SendString(SendStr, *t_socket);

	release_res = ReleaseSemaphore(
		board_semaphore,
		1,
		NULL);
	if (release_res == FALSE)
	{
		return -1;
	}



	if (SendRes == TRNS_FAILED)
	{
		printf("Service socket error while writing, closing thread.\n");
		closesocket(*t_socket);
		return SendRes;
	}
	if (only_board == 0) {
		strcpy(SendStr, "TURN_SWITCH:");
		if (last_player_num == 1) {
			strcat(SendStr, user_name1);
			strcat(SendStr, ";x\n");
		}
		else
		{
			strcat(SendStr, user_name2);
			strcat(SendStr, ";o\n");
		}
		SendRes = SendString(SendStr, *t_socket);

		if (SendRes == TRNS_FAILED)
		{
			printf("Service socket error while writing, closing thread.\n");
			closesocket(*t_socket);
			return SendRes;
		}
		return SendRes;
	}
	else {
		return -1;
	}

}
//===================================================================================//
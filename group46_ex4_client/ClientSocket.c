/* ==============================================
Introduction to Systems Programming
Winter 2017-2018
TEL-AVIV UNIVERSITY
Exercise 4
Uri Cohen                 302825807
Anton Chaplianka          310224209
============================================== */

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <winsock2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SocketShared.h"
#include "SendRecvTools.h"
#include "ClientSocket.h"
#include "BoardTools.h"

//===================================================================================//
char SendStr[256];
int gnrt_msg_rslt;
static HANDLE h_message;
static HANDLE h_input;
static HANDLE write_to_file;
static void ReportErrorAndEndProgram();
SOCKET m_socket;
static char *send_char;
static DWORD wait_res;
static BOOL release_res;
//===================================================================================//

//Reading data coming from the server
static DWORD RecvDataThread(LPVOID lpParam)
{
	char *path;
	path = (char *)lpParam;
	TransferResult_t RecvRes;

	while (1)
	{
		char *AcceptedStr = NULL;
		char *MSG_type = NULL;
		char *param1 = NULL;
		char *param2 = NULL;
		char *param3 = NULL;
		int ret = 0;
		RecvRes = ReceiveString(&AcceptedStr, m_socket);
		if (RecvRes == TRNS_FAILED)
		{
			// *****************************************************
			wait_res = WaitForSingleObject(write_to_file, INFINITE);
			if (wait_res != WAIT_OBJECT_0)
			{
				ReportErrorAndEndProgram();
				return -1;
			}
			PrintToLogFile("Server disconnected. Exiting.\n", path);
			release_res = ReleaseMutex(write_to_file);
			if (release_res == FALSE)
			{
				ReportErrorAndEndProgram();
				return -1;
			}
			// *****************************************************
			printf("Server disconnected. Exiting.\n");
			return 0x555;
		}
		else if (RecvRes == TRNS_DISCONNECTED)
		{
			// *****************************************************
			wait_res = WaitForSingleObject(write_to_file, INFINITE);
			if (wait_res != WAIT_OBJECT_0) 
			{
				ReportErrorAndEndProgram();
				return -1;
			}
			PrintToLogFile("Server disconnected. Exiting.\n", path);
			release_res = ReleaseMutex(write_to_file);
			if (release_res == FALSE) 
			{
				ReportErrorAndEndProgram();
				return -1;
			}
			// *****************************************************
			printf("Server disconnected. Exiting.\n");
			return 0x555;
		}
		else 
		{
			// *****************************************************
			wait_res = WaitForSingleObject(write_to_file, INFINITE);
			if (wait_res != WAIT_OBJECT_0)
			{
				ReportErrorAndEndProgram();
				return -1;
			}
			PrintToLogFile("Received from server: ", path);
			PrintToLogFile(AcceptedStr, path);
			release_res = ReleaseMutex(write_to_file);
			if (release_res == FALSE) 
			{
				ReportErrorAndEndProgram();
				return -1;
			}
			// *****************************************************
			if (ParseMessage(AcceptedStr, &MSG_type, &param1, &param2, &param3) == 0 )
			{
				if (STRINGS_ARE_EQUAL(MSG_type, "NEW_USER_DECLINED"))
				{
					printf("Request to join was refused.\n");
					// *****************************************************
					wait_res = WaitForSingleObject(write_to_file, INFINITE);
					if (wait_res != WAIT_OBJECT_0)
					{
						ReportErrorAndEndProgram();
						return -1;
					}
					PrintToLogFile("Request to join was refused.\n", path);
					release_res = ReleaseMutex(write_to_file);
					if (release_res == FALSE)
					{
						ReportErrorAndEndProgram();
						return -1;
					}
					// *****************************************************
					return 0x555;
				}
				else if (STRINGS_ARE_EQUAL(MSG_type, "NEW_USER_ACCEPTED"))
				{
					printf("Player accepted, you play %s, number of current players: %s\n", param1, param2);
				}
				else if (STRINGS_ARE_EQUAL(MSG_type, "USER_LIST_REPLY"))
				{
					if (param2 == NULL) 
					{
						printf("Players: %s\n", param1);
						// *****************************************************
						wait_res = WaitForSingleObject(write_to_file, INFINITE);
						if (wait_res != WAIT_OBJECT_0)
						{
							ReportErrorAndEndProgram();
							return -1;
						}
						PrintToLogFile("Players: ", path);
						PrintToLogFile(param1, path);
						PrintToLogFile("\n", path);
						release_res = ReleaseMutex(write_to_file);
						if (release_res == FALSE)
						{
							ReportErrorAndEndProgram();
							return -1;
						}
						// *****************************************************
					}
					else {
						printf("Players: %s, %s\n", param1, param2);
						// *****************************************************
						wait_res = WaitForSingleObject(write_to_file, INFINITE);
						if (wait_res != WAIT_OBJECT_0)
						{
							ReportErrorAndEndProgram();
							return -1;
						}
						PrintToLogFile("Players: ", path);
						PrintToLogFile(param1, path);
						PrintToLogFile(", ", path);
						PrintToLogFile(param2, path);
						PrintToLogFile("\n", path);
						release_res = ReleaseMutex(write_to_file);
						if (release_res == FALSE)
						{
							ReportErrorAndEndProgram();
							return -1;
						}
						// *****************************************************
					}
					
				}
				else if (STRINGS_ARE_EQUAL(MSG_type, "GAME_STATE_REPLY"))
				{
					if (STRINGS_ARE_EQUAL(param1, "0"))
					{
						printf("Game has not started\n");
						// *****************************************************
						wait_res = WaitForSingleObject(write_to_file, INFINITE);
						if (wait_res != WAIT_OBJECT_0)
						{
							ReportErrorAndEndProgram();
							return -1;
						}
						PrintToLogFile("Game has not started\n", path);
						release_res = ReleaseMutex(write_to_file);
						if (release_res == FALSE)
						{
							ReportErrorAndEndProgram();
							return -1;
						}
						// *****************************************************
					}
					else if (STRINGS_ARE_EQUAL(param1, "1"))
					{
						printf("%s's turn (%s)\n", param2, param3);
						// *****************************************************
						wait_res = WaitForSingleObject(write_to_file, INFINITE);
						if (wait_res != WAIT_OBJECT_0)
						{
							ReportErrorAndEndProgram();
							return -1;
						}
						PrintToLogFile(param2, path);
						PrintToLogFile("'s turn (", path);
						PrintToLogFile(param3, path);
						PrintToLogFile(")\n", path);
						release_res = ReleaseMutex(write_to_file);
						if (release_res == FALSE)
						{
							ReportErrorAndEndProgram();
							return -1;
						}
						// *****************************************************
					}


				}
				else if (STRINGS_ARE_EQUAL(MSG_type, "BOARD_VIEW_REPLY"))
				{
					if (param1 != NULL)
					{
						print_board(param1);
					}
					else
					{
						printf("Error: Invalid server message. Exiting.\n");
						// *****************************************************
						wait_res = WaitForSingleObject(write_to_file, INFINITE);
						if (wait_res != WAIT_OBJECT_0)
						{
							ReportErrorAndEndProgram();
							return -1;
						}
						PrintToLogFile("Error: Invalid server message. Exiting.\n", path);
						release_res = ReleaseMutex(write_to_file);
						if (release_res == FALSE)
						{
							ReportErrorAndEndProgram();
							return -1;
						}
						// *****************************************************
					}
				}
				else if (STRINGS_ARE_EQUAL(MSG_type, "PLAY_ACCEPTED"))
				{
					printf("Well played\n");
					// *****************************************************
					wait_res = WaitForSingleObject(write_to_file, INFINITE);
					if (wait_res != WAIT_OBJECT_0)
					{
						ReportErrorAndEndProgram();
						return -1;
					}
					PrintToLogFile("Well played\n", path);
					release_res = ReleaseMutex(write_to_file);
					if (release_res == FALSE)
					{
						ReportErrorAndEndProgram();
						return -1;
					}
					// *****************************************************
				}
				else if (STRINGS_ARE_EQUAL(MSG_type, "PLAY_DECLINED"))
				{
					printf("Error: %s\n", param1);
					// *****************************************************
					wait_res = WaitForSingleObject(write_to_file, INFINITE);
					if (wait_res != WAIT_OBJECT_0)
					{
						ReportErrorAndEndProgram();
						return -1;
					}
					PrintToLogFile("Error: ", path);
					PrintToLogFile(param1, path);
					PrintToLogFile("\n", path);
					release_res = ReleaseMutex(write_to_file);
					if (release_res == FALSE)
					{
						ReportErrorAndEndProgram();
						return -1;
					}
					// *****************************************************
				}
				else if (STRINGS_ARE_EQUAL(MSG_type, "GAME_STARTED"))
				{
					printf("Game is on!\n");
					// *****************************************************
					wait_res = WaitForSingleObject(write_to_file, INFINITE);
					if (wait_res != WAIT_OBJECT_0)
					{
						ReportErrorAndEndProgram();
						return -1;
					}
					PrintToLogFile("Game is on!\n", path);
					release_res = ReleaseMutex(write_to_file);
					if (release_res == FALSE)
					{
						ReportErrorAndEndProgram();
						return -1;
					}
					// *****************************************************
				}
				else if (STRINGS_ARE_EQUAL(MSG_type, "TURN_SWITCH"))
				{
					printf("%s's turn (%s)\n", param1, param2);
					// *****************************************************
					wait_res = WaitForSingleObject(write_to_file, INFINITE);
					if (wait_res != WAIT_OBJECT_0)
					{
						ReportErrorAndEndProgram();
						return -1;
					}
					PrintToLogFile(param1, path);
					PrintToLogFile("'s turn (", path);
					PrintToLogFile(param2, path);
					PrintToLogFile(")\n", path);
					release_res = ReleaseMutex(write_to_file);
					if (release_res == FALSE)
					{
						ReportErrorAndEndProgram();
						return -1;
					}
					// *****************************************************
				}
				else if (STRINGS_ARE_EQUAL(MSG_type, "BOARD_VIEW"))
				{
					if (param1 != NULL)
					{
						print_board(param1);
					}
					else
					{
						printf("Error: Invalid server message. Exiting.\n");
						// *****************************************************
						wait_res = WaitForSingleObject(write_to_file, INFINITE);
						if (wait_res != WAIT_OBJECT_0)
						{
							ReportErrorAndEndProgram();
							return -1;
						}
						PrintToLogFile("Error: Invalid server message. Exiting.\n", path);
						release_res = ReleaseMutex(write_to_file);
						if (release_res == FALSE)
						{
							ReportErrorAndEndProgram();
							return -1;
						}
						// *****************************************************
					}
				}
				else if (STRINGS_ARE_EQUAL(MSG_type, "GAME_ENDED"))
				{
					if (param1 == NULL || ((STRINGS_ARE_EQUAL(param1, "1")!=1) && param2 == NULL)) {
						// *****************************************************
						wait_res = WaitForSingleObject(write_to_file, INFINITE);
						if (wait_res != WAIT_OBJECT_0)
						{
							ReportErrorAndEndProgram();
							return -1;
						}
						PrintToLogFile("Custom message: GAME_ENDED was provided with wrong parms\n", path);
						release_res = ReleaseMutex(write_to_file);
						if (release_res == FALSE)
						{
							ReportErrorAndEndProgram();
							return -1;
						}
						// *****************************************************
						return -1;
					}
					else if (STRINGS_ARE_EQUAL(param1, "1"))
					{
						printf("Game ended. Everybody wins!\n");
						// *****************************************************
						wait_res = WaitForSingleObject(write_to_file, INFINITE);
						if (wait_res != WAIT_OBJECT_0)
						{
							ReportErrorAndEndProgram();
							return -1;
						}
						PrintToLogFile("Game ended. Everybody wins!\n", path);
						release_res = ReleaseMutex(write_to_file);
						if (release_res == FALSE)
						{
							ReportErrorAndEndProgram();
							return -1;
						}
						// *****************************************************
						return 0;
					}
					else if (STRINGS_ARE_EQUAL(param1, "0"))
					{
						printf("Game ended. The winner is %s!\n", param2);
						// *****************************************************
						wait_res = WaitForSingleObject(write_to_file, INFINITE);
						if (wait_res != WAIT_OBJECT_0)
						{
							ReportErrorAndEndProgram();
							return -1;
						}
						PrintToLogFile("Game ended. The winner is ", path);
						PrintToLogFile(param2, path);
						PrintToLogFile("!\n", path);
						release_res = ReleaseMutex(write_to_file);
						if (release_res == FALSE)
						{
							ReportErrorAndEndProgram();
							return -1;
						}
						// *****************************************************
						return 0;
					}

				}
				else {
					printf("Error: Invalid server message. Exiting.\n");
					// *****************************************************
					wait_res = WaitForSingleObject(write_to_file, INFINITE);
					if (wait_res != WAIT_OBJECT_0)
					{
						ReportErrorAndEndProgram();
						return -1;
					}
					PrintToLogFile("Error: Invalid server message. Exiting.\n", path);
					release_res = ReleaseMutex(write_to_file);
					if (release_res == FALSE)
					{
						ReportErrorAndEndProgram();
						return -1;
					}
					// *****************************************************
					return -1;
				}
			}
			else
			{
				printf("Error: Invalid server message. Exiting.\n");
				// *****************************************************
				wait_res = WaitForSingleObject(write_to_file, INFINITE);
				if (wait_res != WAIT_OBJECT_0)
				{
					ReportErrorAndEndProgram();
					return -1;
				}
				PrintToLogFile("Error: Invalid server message. Exiting.\n", path);
				release_res = ReleaseMutex(write_to_file);
				if (release_res == FALSE)
				{
					ReportErrorAndEndProgram();
					return -1;
				}
				// *****************************************************
				return -1;
			}
		}

		free(AcceptedStr);
	}

	return 0;
}

//===================================================================================//

// Handling messeges: getting the input from the user, generating the right messege and sending it to the server
static DWORD MsgThread(LPVOID lpParam)
{
	TransferResult_t SendRes;
	MsgThreadParms *parms;
	parms = (MsgThreadParms *)lpParam;
	char *first_msg = NULL;

	// The first operation of the client is to send a message that contains the username of the client,
	//so it happens automatically as the message thread starts up ****************************************
	cnctnt("NEW_USER_REQUEST:", parms->username, &first_msg);
	cnctnt(first_msg, "\n", &first_msg);
	SendRes = SendString(first_msg, m_socket);
	// *****************************************************
	wait_res = WaitForSingleObject(write_to_file, INFINITE);
	if (wait_res != WAIT_OBJECT_0) 
	{
		ReportErrorAndEndProgram();
		return -1;
	}
	PrintToLogFile("Sent to server: ", parms->path);
	PrintToLogFile(first_msg, parms->path);
	release_res = ReleaseMutex(write_to_file);
	if (release_res == FALSE) 
	{
		ReportErrorAndEndProgram();
		return -1;
	}
	// *****************************************************
	if (SendRes == TRNS_FAILED)
	{
		printf("Socket error while trying to write data to socket\n");
		return 0x555;
	}
	// ***************************************************************************************************
	while (1) 
	{
		wait_res = WaitForSingleObject(h_message, INFINITE);
		if (wait_res != WAIT_OBJECT_0) 
		{
			ReportErrorAndEndProgram();
			return -1;
		}
		
		char *p_SendMsg = NULL;
		gnrt_msg_rslt = generate_msg(SendStr, &p_SendMsg);
		if (gnrt_msg_rslt == 0)
		{
			// *****************************************************
			wait_res = WaitForSingleObject(write_to_file, INFINITE);
			if (wait_res != WAIT_OBJECT_0) 
			{
				ReportErrorAndEndProgram();
				return -1;
			}
			PrintToLogFile("Sent to server: ", parms->path);
			PrintToLogFile(p_SendMsg, parms->path);
			release_res = ReleaseMutex(write_to_file);
			if (release_res == FALSE) 
			{
				ReportErrorAndEndProgram();
				return -1;
			}
			// *****************************************************
			SendRes = SendString(p_SendMsg, m_socket);
			if (SendRes == TRNS_FAILED)
			{
				// Takes care of the requirement that the client prints an error if the user asks for the state when the game had already finished
				if (STRINGS_ARE_EQUAL(SendStr, "state"))  
				{
					printf("Error: Game has already ended\n");
					// *****************************************************
					wait_res = WaitForSingleObject(write_to_file, INFINITE);
					if (wait_res != WAIT_OBJECT_0)
					{
						ReportErrorAndEndProgram();
						return -1;
					}
					PrintToLogFile("Error: Game has already ended\n", parms->path);
					release_res = ReleaseMutex(write_to_file);
					if (release_res == FALSE)
					{
						ReportErrorAndEndProgram();
						return -1;
					}
					// *****************************************************
					return 0x555;
				}
				else {
					printf("Socket error while trying to write data to socket\n");
					return 0x555;
				}

			}
		}
		else if (gnrt_msg_rslt == 1)
		{
			release_res = ReleaseSemaphore(
				h_input,
				1,
				NULL);
			if (release_res == FALSE) 
			{
				ReportErrorAndEndProgram();
				return -1;
			}
			return 0x555;
		}
		else if (gnrt_msg_rslt == -1)
		{

			// *****************************************************
			wait_res = WaitForSingleObject(write_to_file, INFINITE);
			if (wait_res != WAIT_OBJECT_0) 
			{
				ReportErrorAndEndProgram();
				return -1;
			}
			PrintToLogFile("Error: Illegal command\n", parms->path);
			release_res = ReleaseMutex(write_to_file);
			if (release_res == FALSE) 
			{
				ReportErrorAndEndProgram();
				return -1;
			}
			// *****************************************************
		}
		

		release_res = ReleaseSemaphore(
			h_input,
			1, 		
			NULL);
		if (release_res == FALSE) 
		{
			ReportErrorAndEndProgram();
			return -1;
		}
	}
}

//===================================================================================//

// Handles getting the input from the user
static DWORD InputThread(void)
{

	while (gnrt_msg_rslt != 1)
	{	
		gets_s(SendStr, sizeof(SendStr)); //Reading a string from the keyboard

		release_res = ReleaseSemaphore(
			h_message,
			1, 		/* Signal that exactly one cell was filled */
			NULL);
		if (release_res == FALSE) 
		{
			ReportErrorAndEndProgram();
			return -1;
		}

		wait_res = WaitForSingleObject(h_input, INFINITE);
		if (wait_res != WAIT_OBJECT_0) 
		{
			ReportErrorAndEndProgram();
			return -1;
		}
	}
	return 0x555;
}

//===================================================================================//

int MainClient(char *path, char *server_ip, char *server_port_char, char *username)
{

	SOCKADDR_IN clientService;
	HANDLE hThread[3];
	MsgThreadParms *parms;
	write_to_file = CreateMutex(
		NULL,   /* default security attributes */
		FALSE,	/* don't lock mutex immediately */
		NULL); /* un-named */
	if (write_to_file == NULL)
	{
		CloseHandle(write_to_file);
		return -1;
	}
	parms = (MsgThreadParms *)malloc(sizeof(MsgThreadParms));
	parms->path = path;
	parms->username = username;
	if (NULL == parms)
	{
		printf("Error when allocating memory\n");
		return -1;
	}

	// Initialize Winsock.
	WSADATA wsaData; 
					

					 //Call WSAStartup and check for errors.
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != NO_ERROR)
		printf("Error at WSAStartup()\n");

	//Call the socket function and return its value to the m_socket variable. 

	// Create a socket.
	m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	// Check for errors to ensure that the socket is a valid socket.
	if (m_socket == INVALID_SOCKET) {
		printf("Error at socket(): %ld\n", WSAGetLastError());
		WSACleanup();
		return -1;
	}
	


	//Create a sockaddr_in object clientService and set  values.
	int server_port_int = atoi(server_port_char);
	clientService.sin_family = AF_INET;
	clientService.sin_addr.s_addr = inet_addr(server_ip); //Setting the IP address to connect to
	clientService.sin_port = htons(server_port_int); //Setting the port to connect to.

												


												 // Call the connect function, passing the created socket and the sockaddr_in structure as parameters. 
												 // Check for general errors.
	if (connect(m_socket, (SOCKADDR*)&clientService, sizeof(clientService)) == SOCKET_ERROR) {
		printf("Failed connecting to server on %s:%s. Exiting.\n",server_ip,server_port_char);
		// *****************************************************
		wait_res = WaitForSingleObject(write_to_file, INFINITE);
		if (wait_res != WAIT_OBJECT_0) 
		{
			ReportErrorAndEndProgram();
			return -1;
		}
		PrintToLogFile("Failed connecting to server on ", path);
		PrintToLogFile(server_ip, path);
		PrintToLogFile(":", path);
		PrintToLogFile(server_port_char, path);
		PrintToLogFile(". Exiting.\n", path);
		release_res = ReleaseMutex(write_to_file);
		if (release_res == FALSE)
		{
			ReportErrorAndEndProgram();
			return -1;
		}
		// *****************************************************

		WSACleanup();
		return -1;
	} 
	else {
		printf("Connected to server on %s:%s\n", server_ip, server_port_char);
		// *****************************************************
		wait_res = WaitForSingleObject(write_to_file, INFINITE);
		if (wait_res != WAIT_OBJECT_0)
		{
			ReportErrorAndEndProgram();
			return -1;
		}
		PrintToLogFile("Connected to server on ", path);
		PrintToLogFile(server_ip, path);
		PrintToLogFile(":", path);
		PrintToLogFile(server_port_char, path);
		PrintToLogFile("\n", path);
		release_res = ReleaseMutex(write_to_file);
		if (release_res == FALSE)
		{
			ReportErrorAndEndProgram();
			return -1;
		}
		// *****************************************************
	}


	/* Create two semaphores in order to synchronize two threads:
	InputThread: gets the input from the users keyboard and stores it in a global variable.
	MsgThread: takes the users' input, then generates and sends the appropriate message to the server.
	We synchronzie the threads in such a way that MsgThread starts taking the info fron the gloabl variable only after InputThread 
	had signaled that it has stored the info. */

	h_message = CreateSemaphore(
		NULL,	
		0,		
		1,		
		NULL); 
	if (h_message == NULL)
	{
		CloseHandle(h_message);
		return -1;
	}
	h_input = CreateSemaphore(
		NULL,	
		0,		
		1,		
		NULL); 
	if (h_input == NULL)
	{
		CloseHandle(h_input);
		return -1;
	}


	/* The three threads we were asked to create:
	As mentioned above, including: RecvDataThread - A thread that recieves data from the server 	*/


	hThread[0] = CreateThread(
		NULL,
		0,
		(LPTHREAD_START_ROUTINE)MsgThread,
		parms,
		0,
		NULL
	);
	if (hThread[0] == NULL)
	{
		CloseHandle(h_input);
		return -1;
	}
	hThread[1] = CreateThread(
		NULL,
		0,
		(LPTHREAD_START_ROUTINE)InputThread,
		NULL,
		0,
		NULL
	);
	if (hThread[1] == NULL)
	{
		CloseHandle(h_input);
		return -1;
	}
	hThread[2] = CreateThread(
		NULL,
		0,
		(LPTHREAD_START_ROUTINE)RecvDataThread,
		path,
		0,
		NULL
	);
	if (hThread[2] == NULL)
	{
		CloseHandle(h_input);
		return -1;
	}



	WaitForMultipleObjects(3, hThread, FALSE, INFINITE);

	TerminateThread(hThread[0], 0x555);
	TerminateThread(hThread[1], 0x555);
	TerminateThread(hThread[2], 0x555);

	CloseHandle(hThread[0]);
	CloseHandle(hThread[1]);
	CloseHandle(hThread[2]);
	CloseHandle(h_message);
	CloseHandle(h_input);
	CloseHandle(write_to_file);

	closesocket(m_socket);

	WSACleanup();

	return 0;
}

//===================================================================================//
int PrintToLogFile(char *p_msg, char *path)  // input params: a pointer to a string and a path-string
{

	errno_t retval;
	FILE *p_stream;

	//printf("path is: %s\n\n", path);
	retval = fopen_s(&p_stream, path, "a");
	if (0 != retval)
	{
		printf("Failed to open file.\n");
		return -1;
	}
	// Write lines
	retval = fputs(p_msg, p_stream);
	if (0 > retval)
	{
		printf("Failed to write to file.\n");
		// Don't return. Try and close the file first.
	}
	// Close file
	retval = fclose(p_stream);
	if (0 != retval)
	{
		printf("Failed to close file.\n");
		return -1;
	}
	return 0;
}

//===================================================================================//
static void ReportErrorAndEndProgram()
{
	printf("PrintToRePortFile error, ending program. Last Error = 0x%x\n", GetLastError());
	return;
}

//===================================================================================//
int cnctnt(char *source1, char *source2, char **p_dest)
{
	int i, j;
	size_t size1 = strlen(source1);
	size_t size2 = strlen(source2);
	size_t size3 = sizeof(char)*(size1 + size2);
	char *dest = (char*)calloc(size3+1,sizeof(char));
	if (dest == NULL) {
		return -1;
	}
	for (i = 0; i < size1; ++i)
	{
		dest[i] = source1[i];	}
	for (j = 0; j < size2; ++j)
	{
		dest[i+j] = source2[j];
	}
	dest[i + j] = '\0';
	*p_dest = dest;
	return 0;
}

//===================================================================================//
int ParseMessage(char *AcceptedStr, char **MessageType, char **param1, char **param2, char **param3) {
	int message_type_end_place = find_char(AcceptedStr, ':', 0);
	int end_of_message = find_char(AcceptedStr, '\n', 0);
	//printf("AcceptedStr %s\n", AcceptedStr);
	//printf("end_of_message %d\n", end_of_message);
	//printf("message_type_end_place  %d\n", message_type_end_place);

	if ((end_of_message == -1) || ((message_type_end_place != -1) && (message_type_end_place > end_of_message))) {
		//check the messeage string fits the protocol
		return -1;
	}
	if (message_type_end_place == -1) {//no params in message
		message_type_end_place = end_of_message;
	}
	char *c_MessageType = malloc(sizeof(char)*message_type_end_place);
	int i;
	for (i = 0; i < message_type_end_place; i++) {
		c_MessageType[i] = AcceptedStr[i];
	}
	c_MessageType[i] = '\0';
	*MessageType = c_MessageType;
	if (find_char(AcceptedStr, ':', 0) == -1) {//no params in message
		return 0;
	}
	char *c_param1;
	int len_of_param1;
	int param1_end_place = find_char(AcceptedStr, ';', 0);
	if (param1_end_place == -1) {//only 1 param in the msg
		param1_end_place = find_char(AcceptedStr, '\n', 0);
	}
	len_of_param1 = param1_end_place - message_type_end_place;
	c_param1 = malloc((len_of_param1) * sizeof(char));
	for (i = message_type_end_place + 1; i < param1_end_place; i++) {
		c_param1[i - message_type_end_place - 1] = AcceptedStr[i];
	}
	c_param1[i - message_type_end_place - 1] = '\0';
	*param1 = c_param1;
	if (find_char(AcceptedStr, ';', 0) == -1) {
		return 0;
	}
	char *c_param2;
	int len_of_param2;
	int param2_end_place = find_char(AcceptedStr, ';', param1_end_place + 1);
	if (param2_end_place == -1) {
		param2_end_place = find_char(AcceptedStr, '\n', 0);
	}
	len_of_param2 = param2_end_place - param1_end_place;
	c_param2 = malloc((len_of_param2) * sizeof(char));
	for (i = param1_end_place + 1; i < param2_end_place; i++) {
		c_param2[i - param1_end_place - 1] = AcceptedStr[i];
	}
	c_param2[i - param1_end_place - 1] = '\0';
	*param2 = c_param2;
	if (find_char(AcceptedStr, ';', param1_end_place + 1) == -1) {
		return 0;
	}
	char *c_param3;
	int len_of_param3;
	int param3_end_place = find_char(AcceptedStr, ';', param2_end_place + 1);
	if (param3_end_place == -1) {
		param3_end_place = find_char(AcceptedStr, '\n', 0);
	}
	len_of_param3 = param3_end_place - param2_end_place;
	c_param3 = malloc((len_of_param3) * sizeof(char));
	for (i = param2_end_place + 1; i < param3_end_place; i++) {
		c_param3[i - param2_end_place - 1] = AcceptedStr[i];
	}
	c_param3[i - param2_end_place - 1] = '\0';
	*param3 = c_param3;
	return 0;
}

//===================================================================================//
int find_char(char *string, char c, int start_from) {
	int i = start_from;
	int found = 0;
	while (!found)
	{
		if (string[i] == c) {
			found = 1;
			return i;
		}
		if (i > strlen(string)) {
			return -1;
		}
		i++;
	}
	return -1;
}

//===================================================================================//
int generate_msg(char *input, char **output)
{
	if (STRINGS_ARE_EQUAL(input, "players"))
	{
		*output = "USER_LIST_QUERY\n";
		return 0;
	}
	else if (STRINGS_ARE_EQUAL(input, "state"))
	{
		*output = "GAME_STATE_QUERY\n";
		return 0;
	}
	else if (STRINGS_ARE_EQUAL(input, "board"))
	{
		*output = "BOARD_VIEW_QUERY\n";
		return 0;
	}
	else if (STRINGS_ARE_EQUAL(input, "play 1 1"))
	{
		*output = "PLAY_REQUEST:1;1\n";
		return 0;
	}
	else if (STRINGS_ARE_EQUAL(input, "play 1 2"))
	{
		*output = "PLAY_REQUEST:1;2\n";
		return 0;
	}
	else if (STRINGS_ARE_EQUAL(input, "play 1 3"))
	{
		*output = "PLAY_REQUEST:1;3\n";
		return 0;
	}
	else if (STRINGS_ARE_EQUAL(input, "play 2 1"))
	{
		*output = "PLAY_REQUEST:2;1\n";
		return 0;
	}
	else if (STRINGS_ARE_EQUAL(input, "play 2 2"))
	{
		*output = "PLAY_REQUEST:2;2\n";
		return 0;
	}
	else if (STRINGS_ARE_EQUAL(input, "play 2 3"))
	{
		*output = "PLAY_REQUEST:2;3\n";
		return 0;
	}
	else if (STRINGS_ARE_EQUAL(input, "play 3 1"))
	{
		*output = "PLAY_REQUEST:3;1\n";
		return 0;
	}
	else if (STRINGS_ARE_EQUAL(input, "play 3 2"))
	{
		*output = "PLAY_REQUEST:3;2\n";
		return 0;
	}
	else if (STRINGS_ARE_EQUAL(input, "play 3 3"))
	{
		*output = "PLAY_REQUEST:3;3\n";
		return 0;
	}
	else if (STRINGS_ARE_EQUAL(input, "exit"))
	{
		return 1;
	} 
	else 
	{
		printf("Error: Illegal command\n");
		return -1;
	}

}

//**********************************************************************************//

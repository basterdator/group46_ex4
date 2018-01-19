/* ==============================================
Introduction to Systems Programming
Winter 2017-2018
TEL-AVIV UNIVERSITY
Exercise 4
Uri Cohen                 302825807
Anton Chaplianka          310224209
============================================== */

//===================================================================================//
#define _WINSOCK_DEPRECATED_NO_WARNINGS


//===================================================================================//
#include <winsock2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SendRecvTools.h"
#include "ClientSocket.h"
#include "BoardTools.h"


//===================================================================================//
char SendStr[256];
int gnrt_msg_rslt;
static HANDLE h_message;
static HANDLE h_input;
static HANDLE write_to_file;
static int ReportErrorAndEndProgram();
SOCKET m_socket;
static char *send_char;
static DWORD wait_res;
static BOOL release_res;


//===================================================================================//

//Reading data coming from the server
//===================================================================================//
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
			}
			PrintToLogFile("Server disconnected. Exiting.\n", path);
			release_res = ReleaseMutex(write_to_file);
			if (release_res == FALSE)
			{
				ReportErrorAndEndProgram();
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
			}
			PrintToLogFile("Server disconnected. Exiting.\n", path);
			release_res = ReleaseMutex(write_to_file);
			if (release_res == FALSE) 
			{
				ReportErrorAndEndProgram();
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
			}
			PrintToLogFile("Received from server: ", path);
			PrintToLogFile(AcceptedStr, path);
			release_res = ReleaseMutex(write_to_file);
			if (release_res == FALSE) 
			{
				ReportErrorAndEndProgram();
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
					}
					PrintToLogFile("Request to join was refused.\n", path);
					release_res = ReleaseMutex(write_to_file);
					if (release_res == FALSE)
					{
						ReportErrorAndEndProgram();
					}
					// *****************************************************
					return 0x555;
				}
				else if (STRINGS_ARE_EQUAL(MSG_type, "NEW_USER_ACCEPTED"))
				{
					if (param1 != NULL && param2 != NULL)
					{
						printf("Player accepted, you play %s, number of current players: %s\n", param1, param2);
					}
					else 
					{
						printf("Error: Invalid server message. Exiting.\n");
						wait_res = WaitForSingleObject(write_to_file, INFINITE);
						if (wait_res != WAIT_OBJECT_0)
						{
							ReportErrorAndEndProgram();
						}
						PrintToLogFile("Error: Invalid server message. Exiting.\n", path);

						if (release_res == FALSE)
						{
							ReportErrorAndEndProgram();
						}
						return -1;
					}
				}
				else if (STRINGS_ARE_EQUAL(MSG_type, "USER_LIST_REPLY"))
				{
					if (param1 != NULL && param2 == NULL) 
					{
						printf("Players: %s (x)\n", param1);
						// *****************************************************
						wait_res = WaitForSingleObject(write_to_file, INFINITE);
						if (wait_res != WAIT_OBJECT_0)
						{
							ReportErrorAndEndProgram();
						}
						PrintToLogFile("Players: ", path);
						PrintToLogFile(param1, path);
						PrintToLogFile(" (x)\n", path);
						release_res = ReleaseMutex(write_to_file);
						if (release_res == FALSE)
						{
							ReportErrorAndEndProgram();
						}
						// *****************************************************
					}
					else if (param1 != NULL && param2 != NULL) 
					{
						printf("Players: %s (x), %s (o)\n", param1, param2);
						// *****************************************************
						wait_res = WaitForSingleObject(write_to_file, INFINITE);
						if (wait_res != WAIT_OBJECT_0)
						{
							ReportErrorAndEndProgram();
						}
						PrintToLogFile("Players: ", path);
						PrintToLogFile(param1, path);
						PrintToLogFile(" (x) , ", path);
						PrintToLogFile(param2, path);
						PrintToLogFile(" (o)\n", path);
						release_res = ReleaseMutex(write_to_file);
						if (release_res == FALSE)
						{
							ReportErrorAndEndProgram();
						}
						// *****************************************************
					}
					else {
						printf("Error: Invalid server message. Exiting.\n");
						wait_res = WaitForSingleObject(write_to_file, INFINITE);
						if (wait_res != WAIT_OBJECT_0)
						{
							ReportErrorAndEndProgram();
						}
						PrintToLogFile("Error: Invalid server message. Exiting.\n", path);
						if (release_res == FALSE)
						{
							ReportErrorAndEndProgram();
						}
						return -1;
					}
					
				}
				else if (STRINGS_ARE_EQUAL(MSG_type, "GAME_STATE_REPLY"))
				{
					if (param1 != NULL) 
					{
						if (STRINGS_ARE_EQUAL(param1, "0"))
						{
							printf("Game has not started\n");
							// *****************************************************
							wait_res = WaitForSingleObject(write_to_file, INFINITE);
							if (wait_res != WAIT_OBJECT_0)
							{
								ReportErrorAndEndProgram();
							}
							PrintToLogFile("Game has not started\n", path);
							release_res = ReleaseMutex(write_to_file);
							if (release_res == FALSE)
							{
								ReportErrorAndEndProgram();
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
							}
							PrintToLogFile(param2, path);
							PrintToLogFile("'s turn (", path);
							PrintToLogFile(param3, path);
							PrintToLogFile(")\n", path);
							release_res = ReleaseMutex(write_to_file);
							if (release_res == FALSE)
							{
								ReportErrorAndEndProgram();
							}
							// *****************************************************
						}
					}
					else
					{
						printf("Error: Invalid server message. Exiting.\n");
						wait_res = WaitForSingleObject(write_to_file, INFINITE);
						if (wait_res != WAIT_OBJECT_0)
						{
							ReportErrorAndEndProgram();
						}
						PrintToLogFile("Error: Invalid server message. Exiting.\n", path);
						if (release_res == FALSE)
						{
							ReportErrorAndEndProgram();
						}
						return -1;
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
						}
						PrintToLogFile("Error: Invalid server message. Exiting.\n", path);
						release_res = ReleaseMutex(write_to_file);
						if (release_res == FALSE)
						{
							ReportErrorAndEndProgram();
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
					}
					PrintToLogFile("Well played\n", path);
					release_res = ReleaseMutex(write_to_file);
					if (release_res == FALSE)
					{
						ReportErrorAndEndProgram();
					}
					// *****************************************************
				}
				else if (STRINGS_ARE_EQUAL(MSG_type, "PLAY_DECLINED"))
				{
					if (param1 != NULL)
					{
						printf("Error: %s\n", param1);
						// *****************************************************
						wait_res = WaitForSingleObject(write_to_file, INFINITE);
						if (wait_res != WAIT_OBJECT_0)
						{
							ReportErrorAndEndProgram();
						}
						PrintToLogFile("Error: ", path);
						PrintToLogFile(param1, path);
						PrintToLogFile("\n", path);
						release_res = ReleaseMutex(write_to_file);
						if (release_res == FALSE)
						{
							ReportErrorAndEndProgram();
						}
						// *****************************************************
					}
					else
					{
						printf("Error: Invalid server message. Exiting.\n");
						return -1;
					}
				}
				else if (STRINGS_ARE_EQUAL(MSG_type, "GAME_STARTED"))
				{
					printf("Game is on!\n");
					// *****************************************************
					wait_res = WaitForSingleObject(write_to_file, INFINITE);
					if (wait_res != WAIT_OBJECT_0)
					{
						ReportErrorAndEndProgram();
					}
					PrintToLogFile("Game is on!\n", path);
					release_res = ReleaseMutex(write_to_file);
					if (release_res == FALSE)
					{
						ReportErrorAndEndProgram();
					}
					// *****************************************************
				}
				else if (STRINGS_ARE_EQUAL(MSG_type, "TURN_SWITCH"))
				{
					if (param1 != NULL && param2 != NULL)
					{
						printf("%s's turn (%s)\n", param1, param2);
						// *****************************************************
						wait_res = WaitForSingleObject(write_to_file, INFINITE);
						if (wait_res != WAIT_OBJECT_0)
						{
							ReportErrorAndEndProgram();
						}
						PrintToLogFile(param1, path);
						PrintToLogFile("'s turn (", path);
						PrintToLogFile(param2, path);
						PrintToLogFile(")\n", path);
						release_res = ReleaseMutex(write_to_file);
						if (release_res == FALSE)
						{
							ReportErrorAndEndProgram();
						}
						// *****************************************************
					}
					else
					{
						printf("Error: Invalid server message. Exiting.\n");
						return -1;
					}
					
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
						}
						PrintToLogFile("Error: Invalid server message. Exiting.\n", path);
						release_res = ReleaseMutex(write_to_file);
						if (release_res == FALSE)
						{
							ReportErrorAndEndProgram();
						}
						// *****************************************************
					}
				}
				else if (STRINGS_ARE_EQUAL(MSG_type, "GAME_ENDED"))
				{
					if (param1 != NULL)
					{
						if (STRINGS_ARE_EQUAL(param1, "1"))
						{
						printf("Game ended. Everybody wins!\n");
						// *****************************************************
						wait_res = WaitForSingleObject(write_to_file, INFINITE);
						if (wait_res != WAIT_OBJECT_0)
						{
							ReportErrorAndEndProgram();
						}
						PrintToLogFile("Game ended. Everybody wins!\n", path);
						release_res = ReleaseMutex(write_to_file);
						if (release_res == FALSE)
						{
							ReportErrorAndEndProgram();
						}
						// *****************************************************
						return 0;
						}
						else if (STRINGS_ARE_EQUAL(param1, "0"))
						{
							if (param2 != NULL)
							{
									printf("Game ended. The winner is %s!\n", param2);
									// *****************************************************
									wait_res = WaitForSingleObject(write_to_file, INFINITE);
									if (wait_res != WAIT_OBJECT_0)
									{
										ReportErrorAndEndProgram();
									}
									PrintToLogFile("Game ended. The winner is ", path);
									PrintToLogFile(param2, path);
									PrintToLogFile("!\n", path);
									release_res = ReleaseMutex(write_to_file);
									if (release_res == FALSE)
									{
										ReportErrorAndEndProgram();
									}
									// *****************************************************
									return 0;
							}
							else
							{
								printf("Error: Invalid server message. Exiting.\n");
								// *****************************************************
								wait_res = WaitForSingleObject(write_to_file, INFINITE);
								if (wait_res != WAIT_OBJECT_0)
								{
									ReportErrorAndEndProgram();
								}
								PrintToLogFile("Error: Invalid server message. Exiting.\n", path);
								release_res = ReleaseMutex(write_to_file);
								if (release_res == FALSE)
								{
									ReportErrorAndEndProgram();
								}
								// *****************************************************
								return -1;
							}
						}
					}
					
				}
				else {   
					printf("Error: Invalid server message. Exiting.\n");
					// *****************************************************
					wait_res = WaitForSingleObject(write_to_file, INFINITE);
					if (wait_res != WAIT_OBJECT_0)
					{
						ReportErrorAndEndProgram();
					}
					PrintToLogFile("Error: Invalid server message. Exiting.\n", path);
					release_res = ReleaseMutex(write_to_file);
					if (release_res == FALSE)
					{
						ReportErrorAndEndProgram();
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
				}
				PrintToLogFile("Error: Invalid server message. Exiting.\n", path);
				release_res = ReleaseMutex(write_to_file);
				if (release_res == FALSE)
				{
					ReportErrorAndEndProgram();
				}
				// *****************************************************
				return -1;
			}
		}

		free(AcceptedStr);
	}

	return 0;
}


// Handling messeges: getting the input from the user, generating the right messege and sending it to the server
//===================================================================================//
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
	}
	PrintToLogFile("Sent to server: ", parms->path);
	PrintToLogFile(first_msg, parms->path);
	release_res = ReleaseMutex(write_to_file);
	if (release_res == FALSE) 
	{
		ReportErrorAndEndProgram();
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
			}
			PrintToLogFile("Sent to server: ", parms->path);
			PrintToLogFile(p_SendMsg, parms->path);
			release_res = ReleaseMutex(write_to_file);
			if (release_res == FALSE) 
			{
				ReportErrorAndEndProgram();
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
					}
					PrintToLogFile("Error: Game has already ended\n", parms->path);
					release_res = ReleaseMutex(write_to_file);
					if (release_res == FALSE)
					{
						ReportErrorAndEndProgram();
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
			}
			PrintToLogFile("Error: Illegal command\n", parms->path);
			release_res = ReleaseMutex(write_to_file);
			if (release_res == FALSE) 
			{
				ReportErrorAndEndProgram();
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
		}
	}
}


// Handles getting the input from the user
//===================================================================================//
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
		}

		wait_res = WaitForSingleObject(h_input, INFINITE);
		if (wait_res != WAIT_OBJECT_0) 
		{
			ReportErrorAndEndProgram();
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
		NULL,   
		FALSE,	
		NULL); 
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


// Prints the latest error message and returns -1
//===================================================================================//
static int ReportErrorAndEndProgram()
{
	printf("PrintToRePortFile error, ending program. Last Error = 0x%x\n", GetLastError());
	return -1;
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


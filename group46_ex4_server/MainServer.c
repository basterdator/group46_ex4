
//===================================================================================//
#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
//===================================================================================//

#include <stdio.h>
#include <string.h>
#include <winsock2.h>
#include <stdlib.h>
#include "SendRecvTools.h"
#include "ServiceThread.h"
//===================================================================================//
#define MAX_LOOPS 400
//===================================================================================//

HANDLE ThreadHandles[NUM_OF_WORKER_THREADS];
SOCKET ThreadInputs[NUM_OF_WORKER_THREADS];
static int FindFirstUnusedThreadSlot();
static void CleanupWorkerThreads();
int PrintToLogFile(char *p_msg);

char user_name1[USER_NAME_SIZE];
char user_name2[USER_NAME_SIZE];
char user_name_init[] = "0000";
char board[9];


HANDLE board_semaphore;
HANDLE write_to_file;
char *g_path;
char *g_server_ip;
char *g_server_port_char;
HANDLE AcceptThreadHandle;

DWORD AcceptConnections();
int active = 0;
//===================================================================================//
int MainServer(char *path, char *server_ip, char *server_port_char)
{
	g_path = path;
	g_server_ip = server_ip;
	g_server_port_char = server_port_char;
	strcpy(board, "000000000");
	strcpy(user_name1, user_name_init);
	strcpy(user_name2, user_name_init);

	board_semaphore = CreateSemaphore(
		NULL,
		1,
		1,
		NULL
	);

	if (board_semaphore == NULL) {
		CloseHandle(board_semaphore);
		return -1;
	}

	write_to_file = CreateMutex(
		NULL,   /* default security attributes */
		FALSE,	/* don't lock mutex immediately */
		NULL); /* un-named */
	if (write_to_file == NULL)
	{
		CloseHandle(write_to_file);
		return -1;
	}

	
	
		 AcceptThreadHandle = CreateThread(
			NULL,
			0,
			(LPTHREAD_START_ROUTINE)AcceptConnections,
			NULL,
			0,
			NULL
		);
		 DWORD Res = WaitForSingleObject(AcceptThreadHandle,300000);

		 if (active == 0) {
			 CloseHandle(AcceptThreadHandle);
			 return 0;
		 }

		 Res = WaitForSingleObject(AcceptThreadHandle, INFINITE);

		 CloseHandle(AcceptThreadHandle);
		 return 0;
}
//===================================================================================//

static int FindFirstUnusedThreadSlot()
{
	int Ind;

	for (Ind = 0; Ind < NUM_OF_WORKER_THREADS; Ind++)
	{
		if (ThreadHandles[Ind] == NULL)
			break;
		else
		{
			// poll to check if thread finished running:
			DWORD Res = WaitForSingleObject(ThreadHandles[Ind], 0);

			if (Res == WAIT_OBJECT_0) // this thread finished running
			{
				CloseHandle(ThreadHandles[Ind]);
				ThreadHandles[Ind] = NULL;
				break;
			}
		}
	}

	return Ind;
}

//===================================================================================//

static void CleanupWorkerThreads()
{
	int Ind;

	for (Ind = 0; Ind < NUM_OF_WORKER_THREADS; Ind++)
	{
		if (ThreadHandles[Ind] != NULL)
		{
			// poll to check if thread finished running:
			DWORD Res = WaitForSingleObject(ThreadHandles[Ind], INFINITE);

			if (Res == WAIT_OBJECT_0)
			{
				closesocket(ThreadInputs[Ind]);
				CloseHandle(ThreadHandles[Ind]);
				ThreadHandles[Ind] = NULL;
				break;
			}
			else
			{
				printf("Waiting for thread failed. Ending program\n");
				return;
			}
		}
	}
}
//===================================================================================//

int PrintToLogFile(char *p_msg)  // input params: a pointer to a string and a path-string
{

	errno_t retval;
	FILE *p_stream;
	char *path = g_path;
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

DWORD AcceptConnections()
{
	
	int Ind;
	int Loop;
	char *server_ip = g_server_ip;
	char *server_port_char = g_server_port_char;
	unsigned long Address;
	SOCKADDR_IN service;
	int bindRes;
	int ListenRes;  
	SOCKET MainSocket = INVALID_SOCKET;
	WSADATA wsaData;
	int StartupRes = WSAStartup(MAKEWORD(2, 2), &wsaData);

	if (StartupRes != NO_ERROR)
	{
		printf("error %ld at WSAStartup( ), ending program.\n", WSAGetLastError());
		// Tell the user that we could not find a usable WinSock DLL.                                  
		return 1;
	}

	MainSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (MainSocket == INVALID_SOCKET)
	{
		printf("Error at socket( ): %ld\n", WSAGetLastError());
		goto server_cleanup_1;
	}


	Address = inet_addr(server_ip);
	if (Address == INADDR_NONE)
	{
		printf("The string \"%s\" cannot be converted into an ip address. ending program.\n", server_ip);
		goto server_cleanup_2;
	}

	service.sin_family = AF_INET;
	service.sin_addr.s_addr = Address;
	int server_port_int = atoi(server_port_char);
	service.sin_port = htons(server_port_int); //The htons function converts a u_short from host to TCP/IP network byte order 
											   //( which is big-endian ).
											   /*
											   The three lines following the declaration of sockaddr_in service are used to set up
											   the sockaddr structure:
											   AF_INET is the Internet address family.
											   "127.0.0.1" is the local IP address to which the socket will be bound.
											   2345 is the port number to which the socket will be bound.
											   */

											   // Call the bind function, passing the created socket and the sockaddr_in structure as parameters. 
											   // Check for general errors.
	bindRes = bind(MainSocket, (SOCKADDR*)&service, sizeof(service));
	if (bindRes == SOCKET_ERROR)
	{
		printf("bind( ) failed with error %ld. Ending program\n", WSAGetLastError());
		goto server_cleanup_2;
	}

	// Listen on the Socket.
	printf("Server is on, waiting for connections...\n");
	ListenRes = listen(MainSocket, SOMAXCONN);
	if (ListenRes == SOCKET_ERROR)
	{
		PrintToLogFile("error: Failed listening on socket\n");
		printf("Failed listening on socket, error %ld.\n", WSAGetLastError());
		goto server_cleanup_2;
	}

	// Initialize all thread handles to NULL, to mark that they have not been initialized
	for (Ind = 0; Ind < NUM_OF_WORKER_THREADS; Ind++)
		ThreadHandles[Ind] = NULL;

	player_params player_params_for_thread;
	for (Loop = 0; Loop < MAX_LOOPS; Loop++)
	{
		SOCKET AcceptSocket = accept(MainSocket, NULL, NULL);
		active = 1;
		if (AcceptSocket == INVALID_SOCKET)
		{
			PrintToLogFile("error: Accepting connection with client failed\n");
			printf("Accepting connection with client failed, error %ld\n", WSAGetLastError());
			goto server_cleanup_3;
		}

		//printf("Client Connected.\n");

		Ind = FindFirstUnusedThreadSlot();

		if (Ind == NUM_OF_WORKER_THREADS) //no slot is available
		{
			PrintToLogFile("No slots available for client, dropping the connection.\n");
			printf("No slots available for client, dropping the connection.\n");
			closesocket(AcceptSocket); //Closing the socket, dropping the connection.
		}
		else
		{
			ThreadInputs[Ind] = AcceptSocket; // shallow copy: don't close 
											  // AcceptSocket, instead close 
											  // ThreadInputs[Ind] when the
											  // time comes.
			player_params_for_thread.player_num = Ind;
			player_params_for_thread.player_soket = &(ThreadInputs[Ind]);
			ThreadHandles[Ind] = CreateThread(
				NULL,
				0,
				(LPTHREAD_START_ROUTINE)ServiceThread,
				&player_params_for_thread,
				0,
				NULL
			);
		}

	} // for ( Loop = 0; Loop < MAX_LOOPS; Loop++ )

server_cleanup_3:

	CleanupWorkerThreads();

server_cleanup_2:
	if (closesocket(MainSocket) == SOCKET_ERROR)
		printf("Failed to close MainSocket, error %ld. Ending program\n", WSAGetLastError());

server_cleanup_1:
	if (WSACleanup() == SOCKET_ERROR)
		printf("Failed to close Winsocket, error %ld. Ending program.\n", WSAGetLastError());
	return 0;
}

//===================================================================================//

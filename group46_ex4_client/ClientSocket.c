/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/
/*
This file was written for instruction purposes for the
course "Introduction to Systems Programming" at Tel-Aviv
University, School of Electrical Engineering.
Last updated by Amnon Drory, Winter 2011.
*/
/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <winsock2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SocketShared.h"
#include "SendRecvTools.h"


/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/
char SendStr[256];
static HANDLE h_message;
static HANDLE h_input;
static void ReportErrorAndEndProgram();
int PrintToLogFile(char *p_msg, char *path);
SOCKET m_socket;
static char *send_char;

/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/

//Reading data coming from the server
static DWORD RecvDataThread(void)
{
	TransferResult_t RecvRes;

	while (1)
	{
		char *AcceptedStr = NULL;
		RecvRes = ReceiveString(&AcceptedStr, m_socket);

		if (RecvRes == TRNS_FAILED)
		{
			printf("Socket error while trying to write data to socket\n");
			return 0x555;
		}
		else if (RecvRes == TRNS_DISCONNECTED)
		{
			printf("Server closed connection. Bye!\n");
			return 0x555;
		}
		else
		{
			printf("%s\n", AcceptedStr);
		}

		free(AcceptedStr);
	}

	return 0;
}

/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/

//Sending data to the server
static DWORD MsgThread(LPVOID lpParam)
{
	DWORD wait_res;
	BOOL release_res;
	TransferResult_t SendRes;
	// The first operation of the client is to send a message that contains the username of the client,
	//so it happens automatically as the message thread starts up ****************************************
	char *username;
	username = (char *)lpParam;
	char *first_msg = NULL;
	cnctnt("NEW_USER_REQUEST:", "anton\n", &first_msg);
	//cnctnt(first_msg, "\n", &first_msg);
//	print_msg(first_msg);
//	printf("\n");
	SendRes = SendString("NEW_USER_REQUEST:anton\n", m_socket);
	if (SendRes == TRNS_FAILED)
	{
		printf("Socket error while trying to write data to socket\n");
		return 0x555;
	}
	// ***************************************************************************************************
	while (1) 
	{
		wait_res = WaitForSingleObject(h_message, INFINITE);
		if (wait_res != WAIT_OBJECT_0) ReportErrorAndEndProgram();

		SendRes = SendString(SendStr, m_socket);

		if (SendRes == TRNS_FAILED)
		{
			printf("Socket error while trying to write data to socket\n");
			return 0x555;
		}

		release_res = ReleaseSemaphore(
			h_input,
			1, 		
			NULL);
		if (release_res == FALSE) ReportErrorAndEndProgram();
	}
}

/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/

//Sending data to the server
static DWORD SendDataThread(void)
{
	DWORD wait_res;
	BOOL release_res;

	while (1)
	{	
		gets_s(SendStr, sizeof(SendStr)); //Reading a string from the keyboard

		release_res = ReleaseSemaphore(
			h_message,
			1, 		/* Signal that exactly one cell was filled */
			NULL);
		if (release_res == FALSE) ReportErrorAndEndProgram();

		wait_res = WaitForSingleObject(h_input, INFINITE);
		if (wait_res != WAIT_OBJECT_0) ReportErrorAndEndProgram();
	}
}

/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/

void MainClient(char *path, char *server_ip, char *server_port_char, char *username)
{
	SOCKADDR_IN clientService;
	HANDLE hThread[3];

	// Initialize Winsock.
	WSADATA wsaData; //Create a WSADATA object called wsaData.
					 //The WSADATA structure contains information about the Windows Sockets implementation.

					 //Call WSAStartup and check for errors.
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != NO_ERROR)
		printf("Error at WSAStartup()\n");

	//Call the socket function and return its value to the m_socket variable. 
	// For this application, use the Internet address family, streaming sockets, and the TCP/IP protocol.

	// Create a socket.
	m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	// Check for errors to ensure that the socket is a valid socket.
	if (m_socket == INVALID_SOCKET) {
		printf("Error at socket(): %ld\n", WSAGetLastError());
		WSACleanup();
		return;
	}
	/*
	The parameters passed to the socket function can be changed for different implementations.
	Error detection is a key part of successful networking code.
	If the socket call fails, it returns INVALID_SOCKET.
	The if statement in the previous code is used to catch any errors that may have occurred while creating
	the socket. WSAGetLastError returns an error number associated with the last error that occurred.
	*/


	//For a client to communicate on a network, it must connect to a server.
	// Connect to a server.

	//Create a sockaddr_in object clientService and set  values.
	int server_port_int = atoi(server_port_char);
	clientService.sin_family = AF_INET;
	clientService.sin_addr.s_addr = inet_addr(server_ip); //Setting the IP address to connect to
	clientService.sin_port = htons(server_port_int); //Setting the port to connect to.

												 /*
												 AF_INET is the Internet address family.
												 */


												 // Call the connect function, passing the created socket and the sockaddr_in structure as parameters. 
												 // Check for general errors.
	if (connect(m_socket, (SOCKADDR*)&clientService, sizeof(clientService)) == SOCKET_ERROR) {
		printf("Failed connecting to server on %s:%s\n",server_ip,server_port_char);
		PrintToLogFile("Failed connecting to server on ", path);
		PrintToLogFile(server_ip, path);
		PrintToLogFile(":", path);
		PrintToLogFile(server_port_char, path);
		PrintToLogFile("\n", path);
		WSACleanup();
		return;
	} 
	else {
		printf("Connected to server on %s:%s\n", server_ip, server_port_char);
		PrintToLogFile("Connected to server on ", path);
		PrintToLogFile(server_ip, path);
		PrintToLogFile(":", path);
		PrintToLogFile(server_port_char, path);
		PrintToLogFile("\n", path);

	}


	/* Create two semaphores in order to synchronize two threads:
	SendDataThread: gets the input from the users keyboard and stores it in a global variable.
	MsgThread: takes the users' input, then generates and sends the appropriate message to the server.
	We synchronzie the threads in such a way that MsgThread starts taking the info fron the gloabl variable only after SendDataThread 
	had signaled that it has stored the info. */

	h_message = CreateSemaphore(
		NULL,	
		0,		
		1,		
		NULL); 
	h_input = CreateSemaphore(
		NULL,	
		0,		
		1,		
		NULL); 

	/* The three threads we were asked to create:
	As mentioned above, including: RecvDataThread - A thread that recieves data from the server 	*/
	hThread[0] = CreateThread(
		NULL,
		0,
		(LPTHREAD_START_ROUTINE)RecvDataThread,
		NULL,
		0,
		NULL
	);

	hThread[1] = CreateThread(
		NULL,
		0,
		(LPTHREAD_START_ROUTINE)MsgThread,
		username,
		0,
		NULL
	);
	hThread[2] = CreateThread(
		NULL,
		0,
		(LPTHREAD_START_ROUTINE)SendDataThread,
		NULL,
		0,
		NULL
	);


	WaitForMultipleObjects(2, hThread, FALSE, INFINITE);

	TerminateThread(hThread[0], 0x555);
	TerminateThread(hThread[1], 0x555);
	TerminateThread(hThread[2], 0x555);

	CloseHandle(hThread[0]);
	CloseHandle(hThread[1]);
	CloseHandle(hThread[2]);

	closesocket(m_socket);

	WSACleanup();

	return;
}



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

static void ReportErrorAndEndProgram()
{
	printf("PrintToRePortFile error, ending program. Last Error = 0x%x\n", GetLastError());
	exit(1);
}

int cnctnt(char *source1, char *source2, char **p_dest)
{
	int i, j;
	int size1 = strlen(source1);
	int size2 = strlen(source2);
	int size3 = sizeof(char)*(size1 + size2);
	char *dest = (char*)calloc(size3+1,sizeof(char));
	if (dest == NULL) {
		return(-1);
	}
	for (i = 0; i < size1; ++i)
	{
		dest[i] = source1[i];
		//printf("%s\n", dest);
	}
	for (j = 0; j < size2; ++j)
	{
		dest[i+j] = source2[j];
		//printf("%s\n", dest);

	}
	dest[i + j] = '\0';
	*p_dest = dest;
	return 0;
}


void print_msg(char *msg)
{
	int i = 0;
	while (msg[i] != '\n')
	{
		printf("%c", msg[i]);
		++i;
	}
}

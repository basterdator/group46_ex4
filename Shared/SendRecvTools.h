/* ==============================================
Introduction to Systems Programming
Winter 2017-2018
TEL-AVIV UNIVERSITY
Exercise 4
Uri Cohen                 302825807
Anton Chaplianka          310224209
============================================== */

#ifndef SEND_RECV_TOOLS_H
#define SEND_RECV_TOOLS_H
//===================================================================================//
#include <string.h>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

//===================================================================================//
#define STRINGS_ARE_EQUAL( Str1, Str2 ) ( strcmp( (Str1), (Str2) ) == 0 )
//===================================================================================//
typedef enum { TRNS_FAILED, TRNS_DISCONNECTED, TRNS_SUCCEEDED } TransferResult_t;

/**
* SendBuffer() uses a socket to send a buffer.
*
* Accepts:
* -------
* Buffer - the buffer containing the data to be sent.
* BytesToSend - the number of bytes from the Buffer to send.
* sd - the socket used for communication.
*
* Returns:
* -------
* TRNS_SUCCEEDED - if sending succeeded
* TRNS_FAILED - otherwise
*/
TransferResult_t SendBuffer(const char* Buffer, int BytesToSend, SOCKET sd);

/**
* SendString() uses a socket to send a string.
* Str - the string to send.
* sd - the socket used for communication.
*/
TransferResult_t SendString(const char *Str, SOCKET sd);

/**
* Accepts:
* -------
* ReceiveBuffer() uses a socket to receive a buffer.
* OutputBuffer - pointer to a buffer into which data will be written
* OutputBufferSize - size in bytes of Output Buffer
* BytesReceivedPtr - output parameter. if function returns TRNS_SUCCEEDED, then this
*					  will point at an int containing the number of bytes received.
* sd - the socket used for communication.
*
* Returns:
* -------
* TRNS_SUCCEEDED - if receiving succeeded
* TRNS_DISCONNECTED - if the socket was disconnected
* TRNS_FAILED - otherwise
*/
TransferResult_t ReceiveBuffer(char* OutputBuffer, int RemainingBytesToReceive, SOCKET sd);

/**
* ReceiveString() uses a socket to receive a string, and stores it in dynamic memory.
*
* Accepts:
* -------
* OutputStrPtr - a pointer to a char-pointer that is initialized to NULL, as in:
*
*		char *Buffer = NULL;
*		ReceiveString( &Buffer, ___ );
*
* a dynamically allocated string will be created, and (*OutputStrPtr) will point to it.
*
* sd - the socket used for communication.
*
* Returns:
* -------
* TRNS_SUCCEEDED - if receiving and memory allocation succeeded
* TRNS_DISCONNECTED - if the socket was disconnected
* TRNS_FAILED - otherwise
*/
TransferResult_t ReceiveString(char** OutputStrPtr, SOCKET sd);

//===================================================================================//

/* ParseMessage() Recieves:
-----------
AcceptedStr - The input message of type string in the format: "<Message type>:<param1>;<param2>;<param3>\n;"
The function parses AcceptedStr and preaks it  down to it's elemets, then produces pointers to strings (output params):
MessageType, param1, param2, param3 (respectively)

NOTICE!: The number of parameters is not constant! the function is prepared to revieve up to 3 parameters but can handle less.*/
int ParseMessage(char *AcceptedStr, char **MessageType, char **param1, char **param2, char **param3);

/*find_char()  - finds the location in <string> of the in first char 'n' that satisfies that for every char <c> {'n' == <c>}, beginning from the index <start_from> */
int find_char(char *string, char c, int start_from);

#endif // SEND_RECV_TOOLS_H
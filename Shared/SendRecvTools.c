/* ==============================================
Introduction to Systems Programming
Winter 2017-2018
TEL-AVIV UNIVERSITY
Exercise 4
Uri Cohen                 302825807
Anton Chaplianka          310224209
============================================== */

//===================================================================================//
#include "SendRecvTools.h"
#include <stdio.h>
#include <string.h>
//===================================================================================//
TransferResult_t SendBuffer(const char* Buffer, int BytesToSend, SOCKET sd)
{
	const char* CurPlacePtr = Buffer;
	int BytesTransferred;
	int RemainingBytesToSend = BytesToSend;

	while (RemainingBytesToSend > 0)
	{
		BytesTransferred = send(sd, CurPlacePtr, RemainingBytesToSend, 0);
		if (BytesTransferred == SOCKET_ERROR)
		{
			printf("send() failed, error %d\n", WSAGetLastError());
			return TRNS_FAILED;
		}

		RemainingBytesToSend -= BytesTransferred;
		CurPlacePtr += BytesTransferred; // <ISP> pointer arithmetic
	}

	return TRNS_SUCCEEDED;
}

//===================================================================================//
TransferResult_t SendString(const char *Str, SOCKET sd)
{
	/* Send the the request to the server on socket sd */
	int TotalStringSizeInBytes;
	TransferResult_t SendRes;



	TotalStringSizeInBytes = (int)(strlen(Str)+1);
	//printf("SendString wants to send %d chars\n", TotalStringSizeInBytes);
	SendRes = SendBuffer(
		(const char *)(&TotalStringSizeInBytes),
		(int)(sizeof(TotalStringSizeInBytes)), // sizeof(int) 
		sd);

	if (SendRes != TRNS_SUCCEEDED) return SendRes;

	SendRes = SendBuffer(
		(const char *)(Str),
		(int)(TotalStringSizeInBytes),
		sd);

	return SendRes;
}

//===================================================================================//
TransferResult_t ReceiveBuffer(char* OutputBuffer, int BytesToReceive, SOCKET sd)
{
	char* CurPlacePtr = OutputBuffer;
	int BytesJustTransferred;
	int RemainingBytesToReceive = BytesToReceive;

	while (RemainingBytesToReceive > 0)
	{
		/* send does not guarantee that the entire message is sent */
		BytesJustTransferred = recv(sd, CurPlacePtr, RemainingBytesToReceive, 0);
		if (BytesJustTransferred == SOCKET_ERROR)
		{
			//printf("recv() failed, error %d\n", WSAGetLastError());
			return TRNS_FAILED;
		}
		else if (BytesJustTransferred == 0)
			return TRNS_DISCONNECTED; // recv() returns zero if connection was gracefully disconnected.

		RemainingBytesToReceive -= BytesJustTransferred;
		CurPlacePtr += BytesJustTransferred; // <ISP> pointer arithmetic
	}

	return TRNS_SUCCEEDED;
}

//===================================================================================//
TransferResult_t ReceiveString(char** OutputStrPtr, SOCKET sd)
{
	/* Recv the request to the server on socket sd */
	int TotalStringSizeInBytes;
	TransferResult_t RecvRes;
	char* StrBuffer = NULL;

	if ((OutputStrPtr == NULL) || (*OutputStrPtr != NULL))
	{
		printf("The first input to ReceiveString() must be "
			"a pointer to a char pointer that is initialized to NULL. For example:\n"
			"\tchar* Buffer = NULL;\n"
			"\tReceiveString( &Buffer, ___ )\n");
		return TRNS_FAILED;
	}


	RecvRes = ReceiveBuffer(
		(char *)(&TotalStringSizeInBytes),
		(int)(sizeof(TotalStringSizeInBytes)), // 4 bytes
		sd);

	if (RecvRes != TRNS_SUCCEEDED) return RecvRes;

	StrBuffer = (char*)calloc(TotalStringSizeInBytes,sizeof(char));

	if (StrBuffer == NULL)
		return TRNS_FAILED;

	RecvRes = ReceiveBuffer(
		(char *)(StrBuffer),
		(int)(TotalStringSizeInBytes),
		sd);

	if (RecvRes == TRNS_SUCCEEDED)
	{
		*OutputStrPtr = StrBuffer;
	}
	else
	{
		free(StrBuffer);
	}

	return RecvRes;
}

//===================================================================================//
int ParseMessage(char *AcceptedStr, char **MessageType, char **param1, char **param2, char **param3) {
	int message_type_end_place = find_char(AcceptedStr, ':', 0);
	int end_of_message = find_char(AcceptedStr, '\n', 0);

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
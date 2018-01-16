/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/
/*
This file was written for instruction purposes for the
course "Introduction to Systems Programming" at Tel-Aviv
University, School of Electrical Engineering, Winter 2011,
by Amnon Drory.
*/
/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/

#ifndef CLIENT_SOCKET_H
#define CLIENT_SOCKET_H

/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/
#include <stdio.h>
#include <string.h>

int MainClient(char *path, char *server_ip, char *server_port_char, char *username);
int PrintToLogFile(char *p_msg, char *path);
int cnctnt(char *source1, char *source2, char **p_dest);
int ParseMessage(char *AcceptedStr, char **MessageType, char **param1, char **param2, char **param3);
int find_char(char *string, char c, int start_from);
int generate_msg(char *input, char **output);


/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/

typedef struct 
{
	char *username;
	char *path;
}MsgThreadParms;




#endif // CLIENT_SOCKET_H
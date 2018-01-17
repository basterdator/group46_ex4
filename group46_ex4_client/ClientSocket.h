/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/

/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/

#ifndef CLIENT_SOCKET_H
#define CLIENT_SOCKET_H

/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/
#include <stdio.h>
#include <string.h>
/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/
/* ADD COMMENTS */
int MainClient(char *path, char *server_ip, char *server_port_char, char *username);
/* ADD COMMENTS */
int PrintToLogFile(char *p_msg, char *path);
/* ADD COMMENTS */
int cnctnt(char *source1, char *source2, char **p_dest);
/* ADD COMMENTS */
int ParseMessage(char *AcceptedStr, char **MessageType, char **param1, char **param2, char **param3);
/* ADD COMMENTS */
int find_char(char *string, char c, int start_from);
/* ADD COMMENTS */
int generate_msg(char *input, char **output);

/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/

/* ADD COMMENTS */
typedef struct 
{
	char *username;
	char *path;
}MsgThreadParms;

#endif // CLIENT_SOCKET_H
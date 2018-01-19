/* ==============================================
Introduction to Systems Programming
Winter 2017-2018
TEL-AVIV UNIVERSITY
Exercise 4
Uri Cohen                 302825807
Anton Chaplianka          310224209
============================================== */

#ifndef CLIENT_SOCKET_H
#define CLIENT_SOCKET_H

//===================================================================================//
#include <stdio.h>
#include <string.h>

//===================================================================================//
/* MainClient() recieves the input parms:
-----------------------------------------------
path - a string that contains the log file path
server_ip - a string that contains the server ip
server_port_char - a string that contains the server port
username - a string that contains the username
-----------------------------------------------
The function operates the flow of the client proccess, calls for the differant threads and handles the closing of the threads
Returns 0 if the flow was successful, and a non-zero code otherwise */
int MainClient(char *path, char *server_ip, char *server_port_char, char *username);

//===================================================================================//
/* PrintToLogFile() recieves the input params:
--------------------------------------------------
p_msg - a string with the message to write to the log file
path - a string that contains the path of the log file.
--------------------------------------------------
The function returns 0 if the write was successful and -1 otherwise */
int PrintToLogFile(char *p_msg, char *path);


//===================================================================================//
/* cnctnt() recieves the following parms:
--------------------------------------------------
source1 - a string (input parm)
source2 - another string (input parm)
p_dest - a pointer to a string (output parm)
--------------------------------------------------
The function points p_dest to a string which is a concatenation of source1 and source2
returns 0 if the operation was successful and -1 o.w. */
int cnctnt(char *source1, char *source2, char **p_dest);


//===================================================================================//
/* generate_msg() gets the parameters: 
--------------------------------------------------
input - a sting with the user input 
output - a pointer to a string to be written to (output parm)
--------------------------------------------------
The function writes the appropriate message to the output string and returns 0 if it was successful
The function prints that the command was illegal and returns -1 if the input string is an invalid command*/
int generate_msg(char *input, char **output);


//===================================================================================//
/* The struct we pass to MsgThread as a parameter*/
typedef struct 
{
	char *username;
	char *path;
}MsgThreadParms;


#endif // CLIENT_SOCKET_H
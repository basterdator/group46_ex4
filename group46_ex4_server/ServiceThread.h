/* ==============================================
Introduction to Systems Programming
Winter 2017-2018
TEL-AVIV UNIVERSITY
Exercise 4
Uri Cohen                 302825807
Anton Chaplianka          310224209
============================================== */
//===================================================================================//
#ifndef SERVICE_THREAD_H
#define SERVICE_THREAD_H


//===================================================================================//
#include <stdio.h>
#include <string.h>
#include <winsock2.h>
#include "SendRecvTools.h"


//===================================================================================//
#define USER_NAME_SIZE 30
#define NUM_OF_WORKER_THREADS 2


//===================================================================================//
typedef struct player_params_s{
	SOCKET *player_soket;
	int player_num;// player num is 0 for the first player and 1 for the second player
}player_params;


//===================================================================================//
DWORD ServiceThread(player_params *p_player_params);

#endif //SERVICE_THREAD_H

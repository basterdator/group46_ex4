/* ==============================================
Introduction to Systems Programming
Winter 2017-2018
TEL-AVIV UNIVERSITY
Exercise 4
Uri Cohen                 302825807
Anton Chaplianka          310224209
============================================== */
#ifndef BOARD_TOOLS_H
#define BOARD_TOOLS_H
//===================================================================================//
/* mark_board() gets the parameters:
------------
board - The input board string with '0' as blanks and 'x' and 'o' as marks (input parameter).
x - A char with the x coordinate to mark.
y - A char with the y coordinate to mark.
mark - A char with the mark to mark (x/o).
p_new_board - The address (&) of the marked board (output parameter).
------------
The function returns 0 if the mark was successful and -1 if it failed.
*/
int mark_board(char *board, char x, char y, char mark, char **p_new_board);


//===================================================================================//
/* he_won() gets the parameters:
------------
board - The input board string with '0' as blanks and 'x' and 'o' as marks (input parameter).
mark - A char with the mark to check whether he won or he didn't (x/o).
------------
The function returns 1 (to be used as TRUE in a condition) if he did, or 0 if he didn't.
It also returns -1 if the input board was invalid. */
int he_won(char *board, char mark);


//===================================================================================//
/*print_board() get the input parameter:
------------
board - The input board string with '0' as blanks and 'x' and 'o' as marks (input parameter).
------------
The function prints the board view to the screen.
Returns 0 if the input was correct (and thus the print was successful)
Returns 1 if the input wasn't a 9 char string.*/
int print_board(char *board);


//===================================================================================//
/*board_full() returns 0 if the board can no longer be marked and 1 if it still has free spaces 
The function will return -1 if the given string is not 9 chars long*/
int board_full(char *board);


#endif // BOARD_TOOLS_H
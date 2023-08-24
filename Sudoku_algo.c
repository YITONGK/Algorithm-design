/* Program to assist in the challenge of solving sudoku puzzles.

   Skeleton program written by Alistair Moffat, ammoffat@unimelb.edu.au,
   August 2021, with the intention that it be modified by students
   to add functionality, as required by the assignment specification.

   Student Authorship Declaration:

   (1) I certify that except for the code provided in the initial skeleton
   file, the  program contained in this submission is completely my own
   individual work, except where explicitly noted by further comments that
   provide details otherwise.  I understand that work that has been developed
   by another student, or by me in collaboration with other students, or by
   non-students as a result of request, solicitation, or payment, may not be
   submitted for assessment in this subject.  I understand that submitting for
   assessment work developed by or in collaboration with other students or
   non-students constitutes Academic Misconduct, and may be penalized by mark
   deductions, or by other penalties determined via the University of
   Melbourne Academic Honesty Policy, as described at
   https://academicintegrity.unimelb.edu.au.

   (2) I also certify that I have not provided a copy of this work in either
   softcopy or hardcopy or any other form to any other student, and nor will I
   do so until after the marks are released. I understand that providing my
   work to other students, regardless of my intention or any undertakings made
   to me by that other student, is also Academic Misconduct.

   (3) I further understand that providing a copy of the assignment
   specification to any form of code authoring or assignment tutoring service,
   or drawing the attention of others to such services and code that may have
   been made available via such a service, may be regarded as Student General
   Misconduct (interfering with the teaching activities of the University
   and/or inciting others to commit Academic Misconduct).  I understand that
   an allegation of Student General Misconduct may arise regardless of whether
   or not I personally make use of such solutions or sought benefit from such
   actions.

   Signed by: Yitong Kong 1254947
   Dated:     Sep 13, 2021

*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

/* these #defines provided as part of the initial skeleton */

#define NDIM 3		/* sudoku dimension, size of each inner square */
#define NDIG (NDIM*NDIM)
			/* total number of values in each row */
#define NGRP 3		/* number of sets each cell is a member of */
#define NSET (NGRP*NDIG)
			/* total number of sets in the sudoku */
#define NCLL (NDIG*NDIG)
			/* total number of cells in the sudoku */

#define ERROR	(-1)	/* error return value from some functions */

/* these global constant arrays provided as part of the initial skeleton,
   you may use them in your code but must not alter them in any way,
   regard them as being completely fixed. They describe the relationships
   between the cells in the sudoku and provide a basis for all of the
   sudoku processing loops */

/* there are 27 different different sets of elements that need to be
   checked against each other, this array converts set numbers to cells,
   that's why its called s2c */
int s2c[NSET][NDIM*NDIM] = {
	/* the first group of nine sets describe the sudoku's rows */
	{  0,  1,  2,  3,  4,  5,  6,  7,  8 },
	{  9, 10, 11, 12, 13, 14, 15, 16, 17 },
	{ 18, 19, 20, 21, 22, 23, 24, 25, 26 },
	{ 27, 28, 29, 30, 31, 32, 33, 34, 35 },
	{ 36, 37, 38, 39, 40, 41, 42, 43, 44 },
	{ 45, 46, 47, 48, 49, 50, 51, 52, 53 },
	{ 54, 55, 56, 57, 58, 59, 60, 61, 62 },
	{ 63, 64, 65, 66, 67, 68, 69, 70, 71 },
	{ 72, 73, 74, 75, 76, 77, 78, 79, 80 },
	/* the second group of nine sets describes the sudoku's columns */
	{  0,  9, 18, 27, 36, 45, 54, 63, 72 },
	{  1, 10, 19, 28, 37, 46, 55, 64, 73 },
	{  2, 11, 20, 29, 38, 47, 56, 65, 74 },
	{  3, 12, 21, 30, 39, 48, 57, 66, 75 },
	{  4, 13, 22, 31, 40, 49, 58, 67, 76 },
	{  5, 14, 23, 32, 41, 50, 59, 68, 77 },
	{  6, 15, 24, 33, 42, 51, 60, 69, 78 },
	{  7, 16, 25, 34, 43, 52, 61, 70, 79 },
	{  8, 17, 26, 35, 44, 53, 62, 71, 80 },
	/* the last group of nine sets describes the inner squares */
	{  0,  1,  2,  9, 10, 11, 18, 19, 20 },
	{  3,  4,  5, 12, 13, 14, 21, 22, 23 },
	{  6,  7,  8, 15, 16, 17, 24, 25, 26 },
	{ 27, 28, 29, 36, 37, 38, 45, 46, 47 },
	{ 30, 31, 32, 39, 40, 41, 48, 49, 50 },
	{ 33, 34, 35, 42, 43, 44, 51, 52, 53 },
	{ 54, 55, 56, 63, 64, 65, 72, 73, 74 },
	{ 57, 58, 59, 66, 67, 68, 75, 76, 77 },
	{ 60, 61, 62, 69, 70, 71, 78, 79, 80 },
};


/* there are 81 cells in a dimension-3 sudoku, and each cell is a
   member of three sets, this array gets filled by the function 
   fill_c2s(), based on the defined contents of the array s2c[][] */
int c2s[NCLL][NGRP];

void
fill_c2s() {
	int s=0, d=0, c;
	for ( ; s<NSET; s++) {
		/* record the first set number each cell is part of */
		for (c=0; c<NDIM*NDIM; c++) {
			c2s[s2c[s][c]][d] = s;
		}
		if ((s+1)%(NGRP*NDIM) == 0) {
			d++;
		}
	}
#if 0
	/* this code available here if you want to see the array
	   cs2[][] that gets created, just change that 0 two lines back
	   to a 1 and recompile */
	for (c=0; c<NCLL; c++) {
		printf("cell %2d: sets ", c);
		for (s=0; s<NGRP; s++) {
			printf("%3d", c2s[c][s]);
		}
		printf("\n");
	}
	printf("\n");
#endif
	return;
}

/* find the row number a cell is in, counting from 1
*/
int
rownum(int c) {
	return 1 + (c/(NDIM*NDIM));
}

/* find the column number a cell is in, counting from 1
*/
int
colnum(int c) {
	return 1 + (c%(NDIM*NDIM));
}

/* find the minor square number a cell is in, counting from 1
*/
int
sqrnum(int c) {
	return 1 + 3*(c/NSET) + (c/NDIM)%NDIM;
}

/* If you wish to add further #defines, put them below this comment,
   then prototypes for the functions that you add

   The only thing you should alter above this line is to complete the
   Authorship Declaration 
*/


/****************************************************************/


/****************************************************************/

/* 
This func is used to print out input sudoku according to required format.
*/
void
print(int sudoku[]){
	int i = 0, cell_unknown = 0;
	for(i = 0; i < NCLL; i++){
		// convert 0 to '.'
		if(sudoku[i] == 0){
			printf(".");
			cell_unknown++;
			if((i + 1) % NDIG != 0){
				printf(" ");
			}
			if((i + 1) % NDIM == 0 && (i + 1) % NDIG != 0){
				printf("| ");
			}
			if((i + 1) % NDIG == 0 && (i + 1) % (NDIM * NDIG) != 0){
				printf("\n");		
			}
			if((i + 1) % (NDIM * NDIG) == 0 && i != (NCLL - 1)){
				printf("\n------+-------+------\n");
			}
		}
		// lay out other null values 
		else{
			printf("%d", sudoku[i]);
			if((i + 1) % NDIG != 0){
				printf(" ");
			}
			if((i + 1) % NDIM == 0 && (i + 1) % NDIG != 0){
				printf("| ");
			}
			if((i + 1) % NDIG == 0 && (i + 1) % (NDIM * NDIG) != 0){
				printf("\n");		
			}
			if((i + 1) % (NDIM * NDIG) == 0 && i != (NCLL - 1)){
				printf("\n------+-------+------\n");
			}
		}
	}
	printf("\n\n%d cells are unknown\n", cell_unknown);
	return;
}

#define SETNUM_ROW_END 9
#define SETNUM_COL_END 18
#define SETNUM_SQR_END 27

int Invalid(int sudoku[], int set_num);
/*
The function will return 0 if the input sudoku is valid or correct,
otherwise, it will return the number of violations occurs in the sudoku.
*/
int dict_size = 10; 
/* dict_size is used to create an array to store the 
number of appearance for 0 to 9, therefore 10 spaces are needed.
Then we can use index to get the value in the array, let array work 
as a dictionary to see whether there are duplicates.
*/
int
Invalid(int sudoku[], int set_num){
	int dict[dict_size];
	int i, invalid = 0;
	int *index = s2c[set_num];
	for(i = 0; i < dict_size; i++){
		dict[i] = 0;
	}
	for(i = 0; i < NDIG; i++){
		dict[sudoku[index[i]]] += 1; 
		// assign the number of appearance of a certain number to 
		// the address of the relavant index
	}
	for(i = 1; i < dict_size; i++){
		if(dict[i] > 1){
			// (dict[i] > 1) means that i appears (dict[i]) times in the set
			// so there is a duplicate
			invalid += 1;
		}
	}
	return invalid;
}

/* function used to print duplicates for rows */
void
print_invalid_row(int sudoku[], int set_num){
	int dict[dict_size];
	int *index = s2c[set_num];
	int i, row_num;
	for(i = 0; i < dict_size; i++){
		dict[i] = 0;
	}
	for(i = 0; i < NDIG; i++){
		dict[sudoku[index[i]]] += 1;
	}
	for(i = 1; i < dict_size; i++){
		if(dict[i] > 1){
			row_num = set_num + 1;
			printf("\nset %2d (row %d): %d instances of %d", 
				set_num, row_num, dict[i], i);
		}
	}
	return;
}

/* function used to print duplicates for columns */
void
print_invalid_col(int sudoku[], int set_num){
	int dict[dict_size];
	int *index = s2c[set_num];
	int i, col_num;
	for(i = 0; i < dict_size; i++){
		dict[i] = 0;
	}
	for(i = 0; i < NDIG; i++){
		dict[sudoku[index[i]]] += 1;
	}
	for(i = 1; i < dict_size; i++){
		if(dict[i] > 1){
			col_num = set_num + 1 - SETNUM_ROW_END;
			printf("\nset %2d (col %d): %d instances of %d", 
				set_num, col_num, dict[i], i);
		}
	}
	return;
}		

/* function used to print duplicates for sqaures */
void
print_invalid_sqr(int sudoku[], int set_num){
	int dict[dict_size];
	int *index = s2c[set_num];
	int i, sqr_num;
	for(i = 0; i < dict_size; i++){
		dict[i] = 0;
	}
	for(i = 0; i < NDIG; i++){
		dict[sudoku[index[i]]] += 1;
	}
	for(i = 1; i < dict_size; i++){
		if(dict[i] > 1){
			sqr_num = set_num + 1 - SETNUM_COL_END;
			printf("\nset %2d (sqr %d): %d instances of %d", 
				set_num, sqr_num, dict[i], i);
		}
	}
	return;
}	


/* main program controls all the action
*/


int
main(int argc, char *argv[]){
	
	int sudoku[NCLL];
	int num, count = 0;
	int i;
	
	/* stage 1 start */
	
	/* first of all, read sudoku and make a one-dimensional array */	
	while(scanf("%d", &num) == 1){
		sudoku[count] = num;
		count++;
	}
	
	/* lay out the sudoku */	
	printf("\n");
	print(sudoku);
		
	/* stage 1 complete */ 
	
	
	/* stage 2 start */
	
	int num_invalid_set = 0, total_violation = 0, num_violation;
	// Three loops to check whether rows, columns, sqaures are valid or not
	for(i = 0; i < SETNUM_ROW_END; i++){
		if((num_violation = Invalid(sudoku, i)) != 0){
			num_invalid_set++;
			total_violation += num_violation;
			print_invalid_row(sudoku, i);
		}
	}
	for(i = SETNUM_ROW_END; i < SETNUM_COL_END; i++){
		if((num_violation = Invalid(sudoku, i)) != 0){
			num_invalid_set++;
			total_violation += num_violation;
			print_invalid_col(sudoku, i);
		}
	}	
	for(i = SETNUM_COL_END; i < SETNUM_SQR_END; i++){
		if((num_violation = Invalid(sudoku, i)) != 0){
			num_invalid_set++;
			total_violation += num_violation;
			print_invalid_sqr(sudoku, i);
		}
	}
	// print # of incorrect sets and total violations
	if(total_violation != 0){
		printf("\n\n%d different sets have violations", num_invalid_set);
		printf("\n%d violations in total\n", total_violation);
		exit(EXIT_FAILURE); 
	// since invalid sudoku, no more steps are needed, exit
	}
	
	/*stage 2 complete */ 
	
	
	/* all done, so pack up bat and ball and head home */
	
	return 0;
}

// Algorithms are fun !
// Algorithms are fun !
// Algorithms are fun !
// Algorithms are fun !
// Algorithms are fun !

/****************************************************************/


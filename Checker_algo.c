/* Program to print and play checker games.

  Skeleton program written by Artem Polyvyanyy, artem.polyvyanyy@unimelb.edu.au,
  September 2021, with the intention that it be modified by students
  to add functionality, as required by the assignment specification.

  Student Authorship Declaration:

  (1) I certify that except for the code provided in the initial skeleton file,
  the program contained in this submission is completely my own individual
  work, except where explicitly noted by further comments that provide details
  otherwise. I understand that work that has been developed by another student,
  or by me in collaboration with other students, or by non-students as a result
  of request, solicitation, or payment, may not be submitted for assessment in
  this subject. I understand that submitting for assessment work developed by
  or in collaboration with other students or non-students constitutes Academic
  Misconduct, and may be penalized by mark deductions, or by other penalties
  determined via the University of Melbourne Academic Honesty Policy, as
  described at https://academicintegrity.unimelb.edu.au.

  (2) I also certify that I have not provided a copy of this work in either
  softcopy or hardcopy or any other form to any other student, and nor will I
  do so until after the marks are released. I understand that providing my work
  to other students, regardless of my intention or any undertakings made to me
  by that other student, is also Academic Misconduct.

  (3) I further understand that providing a copy of the assignment specification
  to any form of code authoring or assignment tutoring service, or drawing the
  attention of others to such services and code that may have been made
  available via such a service, may be regarded as Student General Misconduct
  (interfering with the teaching activities of the University and/or inciting
  others to commit Academic Misconduct). I understand that an allegation of
  Student General Misconduct may arise regardless of whether or not I personally
  make use of such solutions or sought benefit from such actions.

  Signed by: Yitong Kong
  Dated:     12/10/2021

*/

#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <assert.h>
#include <math.h>

/* ---------------------------definition-------------------------------*/
#define BOARD_SIZE          	8    	// board size
#define ROWS_WITH_PIECES    	3    	// number of initial rows with pieces
#define CELL_EMPTY        		'.'  	// empty cell character
#define CELL_BPIECE       		'b'  	// black piece character
#define CELL_WPIECE       		'w'  	// white piece character
#define CELL_BTOWER       		'B'  	// black tower character
#define CELL_WTOWER         	'W'  	// white tower character
#define COST_PIECE          	1    	// one piece cost
#define COST_TOWER        		3    	// one tower cost
#define TREE_DEPTH        		3   	// minimax tree depth
#define COMP_ACTIONS      		10  	// number of computed actions
#define MAX_POSSIBLE_NEXT_MOVE 48		// obtained by 4 * 12
#define NUM_POSSIBLE_MOVE		4		// a piece can move in all four directions
#define UPPERMOST_ROW 			0   	// black piece becomes tower at this row
#define LOWERMOST_ROW 			7   	// white piece becomes tower at this row
#define UP     					-1		// x0 + UP if move a unit up
#define DOWN					1		// x1 + DOWN if move a unit down
#define LEFT					-1		// y0 + LEFT if move a unit left
#define RIGHT					1		// y0 + RIGHT if move a unit right
#define MOVE_DISTANCE			1   	// simple move cause one unit change
#define CAPTURE_DISTANCE 		2 		// capture cause two unit change
#define NUM_PLAYER 				2 		// There are two sides in a game
#define ASCII_DIFFERENCE	  	32     	// difference between lower and upper case
#define ROW_DIFFERENCE_BETWEEN_BOARD_AND_2D_ARRAY 1
/* ---------------------------definition-------------------------------*/

/* black's term if number of action is odd, num_action % 2 == 1 */
/* white's term if number of action is even, num_action % 2 == 0 */

typedef unsigned char board_t[BOARD_SIZE][BOARD_SIZE];  // board type

/* action is the initial-to-final coordinate on the board */
typedef struct{
	int row_start, row_end;
	char col_start, col_end;
}action_t;

/* move is the initial-to-final coordinate in the 2-D array */
typedef struct{
	int x0, y0; // initial coordinate
	int x1, y1; // final coordinate
}move_t;

/* suppose all 12 pieces can move in all four directions, therefore, the maximum
number of next possble move when exploring in the tree is 12 * 4 = 48
so we create a child array pointer of size 48 in the struct node */
typedef struct node node_t;
struct node {
	board_t board;
	int num_children;
	node_t* parent;
	node_t* child[MAX_POSSIBLE_NEXT_MOVE];
	move_t possible_move[NUM_POSSIBLE_MOVE];
	int cost;
};


/* initialize the board as a 2-D array */
void
initialize_board(board_t board){
	int i,j;
	// firstly initialize the board with all '.' 
	for(i = 0; i < BOARD_SIZE; i++){
		for(j = 0; j < BOARD_SIZE; j++){
			board[i][j] = CELL_EMPTY;
		}
	}
	// insert white piece into row 1 2 3 
	// only when the sum of x and y coordinate (i + j) is an odd number, 
	// this grid will have a piece
	for(i = 0; i < ROWS_WITH_PIECES; i++){
		for(j = 0; j < BOARD_SIZE; j++){
			if((i + j) % 2 == 1){
				board[i][j] = CELL_WPIECE;
			}
		}
	}
	// insert black piece into row 6 7 8 
	for(i = BOARD_SIZE - ROWS_WITH_PIECES; i < BOARD_SIZE; i++){
		for(j = 0; j < BOARD_SIZE; j++){
			if((i + j) % 2 == 1){
				board[i][j] = CELL_BPIECE;
			}
		}
	}
	
}

/* print out the board which is a 2-D array according to the format */
void
print_board(board_t board){
	int i,j;
	printf("     A   B   C   D   E   F   G   H");	
	for(i = 0; i < BOARD_SIZE; i++){
		printf("\n   +---+---+---+---+---+---+---+---+\n");
		printf(" %d |", i + 1);
		for(j = 0; j < BOARD_SIZE; j++){
			printf(" %c |", board[i][j]);
		}
	}
	printf("\n   +---+---+---+---+---+---+---+---+\n");

}


/* check_error and return the error type for a move, if valid, then return 0 */
int
check_error(board_t board, int x0, int y0, int x1, int y1, int num_action){
	// Source cell is outside of the board.
	if((x0 < 0 || x0 >=  BOARD_SIZE) || 
	   (y0 < 0 || y0 >=  BOARD_SIZE)){
		return 1;
	}
	// Target cell is outside of the board.
	else if((x1 < 0 || x1 >=  BOARD_SIZE) || 
	   (y1 < 0 || y1 >=  BOARD_SIZE)){
		return 2;
	}
	// Source cell is empty.
	else if(board[x0][y0] == CELL_EMPTY){
		return 3;
	}
	// Target cell is not empty.
	else if(board[x1][y1] != CELL_EMPTY){
		return 4;
	}
	// Source cell holds opponent’s piece/tower.
	// first calculate which side is going to move, then check
	else if(((num_action % 2 == 1) && (board[x0][y0] == CELL_WPIECE)) ||
	   ((num_action % 2 == 1) && (board[x0][y0] == CELL_WTOWER)) ||
   	   ((num_action % 2 == 0) && (board[x0][y0] == CELL_BPIECE)) ||
       ((num_action % 2 == 0) && (board[x0][y0] == CELL_BTOWER))){
    	return 5;
    }
    
    // illegal move
	

    // illegal move for white pieces
    else if(board[x0][y0] == CELL_WPIECE){
		if(((x1 - x0 != MOVE_DISTANCE) && (x1 - x0 != CAPTURE_DISTANCE)) ||
		  ((abs(y1 - y0) != MOVE_DISTANCE) && 
		  	  (abs(y1 - y0) != CAPTURE_DISTANCE))){
			return 6;
		}
		// white pieces can only move downwards
		else if((x1 - x0 == DOWN * CAPTURE_DISTANCE) && 
				(abs(y1 - y0) == CAPTURE_DISTANCE)){
			if((board[(x0 + x1) / 2][(y0 + y1) / 2] != CELL_BPIECE) &&
			   (board[(x0 + x1) / 2][(y0 + y1) / 2] != CELL_BTOWER)){
				return 6;
			}
		}
	}
	// illegal move for black pieces
	else if(board[x0][y0] == CELL_BPIECE){
		if(((x1 - x0 != UP * MOVE_DISTANCE) && 
			(x1 - x0 != UP * CAPTURE_DISTANCE)) ||	
		  ((abs(y1 - y0) != MOVE_DISTANCE) && 
		  	 (abs(y1 - y0) != CAPTURE_DISTANCE))){
			return 6;
		}
		// black pieces can only move upwards
		else if((x1 - x0 == UP * CAPTURE_DISTANCE) && 
			(abs(y1 - y0) == CAPTURE_DISTANCE)){
			if((board[(x0 + x1) / 2][(y0 + y1) / 2] != CELL_WPIECE) &&
			   (board[(x0 + x1) / 2][(y0 + y1) / 2] != CELL_WTOWER)){
				return 6;
			}
		}
	}
	// illegal move for white towers
	else if(board[x0][y0] == CELL_WTOWER){
		if(((abs(x1 - x0) != MOVE_DISTANCE) && 
			(abs(x1 - x0) != CAPTURE_DISTANCE)) ||
		  ((abs(y1 - y0) != MOVE_DISTANCE) && 
		  	(abs(y1 - y0) != CAPTURE_DISTANCE))){
			return 6;
		}
		else if((abs(x1 - x0) == CAPTURE_DISTANCE) && 
			(abs(y1 - y0) == CAPTURE_DISTANCE)){
			if((board[(x0 + x1) / 2][(y0 + y1) / 2] != CELL_BPIECE) &&
			   (board[(x0 + x1) / 2][(y0 + y1) / 2] != CELL_BTOWER)){
				return 6;
			}
		}
	}
	// illegal move for black towers
	else if(board[x0][y0] == CELL_BTOWER){
		if(((abs(x1 - x0) != MOVE_DISTANCE) && 
			(abs(x1 - x0) != CAPTURE_DISTANCE)) ||
		  ((abs(y1 - y0) != MOVE_DISTANCE) && 
		  	(abs(y1 - y0) != CAPTURE_DISTANCE))){
			return 6;
		}
		else if((abs(x1 - x0) == CAPTURE_DISTANCE) && 
			(abs(y1 - y0) == CAPTURE_DISTANCE)){
			if((board[(x0 + x1) / 2][(y0 + y1) / 2] != CELL_WPIECE) &&
			   (board[(x0 + x1) / 2][(y0 + y1) / 2] != CELL_WTOWER)){
				return 6;		
			}
		}
	}
	return 0; 
}

/* apply a move to the board and edit the board */
void
apply_action(board_t board, int x0, int y0, int x1, int y1){
	// apply a move
	if((abs(x1 - x0) == MOVE_DISTANCE) && (abs(y1 - y0) == MOVE_DISTANCE)){		
		board[x1][y1] = board[x0][y0];
		board[x0][y0] = CELL_EMPTY;
	}
	// apply a capture
	if((abs(x1 - x0) == CAPTURE_DISTANCE) && 
		(abs(y1 - y0) == CAPTURE_DISTANCE)){
		board[x1][y1] = board[x0][y0];
		board[x0][y0] = CELL_EMPTY;
		board[(x0 + x1) / 2][(y0 + y1) / 2] = CELL_EMPTY;
	}
}

/* let pieces which reaches the margin become towers */
void
generate_tower(board_t board){
	int i;
	for(i = 0; i < BOARD_SIZE; i++){
		if(board[UPPERMOST_ROW][i] == CELL_BPIECE){
			board[UPPERMOST_ROW][i] = CELL_BTOWER;
		}
	}
	for(i = 0; i < BOARD_SIZE; i++){
		if(board[LOWERMOST_ROW][i] == CELL_WPIECE){
			board[LOWERMOST_ROW][i] = CELL_WTOWER;
		}
	}
}
	

/* calculate the cost of the board according to the formula */
int
board_cost(board_t board){
	int cost = 0;
	int i,j;
	// cost = b + 3 * B - w -3 * W
	for(i = 0; i < BOARD_SIZE; i++){
		for(j = 0; j < BOARD_SIZE; j++){
			if(board[i][j] == CELL_BPIECE){
				cost += COST_PIECE;
			}
			if(board[i][j] == CELL_BTOWER){
				cost += COST_TOWER;
			}
			if(board[i][j] == CELL_WPIECE){
				cost -= COST_PIECE;
			}
			if(board[i][j] == CELL_WTOWER){
				cost -= COST_TOWER;
			}
		}
	}
	return cost;
}


/* duplicate a board, for we don't want to make changes to the original board */
void
duplicate_board(board_t reserve, board_t board){
	int i, j;
	for(i = 0; i < BOARD_SIZE; i++){
		for(j = 0; j < BOARD_SIZE; j++){
			reserve[i][j] = board[i][j];
		}
	}

}


/* to get next possible move, the function needs a starting coordinate (x0, y0), 
an original board, number of action to see who to go, and an array of move_t to 
store the next possible movement, the function will return the total number of 
possible moves */
int
get_next_possible_move(int x0, int y0, board_t board, int num_action, 
						move_t possible_move[]){
	int num_possible_move = 0;
	
	// if a move is valid, it will be stored in the array
	
	// possible move
	
	if(check_error(board, x0, y0, x0 + UP, y0 + RIGHT, num_action) == 0){
		possible_move[num_possible_move].x0 = x0;
		possible_move[num_possible_move].y0 = y0;
		possible_move[num_possible_move].x1 = x0 + UP;
		possible_move[num_possible_move].y1 = y0 + RIGHT;
		num_possible_move++;
	}
	if(check_error(board, x0, y0, x0 + DOWN, y0 + RIGHT, num_action) == 0){
		possible_move[num_possible_move].x0 = x0;
		possible_move[num_possible_move].y0 = y0;
		possible_move[num_possible_move].x1 = x0 + DOWN;
		possible_move[num_possible_move].y1 = y0 + RIGHT;
		num_possible_move++;
	}
	if(check_error(board, x0, y0, x0 + DOWN, y0 + LEFT, num_action) == 0){
		possible_move[num_possible_move].x0 = x0;
		possible_move[num_possible_move].y0 = y0;
		possible_move[num_possible_move].x1 = x0 + DOWN;
		possible_move[num_possible_move].y1 = y0 + LEFT;
		num_possible_move++;
	}
	if(check_error(board, x0, y0, x0 + UP, y0 + LEFT, num_action) == 0){
		possible_move[num_possible_move].x0 = x0;
		possible_move[num_possible_move].y0 = y0;
		possible_move[num_possible_move].x1 = x0 + UP;
		possible_move[num_possible_move].y1 = y0 + LEFT;
		num_possible_move++;
	}
	
	// possible capture, so a checker is moving 2 units at a time
	
	if(check_error(board, x0, y0, x0 + CAPTURE_DISTANCE * UP, 
		y0 + CAPTURE_DISTANCE * RIGHT, num_action) == 0){
		possible_move[num_possible_move].x0 = x0;
		possible_move[num_possible_move].y0 = y0;
		possible_move[num_possible_move].x1 = x0 + CAPTURE_DISTANCE * UP;
		possible_move[num_possible_move].y1 = y0 + CAPTURE_DISTANCE * RIGHT;
		num_possible_move++;
	}
	if(check_error(board, x0, y0, x0 + CAPTURE_DISTANCE * DOWN, 
		y0 + CAPTURE_DISTANCE * RIGHT, num_action) == 0){
		possible_move[num_possible_move].x0 = x0;
		possible_move[num_possible_move].y0 = y0;
		possible_move[num_possible_move].x1 = x0 + CAPTURE_DISTANCE * DOWN;
		possible_move[num_possible_move].y1 = y0 + CAPTURE_DISTANCE * RIGHT;
		num_possible_move++;
	}
	if(check_error(board, x0, y0, x0 + CAPTURE_DISTANCE * DOWN, 
		y0 + CAPTURE_DISTANCE LEFT, num_action) == 0){
		possible_move[num_possible_move].x0 = x0;
		possible_move[num_possible_move].y0 = y0;
		possible_move[num_possible_move].x1 = x0 + CAPTURE_DISTANCE * DOWN;
		possible_move[num_possible_move].y1 = y0 + CAPTURE_DISTANCE * LEFT;
		num_possible_move++;
	}
	if(check_error(board, x0, y0, x0 + CAPTURE_DISTANCE * UP, 
		y0 + CAPTURE_DISTANCE * LEFT, num_action) == 0){
		possible_move[num_possible_move].x0 = x0;
		possible_move[num_possible_move].y0 = y0;
		possible_move[num_possible_move].x1 = x0 + CAPTURE_DISTANCE * UP;
		possible_move[num_possible_move].y1 = y0 + CAPTURE_DISTANCE * LEFT;
		num_possible_move++;
	}
	else{
	}
	return num_possible_move;
}
	
/* create a new node using malloc and store necessary values in it, manipulate
the board and store it for further use */
node_t*
get_new_node(node_t* parent, move_t move){
	node_t* new_node = (node_t*)malloc(sizeof(node_t));
	assert(new_node != NULL);
	// annotate the new node
	new_node->parent = parent;
	new_node->num_children = 0;
	board_t reserve;
	duplicate_board(reserve, parent->board);
	apply_action(reserve, move.x0, move.y0, move.x1, move.y1);
	generate_tower(reserve);
	duplicate_board(new_node->board, reserve);
	new_node->cost = board_cost(new_node->board);
	return new_node;
}

/* given a root, find all different condtions, record them as chilren */
void
extend_tree(node_t* parent, int num_action){
	int i, j, k;
	char checker[NUM_PLAYER] = {CELL_WPIECE, CELL_BPIECE};
	char who_to_go = checker[num_action % 2];
	for(i = 0; i < BOARD_SIZE; i++){
		for(j = 0; j < BOARD_SIZE; j++){
			if(parent->board[i][j] == who_to_go ||
			parent->board[i][j] == who_to_go - ASCII_DIFFERENCE){
				// here start to search possible move and develop child node
				int num_moves = get_next_possible_move(i, j, parent->board, 
					num_action, parent->possible_move);				
				for(k = 0; k < num_moves; k++){
					node_t* child = get_new_node(parent, 
												  parent->possible_move[k]);					
					parent->child[parent->num_children] = child;
					parent->num_children++;
					// record the address of every child node and number of them
				}
			}
		}
	}
	
}

/* calculate the initial and final coordinates of a movement according to the 
change of board */
move_t
get_move(board_t initial_board, board_t final_board, int num_action){
	move_t move;
	int i, j;
	if(num_action % 2 == 1){
		// black side is moving
		for(i = 0; i < BOARD_SIZE; i++){
			for(j = 0; j < BOARD_SIZE; j++){
				if((final_board[i][j] == CELL_BPIECE || 
					final_board[i][j] == CELL_BTOWER) &&
				   (initial_board[i][j] == CELL_EMPTY)){
					move.x1 = i;
					move.y1 = j;
				}
				if((initial_board[i][j] == CELL_BPIECE ||
					initial_board[i][j] == CELL_BTOWER) &&
				   (final_board[i][j] == CELL_EMPTY)){
					move.x0 = i;
					move.y0 = j;
				}
			}			
		}
	}
	else{
		// white side is moving
		for(i = 0; i < BOARD_SIZE; i++){
			for(j = 0; j < BOARD_SIZE; j++){
				if((final_board[i][j] == CELL_WPIECE || 
					final_board[i][j] == CELL_WTOWER) &&
				   (initial_board[i][j] == CELL_EMPTY)){
					move.x1 = i;
					move.y1 = j;
				}
				if((initial_board[i][j] == CELL_WPIECE || 
					initial_board[i][j] == CELL_WTOWER) &&
				   (final_board[i][j] == CELL_EMPTY)){
					move.x0 = i;
					move.y0 = j;
				}
			}
		}
	}
	return move;
}

/* convert a move into an action */
action_t
move_to_action(move_t move){
	action_t action;
	char column[BOARD_SIZE] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H'};
	action.row_start = move.x0 + ROW_DIFFERENCE_BETWEEN_BOARD_AND_2D_ARRAY;
	action.row_end = move.x1 + ROW_DIFFERENCE_BETWEEN_BOARD_AND_2D_ARRAY;
	action.col_start = column[move.y0];
	action.col_end = column[move.y1];
	return action;
}
	
/* computer automatically searching for the next preferred move */
void
search_tree(node_t* root, int num_action){
	int i, j, k;
	board_t decision;
	// for black, we search max cost in depth 3, min in depth 2, max in depth 1
	if(num_action % 2 == 1){
		int temp_cost_depth_3 = INT_MIN;
		for(i = 0; i < root->num_children; i++){
			for(j = 0; j < root->child[i]->num_children; j++){
				for(k = 0; k < root->child[i]->child[j]->num_children; k++){
					if(root->child[i]->child[j]->child[k]->cost 
						> temp_cost_depth_3){
						temp_cost_depth_3 = 
						root->child[i]->child[j]->child[k]->cost;
						// do updates when finding larger cost
					}
				root->child[i]->child[j]->cost = temp_cost_depth_3;
				// assign board cost in depth 2 with the max found in depth 3
				}
			}
		}
		int temp_cost_depth_2 = INT_MAX;
		for(i = 0; i < root->num_children; i++){
			for(j = 0; j < root->child[i]->num_children; j++){		
				if(root->child[i]->child[j]->cost < temp_cost_depth_2){
					temp_cost_depth_2 = root->child[i]->child[j]->cost;
					// do updates when finding smaller cost
				}
			root->child[i]->cost = temp_cost_depth_2;
			// assign board cost in depth 1 with the min found in depth 2
			}
		}
		int temp_cost_depth_1 = INT_MIN;
		for(i = 0; i < root->num_children; i++){
			if(root->child[i]->cost > temp_cost_depth_1){
				temp_cost_depth_1 = root->child[i]->cost;
				// find the max in depth 1 and make a decision
				duplicate_board(decision, root->child[i]->board);
			}
		}
	}
	// for white, we search min cost in depth 3, max in depth 2, min in depth 1
	else{
		int temp_cost_depth_3 = INT_MAX;
		for(i = 0; i < root->num_children; i++){
			for(j = 0; j < root->child[i]->num_children; j++){
				for(k = 0; k < root->child[i]->child[j]->num_children; k++){
					if(root->child[i]->child[j]->child[k]->cost 
						< temp_cost_depth_3){
						temp_cost_depth_3 = 
						root->child[i]->child[j]->child[k]->cost;
						// do updates when finding smaller cost
					}
				root->child[i]->child[j]->cost = temp_cost_depth_3;
				// assign board cost in depth 2 with the min found in depth 3
				}
			}
		}
		int temp_cost_depth_2 = INT_MIN;
		for(i = 0; i < root->num_children; i++){
			for(j = 0; j < root->child[i]->num_children; j++){		
				if(root->child[i]->child[j]->cost > temp_cost_depth_2){
					temp_cost_depth_2 = root->child[i]->child[j]->cost;
					// do updates when finding larger cost
				}
			root->child[i]->cost = temp_cost_depth_2;
			// assign board cost in depth 1 with the max found in depth 2
			}
		}
		int temp_cost_depth_1 = INT_MAX;
		for(i = 0; i < root->num_children; i++){
			if(root->child[i]->cost < temp_cost_depth_1){
				temp_cost_depth_1 = root->child[i]->cost;
				// find the min in depth 1 and make a decision
				duplicate_board(decision, root->child[i]->board);		
			}
		}
	}
	
	// already found a decision (we know which board to go next)
	// and can thus work out the action (who to go and how to go)
	move_t move = get_move(root->board, decision, num_action);
	action_t action = move_to_action(move);
	printf("=====================================\n");
	if(num_action % 2 == 1){
			printf("*** BLACK ACTION #%d: %c%d-%c%d\n", num_action, 
			action.col_start, action.row_start, action.col_end, action.row_end);
	}
	else{
		printf("*** WHITE ACTION #%d: %c%d-%c%d\n", num_action,
		action.col_start, action.row_start, action.col_end, action.row_end);
	}
	printf("BOARD COST: %d\n", board_cost(decision));
	print_board(decision);
}


/* a recursive function to free the dynamic memory, free the root after 
finishing freeing all the other branches */
void
free_tree(node_t* root){
	int i;
	if(root == NULL){
		return;
	}
	for(i = 0; i < root->num_children; i++){
		free_tree(root->child[i]);
	}
	free(root);
}

/* generate a tree of depth 3 and search it for best next action */
void
stage_1(board_t board, int num_action){
	int i, j;
	node_t* root = (node_t*)malloc(sizeof(node_t));
	assert(root != NULL);
	root->parent = NULL; 
	// since it is the root, it has no parent
	duplicate_board(root->board, board);
	// above is the initialisation of the root
	extend_tree(root, num_action);	 // depth 1 
	for(i = 0; i < root->num_children; i++){
		extend_tree(root->child[i], num_action + 1);  // depth 2
		for(j = 0; j < root->child[i]->num_children; j++){
			extend_tree(root->child[i]->child[j], num_action + 2);  // depth 3
		}
	}
	
	search_tree(root, num_action);
	free_tree(root);	
	printf("\n");
}

int
main(int argc, char *argv[]) {
    // YOUR IMPLEMENTATION OF STAGES 0-2

    /* stage 0 start */
    
    board_t board;
    // set up and print out the initial board
    initialize_board(board);
	printf("BOARD SIZE: 8x8\n#BLACK PIECES: 12\n#WHITE PIECES: 12\n");
	print_board(board);
    
	// start to read actions from input text
	int num_action = 0;
	int error_type;
	int cost;
	action_t action;
	// here action is the string we read from the text
	while (scanf("%c%d-%c%d", &action.col_start, &action.row_start, 
		   &action.col_end, &action.row_end) == 4){
		num_action ++;
		getchar();
		int x0 = action.row_start - ROW_DIFFERENCE_BETWEEN_BOARD_AND_2D_ARRAY;
		int x1 = action.row_end - ROW_DIFFERENCE_BETWEEN_BOARD_AND_2D_ARRAY;
		// a capital letter - char('A') will give us the corresponding index
		// in a 2-D array according to ASCII
		int y0 = action.col_start - 'A'; 
		int y1 = action.col_end - 'A';
		error_type = check_error(board, x0, y0, x1, y1, num_action);
		if(error_type == 0){
			apply_action(board, x0, y0, x1, y1);
			generate_tower(board);
			printf("=====================================");
			if(num_action % 2 == 1){
				printf("\nBLACK ACTION #%d: %c%d-%c%d\n", num_action, 
					action.col_start, action.row_start, 
					action.col_end, action.row_end);
			}
			else{
				printf("\nWHITE ACTION #%d: %c%d-%c%d\n", num_action,
					action.col_start, action.row_start, 
					action.col_end, action.row_end);
			}
			cost = board_cost(board);
			printf("BOARD COST: %d\n", cost);
			print_board(board);
		}
		// if error happens, indicate the error and exit the program
		else if(error_type == 1){
			printf("ERROR: Source cell is outside of the board.\n");
			exit(EXIT_FAILURE);
		}
		else if(error_type == 2){
			printf("ERROR: Target cell is outside of the board.\n");
			exit(EXIT_FAILURE);
		}
		else if(error_type == 3){
			printf("ERROR: Source cell is empty.\n");
			exit(EXIT_FAILURE);
		}
		else if(error_type == 4){
			printf("ERROR: Target cell is not empty.\n");
			exit(EXIT_FAILURE);
		}
		else if(error_type == 5){
			printf("ERROR: Source cell holds opponent's piece/tower.\n");
			exit(EXIT_FAILURE);
		}
		else if(error_type == 6){
			printf("ERROR: Illegal action.\n");
			exit(EXIT_FAILURE);
		}
	}
	num_action++;
	
	/* stage 0 end */
	
// Algorithms are fun ! Algorithms are fun ! Algorithms are fun ! 
// Algorithms are fun ! Algorithms are fun ! Algorithms are fun ! 
// Algorithms are fun ! Algorithms are fun ! Algorithms are fun ! 
// Algorithms are fun ! Algorithms are fun ! Algorithms are fun ! 
// Algorithms are fun ! Algorithms are fun ! Algorithms are fun ! 
	
	/* stage 1 start */
	
	char next;
	next = action.col_start; // get the command from input
	
	if(next == 'A'){
		stage_1(board, num_action);
	}
	
	/* an extra description I would like to make is that my program can't run 
	stage 1 as expected on my MINGW compiler, but it works well in GROK and 
	dim_fox testing, and I don't know why */
	
	/* stage 1 end */
	
// Algorithms are fun ! Algorithms are fun ! Algorithms are fun ! 
// Algorithms are fun ! Algorithms are fun ! Algorithms are fun ! 
// Algorithms are fun ! Algorithms are fun ! Algorithms are fun ! 
// Algorithms are fun ! Algorithms are fun ! Algorithms are fun ! 
// Algorithms are fun ! Algorithms are fun ! Algorithms are fun ! 	

	/* stage 2 start */
	
	if(next == 'P'){
		for(int i = 0; i < COMP_ACTIONS; i++){
			stage_1(board, num_action);
			num_action++;
		}
	}

	
	/* stage 2 end */
	
	
	return EXIT_SUCCESS;            // exit program with the success code
}

/* THE END -------------------------------------------------------------------*/
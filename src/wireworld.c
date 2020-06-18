#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "neillncurses.h"

/* #define GENS 1000 board displayed for 1000 generations in original task,
      not in ncurses.
#define HALF 2 also not necessary */
#define WIDTH 40 /* board will be 40 x 40 */
/* squares defined based on rules */
#define HEAD 'H'
#define EMPTY ' '
#define TAIL 't'
#define COPPER 'c'
/* various exit codes used in case of invalid chars/files */
#define INVALID 2
#define VALIDFILE 2
#define INVALIDFILE 3
/* delay for printing in colour */
#define DELAY 1000.0

void test(void);
void colourprint(FILE *wireworldfile);
void validinput(int argument);
void fillarray(char gen[WIDTH][WIDTH], FILE *wireworldfile);
int isvalid(char c);
void calc1gen(char gen_A[WIDTH][WIDTH], char gen_B[WIDTH][WIDTH]);
int access(int i, int j);
char applyrules(char gen[WIDTH][WIDTH], int i, int j);
int calchead(char gen[WIDTH][WIDTH], int i, int j);
/* void alternate(char gen_A[WIDTH][WIDTH], char gen_B[WIDTH][WIDTH]);
     function not used with ncurses update. */
/* void printarray(char gen[WIDTH][WIDTH]);
       print array function not needed with ncurses addition */

/* takes in command line argument w/ file */
int main(int argc, char *argv[]){
  FILE *wireworldfile;

    /*test();*/
    wireworldfile = fopen(argv[1], "r");
    validinput(argc); /*to check that one file was inputed */
    colourprint(wireworldfile); /*pass file on to fill array, change colours, and print */

  return 0;
}

/* function to add colour and print the contents of the file */
void colourprint(FILE *wireworldfile){
  char gen_A[WIDTH][WIDTH] = {{0}};
  char gen_B[WIDTH][WIDTH] = {{0}};
  NCURS_Simplewin sw;

  Neill_NCURS_Init(&sw);
/* heads -> blue, tails -> red, copper -> yellow, background -> black.
 As the function accepts a string #define'd chars not used */
  Neill_NCURS_CharStyle(&sw, "H", COLOR_BLUE, COLOR_BLUE, A_NORMAL);
  Neill_NCURS_CharStyle(&sw, "t", COLOR_RED, COLOR_RED, A_NORMAL);
  Neill_NCURS_CharStyle(&sw, "c", COLOR_YELLOW, COLOR_YELLOW, A_NORMAL);
  Neill_NCURS_CharStyle(&sw, " ", COLOR_BLACK, COLOR_BLACK, A_NORMAL);

  fillarray(gen_A, wireworldfile);

  do{
    /* calculates the content of each gen based on the above rules */
    calc1gen(gen_A, gen_B);
    /* sets the array to be printed in colour */
    Neill_NCURS_PrintArray(&gen_A[0][0], WIDTH, WIDTH, &sw);
    Neill_NCURS_Delay(DELAY);
    /* calculates the next generation, switching back and forth */
    calc1gen(gen_B, gen_A);
    Neill_NCURS_PrintArray(&gen_B[0][0], WIDTH, WIDTH, &sw);
    Neill_NCURS_Delay(DELAY);
    /* allows for the use to escape from the program */
    Neill_NCURS_Events(&sw);
  }
  while(!sw.finished);

    fclose(wireworldfile); /*file opened in main */
    atexit(Neill_NCURS_Done);
    exit(EXIT_SUCCESS);
}


/* check valid file inputed on command line */
void validinput(int argument){
/* argument count is two (VALIDFILE) = command line argument used correctly */
  if(argument != VALIDFILE){
    fprintf(stderr, "ERROR! Invalid file type inputed.\n");
    atexit(Neill_NCURS_Done);
    exit(INVALIDFILE);
  }
}


/*fill the array with the contents of the file */
void fillarray(char gen[WIDTH][WIDTH], FILE *wireworldfile){
  int x = 0;
  int y = 0;
  char c;

/* getting the chars from the file */
  while((c = getc(wireworldfile)) != EOF){
    /* if it is a valid aray */
    if(isvalid(c)){
      /* ie if it is not the end of the row, add that to the array */
      if(c != '\n'){
        gen[x][y] = c;
        y++;
      }
    }
    /* if not, add one to the column*/
    else{
      x++;
    }
  }
}


/* checks that it is one of the valid chars as per the rules */
int isvalid(char c){
  int valid = 1;

    if((c == HEAD) || (c == EMPTY) || (c == TAIL) || (c == COPPER) || (c == '\n')){
      return valid;
    }
  /* if the file contains an invalid char, exit out of the program */
    else{
      fprintf(stderr, "ERROR 1! Invalid character; %c.\n", c);
      atexit(Neill_NCURS_Done);
      exit(INVALID);
    }
}


/* apply the rules to the values in gen_A (current gen) and fill gen_B (temp gen) with the
 new values so as not to alter the current gen*/
void calc1gen(char gen_A[WIDTH][WIDTH], char gen_B[WIDTH][WIDTH]){
  int i;
  int j;
  char newvalue[WIDTH][WIDTH];

  for(i = 0; i < WIDTH; i++){
    for(j = 0; j < WIDTH; j++){
      /* apply the rules of wireworld before altering the contents of the array */
        newvalue[i][j] = applyrules(gen_A, i, j);
        /* fills array B (temp array) with those altered by the rules (the new copies) */
        gen_B[i][j] = newvalue[i][j];
    }
  }
}


/* ensures that the array does not go out of bounds of the 40 x 40 array */
int access(int i, int j){
  /* i and j are the coordinates of the array being passed */
  if(i < 0 || i >= WIDTH || j < 0 || j >= WIDTH){
    return 0;
  }
  return 1;
}


/* switch the value of the coordinate depending on the rules and pass it back
to the temp array */
char applyrules(char gen[WIDTH][WIDTH], int i, int j){
  /* calculates the contents of the surrounding 8 cells for each point being passed in */
  int countH = calchead(gen, i, j);

  /* rules applied based on rules from line 1 - 8 */
  switch(gen[i][j]){
    case EMPTY :
      return EMPTY;
      break;

    case HEAD :
      return TAIL;
      break;

    case TAIL :
      return COPPER;
      break;

    case COPPER :
    /* switches to electron head if number of heads in surrounding cells == 1 or
    n == 2, as returned from the calchead function */
      if(countH == 1 || countH == 2){
        return HEAD;
      }
      else{
        return COPPER;
      }
      break;

    default :
    /* backup check - if not one of the above chars then it is invalid */
      fprintf(stderr, "ERROR 2! Invalid character.\n");
      atexit(Neill_NCURS_Done);
      exit(INVALID);
    }
}


int calchead(char gen[WIDTH][WIDTH], int i, int j){
/* function to calculate total electron heads ('H') in file */
  int totalhead = 0;
  int x;
  int y;

/* to check contents of surrounding 8 cells */
  for(x = i - 1; x <= i + 1; x++){
    for(y = j - 1; y <= j + 1; y++){
      /* if it's not out of bounds, if it's a head and if the cell being check is
      not itself - same coordinates (as only surrounding 8)*/
      if(access(x, y) && gen[x][y] == HEAD && !(x == i && y == j)){
        totalhead++;
      }
    }
  }
  return totalhead;
}


void test(void){
  /* test array to check my functions */
  /* char testarray[WIDTH][WIDTH] = {{HEAD, HEAD, TAIL},{HEAD, HEAD, TAIL}};*/
  /*print array to check that #defines work */
  /* printarray(testarray); */
/* testing isvalid function for valid states from the input file */
/*
   assert(isvalid('H') == 1);
   assert(isvalid('h') == 2);
   assert(isvalid(' ') == 1);
   assert(isvalid('i') == 2);
   assert(isvalid('!') == 2);
   assert(isvalid('c') == 1);
   assert(isvalid('\n') == 1);
   assert(isvalid('hi') == 1);
*/
/*testing that the array does not go out of bounds */
/*
   assert(access(-1, -1) == 1);
   assert(access(-1, 1) == 1);
   assert(access(-3, 2) == 1);
   assert(access(2, -3) == 1);
   assert(access(2, 2) == 0);
   assert(access(40, 40) == 0);
   assert(access(-41, 40) == 1);
*/
  /* passing in 1, 1 because middle point
   3 because not counting the middle array. */
  /* assert(calchead(testarray, 1, 1) == 3); */ /* true */
  /* assert(calchead(testarray, 1, 1) == 4); */ /* false*/
  /* assert(calchead(testarray, 2, 2) == 3); */ /* true */

/* assuring that #defines and switch statement returns the correct code */
/*
  assert(applyRules(EMPTY) == EMPTY);
  assert(applyRules(HEAD) == TAIL);
  assert(applyRules(TAIL) == COPPER);
  assert(applyRules('d') != EMPTY);
  assert(applyRules('e') != COPPER);
  assert(applyRules('m') != HEAD);
  assert(applyRules('t') == TAIL);
  assert(applyRules('g') != TAIL);
*/

}



/* alternate function not needed with the ncurses addition.
void alternate(char gen_A[WIDTH][WIDTH], char gen_B[WIDTH][WIDTH]){
   half the generations as dealing with two at a time but not needed with ncurses
    for (i = 0; i < GENS/HALF; i++){
      calc1gen(gen_A, gen_B);
      calc1gen(gen_B, gen_A);
      printarray(gen_B);
      printarray(gen_A);
   }
}
*/


/* print array function not needed with the ncurses addition.
void printarray(char gen[WIDTH][WIDTH]){
  int i, j;
    for(i = 0; i < WIDTH; i++){
      for (j = 0; j < WIDTH; j++){
        printf("%c", gen[i][j]);
      }
    printf("\n");
    }
}
*/

/*
 * Copyright (c) 2000 by Stephen Montgomery-Smith <stephen@math.missouri.edu>
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation.
 *
 * This file is provided AS IS with no warranties of any kind.  The author
 * shall have no liability with respect to the infringement of copyrights,
 * trade secrets or any patents by this file or any part thereof.  In no
 * event will the author be liable for any lost revenue or profits or
 * other special, indirect and consequential damages.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

char *description = 
"Problem 65.\n"
"\n";

#define nr_polyominoes 35
#define polyominoes_len 6

/* A preprocessed list in which all the possible transforms have
   already been done to the pentominoes, and then all the points
   are converted to displacements via ARRAY_INDEX_DISP.
*/

typedef int displ_type[nr_polyominoes][8][polyominoes_len];
displ_type displ_lu, displ_rd;
int transform_len[nr_polyominoes];

#define width  54
#define height 6

#define ARRAY_INDEX_DISP(x,y) ((x)*(height+2*(polyominoes_len-1))+(y))
#define ARRAY(x,y) (array[ARRAY_INDEX_DISP(x+polyominoes_len-1,y+polyominoes_len-1)])
signed char array[(width+2*(polyominoes_len-1))*(height+2*(polyominoes_len-1))];
int attached[nr_polyominoes];
long nr_found = 0;

int print_choice;

#define RAND(n) ((int)((double)rand()*(n)/((double)RAND_MAX+1)))

typedef struct {int x,y;} point_type;

static struct {int len; point_type point[6];
               int transform_len, transform_list[8], max_white;} 
  polyomino[35] =
{
/*
xxxxxx 
*/
  {6, {{0,0}, {1,0}, {2,0}, {3,0}, {4,0}, {5,0}},
   2, {0, 1, -1, -1, -1, -1, -1, -1}, 3},
/*
xxxxx  
    x  
*/
  {6, {{0,0}, {1,0}, {2,0}, {3,0}, {4,0}, {4,1}},
   8, {0, 1, 2, 3, 4, 5, 6, 7}, 3},
/*
xxxxx  
   x   
*/
  {6, {{0,0}, {1,0}, {2,0}, {3,0}, {3,1}, {4,0}},
   8, {0, 1, 2, 3, 4, 5, 6, 7}, 4},
/*
xxxxx  
  x    
*/
  {6, {{0,0}, {1,0}, {2,0}, {2,1}, {3,0}, {4,0}},
   4, {0, 1, 2, 3, -1, -1, -1, -1}, 3},
/*
   x   
xxxx   
   x   
*/
  {6, {{0,0}, {1,0}, {2,0}, {3,-1}, {3,0}, {3,1}},
   4, {0, 1, 2, 3, -1, -1, -1, -1}, 4},
/*
xxxx   
   xx  
*/
  {6, {{0,0}, {1,0}, {2,0}, {3,0}, {3,1}, {4,1}},
   8, {0, 1, 2, 3, 4, 5, 6, 7}, 3},
/*
xxxx   
  xx   
*/
  {6, {{0,0}, {1,0}, {2,0}, {2,1}, {3,0}, {3,1}},
   8, {0, 1, 2, 3, 4, 5, 6, 7}, 3},
/*
xxxx   
   x   
   x   
*/
  {6, {{0,0}, {1,0}, {2,0}, {3,0}, {3,1}, {3,2}},
   8, {0, 1, 2, 3, 4, 5, 6, 7}, 3},
/*
  x    
xxxx   
   x   
*/
  {6, {{0,0}, {1,0}, {2,-1}, {2,0}, {3,0}, {3,1}},
   8, {0, 1, 2, 3, 4, 5, 6, 7}, 3},
/*
xxxx   
 x x   
*/
  {6, {{0,0}, {1,0}, {1,1}, {2,0}, {3,0}, {3,1}},
   8, {0, 1, 2, 3, 4, 5, 6, 7}, 4},
/*
 x     
xxxx   
   x   
*/
  {6, {{0,0}, {1,-1}, {1,0}, {2,0}, {3,0}, {3,1}},
   8, {0, 1, 2, 3, 4, 5, 6, 7}, 4},
/*
xxxx   
x  x   
*/
  {6, {{0,0}, {0,1}, {1,0}, {2,0}, {3,0}, {3,1}},
   4, {0, 1, 2, 3, -1, -1, -1, -1}, 3},
/*
   x   
xxxx   
x      
*/
  {6, {{0,0}, {0,1}, {1,0}, {2,0}, {3,-1}, {3,0}},
   4, {0, 1, 4, 5, -1, -1, -1, -1}, 3},
/*
  x    
xxxx   
  x    
*/
  {6, {{0,0}, {1,0}, {2,-1}, {2,0}, {2,1}, {3,0}},
   4, {0, 1, 2, 3, -1, -1, -1, -1}, 4},
/*
xxxx   
 xx    
*/
  {6, {{0,0}, {1,0}, {1,1}, {2,0}, {2,1}, {3,0}},
   4, {0, 1, 2, 3, -1, -1, -1, -1}, 3},
/*
xxxx   
  x    
  x    
*/
  {6, {{0,0}, {1,0}, {2,0}, {2,1}, {2,2}, {3,0}},
   8, {0, 1, 2, 3, 4, 5, 6, 7}, 3},
/*
 x     
xxxx   
  x    
*/
  {6, {{0,0}, {1,-1}, {1,0}, {2,0}, {2,1}, {3,0}},
   4, {0, 1, 4, 5, -1, -1, -1, -1}, 3},
/*
  xx   
xxx    
  x    
*/
  {6, {{0,0}, {1,0}, {2,-1}, {2,0}, {2,1}, {3,-1}},
   8, {0, 1, 2, 3, 4, 5, 6, 7}, 3},
/*
 xx    
xxx    
  x    
*/
  {6, {{0,0}, {1,-1}, {1,0}, {2,-1}, {2,0}, {2,1}},
   8, {0, 1, 2, 3, 4, 5, 6, 7}, 3},
/*
  x    
xxx    
x x    
*/
  {6, {{0,0}, {0,1}, {1,0}, {2,-1}, {2,0}, {2,1}},
   8, {0, 1, 2, 3, 4, 5, 6, 7}, 4},
/*
xxx    
  xxx  
*/
  {6, {{0,0}, {1,0}, {2,0}, {2,1}, {3,1}, {4,1}},
   4, {0, 1, 4, 5, -1, -1, -1, -1}, 3},
/*
xxx    
  xx   
   x   
*/
  {6, {{0,0}, {1,0}, {2,0}, {2,1}, {3,1}, {3,2}},
   8, {0, 1, 2, 3, 4, 5, 6, 7}, 3},
/*
xxx    
 xxx   
*/
  {6, {{0,0}, {1,0}, {1,1}, {2,0}, {2,1}, {3,1}},
   4, {0, 1, 4, 5, -1, -1, -1, -1}, 4},
/*
xxx    
  xx   
  x    
*/
  {6, {{0,0}, {1,0}, {2,0}, {2,1}, {2,2}, {3,1}},
   8, {0, 1, 2, 3, 4, 5, 6, 7}, 4},
/*
 x     
xxx    
  xx   
*/
  {6, {{0,0}, {1,-1}, {1,0}, {2,0}, {2,1}, {3,1}},
   8, {0, 1, 2, 3, 4, 5, 6, 7}, 4},
/*
xxx    
x xx   
*/
  {6, {{0,0}, {0,1}, {1,0}, {2,0}, {2,1}, {3,1}},
   8, {0, 1, 2, 3, 4, 5, 6, 7}, 3},
/*
xxx    
 xx    
  x    
*/
  {6, {{0,0}, {1,0}, {1,1}, {2,0}, {2,1}, {2,2}},
   4, {0, 1, 2, 3, -1, -1, -1, -1}, 4},
/*
 x     
xxx    
 xx    
*/
  {6, {{0,0}, {1,-1}, {1,0}, {1,1}, {2,0}, {2,1}},
   4, {0, 1, 2, 3, -1, -1, -1, -1}, 4},
/*
xxx    
xxx    
*/
  {6, {{0,0}, {0,1}, {1,0}, {1,1}, {2,0}, {2,1}},
   2, {0, 1, -1, -1, -1, -1, -1, -1}, 3},
/*
xxx    
  x    
  xx   
*/
  {6, {{0,0}, {1,0}, {2,0}, {2,1}, {2,2}, {3,2}},
   8, {0, 1, 2, 3, 4, 5, 6, 7}, 3},
/*
xxx    
  x    
 xx    
*/
  {6, {{0,0}, {1,0}, {1,2}, {2,0}, {2,1}, {2,2}},
   8, {0, 1, 2, 3, 4, 5, 6, 7}, 3},
/*
 x     
xxx    
x x    
*/
  {6, {{0,0}, {0,1}, {1,-1}, {1,0}, {2,0}, {2,1}},
   4, {0, 1, 2, 3, -1, -1, -1, -1}, 3},
/*
  xx   
xxx    
x      
*/
  {6, {{0,0}, {0,1}, {1,0}, {2,-1}, {2,0}, {3,-1}},
   8, {0, 1, 2, 3, 4, 5, 6, 7}, 3},
/*
 xx    
xxx    
x      
*/
  {6, {{0,0}, {0,1}, {1,-1}, {1,0}, {2,-1}, {2,0}},
   8, {0, 1, 2, 3, 4, 5, 6, 7}, 3},
/*
xx     
 xx    
  xx   
*/
  {6, {{0,0}, {1,0}, {1,1}, {2,1}, {2,2}, {3,2}},
   4, {0, 1, 4, 5, -1, -1, -1, -1}, 3}};

void get_user_input(int argc, char **argv) {
  char input[101];

  if (argc>=2) print_choice = atoi(argv[1]);
  else         print_choice = 0;

  if (print_choice != 1 && print_choice != 3)
    fprintf(stderr,description);

  while(print_choice<1 || print_choice>3) {
    fprintf(stderr,"Select your printing choice:\n"
           "  1) Just count solutions;\n"
           "  2) Pretty print the solutions;\n"
           "  3) Ugly print the solutions.\n"
           "Input choice (number between 1 and 3): ");
    fgets(input,100,stdin);
    print_choice = atoi(input);
    fprintf(stderr,"\n");
  }
}

char colors[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";

void ugly_print_array() {
  int x,y;

  printf("\n");
  for(y=0;y<height;y++) {
    for(x=0;x<width;x++)
      printf("%c",ARRAY(x,y)==-2 ? ' ' : ARRAY(x,y)==-1 ? '.' : colors[(int)ARRAY(x,y)]);
    printf("\n");
  }
  printf("\n");
}

void pretty_print_array() {
  int x,y;

  printf("\n");
  for(y=0;y<=height;y++) {
    for(x=0;x<=width;x++) {
/* print corner */
      if (ARRAY(x,y) == ARRAY(x,y-1) && ARRAY(x,y) == ARRAY(x-1,y) && ARRAY(x,y) == ARRAY(x-1,y-1))
        printf(" ");
      else if (ARRAY(x,y) == ARRAY(x,y-1) && ARRAY(x,y) == ARRAY(x-1,y))
        printf("+");
      else if (ARRAY(x-1,y) == ARRAY(x-1,y-1) && ARRAY(x-1,y) == ARRAY(x,y))
        printf("+");
      else if (ARRAY(x,y-1) == ARRAY(x-1,y-1) && ARRAY(x,y-1) == ARRAY(x,y))
        printf("+");
      else if (ARRAY(x-1,y-1) == ARRAY(x-1,y) && ARRAY(x-1,y-1) == ARRAY(x,y-1))
        printf("+");
      else if (ARRAY(x,y-1) == ARRAY(x-1,y-1) || ARRAY(x,y) == ARRAY(x-1,y))
        printf("-");
      else if (ARRAY(x-1,y) == ARRAY(x-1,y-1) || ARRAY(x,y) == ARRAY(x,y-1))
        printf("|");
      else
        printf("+");

/* print horizontal edge */
      if (x<width) {
        if (ARRAY(x,y) != ARRAY(x,y-1))
          printf("--");
        else
          printf("  ");
      }
    }
    printf("\n");

    if (y<height) {
      for(x=0;x<=width;x++) {
/* print vertical edge */
        if (ARRAY(x,y) != ARRAY(x-1,y)) 
          printf("|");
        else
          printf(" ");
/* print middle of squares */
        if (x<width)
          printf("  ");
      }
      printf("\n");
    }
  }
  printf("\n");
}

void transform(int x, int y, int transform_no, int *out_x, int *out_y) {
  switch (transform_no) {
    case 0: *out_x=x;
            *out_y=y;
            break;
    case 1: *out_x=-y;
            *out_y=x;
            break;
    case 2: *out_x=-x;
            *out_y=-y;
            break;
    case 3: *out_x=y;
            *out_y=-x;
            break;
    case 4: *out_x=-x;
            *out_y=y;
            break;
    case 5: *out_x=y;
            *out_y=x;
            break;
    case 6: *out_x=x;
            *out_y=-y;
            break;
    case 7: *out_x=-y;
            *out_y=-x;
            break;
  }
}

void random_permutation(int n, int a[]) {
  int i,j,k,r;

  for (i=0;i<n;i++) a[i] = -1;
  for (i=0;i<n;i++) {
    r=RAND(n-i);
    k=0;
    while(a[k]!=-1) k++;
    for (j=0;j<r;j++) {
      k++;
      while(a[k]!=-1) k++;
    }
    a[k]=i;
  }
}

int compare(const void *p1, const void *p2) {
  if (((point_type*)p1)->x<((point_type*)p2)->x) return -1;
  if (((point_type*)p1)->x>((point_type*)p2)->x) return 1;
  if (((point_type*)p1)->x==((point_type*)p2)->x) {
    if (((point_type*)p1)->y<((point_type*)p2)->y) return -1;
    if (((point_type*)p1)->y>((point_type*)p2)->y) return 1;
  }
  return 0;
}

void compute_displ() {
  int poly_no, index, x_offset, y_offset, i;
  int perm1[nr_polyominoes], perm2[8];
  point_type poly[polyominoes_len];

  srand(time(NULL));

  random_permutation(nr_polyominoes,perm1);
  for (poly_no=0;poly_no<nr_polyominoes;poly_no++) {
    transform_len[perm1[poly_no]] = polyomino[poly_no].transform_len;
    random_permutation(polyomino[poly_no].transform_len,perm2);
    for (index=0;index<polyomino[poly_no].transform_len;index++) {
      for (i=0;i<polyominoes_len;i++) {
        transform(polyomino[poly_no].point[i].x, polyomino[poly_no].point[i].y,
                  polyomino[poly_no].transform_list[index],
                  &poly[i].x, &poly[i].y);
      }

      x_offset = 1000;
      for (i=0;i<polyominoes_len;i++)
        if (poly[i].x<x_offset) x_offset=poly[i].x;
      y_offset = 1000;
      for (i=0;i<polyominoes_len;i++) if (poly[i].x == x_offset)
        if (poly[i].y<y_offset) y_offset=poly[i].y;
      for (i=0;i<polyominoes_len;i++) {
        poly[i].x -= x_offset;
        poly[i].y -= y_offset;
      }
      qsort(poly,polyominoes_len,sizeof(point_type),compare);
      for (i=0;i<polyominoes_len;i++) {
        displ_lu[perm1[poly_no]][perm2[index]][i] = ARRAY_INDEX_DISP(poly[i].x,poly[i].y);
      }

      x_offset = -1000;
      for (i=0;i<polyominoes_len;i++)
        if (poly[i].x>x_offset) x_offset=poly[i].x;
      y_offset = -1000;
      for (i=0;i<polyominoes_len;i++) if (poly[i].x == x_offset)
        if (poly[i].y>y_offset) y_offset=poly[i].y;
      for (i=0;i<polyominoes_len;i++) {
        poly[i].x -= x_offset;
        poly[i].y -= y_offset;
      }
      qsort(poly,polyominoes_len,sizeof(point_type),compare);
      for (i=0;i<polyominoes_len;i++) {
        displ_rd[perm1[poly_no]][perm2[index]][i] = ARRAY_INDEX_DISP(poly[i].x,poly[i].y);
      }
    }
  }
}

/* This recursive procedure actually does all the work.  It first looks
   for the first available point to attach a pentomino.  Then
   it tries to attach all the pentominoes in all possible ways to
   that that point.  If it succeeds then it recursively calls itself
   to add more pentominoes.  When all pentominoes have been attached
   it sends the solution to the printing procedure.
*/

void recursive_search(signed char *place_to_attach, int nr_attached) {
  int poly_no,index,i;
  int *current_displ;
  displ_type *displ=NULL;

  if (nr_attached == nr_polyominoes) {
    nr_found++;
    if (print_choice == 2) {
      printf("Number %ld\n",nr_found);
      pretty_print_array();
      fflush(stdout);
    } else if (print_choice == 3) {
      printf("Number %ld\n",nr_found);
      ugly_print_array();
      fflush(stdout);
    }
    return;
  }

  if (place_to_attach<=&ARRAY(18,2)) {
    displ=&displ_lu;
    for (;*place_to_attach!=-1 && place_to_attach<=&ARRAY(18,3);place_to_attach++);
    if (*place_to_attach!=-1)
      place_to_attach = &ARRAY(53,5);
  }

  if (place_to_attach>=&ARRAY(35,3)) {
    displ=&displ_rd;
    for (;*place_to_attach!=-1 && place_to_attach>=&ARRAY(35,3);place_to_attach--);
    if (*place_to_attach!=-1)
      place_to_attach = &ARRAY(19,0);
  }

  if (place_to_attach>=&ARRAY(19,0) && place_to_attach<=&ARRAY(34,5)) {
    displ=&displ_lu;
    for (;*place_to_attach!=-1;place_to_attach++);
  }



  for (poly_no=0;poly_no<nr_polyominoes;poly_no++) if (!(attached[poly_no])) {
    for (index=0;index<transform_len[poly_no];index++) {
      current_displ = (*displ)[poly_no][index];
      for (i=0;i<polyominoes_len;i++) {
        if (place_to_attach[current_displ[i]] != -1)
          goto label;
      }

      for (i=0;i<polyominoes_len;i++)
        place_to_attach[current_displ[i]] = poly_no;
      attached[poly_no] = 1;
      recursive_search(place_to_attach,nr_attached+1);
      for (i=0;i<polyominoes_len;i++)
        place_to_attach[current_displ[i]] = -1;
      attached[poly_no] = 0;

      label:;
    }
  }
}

int main(int argc, char **argv) {
  int x,y,i;

  get_user_input(argc, argv);
  compute_displ();
/* set the border values to -2 */
  for (i=0;i<sizeof(array)/sizeof(unsigned char);i++) array[i] = -2;
/* and initialise the blank spaces to -1 */
  for (y=0;y<3;y++) for (x=0;x<35;x++) ARRAY(x,y) = -1;
  for (y=3;y<6;y++) for (x=19;x<19+35;x++) ARRAY(x,y) = -1;
  memset(attached,0,sizeof(attached));
  recursive_search(&ARRAY(0,0),0);
  if (print_choice == 1)
    printf("Total number %ld\n",nr_found);
  exit(0);
}

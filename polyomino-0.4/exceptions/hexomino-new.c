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

/* This is a try at improving the search strategy.  Still in development. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *description = 
"Place the shape\n"
"\n"
" x\n"
"xxxxx\n"
"\n"
"into a 24 by 23 rectangle\n";

#define width 24
#define height 23

/* List of coordinates for the polyomino. */

typedef struct {int x,y;} point_type;

point_type pent_list[6] = 
{{0,0}, {1,0}, {2,0}, {3,0}, {4,0}, {1,1}};

/* List of transformations that need to be considered
   for each pentomino.  See the function transform
   below.
*/

struct transform_struct {int len; int transform_no[8];} transform_list = 
{8, {0, 1, 2, 3, 4, 5, 6, 7}};

/* A preprocessed list in which all the possible transforms have
   already been done to the pentominoes, and then all the points
   are converted to displacements via ARRAY_INDEX_DISP.
*/
int displ[8][6];

#define ARRAY_INDEX_DISP(x,y) ((x)*(height+8)+(y))
#define ARRAY(x,y) (array[ARRAY_INDEX_DISP(x+4,y+4)])
signed char array[(width+8)*(height+8)];
long nr_found = 0;

int print_choice;

#define RAND(n) ((int)((double)rand()*(n)/((double)RAND_MAX+1)))

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

void ugly_print_array() {
  int x,y;

  printf("\n");
  for(y=0;y<height;y++) {
    for(x=0;x<width;x++)
      printf("%d ",ARRAY(x,y));
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

void rand_perm(int n, int a[]) {
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
  int index, x_offset, y_offset, i;
  point_type poly[6];

    for (index=0;index<transform_list.len;index++) {
      for (i=0;i<6;i++) {
        transform(pent_list[i].x, pent_list[i].y,
                  transform_list.transform_no[index],
                  &poly[i].x, &poly[i].y);
      }
      x_offset = 1000;
      for (i=0;i<6;i++)
        if (poly[i].x<x_offset) x_offset=poly[i].x;
      y_offset = 1000;
      for (i=0;i<6;i++) if (poly[i].x == x_offset)
        if (poly[i].y<y_offset) y_offset=poly[i].y;
      for (i=0;i<6;i++) {
        poly[i].x -= x_offset;
        poly[i].y -= y_offset;
      }

      qsort(poly,6,sizeof(point_type),compare);
      for (i=0;i<6;i++) {
        displ[index][i] = ARRAY_INDEX_DISP(poly[i].x,poly[i].y);
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

int work = 0;

void recursive_search(signed char *place_to_attach, int nr_attached,
                      char *old_reason_not_to_attach,
                      int *detach_at) {
  int index,i;
  int *current_displ;
  char reason_not_to_attach[92];

work++;
/*
if (work%100==0)
pretty_print_array();
*/

  if (nr_attached == 92) {
    nr_found++;
    if (print_choice == 2) {
      printf("Number %ld\n",nr_found);
      pretty_print_array();
      fflush(stdout);
    } else if (print_choice == 3) {
      ugly_print_array();
      fflush(stdout);
    }
    *detach_at = nr_attached-1;
    return;
  }

  for(;*place_to_attach!=-1;place_to_attach++);

  memset(reason_not_to_attach,0,sizeof(reason_not_to_attach));
  if (place_to_attach[ARRAY_INDEX_DISP(-1,0)]>=0)
    reason_not_to_attach[(int)place_to_attach[ARRAY_INDEX_DISP(-1,0)]]=1;
  if (place_to_attach[ARRAY_INDEX_DISP(0,-1)]>=0)
    reason_not_to_attach[(int)place_to_attach[ARRAY_INDEX_DISP(0,-1)]]=1;

  for (index=0;index<transform_list.len;index++) {
    current_displ = displ[index];
    for (i=1;i<6;i++) {
      if (place_to_attach[current_displ[i]] != -1) {
        if (place_to_attach[current_displ[i]] >= 0)
          reason_not_to_attach[(int)place_to_attach[current_displ[i]]] = 1;
        goto label;
      }
    }

    for (i=0;i<6;i++)
      place_to_attach[current_displ[i]] = nr_attached;
    recursive_search(place_to_attach,nr_attached+1,reason_not_to_attach,detach_at);
    for (i=0;i<6;i++)
      place_to_attach[current_displ[i]] = -1;
/*
if(*detach_at!=nr_attached)
printf("%d %d\n",nr_attached,*detach_at);
if (work%1==0)
printf("%d %d\n",nr_attached,*detach_at);
if (*detach_at==0) pretty_print_array();
*/
    if (*detach_at < nr_attached) {
      for (i=0;i<92;i++) old_reason_not_to_attach[i] |= reason_not_to_attach[i];
      return;
    }
    label:;
  }
  *detach_at = nr_attached-1;
  while (*detach_at>0 && reason_not_to_attach[*detach_at]==0) (*detach_at)--;
  for (i=0;i<92;i++) old_reason_not_to_attach[i] |= reason_not_to_attach[i];
}

int main(int argc, char **argv) {
  int x,y,i;
  char junk1[92];
  int junk2;

  get_user_input(argc, argv);
  compute_displ();
/* set the border values to -2 */
  for (i=0;i<sizeof(array)/sizeof(unsigned char);i++) array[i] = -2;
/* and initialise the blank spaces to -1 */
  for (x=0;x<width;x++) for (y=0;y<height;y++) ARRAY(x,y) = -1;
  recursive_search(&ARRAY(0,0),0,junk1,&junk2);
  if (print_choice == 1)
    printf("Total number %ld\n",nr_found);
  exit(0);
}

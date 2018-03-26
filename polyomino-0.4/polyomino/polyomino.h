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
#include <time.h>
#include <math.h>
#include <string.h>

typedef struct {int x,y;} point_type;

static struct {int len; point_type point[polyomino_len];
               int transform_len, transform_list[8], max_white;} 
  polyomino[nr_polyominoes];

#include "poly-list.h"

#define RAND(n) ((int)((double)rand()*(n)/((double)RAND_MAX+1)))

#define ARRAY_INDEX_DISP(x,y) ((x)*(height+2*(polyomino_len-1))+(y))
#define ARRAY(x,y) (array[ARRAY_INDEX_DISP(x+polyomino_len-1,y+polyomino_len-1)])
int len[nr_polyominoes];

signed char array[(max_width+2*(polyomino_len-1))*(max_height+2*(polyomino_len-1))];
int attached[nr_polyominoes];
double nr_found = 0;

int print_choice;

#ifndef width
int width;
#endif
#ifndef height
int height;
#endif
#ifndef search_method
int search_method;
#endif
#ifndef check_size_of_blank_regions
int check_size_of_blank_regions;
#endif
#ifndef all_rotations
int all_rotations;
#endif
#ifndef rand_seed
int rand_seed;
#endif

#ifdef CHECK_PROGRESS
struct {int poly_no;} list[nr_polyominoes];
int orig_transform_len[nr_polyominoes];
time_t time_started;
#endif

/* A preprocessed list in which all the possible transforms have
   already been done to the pentominoes, and then all the points
   are converted to displacements via ARRAY_INDEX_DISP.
*/

int displ_lu[nr_polyominoes][8][polyomino_len];
int transform_len_lu[nr_polyominoes];

#define LEFT       1<<0
#define RIGHT      1<<1
#define UP         1<<2
#define DOWN       1<<3
#define LEFT_UP    1<<4
#define LEFT_DOWN  1<<5
#define RIGHT_UP   1<<6
#define RIGHT_DOWN 1<<7
int displ_ws[256][nr_polyominoes][8*polyomino_len][polyomino_len];
int transform_len_ws[256][nr_polyominoes];

#define get_arg(dst)					\
  if (argc>=arg_no+1) (dst) = atoi(argv[arg_no]);	\
  else                (dst) = -1;			\
  arg_no++

void get_user_input(int argc, char **argv) {
  char input[101];
  int arg_no = 1;
#ifndef width
  int choice, i, n=0, m;

  get_arg(choice);
#endif
  get_arg(print_choice);
#ifndef search_method
  get_arg(search_method);
#endif
#ifndef check_size_of_blank_regions
  get_arg(check_size_of_blank_regions);
#endif
#ifndef all_rotations
  get_arg(all_rotations);
  if (all_rotations == -1) all_rotations = 0;
#endif
#ifndef rand_seed
  get_arg(rand_seed);
#endif

  if (print_choice != 1 && print_choice != 3)
    fprintf(stderr,description);

#ifndef width
  for (i=min_height;i*i<area_rectangle;i++)
    if (area_rectangle/i*i == area_rectangle) n++;
  while(choice<1 || choice>n) {
    fprintf(stderr,"Choose size of rectangle:\n");
    m = 0;
    for (i=min_height;i*i<area_rectangle;i++) if (area_rectangle/i*i == area_rectangle) {
      m++;
      fprintf(stderr,  "%d) %d by %d rectangle.\n",m,area_rectangle/i,i);
    }
    fprintf(stderr,"Input choice (number between 1 and %d): ",n);
    fgets(input,100,stdin);
    choice = atoi(input);
    fprintf(stderr,"\n");
  }
  for (i=min_height,m=0;m<choice;i++) if (area_rectangle/i*i == area_rectangle) m++;
  i--;
  width = area_rectangle/i;
  height = i;
#endif

  while(print_choice<1 || print_choice>4) {
    fprintf(stderr,"Select your printing choice:\n"
           "  1) Just count solutions;\n"
           "  2) Pretty print the solutions;\n"
           "  3) Ugly print the solutions;\n"
           "  4) Pretty print every 1000th solutions.\n"
           "Input choice (number between 1 and 4): ");
    fgets(input,100,stdin);
    print_choice = atoi(input);
    fprintf(stderr,"\n");
  }

#ifndef search_method
  if (search_method<1 || search_method>4) fprintf(stderr,
"Here we select the method of finding the next blank space to try to place\n"
"all remaining polyominoes in all possible orientations.\n"
"Method 1.  Look for the left most column with blank spaces, and then look\n"
"for the upper most blank space.\n"
"Method 2.  For each blank space, for each remaining polyomino, count the\n"
"number of ways it can be placed covering that blank space.  Choose the\n"
"blank space with the smallest total.\n"
"Method 3.  If there is a blank space in which all four immediate neighbors\n"
"are filled, choose that.  Otherwise choose one with three immediate\n"
"neighbors filled.  Otherwise choose one with two opposite neighbors filled.\n"
"Otherwise choose one with two adjecent neighbors filled.  In case of a tie,\n"
"use method 1.\n"
"Method 4.  Like method 2, but in determining how many ways each polyomino\n"
"can be placed, only look at the eight immediate neighbors (left-right,\n"
"up-down and diagonal).\n");

  while(search_method<1 || search_method>4) {
    fprintf(stderr,"Select your search_method:\n"
           "  1) Work from left to right;\n"
           "  2) Use worst score method;\n"
           "  3) Use small space method;\n"
           "  4) Use short worst score method.\n"
           "Input choice (number between 1 and 4): ");
    fgets(input,100,stdin);
    search_method = atoi(input);
    fprintf(stderr,"\n");
  }
#endif

#ifndef check_size_of_blank_regions
  if (check_size_of_blank_regions<0) fprintf(stderr,
"Here we select the option of doing the following.  Before each polyomino\n"
"is place, look at all the connected components formed by the blank spaces,\n"
"and count how many blank spaces are in each.  See that from a numerical point\n"
"of view that this number is compatible with the problem in hand (for example\n"
"with the pentominoes problem that the number is divisible by 5).  Select the\n"
"next blank space (using one of the methods described above) only from the\n"
"component containing the fewest blank spaces.\n");

  while(check_size_of_blank_regions<0) {
    fprintf(stderr,"Do you wish to check the size of blank regions? (y or n): ");
    fgets(input,100,stdin);
    if (strcmp(input,"y\n")==0 || strcmp(input,"Y\n")==0)
      check_size_of_blank_regions = 1;
    if (strcmp(input,"n\n")==0 || strcmp(input,"N\n")==0)
      check_size_of_blank_regions = 0;
    fprintf(stderr,"\n");
  }
#endif
}

char colors[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"
                "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"
                "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"
                "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"
                "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"
                "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"
                "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"
                "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";

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

void rotate_pretty_print_array() {
  int x,y;

  printf("\n");
  for(x=0;x<=width;x++) {
    for(y=0;y<=height;y++) {
/* print corner */
      if (ARRAY(x,y) == ARRAY(x-1,y) && ARRAY(x,y) == ARRAY(x,y-1) && ARRAY(x,y) == ARRAY(x-1,y-1))
        printf(" ");
      else if (ARRAY(x,y) == ARRAY(x-1,y) && ARRAY(x,y) == ARRAY(x,y-1))
        printf("+");
      else if (ARRAY(x,y-1) == ARRAY(x-1,y-1) && ARRAY(x,y-1) == ARRAY(x,y))
        printf("+");
      else if (ARRAY(x-1,y) == ARRAY(x-1,y-1) && ARRAY(x-1,y) == ARRAY(x,y))
        printf("+");
      else if (ARRAY(x-1,y-1) == ARRAY(x,y-1) && ARRAY(x-1,y-1) == ARRAY(x-1,y))
        printf("+");
      else if (ARRAY(x-1,y) == ARRAY(x-1,y-1) || ARRAY(x,y) == ARRAY(x,y-1))
        printf("-");
      else if (ARRAY(x,y-1) == ARRAY(x-1,y-1) || ARRAY(x,y) == ARRAY(x-1,y))
        printf("|");
      else
        printf("+");

/* print horizontal edge */
      if (y<height) {
        if (ARRAY(x,y) != ARRAY(x-1,y))
          printf("--");
        else
          printf("  ");
      }
    }
    printf("\n");

    if (x<width) {
      for(y=0;y<=height;y++) {
/* print vertical edge */
        if (ARRAY(x,y) != ARRAY(x,y-1)) 
          printf("|");
        else
          printf(" ");
/* print middle of squares */
        if (y<height)
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

void compute_displ_lu() {
  int poly_no, index, x_offset, y_offset, i;
  int perm1[nr_polyominoes], perm2[8];
  point_type poly[polyomino_len];

  random_permutation(nr_polyominoes,perm1);
  for (poly_no=0;poly_no<nr_polyominoes;poly_no++) {
    len[perm1[poly_no]] = polyomino[poly_no].len;
#ifdef CHECK_PROGRESS
    orig_transform_len[perm1[poly_no]] = polyomino[poly_no].transform_len;
#endif
    transform_len_lu[perm1[poly_no]] = polyomino[poly_no].transform_len;
    random_permutation(polyomino[poly_no].transform_len,perm2);
    for (index=0;index<polyomino[poly_no].transform_len;index++) {
      for (i=0;i<polyomino[poly_no].len;i++) {
        transform(polyomino[poly_no].point[i].x, polyomino[poly_no].point[i].y,
                  polyomino[poly_no].transform_list[perm2[index]],
                  &poly[i].x, &poly[i].y);
      }
      x_offset = 1000;
      for (i=0;i<polyomino[poly_no].len;i++)
        if (poly[i].x<x_offset) x_offset=poly[i].x;
      y_offset = 1000;
      for (i=0;i<polyomino[poly_no].len;i++) if (poly[i].x == x_offset)
        if (poly[i].y<y_offset) y_offset=poly[i].y;
      for (i=0;i<polyomino[poly_no].len;i++) {
        poly[i].x -= x_offset;
        poly[i].y -= y_offset;
      }
      qsort(poly,polyomino[poly_no].len,sizeof(point_type),compare);
      for (i=0;i<polyomino[poly_no].len;i++) {
        displ_lu[perm1[poly_no]][index][i] = 
          ARRAY_INDEX_DISP(poly[i].x,poly[i].y);
      }
    }
  }
}

void compute_displ_ws() {
  int poly_no, index, i, j, b;
  int perm1[nr_polyominoes], perm2[8], perm3[polyomino_len];
  point_type poly[polyomino_len];

  random_permutation(nr_polyominoes,perm1);
  for (poly_no=0;poly_no<nr_polyominoes;poly_no++) {
    len[perm1[poly_no]] = polyomino[poly_no].len;
#ifdef CHECK_PROGRESS
    orig_transform_len[perm1[poly_no]] = polyomino[poly_no].transform_len;
#endif
    for (b=0;b<256;b++)
      transform_len_ws[b][perm1[poly_no]] = 0;
    random_permutation(polyomino[poly_no].transform_len,perm2);
    random_permutation(polyomino[poly_no].len,perm3);
    for (index=0;index<polyomino[poly_no].transform_len;index++) {
      for (i=0;i<polyomino[poly_no].len;i++) {
        transform(polyomino[poly_no].point[i].x, polyomino[poly_no].point[i].y,
                  polyomino[poly_no].transform_list[perm2[index]],
                  &poly[i].x, &poly[i].y);
      }

      qsort(poly,polyomino[poly_no].len,sizeof(point_type),compare);
      for (b=0;b<256;b++) for (j=0;j<polyomino[poly_no].len;j++) {
        for (i=0;i<polyomino[poly_no].len;i++) {
          if (b&LEFT && poly[i].x-poly[perm3[j]].x==-1 && poly[i].y-poly[perm3[j]].y==0)
            goto label;
          if (b&RIGHT && poly[i].x-poly[perm3[j]].x==1 && poly[i].y-poly[perm3[j]].y==0)
            goto label;
          if (b&UP && poly[i].x-poly[perm3[j]].x==0 && poly[i].y-poly[perm3[j]].y==-1)
            goto label;
          if (b&DOWN && poly[i].x-poly[perm3[j]].x==0 && poly[i].y-poly[perm3[j]].y==1)
            goto label;
          if (b&LEFT_UP && poly[i].x-poly[perm3[j]].x==-1 && poly[i].y-poly[perm3[j]].y==-1)
            goto label;
          if (b&LEFT_DOWN && poly[i].x-poly[perm3[j]].x==-1 && poly[i].y-poly[perm3[j]].y==1)
            goto label;
          if (b&RIGHT_UP && poly[i].x-poly[perm3[j]].x==1 && poly[i].y-poly[perm3[j]].y==-1)
            goto label;
          if (b&RIGHT_DOWN && poly[i].x-poly[perm3[j]].x==1 && poly[i].y-poly[perm3[j]].y==1)
            goto label;
        }
        for (i=0;i<polyomino[poly_no].len;i++) {
          displ_ws[b][perm1[poly_no]][transform_len_ws[b][perm1[poly_no]]][i] = 
            ARRAY_INDEX_DISP(poly[i].x-poly[perm3[j]].x,poly[i].y-poly[perm3[j]].y);
        }
        transform_len_ws[b][perm1[poly_no]]++;
        label:;
      }
    }
  }
}

#ifdef CHECK_PROGRESS
void print_estimate() {
  double all=0, done=0;
  int this_all, this_done;
  int i,j;
  time_t time_done;
  double time_remaining;

  for (i=0;i<nr_polyominoes;i++) {
    this_all = 0;
    for (j=i;j<nr_polyominoes;j++)
      this_all += orig_transform_len[list[j].poly_no];
    this_done = 0;
    for (j=i;j<nr_polyominoes;j++) if (list[j].poly_no<list[i].poly_no)
      this_done += orig_transform_len[list[j].poly_no];
    done *= this_all;
    all *= this_all;
    done += this_done;
    all += this_all-1;
  }

  printf("Estimated total number %.3g\n",floor(all/done*nr_found));
  printf("Estimate percentage done %.3g\n",done/all*100);

  time_done = time(NULL)-time_started;
  printf("Time ellapsed ");
  if (time_done/(3600*24)>0)
    printf("%ld days ",time_done/(3600*24));
  if (time_done/3600>0)
    printf("%ld hours ",(time_done%(3600*24)/3600));
  if (time_done/60>0)
    printf("%ld minutes ",(time_done%(3600)/60));
  printf("%ld seconds\n",time_done%60);

  if (done>0 && done<all) {
    time_remaining = time_done*(all/done-1);
    printf("Estimated time remaining ");
    if (time_remaining/(3600*24*365.25)>=1)
      printf("%g years\n",rint(time_remaining/(360*24*365.25))/10);
    else if (time_remaining/(3600*24*30)>=1)
      printf("%g months\n",rint(time_remaining/(360*24*30))/10);
    else if (time_remaining/(3600*24)>=1)
      printf("%g days\n",rint(time_remaining/(360*24))/10);
    else if (time_remaining/3600>=1)
      printf("%g hours\n",rint(time_remaining/360)/10);
    else if (time_remaining/60>=1)
      printf("%g minutes\n",rint(time_remaining/6)/10);
    else
      printf("%g seconds\n",rint(time_remaining));
  }
}

#endif

#ifdef IDENTICAL

#define ARRAY_TRANS(transform,x,y)				\
  ((transform)==0 ? ARRAY(x,y) :				\
   (transform)==2 ? ARRAY(width-1-(x),height-1-(y)) :		\
   (transform)==4 ? ARRAY(width-1-(x),y) :			\
   (transform)==6 ? ARRAY(x,height-1-(y)) : 0)

int check_symmetry(int transform) {
  int map[width*height];
  int m,x,y;

  for (m=0;m<width*height;m++) map[m] = -1;
  for (x=0;x<width;x++) for (y=0;y<height;y++)
    if (map[(int)ARRAY(x,y)]==-1)
      map[(int)ARRAY(x,y)] = ARRAY_TRANS(transform,x,y);
    else
      if (map[(int)ARRAY(x,y)]!=ARRAY_TRANS(transform,x,y))
        return 0;
  return 1;
}

#endif

void print_solution() {
  if (print_choice == 2) {
    printf("Number %.15g\n",nr_found);
#ifdef IDENTICAL
    if (check_symmetry(2))
      printf("180 degree rotational symmetry.\n");
    if (check_symmetry(4))
      printf("reflective vertical symmetry\n");
    if (check_symmetry(6))
      printf("reflective horizontal symmetry\n");
#endif
#ifdef CHECK_PROGRESS
    print_estimate();
#endif
    if (width*3+1<75 || width<height)
      pretty_print_array();
    else
      rotate_pretty_print_array();
    fflush(stdout);
  } else if (print_choice == 3) {
    printf("Number %.15g\n",nr_found);
#ifdef IDENTICAL
    if (check_symmetry(2))
      printf("180 degree rotational symmetry.\n");
    if (check_symmetry(4))
      printf("reflective vertical symmetry\n");
    if (check_symmetry(6))
      printf("reflective horizontal symmetry\n");
#endif
#ifdef CHECK_PROGRESS
    print_estimate();
#endif
    ugly_print_array();
    fflush(stdout);
  } else if (print_choice == 4 && floor(nr_found/1000)*1000 == nr_found) {
    printf("Number %.15g\n",nr_found);
#ifdef CHECK_PROGRESS
    print_estimate();
#endif
    if (width*3+1<75 || width<height)
      pretty_print_array();
    else
      rotate_pretty_print_array();
    fflush(stdout);
  }
}

int count_adjacent_blanks(signed char *p, signed char blank) {
  int count = 1;

  *p = blank;
  if (p[ARRAY_INDEX_DISP(-1,0)]==-1)
    count += count_adjacent_blanks(p+ARRAY_INDEX_DISP(-1,0),blank);
  if (p[ARRAY_INDEX_DISP( 1,0)]==-1)
    count += count_adjacent_blanks(p+ARRAY_INDEX_DISP( 1,0),blank);
  if (p[ARRAY_INDEX_DISP(0,-1)]==-1)
    count += count_adjacent_blanks(p+ARRAY_INDEX_DISP(0,-1),blank);
  if (p[ARRAY_INDEX_DISP(0, 1)]==-1)
    count += count_adjacent_blanks(p+ARRAY_INDEX_DISP(0, 1),blank);
  return count;
}

int find_blank_lu(signed char **place_to_attach, signed char smallest_blank) {
  if (check_size_of_blank_regions)
    *place_to_attach=&ARRAY(0,0);
  while(**place_to_attach!=smallest_blank) (*place_to_attach)++;

  return 1;
}

int find_blank_ws(signed char **place_to_attach, signed char smallest_blank,
                  int *bitmap) {
  int poly_no, index, i;
  int worst_score, score, b;
  signed char *p;
  int *current_displ;

  worst_score=1000000000;
  for(p=&ARRAY(0,0);p<=&ARRAY(width,height);p++) if (*p==smallest_blank) {
    b = 0;
    if (p[ARRAY_INDEX_DISP(-1, 0)] != smallest_blank) b|=LEFT;
    if (p[ARRAY_INDEX_DISP( 1, 0)] != smallest_blank) b|=RIGHT;
    if (p[ARRAY_INDEX_DISP( 0,-1)] != smallest_blank) b|=UP;
    if (p[ARRAY_INDEX_DISP( 0, 1)] != smallest_blank) b|=DOWN;
    if (p[ARRAY_INDEX_DISP(-1,-1)] != smallest_blank) b|=LEFT_UP;
    if (p[ARRAY_INDEX_DISP(-1, 1)] != smallest_blank) b|=LEFT_DOWN;
    if (p[ARRAY_INDEX_DISP( 1,-1)] != smallest_blank) b|=RIGHT_UP;
    if (p[ARRAY_INDEX_DISP( 1, 1)] != smallest_blank) b|=RIGHT_DOWN;
    if (b!=0) {
      score=0;

      for (poly_no=0;poly_no<nr_polyominoes;poly_no++) if (!attached[poly_no]) {
        for (index=0;index<transform_len_ws[b][poly_no];index++) {
          current_displ = displ_ws[b][poly_no][index];
          for (i=0;i<LEN(poly_no);i++) {
            if (p[current_displ[i]] != smallest_blank)
              goto label2;
          }
          score++;
          if (score>=worst_score) goto label3;
          label2:;
        }
      }
      if (score == 0)
        return 0;
      if (score<worst_score) {
        worst_score = score;
        *place_to_attach = p;
        *bitmap = b;
      }
    }
    label3:;
  }
  return 1;
}

int find_blank_sws(signed char **place_to_attach, signed char smallest_blank,
                   int *bitmap) {
  int poly_no;
  int worst_score, score, b;
  signed char *p;

  worst_score=1000000000;
  for(p=&ARRAY(0,0);p<=&ARRAY(width,height);p++) if (*p==smallest_blank) {
    b = 0;
    if (p[ARRAY_INDEX_DISP(-1, 0)] != smallest_blank) b|=LEFT;
    if (p[ARRAY_INDEX_DISP( 1, 0)] != smallest_blank) b|=RIGHT;
    if (p[ARRAY_INDEX_DISP( 0,-1)] != smallest_blank) b|=UP;
    if (p[ARRAY_INDEX_DISP( 0, 1)] != smallest_blank) b|=DOWN;
    if (p[ARRAY_INDEX_DISP(-1,-1)] != smallest_blank) b|=LEFT_UP;
    if (p[ARRAY_INDEX_DISP(-1, 1)] != smallest_blank) b|=LEFT_DOWN;
    if (p[ARRAY_INDEX_DISP( 1,-1)] != smallest_blank) b|=RIGHT_UP;
    if (p[ARRAY_INDEX_DISP( 1, 1)] != smallest_blank) b|=RIGHT_DOWN;
    if (b!=0) {
      score=0;

      for (poly_no=0;poly_no<nr_polyominoes;poly_no++) if (!attached[poly_no]) {
        score += transform_len_ws[b][poly_no];
        if (score>=worst_score) goto label3;
      }
      if (score == 0) {
        if (check_size_of_blank_regions)
          for(p=&ARRAY(0,0);p<=&ARRAY(width,height);p++) 
            if (*p<=-10) *p=-1;
        return 0;
      }
      if (score<worst_score) {
        worst_score = score;
        *place_to_attach = p;
        *bitmap = b;
      }
    }
    label3:;
  }
  return 1;
}

int find_blank_small(signed char **place_to_attach, signed char smallest_blank,
                     int *bitmap) {
  int best_score, score, b;
  signed char *p;

  best_score=0;
  for(p=&ARRAY(0,0);p<=&ARRAY(width,height);p++) if (*p==smallest_blank) {
    b = 0;
    score = 0;
    if (p[ARRAY_INDEX_DISP(-1, 0)] != smallest_blank) {
      b|=LEFT;
      score += 20;
    }
    if (p[ARRAY_INDEX_DISP( 1, 0)] != smallest_blank) {
      b|=RIGHT;
      score += (b&LEFT)?40:20;
    }
    if (p[ARRAY_INDEX_DISP( 0,-1)] != smallest_blank) {
      b|=UP;
      score += 20;
    }
    if (p[ARRAY_INDEX_DISP( 0, 1)] != smallest_blank) {
      b|=DOWN;
      score += (b&UP)?40:20;
    }
    if (p[ARRAY_INDEX_DISP(-1,-1)] != smallest_blank) {
      b|=LEFT_UP;
      score++;
    }
    if (p[ARRAY_INDEX_DISP(-1, 1)] != smallest_blank) {
      b|=LEFT_DOWN;
      score++;
    }
    if (p[ARRAY_INDEX_DISP( 1,-1)] != smallest_blank) {
      b|=RIGHT_UP;
      score++;
    }
    if (p[ARRAY_INDEX_DISP( 1, 1)] != smallest_blank) {
      b|=RIGHT_DOWN;
      score++;
    }
    if (score>=120) {
      if (check_size_of_blank_regions)
        for(p=&ARRAY(0,0);p<=&ARRAY(width,height);p++) 
          if (*p<=-10) *p=-1;
      return 0;
    }
    if (score>best_score) {
      best_score = score;
      *place_to_attach = p;
      *bitmap = b;
    }
  }
  return 1;
}

/* This recursive procedure actually does all the work.  It first looks
   for the first available point to attach a polyomino.  Then
   it tries to attach all the polyominoes in all possible ways to
   that that point.  If it succeeds then it recursively calls itself
   to add more polyominoes.  When all polyominoes have been attached
   it sends the solution to the printing procedure.
*/

void recursive_search(signed char *place_to_attach, int nr_attached) {
#ifndef IDENTICAL
  int poly_no;
#endif
  int index,i,bitmap;
  int *current_displ;
  int count, c, smallest_count;
  int blank_found;
  signed char blank, smallest_blank = -1;
  signed char *p;

  if (nr_attached == nr_polyominoes) {
    nr_found++;
    print_solution();
    return;
  }

  if (check_size_of_blank_regions) {
    smallest_count = 1000000000;
    blank = -10;
    for (p=&ARRAY(0,0);p<=&ARRAY(width,height);p++) if (*p==-1) {
      c = count = count_adjacent_blanks(p,blank);
      BLANK_REGION_TEST {
        for(p=&ARRAY(0,0);p<=&ARRAY(width,height);p++) 
          if (*p<=-10) *p=-1;
        return;
      }
      if (count<smallest_count) {
        smallest_count = count;
        smallest_blank = blank;
      }
      blank--;
    }
  }

  if (search_method==1)
    blank_found = find_blank_lu(&place_to_attach, smallest_blank);
  else if (search_method==2)
    blank_found = find_blank_ws(&place_to_attach, smallest_blank, &bitmap);
  else if (search_method==3)
    blank_found = find_blank_sws(&place_to_attach, smallest_blank, &bitmap);
  else if (search_method==4)
    blank_found = find_blank_small(&place_to_attach, smallest_blank, &bitmap);
  else
    blank_found = 0; /* Put here to stop -Wall errors. */

  if (check_size_of_blank_regions) {
    for(p=&ARRAY(0,0);p<=&ARRAY(width,height);p++) 
      if (*p<=-10) *p=-1;
  }

  if (!blank_found) return;

  if (search_method==1) {
#ifdef IDENTICAL
#define poly_no nr_attached
#else
    for (poly_no=0;poly_no<nr_polyominoes;poly_no++) if (!attached[poly_no]) {
#endif
      for (index=0;index<transform_len_lu[poly_no];index++) {
        current_displ = displ_lu[poly_no][index];
        for (i=0;i<LEN(poly_no);i++) {
          if (place_to_attach[current_displ[i]] != -1)
            goto label1;
        }

        for (i=0;i<LEN(poly_no);i++)
          place_to_attach[current_displ[i]] = poly_no;
        attached[poly_no] = 1;
#ifdef CHECK_PROGRESS
        list[nr_attached].poly_no=poly_no;
#endif
        recursive_search(place_to_attach,nr_attached+1);
        for (i=0;i<LEN(poly_no);i++)
          place_to_attach[current_displ[i]] = -1;
        attached[poly_no] = 0;

        label1:;
      }
#ifdef IDENTICAL
#undef poly_no nr_attached
#else
    }
#endif
  }
  else {
#ifdef IDENTICAL
#define poly_no nr_attached
#else
    for (poly_no=0;poly_no<nr_polyominoes;poly_no++) if (!attached[poly_no]) {
#endif
      for (index=0;index<transform_len_ws[bitmap][poly_no];index++) {
        current_displ = displ_ws[bitmap][poly_no][index];
        for (i=0;i<LEN(poly_no);i++) {
          if (place_to_attach[current_displ[i]] != -1)
            goto label2;
        }

        for (i=0;i<LEN(poly_no);i++)
          place_to_attach[current_displ[i]] = poly_no;
        attached[poly_no] = 1;
#ifdef CHECK_PROGRESS
        list[nr_attached].poly_no=poly_no;
#endif
        recursive_search(place_to_attach,nr_attached+1);
        for (i=0;i<LEN(poly_no);i++)
          place_to_attach[current_displ[i]] = -1;
        attached[poly_no] = 0;

        label2:;
      }
#ifdef IDENTICAL
#undef poly_no nr_attached
#else
    }
#endif
  }
}

/* Stop unused variable errors */
void dummy() {
  void *junk;

  junk = tetromino;
  junk = pentomino;
  junk = heptomino;
}

void limit_rotations() {
  int i;

  if (!all_rotations)
    while (1) {
      i = RAND(nr_polyominoes);
      if (polyomino[i].transform_len == 8) {
        polyomino[i].transform_len = 2;
        polyomino[i].transform_list[0] = RAND(4)*2;
        polyomino[i].transform_list[1] = RAND(4)*2+1;
        break;
      }
    }
}

int test_symmetry(point_type *point, int len, int transform_no) {
  point_type point1[polyomino_len], point2[polyomino_len];
  int i,x_min,y_min;

  memcpy(point1,point,len*sizeof(point_type));
  x_min = 1000000;
  for (i=0;i<len;i++)
    if (point1[i].x < x_min) x_min = point1[i].x;
  y_min = 1000000;
  for (i=0;i<len;i++)
    if (point1[i].y < y_min) y_min = point1[i].y;
  for (i=0;i<len;i++) {
    point1[i].x -= x_min;
    point1[i].y -= y_min;
  }
  qsort(point1,len,sizeof(point_type),compare);

  for (i=0;i<len;i++)
    transform(point[i].x,point[i].y,transform_no,
              &point2[i].x,&point2[i].y);
  x_min = 1000000;
  for (i=0;i<len;i++)
    if (point2[i].x < x_min) x_min = point2[i].x;
  y_min = 1000000;
  for (i=0;i<len;i++)
    if (point2[i].y < y_min) y_min = point2[i].y;
  for (i=0;i<len;i++) {
    point2[i].x -= x_min;
    point2[i].y -= y_min;
  }
  qsort(point2,len,sizeof(point_type),compare);

  for (i=0;i<len;i++)
    if (point1[i].x!=point2[i].x || point1[i].y!=point2[i].y)
      return 0;
  return 1;
}

void limit_one_sided_rotations() {
  int i;

  if (!all_rotations)
    while (1) {
      i = RAND(nr_polyominoes);
      if ((test_symmetry(polyomino[i].point,polyomino[i].len,5)
           || test_symmetry(polyomino[i].point,polyomino[i].len,7))
           && !test_symmetry(polyomino[i].point,polyomino[i].len,1)
           && !test_symmetry(polyomino[i].point,polyomino[i].len,2)
           && !test_symmetry(polyomino[i].point,polyomino[i].len,3)) {
        polyomino[i].transform_len = 1;
        polyomino[i].transform_list[0] = RAND(4);
        break;
      }
    }
}

void make_one_sided_tetromino() {
  int i,j,t,u;

  j=0;
  for (i=0;i<5;i++) {
    one_sided_tetromino[j] = tetromino[i];
    for (t=0;t<8;t++)
      if (one_sided_tetromino[j].transform_list[t]>=4) {
        one_sided_tetromino[j].transform_len = t;
        j++;
        one_sided_tetromino[j] = tetromino[i];
        for (u=t;u<8;u++)
          one_sided_tetromino[j].transform_list[u-t] = 
            one_sided_tetromino[j].transform_list[u];
        one_sided_tetromino[j].transform_len -= t;
        break;
      }
    j++;
  }
}

void make_one_sided_pentomino() {
  int i,j,t,u;

  j=0;
  for (i=0;i<12;i++) {
    one_sided_pentomino[j] = pentomino[i];
    for (t=0;t<8;t++)
      if (one_sided_pentomino[j].transform_list[t]>=4) {
        one_sided_pentomino[j].transform_len = t;
        j++;
        one_sided_pentomino[j] = pentomino[i];
        for (u=t;u<8;u++)
          one_sided_pentomino[j].transform_list[u-t] = 
            one_sided_pentomino[j].transform_list[u];
        one_sided_pentomino[j].transform_len -= t;
        break;
      }
    j++;
  }
}

void make_one_sided_hexomino() {
  int i,j,t,u;

  j=0;
  for (i=0;i<35;i++) {
    one_sided_hexomino[j] = hexomino[i];
    for (t=0;t<8;t++)
      if (one_sided_hexomino[j].transform_list[t]>=4) {
        one_sided_hexomino[j].transform_len = t;
        j++;
        one_sided_hexomino[j] = hexomino[i];
        for (u=t;u<8;u++)
          one_sided_hexomino[j].transform_list[u-t] = 
            one_sided_hexomino[j].transform_list[u];
        one_sided_hexomino[j].transform_len -= t;
        break;
      }
    j++;
  }
}

#define copy_polyomino(dst,src)						\
  (dst).len = (src).len;						\
  memcpy((dst).point,(src).point,sizeof(point_type)*(src).len);		\
  (dst).transform_len = (src).transform_len;				\
  memcpy((dst).transform_list,(src).transform_list,sizeof(int)*8);	\
  (dst).max_white = (src).max_white

#define load_set(poly_set,size)						\
  for (i=0;i<size;i++) {						\
    copy_polyomino(polyomino[k],poly_set[i]);				\
    k++;								\
  }

#define load_tetromino							\
  load_set(tetromino,5)

#define load_pentomino							\
  load_set(pentomino,12)

#define load_one_sided_tetromino					\
  make_one_sided_tetromino();						\
  load_set(one_sided_tetromino,7)

#define load_one_sided_pentomino					\
  make_one_sided_pentomino();						\
  load_set(one_sided_pentomino,18)

#define load_hexomino							\
  load_set(hexomino,35)

#define load_one_sided_hexomino						\
  make_one_sided_hexomino();						\
  load_set(one_sided_hexomino,60)

#define load_heptomino							\
  load_set(heptomino,108)

void make_polyomino();
void carve_out_shape();

int main(int argc, char **argv) {
  int x,y,i;

  get_user_input(argc, argv);
  if (rand_seed == -1)
    srand(time(NULL));
  else
    srand(rand_seed);
  make_polyomino();
  if (search_method == 1)
    compute_displ_lu();
  else
    compute_displ_ws();
/* set the border values to -2 */
  for (i=0;i<sizeof(array)/sizeof(unsigned char);i++) array[i] = -2;
/* and initialise the blank spaces to -1 */
  for (x=0;x<width;x++) for (y=0;y<height;y++) ARRAY(x,y) = -1;
  carve_out_shape();
  memset(attached,0,sizeof(attached));
#ifdef CHECK_PROGRESS
  time_started = time(NULL);
#endif
  recursive_search(&ARRAY(0,0),0);
  if (print_choice == 1 || print_choice == 4)
    printf("Total number %.15g\n",nr_found);
  exit(0);
}

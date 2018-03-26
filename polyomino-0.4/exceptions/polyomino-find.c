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

#define MAX_N 100

char *description = "";

typedef struct {int x,y;} point_type;

struct polyomino_struct {
  int len, x_max, y_max;
  point_type *point;
  struct polyomino_struct *next;
  int transform_len, transform_list[8];
  int max_white;
} *first_polyomino[MAX_N+1];

int nr_found[MAX_N+1];

int print_choice;

void get_user_input(int argc, char **argv, int *n) {
  char input[101];

  if (argc>=2) *n = atoi(argv[1]);
  else         *n = 0;
  if (argc>=3) print_choice = atoi(argv[2]);
  else         print_choice = 0;

  if (print_choice != 1 && print_choice != 3)
    fprintf(stderr,description);

  while(*n<1 || *n>MAX_N) {
    fprintf(stderr,"Choose the number of squares in the polyomino: ");
    fgets(input,100,stdin);
    *n = atoi(input);
    fprintf(stderr,"\n");
  }

  while(print_choice<1 || print_choice>4) {
    fprintf(stderr,"Select your printing choice:\n"
           "  1) Just count solutions;\n"
           "  2) Pretty print the solutions;\n"
           "  3) Ugly print the solutions.\n"
           "  4) Print C code.\n"
           "Input choice (number between 1 and 4): ");
    fgets(input,100,stdin);
    print_choice = atoi(input);
    fprintf(stderr,"\n");
  }
}

int array(struct polyomino_struct p, int x, int y) {
  int i;

  for (i=0;i<p.len;i++)
    if (p.point[i].x == x && p.point[i].y == y)
      return 0;
  return -1;
}

void pretty_print(struct polyomino_struct p) {
  int x,y;

  for(y=0;y<=p.y_max+1;y++) {
    for(x=0;x<=p.x_max+1;x++) {
/* print corner */
      if (array(p,x,y) == array(p,x,y-1) && array(p,x,y) == array(p,x-1,y) && array(p,x,y) == array(p,x-1,y-1))
        printf(" ");
      else if (array(p,x,y) == array(p,x,y-1) && array(p,x,y) == array(p,x-1,y))
        printf("+");
      else if (array(p,x-1,y) == array(p,x-1,y-1) && array(p,x-1,y) == array(p,x,y))
        printf("+");
      else if (array(p,x,y-1) == array(p,x-1,y-1) && array(p,x,y-1) == array(p,x,y))
        printf("+");
      else if (array(p,x-1,y-1) == array(p,x-1,y) && array(p,x-1,y-1) == array(p,x,y-1))
        printf("+");
      else if (array(p,x,y-1) == array(p,x-1,y-1) || array(p,x,y) == array(p,x-1,y))
        printf("-");
      else if (array(p,x-1,y) == array(p,x-1,y-1) || array(p,x,y) == array(p,x,y-1))
        printf("|");
      else
        printf("+");

/* print horizontal edge */
      if (x<p.x_max+1) {
        if (array(p,x,y) != array(p,x,y-1))
          printf("--");
        else
          printf("  ");
      }
    }
    printf("\n");

    if (y<p.y_max+1) {
      for(x=0;x<=p.x_max+1;x++) {
/* print vertical edge */
        if (array(p,x,y) != array(p,x-1,y)) 
          printf("|");
        else
          printf(" ");
/* print middle of squares */
        if (x<p.x_max+1)
          printf("  ");
      }
      printf("\n");
    }
  }
}

void ugly_print(struct polyomino_struct p) {
  int x,y;

  for(y=0;y<=p.y_max;y++) {
    for(x=0;x<=p.x_max;x++)
      printf("%c",array(p,x,y)==-1 ? ' ' : 'x');
    printf("\n");
  }
}

void c_print(struct polyomino_struct p) {
  int i;

  printf("/*\n");
  ugly_print(p);
  printf("*/\n  {%d, {",p.len);
  for (i=0;i<p.len;i++)
    printf("{%d,%d}%s",p.point[i].x,p.point[i].y,(i<p.len-1)?", ":"},\n");

  printf("   %d, {",p.transform_len);
  for (i=0;i<8;i++)
    printf("%d%s",p.transform_list[i],i<7?", ":"}, ");

  printf("%d}",p.max_white);
}

void polyomino_transform(struct polyomino_struct *p, int transform_no);

void print_all_polyominoes(int len) {
  struct polyomino_struct *p;
  int nr_found = 0;

  for (p=first_polyomino[len];p!=NULL;p=p->next) {
    if (p->y_max>p->x_max)
      polyomino_transform(p,1);
    nr_found++;
  }

  if (print_choice == 4)
    printf("static struct {int len; point_type point[%d];\n"
           "               int transform_len, transform_list[8], max_white;} polyomino[%d] =\n"
           "{\n", len, nr_found);

  for (p=first_polyomino[len];p!=NULL;p=p->next) {
    if (print_choice == 2) {
      pretty_print(*p);
      printf("\n");
    }
    if (print_choice == 3) {
      ugly_print(*p);
      printf("\n");
    }
    else if (print_choice == 4) {
      c_print(*p);
      if (p->next!=NULL) printf(",\n");
    }
  }

  if (print_choice == 1)
    printf("Total number %d\n",nr_found);
  if (print_choice==4)
    printf("};\n\n\nnr_polyominoes = %d;\n",nr_found);

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

int compare(const void *p1, const void *p2) {
  if (((point_type*)p1)->x<((point_type*)p2)->x) return -1;
  if (((point_type*)p1)->x>((point_type*)p2)->x) return 1;
  if (((point_type*)p1)->x==((point_type*)p2)->x) {
    if (((point_type*)p1)->y<((point_type*)p2)->y) return -1;
    if (((point_type*)p1)->y>((point_type*)p2)->y) return 1;
  }
  return 0;
}

void destroy_polyomino(struct polyomino_struct p) {
  if (p.point==NULL) free(p.point);
}

void polyomino_transform(struct polyomino_struct *p, int transform_no) {
  int x_min, x_max, y_min, y_max, i;

  if (transform_no == 0)
    return;

  for (i=0;i<p->len;i++)
    transform(p->point[i].x, p->point[i].y, transform_no, 
              &(p->point[i].x), &(p->point[i].y));

  x_min=y_min=1000000;
  x_max=y_max=-1000000;
  for (i=0;i<p->len;i++) {
    if (p->point[i].x<x_min) x_min = p->point[i].x;
    if (p->point[i].x>x_max) x_max = p->point[i].x;
    if (p->point[i].y<y_min) y_min = p->point[i].y;
    if (p->point[i].y>y_max) y_max = p->point[i].y;
  }

  p->x_max = x_max-x_min;
  p->y_max = y_max-y_min;

  for (i=0;i<p->len;i++) {
    p->point[i].x -= x_min;
    p->point[i].y -= y_min;
  }

  qsort(p->point,p->len,sizeof(point_type),compare);
}

struct polyomino_struct polyomino_from_points(int len, point_type *point) {
  struct polyomino_struct p, temp_p;
  int x_min, x_max, y_min, y_max, i, t;
  int transforms[8];
  point_type *temp_point;

  p.len = len;

  x_min=y_min=1000000;
  x_max=y_max=-1000000;
  for (i=0;i<len;i++) {
    if (point[i].x<x_min) x_min = point[i].x;
    if (point[i].x>x_max) x_max = point[i].x;
    if (point[i].y<y_min) y_min = point[i].y;
    if (point[i].y>y_max) y_max = point[i].y;
  }

  p.x_max = x_max-x_min;
  p.y_max = y_max-y_min;

  p.point = malloc(len*sizeof(point_type));
  for (i=0;i<len;i++) {
    p.point[i].x=point[i].x-x_min;
    p.point[i].y=point[i].y-y_min;
  }
  qsort(p.point,len,sizeof(point_type),compare);

  p.next = NULL;

  for (i=0;i<8;i++) transforms[i]=1;
  temp_point = malloc(sizeof(point_type)*len);
  temp_p = p;
  temp_p.point = temp_point;
  p.transform_len = 0;

  for (t=0;t<8;t++) {
    memcpy(temp_point,p.point,sizeof(point_type)*len);
    polyomino_transform(&temp_p,t);
    for (i=0;i<p.len && p.point[i].x==temp_p.point[i].x 
                     && p.point[i].y==temp_p.point[i].y;i++);
    if (i==p.len) {
      if (t==1)
        transforms[1] = transforms[3]  = transforms[5] = transforms[7] = 0;
      if (t==2)
        transforms[2] = transforms[3] = transforms[6] = transforms[7] = 0;
      if (t>=4)
        transforms[4] = transforms[5] = transforms[6] = transforms[7] = 0;
    }
  }
  for (i=0;i<8;i++) if (transforms[i]) {
    p.transform_list[p.transform_len] = i;
    p.transform_len++;
  }
  for (i=p.transform_len;i<8;i++)
    p.transform_list[i] = -1;
  free(temp_point);

  p.max_white = 0;
  for (i=0;i<len;i++) if ((p.point[i].x+p.point[i].y)%2) p.max_white++;
  if (p.max_white<=len/2) p.max_white = len - p.max_white;

  return p;
}

int polyomino_equal(struct polyomino_struct p1,
                     struct polyomino_struct p2) {
  int i;

  if (p1.len != p1.len || p1.transform_len != p2.transform_len)
    return 0;

  for (i=0;i<p1.len && p1.point[i].x==p2.point[i].x 
                    && p1.point[i].y==p2.point[i].y;i++);
  return i==p1.len;
}

int found(struct polyomino_struct p1) {
  struct polyomino_struct *p, temp_p;
  int index;
  point_type *temp_point;

  temp_point = malloc(sizeof(point_type)*p1.len);
  temp_p = p1;
  temp_p.point = temp_point;

  for (index=0;index<p1.transform_len;index++) {
    memcpy(temp_point,p1.point,sizeof(point_type)*p1.len);
    polyomino_transform(&temp_p,p1.transform_list[index]);
    for (p=first_polyomino[p1.len];p!=NULL;p=p->next)
      if (polyomino_equal(temp_p, *p)) {
        free(temp_point);
        return 1;
      }
  }
  free(temp_point);
  return 0;
}

int add_polyomino(struct polyomino_struct p1) {
  struct polyomino_struct **p;

  if (found(p1)) {
    return 0;
  }
  else {
    for (p=&(first_polyomino[p1.len]);*p!=NULL;p=&((*p)->next));
    *p = malloc(sizeof(struct polyomino_struct));
    memcpy(*p,&p1,sizeof(struct polyomino_struct));
    (*p)->next = NULL;
  }
  return 1;
}

void initialize(int n) {
  int i;
  point_type *point;

  for (i=0;i<=n;i++) first_polyomino[i] = NULL;

  point = malloc(sizeof(point_type));
  point->x = 0;
  point->y = 0;
  add_polyomino(polyomino_from_points(1,point));
}

#define add_point(d1,d2)					\
  if (array(*p,point[i].x+(d1),point[i].y+(d2))==-1) {	\
    point[len-1].x = point[i].x+(d1);			\
    point[len-1].y = point[i].y+(d2);			\
    new_p = polyomino_from_points(len,point);		\
    add_polyomino(new_p);		\
  }

void create_polyominoes(int len) {
  struct polyomino_struct *p, new_p;
  point_type *point;
  int i;

  point = malloc(sizeof(point_type)*len);
  for (p=first_polyomino[len-1];p!=NULL;p=p->next) {
    memcpy(point,p->point,sizeof(point_type)*(len-1));
    for (i=0;i<len-1;i++) {
      add_point(1,0);
      add_point(-1,0);
      add_point(0,1);
      add_point(0,-1);
    }
  }
  free(point);
}

int main(int argc, char **argv) {
  int i;
  int n;

  get_user_input(argc, argv, &n);
  initialize(n);

  for (i=2;i<=n;i++) {
    create_polyominoes(i);
  }
  print_all_polyominoes(n);

  exit(0);
}

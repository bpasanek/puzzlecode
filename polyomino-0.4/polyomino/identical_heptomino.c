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

char *description = 
"Place the shape\n"
"\n"
" xx\n"
"xxxxx\n"
"\n"
"into a 26 by 21 rectangle\n";

#define nr_polyominoes 78
#define polyomino_len 7
#define area_rectangle nr_polyominoes*polyomino_len

#define width 26
#define height 21
#define min_height height

#define max_width width
#define max_height height

#define IDENTICAL

#define LEN(poly_no) polyomino_len

#define BLANK_REGION_TEST if (count%polyomino_len!=0)

/*
#define search_method 2
#define check_size_of_blank_regions 1
*/

#undef CHECK_PROGRESS

#include "polyomino.h"

static struct {int len; point_type point[7];
               int transform_len, transform_list[8], max_white;} 
  special_heptomino =
/*
xxxxx  
  xx   
*/
  {7, {{0,0}, {1,0}, {2,0}, {2,1}, {3,0}, {3,1}, {4,0}},
   8, {0, 1, 2, 3, 4, 5, 6, 7}, 4};

void make_polyomino() {
  int i;

  for (i=0;i<nr_polyominoes;i++) {
    copy_polyomino(polyomino[i],special_heptomino);
  }
}

void carve_out_shape() {
}

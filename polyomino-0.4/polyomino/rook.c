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
"Rook problem.\n"
"\n";

#define nr_polyominoes 35
#define polyomino_len 6
#define area_rectangle nr_polyominoes*polyomino_len

#define width  15
#define height 18
#define min_height height

#define max_width width
#define max_height height

#define LEN(poly_no) polyomino_len

#define BLANK_REGION_TEST if (count%polyomino_len!=0)

#define search_method 2
#define check_size_of_blank_regions 1
#define all_rotations 1

#define CHECK_PROGRESS

#include "polyomino.h"

void make_polyomino() {
  int i, k=0;

  load_hexomino;
}

void carve_out_shape() {
  int x,y;

/* Carve out the rook */
  for (x=3;x<=11;x++)
    ARRAY(x,13) = -2;
 for (x=4;x<=10;x++)
    ARRAY(x,12) = -2;
  for (y=5;y<=11;y++)
    for (x=5;x<=9;x++)
      ARRAY(x,y) = -2;
  for (x=6;x<=8;x++)
    ARRAY(x,4) = -2;
  for (y=4;y<=6;y++)
    ARRAY(4,y) = -2;
  for (y=4;y<=6;y++)
    ARRAY(10,y) = -2;
}

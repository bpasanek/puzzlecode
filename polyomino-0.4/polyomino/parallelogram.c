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

#define width  28
#define height 14
#define min_height height

#define max_width width
#define max_height height

#define LEN(poly_no) polyomino_len

#define BLANK_REGION_TEST if (count%polyomino_len!=0)

#define search_method 2
#define check_size_of_blank_regions 1
#define all_rotations 1

#include "polyomino.h"

void make_polyomino() {
  int i, k=0;

  load_hexomino;
}

void carve_out_shape() {
  int x,y;

  memset(array,-2,sizeof(array));
  for (y=0;y<14;y++)
    for (x=0;x<15;x++)
      ARRAY(x+y,y) = -1;
}

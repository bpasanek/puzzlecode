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
"This program attempts to find all the ways of placing all one hundred and\n"
"eight heptominoes into a rectangle with three holes missing.\n"
"\n";

#define nr_polyominoes 108
#define polyomino_len  7
#define area_rectangle nr_polyominoes*polyomino_len

#define width 33
#define height 23

#define max_width width
#define max_height height
#define min_height height

#define LEN(poly_no) polyomino_len

#define BLANK_REGION_TEST if (count%polyomino_len!=0)

#include "polyomino.h"

void make_polyomino() {
  int i,k=0;

  load_heptomino;
  limit_rotations();
}

void carve_out_shape() {
  int i;

  for (i=0;i<3;i++)
    ARRAY(5+11*i,11) = -2;
}

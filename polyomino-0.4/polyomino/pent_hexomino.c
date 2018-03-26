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
"This program attempts to find all the ways of placing all twelve pentominoes\n"
"and all thirty five hexominoes into a rectangle.\n"
"\n";

#define nr_polyominoes 47
#define polyomino_len 6
#define area_rectangle 270

#define max_width 15
#define max_height 90
#define min_height 5

#define LEN(poly_no) len[poly_no]

#define BLANK_REGION_TEST		\
    while (c>=0 && c%5!=0) c -= 6;	\
    if (c < 0)

#define CHECK_PROGRESS

#include "polyomino.h"

void make_polyomino() {
  int i, k=0;

  load_pentomino;
  load_hexomino;
  limit_rotations();
}

void carve_out_shape() {
}

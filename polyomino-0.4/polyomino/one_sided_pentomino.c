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
"This program attempts to find all the ways of placing all sixty one-sided\n"
"hexominoes into a rectangle.\n"
"\n";

#define nr_polyominoes 18
#define polyomino_len 5
#define area_rectangle nr_polyominoes*polyomino_len

#define max_width 30
#define max_height 9
#define min_height 3

#define LEN(poly_no) polyomino_len

#define BLANK_REGION_TEST if (count%polyomino_len!=0)

#define search_method 1
#define check_size_of_blank_regions 0

#define CHECK_PROGRESS

#include "polyomino.h"

void make_polyomino() {
  int i,k=0;

  load_one_sided_pentomino;
  limit_one_sided_rotations();
}

void carve_out_shape() {
}

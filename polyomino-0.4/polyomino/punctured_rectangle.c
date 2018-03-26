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
"Punctured rectangle problem.\n"
"\n";

#define nr_polyominoes 35
#define polyomino_len 6
#define area_rectangle nr_polyominoes*polyomino_len

#define width 45
#define height 5
#define min_height height

#define max_width width
#define max_height height

#define LEN(poly_no) polyomino_len

#define BLANK_REGION_TEST if (count%polyomino_len!=0)

/*
#define search_method 2
#define check_size_of_blank_regions 1
*/

#define CHECK_PROGRESS

#include "polyomino.h"

void make_polyomino() {
  int i, k=0;

  load_hexomino;
}

void carve_out_shape() {
  int i;

  for (i=0;i<15;i++)
    ARRAY(1+3*i,2) = -2;

}

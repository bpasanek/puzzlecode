/* Needs work, maybe too much.  */

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
"Problem 65.\n"
"\n";

#define nr_polyominoes 35
#define polyomino_len 6

#define width  54
#define height 6
#define min_height height

#define max_width width
#define max_height height

#define LEN(poly_no) polyomino_len

#define BLANK_REGION_TEST if (count%polyomino_len!=0)

#define SPECIAL_SEARCH_METHOD_LU

#define CHECK_PROGRESS

#include "polyomino.h"

void make_polyomino() {
  int i, k=0;

  load_hexomino;
}

void carve_out_shape () {
  int x,y;

  for (y=0;y<3;y++) for (x=0;x<35;x++) ARRAY(x,y) = -1;
  for (y=3;y<6;y++) for (x=19;x<19+35;x++) ARRAY(x,y) = -1;
}


int find_blank_special_a(signed char **place_to_attach, signed char smallest_blank) {
  if (check_size_of_blank_regions)
    *place_to_attach=&ARRAY(0,0);

  if (*place_to_attach<=&ARRAY(18,2)) {
    displ=&displ_lu;
    for (;**place_to_attach!=smallest_blank && *place_to_attach<=&ARRAY(18,3);*place_to_attach++);
    if (**place_to_attach!=smallest_blank)
      *place_to_attach = &ARRAY(53,5);
  }

  if (*place_to_attach>=&ARRAY(35,3)) {
    displ=&displ_rd;
    for (;**place_to_attach!=smallest_blank && *place_to_attach>=&ARRAY(35,3);*place_to_attach--);
    if (**place_to_attach!=smallest_blank)
      *place_to_attach = &ARRAY(19,0);
  }

  if (*place_to_attach>=&ARRAY(19,0) && *place_to_attach<=&ARRAY(34,5)) {
    displ=&displ_lu;
    for (;**place_to_attach!=smallest_blank;*place_to_attach++);
  }

  return 1;
}



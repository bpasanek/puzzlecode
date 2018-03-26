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

int main (int argc, char **argv) {
  FILE *out;
  char s[1001], fname[1000];
  int count = 0;

  if (argc!=2 || (out = fopen(argv[1],"w"))==NULL)
    exit(1);
  sprintf(fname,"%s.old",argv[1]);

  while (fgets(s,1000,stdin)!=NULL) {
    count++;
    fputs(s,stdout);
    fflush(stdout);
    fputs(s,out);
    fflush(out);
    if (count >= 1000) {
      fclose(out);
      remove(fname);
      rename(argv[1],fname);
      out = fopen(argv[1],"w");
      count = 0;
    }
  }
  exit(0);
}

Apparently the standard C/C++ libraries do not provide a single random number
generator common to both Windows and Linux platforms.  I wanted the results of
MonteCarlo runs to be the same regardless of what machine I run it on
(assuming of course you keep the number of trials, range and seed the same).
So I downloaded V2.1 of the "Mersenne Twister" random number generator which
is said to be quite good.  Only their README file, their LICENSE file and the
few source files actually used by polycube are included here.  The files that
remain are unmodified.  The Mersenne Twister homepage is found at:

    http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/emt.html

There's also a wikipedia article on the Mersenne Twister:

    http://en.wikipedia.org/wiki/Mersenne_twister

Matt

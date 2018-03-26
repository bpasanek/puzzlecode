# Puzzle Poesis
This is one of two GitHub repos for UVa's Puzzle Poetry group. The public-facing website, which showcases our puzzles, is at http://puzzlepoesis.org. The Jekyll code and Markdown files that generate that site are maintained at https://github.com/bpasanek/puzzlepoesis

### The _Increase_ Puzzle Project

The puzzlecode repository houses code for solving poetry puzzles (polyomino puzzles) and related puzzle-poem games. At present, this readme file narrowly addresses our efforts to convert a variety of sonnets into polyomino puzzles. Our specific, ongoing project aims at converting Shakespeare's "procreation" sonnets (Sonnets 1 to 17) into a sequence of puzzles. These will be laser cut from wood, acrylic, and other materials, and then assembled as an art-object titled _Increase_. 

Preparing these puzzles requires finding ways to pack pentominos into a sonnet-shaped frame, aligning words with the cuts that define the edges of the polyomino pieces. Working by hand, we've carved up several sonnets into a set of pentomino shapes but we hope to do better, finding all possible solutions to our sequence of sonnets, that is, all the ways that a given sonnet can be cut up into pentomino shapes. We are making progress. This semester and last we've been matched poem shapes to sestets with success, and we are currently working on generating a list of possible polyomino solutions for the octave.

For reference, the complete set of pentominos appears below with the pieces labeled alphabetically:

![12 Pentominoes](/images/pentominoes.jpg)

An example of a puzzle created by hand (Sonnet 1):

![Sonnet 1, cut up by hand](/images/sonnet1.jpg)

Finally, an example of a sonnet sestet (the last 6 lines of a sonnet) "piecifed", that is, matched against a solution to the polyomino problem that packs 12 pentominos into a 6x10 grid. A human puzzle solver who is handed these pieces of poetry has to configure them into pentameter lines, the first of which will read "Then of thy beauty do I question make": 

![Sonnet 12, sestet, laser cut](/images/sonnet12-sestet-wb.jpg)

### Puzzle Solving: Overview

Polyomino puzzles are solved computationally, and therefore exhaustively. In many cases, the solutions to classic pentomino puzzles are available online. We have studied the solutions that are elegantly displayed and interlinked at https://isomerdesign.com/Pentomino/ and at https://gp.home.xs4all.nl/PolyominoSolver/Polyomino.html

Puzzles are categorized by the pieces employed and the solution space (In a popular puzzle, the  solver is encouraged to fit pentominos and one square tetromino into an 8x8 box). 

In the case of sonnets, we have a 14x10 structure (14 lines, 10 syllables per line), which doesn't correspond to an obvious puzzle. However, in a classic puzzle, the 12 free pentomino shapes are assembled into a 6x10 rectangle. English majors may note that 6x10 is the shape of a sestet, the last six lines of a sonnet; and traditionally, the sestet (six lines of pentameter verse) provides an answer to the question posed in the first eight lines of the poem, the octave. It may then be ideal -- and cleverly meaningful -- to pack the octave and sestet of our sonnets separately, working within the sonnet form as best we are able. What does this sort of packing will entail? For one thing, if the puzzle is constructed entirely from pentominos, pieces will have to appear more than once: at least once in the sestet, then most or all will appear again in the octave, a few pieces, two or three times more. While it would be most elegant to pack the octave with as many different shapes as will fit (each of the pentominos and each of the tetrominos); piecifying the puzzle in a way that reuses shapes makes the solving of the sonnet more challenging as pieces that belong in the sestet may be incongrously and erroneously inserted into the octave and vice versa.

Sestets are easily matched to the list of 2,339 solutions we downloaded from isomerdesign.com (Brad Pasanek, with the help of Clay Ford, has worked out a workflow for matching poems to puzzles in R, see the folder r-puzzlings for scripts and data.) The octave of a sonnet is another matter. There are 3,386,001,688 solutions to the polyomino problem that uses each pentomino and each tetromino once to tile an 8x10 grid. We don't have a list of solutions to this problem and will have to generate one.

#### Solving the Octave in Python

We are currently experimenting with a Python solver that was designed by David J. Goodger and tweaked (slightly) by Brad Pasanek to print out puzzle solutions more compactly. On Rivanna, we've generated as many as 9 million solutions to the 8x10 pentomino-tetromino puzzle in 3 days. Unfortunately, this means it's going to take us as long as three years to find all the possible solutions of the puzzle. Matching poems to these solutions will go more quickly, of course, but this is where we are stuck at the moment. Perhaps we will be able to parallelize our code and run several solvers at the same time; or perhaps we should start fresh with a different solving routine in a new language. (ARCS at UVa reports that changing from Python to C might help us gain an order of magnitude in speed.)

#### Solving the Octave in C
Katherine Holcomb has adapted and uploaded here C++ code that leverages 4 different puzzle-solving algorithms (DLX, 2. MCH, EMCH, de Bruijn). Unpack the tarball polycube.tgz for code, modules, and makefiles. The code originates from http://www.mattbusche.org/projects/polycube/

Katherine includes the following instructions:
1. module load gcc/4.8.2  -- it *will not* link with the default 5.4.0 because boost was built with 4.8.2
2. module load boost (optional since I put full paths into the makefile, but will be required to run it to set LD_LIBRARY_PATH)
3. cd src
4. make

We hope to rewrite this code to solve sonnet-shaped puzzles. 
    
### Create New Puzzles
To create a new puzzle...

An empty sonnet (14 x 10 grid):

     L:stationary=*
        . . . . . . . . . .
        . . . . . . . . . .
        . . . . . . . . . .
        . . . . . . . . . .
        . . . . . . . . . .
        . . . . . . . . . .
        . . . . . . . . . .
        . . . . . . . . . .
        . . . . . . . . . .
        . . . . . . . . . .
        . . . . . . . . . .
        . . . . . . . . . .
        . . . . . . . . . .
        . . . . . . . . . .
        ~L
  
Again, we are treating poems as frames into which words are packed (a so-called packing puzzle). For the purposes of solving a sonnet, the words in the poem are assigned to pentomino shapes. A puzzle is solved when a set of 28 pentominos have been packed into the frame, so that each word in the poem is associated with a placed pentomino piece. In fact, a packed pentomino piece will gather at least one five-syllable word--the "I" pentomino piece inserted horizontally is the right shape to do so--or at most five one-syllable words. 

Note, our solutions cannot divide words, which means there are constraints (represented below with question marks and hyphens) that our current modified pentomino solver has not yet been programmed to respect.
  
  Sonnet 2:
  
       L:stationary=*
        . ?-? ?-? . ?-? . .
        . . . ?-? . . ?-? .
        . . . ?-?-? . . . .
        . . . ?-? . . . . .
        . ?-? . . . . ?-? .
        . . . ?-? . . ?-? .
        . . ?-? . . . ?-? .
        . . . ?-? . . ?-? .
        . . . . ?-? . ?-? .
        . . . ?-? . . . . .
        . . . . . . . . ?-?
        ?-? . ?-? . ?-?-? .
        . . . . . . . . . .
        . . . . . . . . . .
        ~L
  
 Sonnet 2:
 
 [1] "When forty winters shall besiege thy brow"          
 [2] "And dig deep trenches in thy beauty's field,"       
 [3] "Thy youth's proud livery, so gazed on now,"         
 [4] "Will be a tattered weed of small worth held."       
 [5] "Then being asked where all thy beauty lies,"        
 [6] "Where all the treasure of thy lusty days,"          
 [7] "To say within thine own deep-sunken eyes"           
 [8] "Were an all-eating shame and thriftless praise."    
 [9] "How much more praise deserved thy beauty's use"     
[10] "If thou couldst answer \"This fair child of mine"   
[11] "Shall sum my count and make my old excuse,\""       
[12] "Proving his beauty by succession thine."            
[13] "  This were to be new made when thou art old"       
[14] "  And see thy blood warm when thou feel'st it cold."

How to rewrite the C code? A good question! The hyphenated positions in the grid above should be filled first in any revised walk-back routine. That is, the first pentominoes should be placed over these squares in such a way that no piece covers only part of a poem position marked with a ?-?, ?-?-?, etc. Performance might be optimized if the partially packed puzzle is then checked for areas that won't fit a pentomino. Alternately, brute packing of shapes can continue until the solver fails and has to back up and start again.

Each of the 17 sonnets has a different set of constraints on pentomino placement and word-division. It may well be that there are sonnets that do not have elegant octave or sestet solutions. Indeed, we already know that only three of the first 17 have sestet solutions. 

An extra headache: 7 sonnets (sonnets 3, 8, 9, 10, 11, 15, 17) have lines in them that have more than ten syllables. We'll need to adjust these lines by hand (eliding syllables or crowding them) before pursuing scripted solutions.

-- Brad Pasanek (2/13/2018)

### More Resources
Discussion of pentomino puzzle solving at
http://www.mattbusche.org/blog/article/polycube/

C++ code adapted from http://www.mattbusche.org/projects/polycube/

### See also
A online collection of solved pentomino puzzles: https://isomerdesign.com/Pentomino/   
Gerard Putter's polyomino solver: https://gp.home.xs4all.nl/PolyominoSolver/Polyomino.html  
David Goodger's Python solver: http://puzzler.sourceforge.net/README.html  
Stephen Montgomery-Smith's polyomino solver: https://faculty.missouri.edu/~stephen/software/#polyominoes  
(Note, Montgomery-Smith seems to have solved the 8x10 puzzle and contributed the solution count to Gerard Putter's page)

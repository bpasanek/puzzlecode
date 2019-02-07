# Puzzle Poesis
This is one of two GitHub repos for UVa's Puzzle Poetry group. Here are collected packages for solving polyomino puzzles. The public-facing website, which showcases our fabricated puzzles, is at http://puzzlepoesis.org. The Jekyll code and Markdown files that generate that site are maintained at https://github.com/bpasanek/puzzlepoesis. 

Packages and code are organized in the following directories:  
- `r-puzzlings` contains R scripts we use to match poems to solved puzzle configurations. Brad Pasanek wrote most of these, often with the help of Clay Ford. 
- `puzzler-tweaked` is David Goodger's Python puzzle solver. With the help of Shane Lin and Brandon Walsh, I/Brad altered the code so that it prints out its solutions compactly. The scripts work but the reporting is now broken (because I/Brad suck at coding). On Rivanna the solver is run by means of a slurm script and writes out to a file on Brad's scratch directory.
- `polyomino-0.4` contains David Montgomery Smith's puzzle solvers, which are written in C. It's currently hoped that this code will help us solve the 8x10 pentomino-tetromino puzzle (defined here: `polyomino/tetr_pentomino.c`) on the Rivanna cluster. Note, this code, like Python package above, has been altered so that it prints more compactly. Katherine Holcomb helped revise the code: that is, mostly we commented out reporting on progress. The solver runs by means of a slurm script on Rivanna and writes out its solutions to Rivanna's scratch directory.
- `polycube` also contains C code (C++) that we are experimenting with. 

Illustrator files are kept here and on Dropbox. Minutes from our weekly meetings are also archived here. 

### Overview of the _Increase_ Puzzle Project

The `puzzlecode` repository houses code for solving poetry puzzles (polyomino puzzles) and related puzzle-poem games. At present, this readme file narrowly addresses our efforts to convert a sequence of sonnets into polyomino puzzles. Specifically, our project aims at converting Shakespeare's "procreation" sonnets (Sonnets 1 to 17) into a set of puzzles. These will be laser cut from wood, acrylic, and other materials, and then packaged as an art object titled _Increase_. 

Designing these puzzles requires finding ways to pack pentominos into a sonnet-shaped frame, aligning words with the cuts that define the edges of the polyomino pieces. Working by hand, we've carved up several sonnets into a set of pentomino shapes but we hope to do better, finding all possible solutions to our sequence of sonnets, that is, all the ways that a given sonnet can be cut up into pentomino shapes. We are making progress. In the first two years of study, we've been matched poem shapes to sestets with success, and we are currently working on generating a list of possible polyomino solutions for the octave.

For reference, the complete set of pentominos appears below with the pieces labeled alphabetically:

![12 Pentominoes](/images/pentominoes.jpg)

An early sketch of a sonnet puzzle (Sonnet 1) worked out by hand on graph paper:

![Sonnet 1, cut up by hand](/images/sonnet1.jpg)

Finally, an example of a sonnet sestet (in this case the last six lines of a Sonnet 12) "piecifed," that is, matched against a solution or solutions to the polyomino problem that packs twelve pentominos into a 6x10 grid. These matched solutions guide our design process. After a poem is matched to a polyomino solution, the puzzle is mocked up in Adobe Illustrator, with text laid out on a raster layer and the edges of the pieces drawn on a vector layer. Puzzles are then laser cut out of acrylic or wood. A human puzzle solver who is handed these pieces of poetry may be asked to arrange the pieces in six pentameter lines, the first of which, if configured in Shakespeare's original ordering, will read thus: "Then of thy beauty do I question make".

![Sonnet 12, sestet, laser cut](/images/sonnet12-sestet-wb.jpg)

### Puzzle Solving: Overview

Polyomino puzzles are traditionally designated by the perimeter of shape to be tiled and categorized by the pieces employed and the area to be covered (for example, both 6x10 or 5x12 frames can be packed with the twelve unique pentominos). In one popular puzzle, the solver is encouraged to fit pentominos and one square tetromino into an 8x8 chessboard frame. These canonical puzzles have been solved computationally and therefore exhaustively, with many of these solutions to classic pentomino puzzles available online. We have studied the solutions that are elegantly displayed and interlinked at https://isomerdesign.com/Pentomino/ and at https://gp.home.xs4all.nl/PolyominoSolver/Polyomino.html

In the case of the sonnet form, we have a 14x10 structure (14 lines, 10 syllables per line), which doesn't correspond to an obvious or established polyomino puzzle shape. However, in one well understood, classic puzzle, the twelve free pentomino shapes may be assembled in a 6x10 rectangle. English majors will remember that 6x10 is the shape of a sestet, the last six lines of a sonnet; and traditionally, the sestet (six lines of pentameter verse) provides an answer to the question posed in the first eight lines of the poem, the octave. We maintain that it may then be ideal -- and cleverly meaningful -- to pack, when possible, the octave and sestet of our sonnets separately, working within the sonnet form as best we are able. 

What does this sort of octave-sestet packing entail? For one thing, if a sonnet puzzle is constructed entirely from pentominos, some pieces will have to appear more than once: at least once in the sestet, and then most or all will appear again in the octave--a few pieces, two or three times more. Perhaps it would be more elegant to pack the octave with as many different shapes as will fit (each of the pentominos and each of the tetrominos), or perhaps we should try to design some puzzles that use only one pentomino shape 28 times. In either case, piecifying the puzzle in a way that reuses shapes makes the solving of the sonnet more challenging as pieces that belong in the sestet may be incongrously and erroneously inserted into the octave and vice versa.

We've already found that a number of sestets--from Shakespeare, Christina Rossetti, Claude McKay, and others--are readily matched to the list of 2,339 solutions we downloaded from isomerdesign.com (Brad Pasanek, with the help of Clay Ford, has written out a workflow for matching poems to puzzles in R. See the folder `r-puzzlings` above for scripts and data.) The octave of a sonnet is another matter. There are 3,386,001,688 solutions to the polyomino problem that uses each pentomino and each tetromino once to tile an 8x10 grid. We don't currently have a list of solutions to this problem but are working with Jesse Alloy, an undergraduate CS major, to generate one.

#### Solving the Octave in Python

We  experimented with a Python solver that was designed by David J. Goodger and tweaked (slightly) by Brad Pasanek to print out puzzle solutions more compactly. On Rivanna, we've generated as many as 9 million solutions to the 8x10 pentomino-tetromino puzzle in 3 days. Unfortunately, this means it's going to take at least three years of computation to find all the possible solutions of the puzzle! Matching poems to these solutions would go more quickly, of course, but this is where Brad bogged down. Perhaps we will be able to parallelize our code and run several solvers at the same time; or perhaps we should start fresh with a different solving routine in a new language.

#### Solving the Octave in C
ARCS at UVa reported that changing from Python to C might help us gain an order of magnitude in speed. And we've been trying out new packages. We looked then into using two C solvers: `polyomino-0.4` and `polycube`.

Jacalyn Huband and Katherine Holcomb have been generously consulting with us as we prepare and run jobs on UVa's High Performance Computing Cluster. Most recently Katherine has helped Brad Pasanek and Timothy Schott rewrite some files in the `polyomino-0.4` package. On March 26, 2018, Katherine contributed mightily to our second attempt to generate the complete set of tilings for the 8x10 tetromino-pentomino problem. 

Katherine Holcomb has also adapted and uploaded here the `polycube` C++ code. It leverages four different puzzle-solving algorithms (DLX, 2. MCH, EMCH, de Bruijn) and should be an even faster application. This code originates from http://www.mattbusche.org/projects/polycube/. Unpack the tarball polycube.tgz for code, modules, and makefiles. 

Katherine includes the following instructions for working with this code on Rivanna (these commands to appear in a slurm script):
1. module load gcc/4.8.2  -- it *will not* link with the default 5.4.0 because boost was built with 4.8.2
2. module load boost (she writes, "optional since I put full paths into the makefile, but will be required to run it to set LD_LIBRARY_PATH")
3. cd src
4. make

We hope to rewrite one or both of these packages to solve sonnet-shaped puzzles. We have also been talking with Jacalyn Huband and Nathan Brunelle about writing our own solver, something that works from the poems, piecifying them directly.
    
### Notes, for Polycube

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

-- Brad Pasanek (2/13/2018, most recently revised March 27, 2018)

### More Resources and references
Discussion of pentomino puzzle solving at
http://www.mattbusche.org/blog/article/polycube/


### See also
A online collection of solved pentomino puzzles: https://isomerdesign.com/Pentomino/   
Gerard Putter's polyomino solver: https://gp.home.xs4all.nl/PolyominoSolver/Polyomino.html  
David Goodger's Python solver: http://puzzler.sourceforge.net/README.html  
C++ code adapted from http://www.mattbusche.org/projects/polycube/  
Stephen Montgomery-Smith's polyomino solver: https://faculty.missouri.edu/~stephen/software/#polyominoes  
(Note, Montgomery-Smith seems to have solved the 8x10 puzzle and contributed the solution count to Gerard Putter's page)
Advanced reading: Donald Knuth's article on "dancing links": https://www.ocf.berkeley.edu/~jchu/publicportal/sudoku/0011047.pdf

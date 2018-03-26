# Sonnet Solver
# February 8, 2018
# Brad Pasanek, with various functions contributed by Clay Ford

# This workflow requires the following package:
library("stringr")

### Overview ### -----

# The following scripts and functions are used in "solving" sonnet structures by matching 
# the syllabic structure of a given sonnet to polyomino configurations in a larger set 
# of congruent polyomino puzzle solutions. In many cases lines of verse will match one 
# or more puzzle configurations.

# This script originated in an effort to match the last six lines of a Shakespeare sonnet to a large set 
# of 6x10 pentomino solutions. The 6x10 puzzle in which each pentomino appears once is readily solvable, with
# a computer! Here, for example, are the 2,339 solutions ingeniously displayed on a web site: 
# https://isomerdesign.com/Pentomino/6x10/index.html

#Basic Recipe for Converting Sonnet Forms to Puzzles
# 1. Import Polyomino Solutions: sestet and octave solutions.
# 2. Process Solutions, Converting alphabetical representations of polyomino pieces to 
#     "syllabeads" and hyphens. (A whole sonnet requires octave and sestet solutions to be processed.)
# 3. Generate a list with hyphen positions from lists of syllabeaded solutions
# 4. Match lines of poetry to these processed solutions by finding solutions that have (at 
#     minimum) one hypen in every position that a poem has a hyphen. 
#     Note, these need not be exact matches. There may be fewer hyphens in an abstracted 
#     line of poetry than in the abstracted puzzle solution. That means a poem may match
#     multiple puzzle solutions. 

# The code that follows will work for various pentameter poems and polyomino puzzles, but it is currently 
# configured for sonnets. Some vocab then: an octave is the first 8 lines of a sonnet; a sestet, 
# the last six. A sonnet's octave, seen as a grid of syllables, fills an 8x10 rectangle (8 lines, 
# 10 syllables per pentamter line). A sestet fills a 6x10 grid. There are 2339 ways to pack 12 
# pentominos into a 6x10 rectangle, and there are 3386001688 ways to pack 12 pentominos and 5 
# tetrominos into the 8x10 rectangle.

# The complete solution space of a sonnet ("Let me count the ways") is 2339 sestet solutions x 3,386,001,688 
# octave solutions (using the 12 pentominos and 5 tetrominos). That's 7,919,857,948,232 possible puzzles (nearly 8 trillion). 

# I have downloaded the set of 6x10 solutions from Isomer design: https://isomerdesign.com/Pentomino/6x10/solutions.txt
# Each solution in the list of 2339 solutions looks like this: 
# FFIIIIILZZ VFFYLLLLZN VFYYYYXZZN VVVTWXXXNN PPPTWWXUNU PPTTTWWUUU
# This is a more compact version of a solved 6x10 rectangle:

# F F I I I I I L Z Z 
# V F F Y L L L L Z N 
# V F Y Y Y Y X Z Z N 
# V V V T W X X X N N 
# P P P T W W X U N U 
# P P T T T W W U U U

# I've been turning these into a structures in which a • corresponds to a syllable. A multisyllabic word is represented 
# thus: •-• (2 syllables) or •-•-•-• (4 syllables). 

# •-• •-•-•-•-• • •-•
# • •-• • •-•-•-• • •
# • • •-•-•-• • •-• •
# •-•-• • • •-•-• •-•
# •-•-• • •-• • • • •
# •-• •-•-• •-• •-•-•

# To generate octave solutions, I modified and ran a Python polyomino solver. Currently I'm still 
# working on running the script to completion. It is slowgoing on my laptop: I've generated over 
# 3 million solutions. 3 orders of magnitude fewer the complete known set of solutions.

### Import Solutions ### -----

# Sestet Solutions (A sestet corresponds to a 6x10 rectangle)
# Import 2,339 6x10 solutions from https://isomerdesign.com/Pentomino/6x10/solutions.txt
setwd("~/projects/puzzlepoesis-files/r-puzzlings/")
sestetsolutions<-scan("solutions-2339-6x10.txt",what="char",sep="\n")
sestetsolutions<-gsub(" *[0-9]+, ","",sestetsolutions) # YOU HAVE TO CLEAN!
length(sestetsolutions) #2339 solutions
str(sestetsolutions)

# Octave Solutions (An octave corresponds to a 8x10 rectangle)
# Importing solutions after running Python scripts from http://puzzler.sourceforge.net/README.html
# These are currently running and writing to a file called polyominoes-45-8x10-newsolutions.txt
# I've split this file on the command line with the following command:
# > split -l 100000 polyominoes-45-8x10-newsolutions.txt
# this results in files named aa to bl (dividing up nearly 4 million solutions into 38 files). 
# After using Clay Ford's checkSol function, I came to believe that there are no solutions for the octave I'm 
# currently trying to match ("EBB's "How do I love thee? Let me count the ways") in xaa to xbl. Below is a 
# script for reading in files. 
# I'm worried about memory troubles... But none yet!

setwd("/Users/bradpasanek/projects/puzzlepoesis-files/puzzlesolvers/puzzler/solution-splitfiles")

# Bring in one of the split files
octavesolutions.xae<-scan('ae',what="char",sep="\n") 
length(octavesolutions.xae) #100000 solutions

# Bring in all of the split files: 38 of them. The last split (bl) only has 86991 solutions in it.
solutionsplits<-list()
files<-list.files(pattern="[a|b].")
for(i in 1:length(files)){
  text<-scan(files[i],what="char",sep="\n")
  solutionsplits[[files[i]]]<-text
}

3386001688-3700000-86991 # how many solutions left to test
str(solutionsplits) # list of 38 solution files (nearly 4 million solutions, 635 MB list)
# Here's one that I've erroneously matched to EBB's octave (below):
solutionsplits$aa[1048]

### Processing Solutions: Converting Puzzle Solutions to 'Beaded' Representation ### -----

# I am going to convert solutions that name the puzzle pieces to something more abstract:
# o for each syllable, -s for marking syllables in a word. For puzzle solving, the main
# rule is that you can't divide a word. Thus a '-' means don't divide, a piece's contiguous squares
# have to cover these linked positions in the grid. Likewise, in a line of verse, a three syllable
# looks like this: o-o-o

# Loop over solutions, inserting hyphens and replacing letters with os. 
# This is slowgoing and may not be a necessary step.

# Find below a function called beader() for converting solutions that look like this: 
# test<-"ZNNNTTTzzt ZZZNNTzztt LLZVUTUWWt LPPVUUUXWW LPPVVVXXXW LFPlllYXoo FFFlYYYYoo FIIIIIiiii"
# to this:
# oo-o-oo-o-oo-oo o-o-oo-ooo-oo-o o-ooooooo-oo oo-ooo-o-o-ooo-o oo-oo-o-oo-o-oo oooo-o-oooo-o o-o-ooo-o-o-oo-o oo-o-o-o-oo-o-o-o, etc.

beader<-function(alphasolution){
  beadsolutions.l<-list()
  for(j in 1:length(alphasolution)){
    newstring<-NULL
    process<-unlist(str_split(alphasolution[j],""))
    for(i in 1:length(process)){
      if(i<length(process)){
        char1<-process[i]
        char2<-process[i+1]}
      if(char1==char2){
        char1<-paste0(char1,"-")
        newstring<-c(newstring,char1)
      }else{
        newstring<-c(newstring,process[i])
      }
    }
    hyphensolution<-str_c(newstring,collapse="")
    hyphensolution<-str_replace_all(hyphensolution,"[A-Za-z]","o")
    #print(hyphensolution) # print if testing
    beadsolutions.l[[paste0("solut",j)]]<-hyphensolution
  }
  return(beadsolutions.l)
}

# Test the beader function on a list of solutions
str(solutionsplits[[1]][1:5])
testsolutions<-solutionsplits[[1]][1:5]
my_solution <- beader(testsolutions)
str(my_solution)

#How long does this take? (Getting between .001 and .005 elapsed to convert a single solution to beads)
start <- proc.time()
beadsolutions.l<-beader(test)
print(proc.time() - start)

### Processing Solutions: Sestet (Conversion to Beads) ### ------
# This is much less computationally intensive that what follows.
start <- proc.time()
sestetsolutions.l<-beader(sestetsolutions) # 2339 beaded solutions created.
print(proc.time() - start)
# user  system elapsed 
# 6.060  49.372 127.621

### Processing Solutions: Octave (Conversion to Beads) ### -----

# Recap: I have an incomplete set of solutions generated by a Python script. These have been imported as a list.
str(solutionsplits) # list of 38, each list element corresponding to a file filled with 100,000 solutions

# This is going to take a while, I fear:
start <- proc.time()
beadsolutions.l<-beader(solutionsplits[[1]])
print(proc.time() - start)
# Running 100000 solutions takes this long:
# user  system elapsed 
# 538.143  38.094 607.540 -- that's 10 minutes per file of solution splits.

# Pass all the split files solved by the Python solver through the beader: 
# These are the 8x10 octave solution files, 38 of them. Run overnight? Or on Rivanna?
beadsolutions.l<-list()
start <- proc.time()
for(i in 1:length(solutionsplits)){
  print(names(solutionsplits[i]))
  beadsolutions.l[[names(solutionsplits[i])]]<-beader(solutionsplits[[i]])
}
print(proc.time() - start)
# Ran over night:
# user    system   elapsed 
# 19211.712  1234.561 21059.276 That's 5.8 hours

str(beadsolutions.l) # 38 element list, each element named aa, ab, etc. In each element are 100,000 beaded solutions


### Convert Beads to A List of Hyphen Positions ### -------

# I'm processing the "solutions" to create yet another abstraction: hyphen positions for beaded 
# solutions. Clay Ford wrote the following function to convert my bead representations to a list 
# of positions in a line in which there are hyphens. 

# A pentameter line might be cast and recast in the following ways.

# As words: 
# Thou that art now the world's fresh ornament (Sonnet 1, line 9)
#     Note, OR-NA-MENT is a trisyllablic word and can't be divided.

# As beads:
# oooooooo-o-o
#     Note, o-o-o represents the trisyllablic word 'ornament' and can't be divided.

# The possible positions of an indivisible hyphen can be found in these pentameter lines.
# 10 beads, string split, means 11 possible positions 
# (albeit position 1 and position 11 never have hypens in them):
# 1 . 2 . 3 . 4 . 5 . 6 . 7 . 8 . 9 . 10 . 11

# The hyphenated positions of a line (line 9 in sonnet 1) are 9 and 10. These
# oooooooo-o-o
# [1]  9 10

# The full sestet of Sonnet 1 (6 lines)
# oooooooo-o-o oo-oo-oooo-oo o-ooooo-ooo-o oo-oooooo-o-o o-ooooooo-oo oooooooooo

#Here's the sestet for Sonnet 1 as a list of hyphen positions:
testsestetsolutions.l<-list(list(c(9,0),c(3,5,9),c(2,7,10),c(3,9,10),c(2,9),integer(0)))
str(testsestetsolutions.l)
# [[1]]
# [[1]][[1]]
# [1]  9 10
# 
# [[1]][[2]]
# [1] 3 5 9
# 
# [[1]][[3]]
# [1]  2  7 10
# 
# [[1]][[4]]
# [1]  3  9 10
# 
# [[1]][[5]]
# [1] 2 9
# 
# [[1]][[6]]
# integer(0)

# -- Note, I've made this a two step process: I convert puzzle configurations to  beaded abstractions 
# so that they look more like the lines of verse that I create (by hand). 

# It might make more sense to skip the beading step and write a function that reads in the alphabetical 
# solutions and outputs information about which positions have contiguous and identical characters.
# That is to say, why not jump directly to hyphen locating (locating positions which can't be separated 
# from contiguous positions)? 

# The following function, written by Clay Ford, generates the list (as shown above) that contains 
# the positions of hyphens in a solutions and in the poems...
# Confusing but handy!

genPos <- function(x){
  if(is.list(x)){                                # for the solutions
    check.l <- lapply(x, 
                      function(x)str_split(str_split(string = x, " ")[[1]], 
                                           pattern = 'o'))
    check.l <- lapply(check.l, function(x)lapply(x, function(y)which(y=="-")))
  } else{                                        # for the list of sestets from sonnets
    tmp <- lapply(str_split(string = x, " "), function(x)str_split(x, pattern = 'o'))
    check.l <- lapply(tmp, function(x)lapply(x, function(y)which(y=="-")))
    
  }
  check.l
}

# Apply the genPos function to imported sestet solutions:
str(sestetsolutions.l)
sestetsolutions.l[1]
sestetsolutions.h.l<-genPos(sestetsolutions.l)

# Apply the genPos to one element of beadsolutions.l
start <- proc.time()
testhyphenatedsolutions.l<-genPos(beadsolutions.l[[27]])
print(proc.time() - start)
# user  system elapsed 
# 22.660   1.313  25.785

str(testhyphenatedsolutions.l) #100000 solutions


### EBB Octave ### ------

# I am currently trying to match Elizabeth Barrett Browning's famous sonnet to polyomino
# solutions. This may be too clever, but it's a Valentine's Day puzzle: the sonnet begins
# "How do I love thee? Let me count the ways." That's what I'm doing. I've got the sestets
# sorted, I think. But I haven't matched any octaves...

# Here are the first 8 lines of the sonnet:
ebboctave<-"oooooooooo oooooooooo oooooo-oooo ooooo-ooo-oo ooooo-ooo-oo oo-oooooo-o-o oooo-oooooo oooo-oooooo"
# NOTE: line 5 has 11 or 12 syllables! "I love thee to the level of every day’s" -- dropped a bead (will need to 
# squeeze the words "to the" in Adobe Illustrator onto a single grid square (CHEATING!). 
# Also, I'm reading every as elided, two syllables: "ev'ry"...
ebbhypoctavepositions.l<-genPos(ebboctave)
str(ebbhypoctavepositions.l) # list of 1 element, in that element, a list of 8 lines. Good?
ebbhypoctavepositions.l[[1]] # list of 8 elements, each element a line in the octave
ebbhypoctavepositions.l[[1]][[4]] # returns position of hyphens in line 4
 
### Matching Functions ### ----------------

# Clay's function that checks if a given poem matches a given solution
checkOneSolution <- function(poem, solution){
  tmp_poem <- lapply(poem[[1]], unlist)
  tmp_sol <- lapply(solution[[1]], unlist)
  chk <- mapply(function(x,y)x %in% y, tmp_poem, tmp_sol)
  all(sapply(chk, all))
}

#test Clay's function with a simple, dumb example
testpoem<-list(list(integer(0),2))
testsolutions<-list(solut1=list(integer(0),3),solut2=list(1,2:3))
all(testpoem[[1]][[1]] %in% testsolutions[[1]][1]) # returns a TRUE
all(testpoem[[1]][[2]] %in% testsolutions[[1]][2]) # returns a FALSE
all(testpoem[[1]][[1]] %in% testsolution[[2]][1]) # returns a TRUE
all(testpoem[[1]][[1]] %in% testsolution[[2]][2]) # returns a TRUE
checkOneSolution(testpoem,testsolutions[1])
checkOneSolution(testpoem,testsolutions[2]) # it's working!

# But in fact, we need a function that matches a poem against a list of solutions!
# Clay tweaked his function above, thus:
checkSolution <- function(poem, solution){
  tmp_poem <- lapply(poem[[1]], unlist)
  chk <- lapply(solution, function(x)mapply(function(x,y)x %in% y, tmp_poem, x))
  chk2 <- lapply(chk, function(x)all(unlist(x)))
  which(chk2 == TRUE)
}

# Try with an octave that should match solutions
testoctave<-"ooo-ooooooo oooooooooo ooooooooo-o oooooooooo oooooooooo oooooooooo oooooooooo oooooooooo" #from beadsolutions.l[[27]][1]
testoctave.l<-genPos(testoctave)
start <- proc.time()
matchedsolutions<-checkSolution(testoctave.l,testhyphenatedsolutions.l)
print(proc.time() - start)
length(matchedsolutions) # found 31342 solutions!
length(testhyphenatedsolutions.l) # out of 100000 possible solutions
# GOOD. IT'S WORKING (I think)

# Try with EBB and a test solution split file
matchedsolutions<-checkSolution(ebbhypoctavepositions.l,testhyphenatedsolutions.l)
length(matchedsolutions) # no solutions!

### The Main Event: Match EBB Octave, checking against all the Solutions ### ------- 
# Trying with EBB's first eight lines and all the Python generated 8x10 solution split files...

#Here's how I generated a single list of 100,000 (out of the nearly 4 million solutions) to test.
testhyphenatedsolutions.l<-genPos(beadsolutions.l[[36]])
length(beadsolutions.l) # 38 split solution files created by Python
names(beadsolutions.l) # each split solution file has a name: "aa", "ab", etc.

allmatchedsolutions.l<-list() # Initialize a list to store matches

# Loop through all beaded solutions and convert them to a map of hyphenated positions (an early try 
# required me to run the loop(s) over night (took about 6 hours to process):
start <- proc.time()
allsolutionset.l<-list()
for(i in 1:length(beadsolutions.l)){
  allsolutionset.l[[names(beadsolutions.l[i])]]<-genPos(beadsolutions.l[[i]])
}
print(proc.time() - start)
# user   system  elapsed 
# 597.945  378.807 1252.357 -- that's about 20 mins

# try to match EBB's octave to any one of these octaves...
# This may well have to run for a while
start <- proc.time()
matchedsolutions.l<-list()
for(i in 1:length(allsolutionset.l)){
  matchedsolutions.l[[names(allsolutionset.l[i])]]<-checkSolution(ebbhypoctavepositions.l,allsolutionset.l[[i]])
}
print(proc.time() - start)
# user   system  elapsed 
# 386.249  551.971 1268.039 

lengths(matchedsolutions.l)
# NO MATCHES!!!!!! AGHHHGHGHGHGHHHHHH

# Is this for REAL!? 
# What if I hand it a ready-made match...
testpositionlist.l<-list(list(
  integer(0),
  c(2,3,5,8,10), # this line corresponds to line 2 of solution 1 in the solution split aa
  integer(0),
  integer(0),
  integer(0),
  integer(0),
  integer(0),
  integer(0)
))

for(i in 1:2){
  matchedsolutions.l[[names(allsolutionset.l[i])]]<-checkSolution(testpositionlist.l,allsolutionset.l[[i]])
}

# Quick peek shows that there are lots of matches in $aa.
#Because of the walk-back algorithm there are going to be groups of solutions that look-alike... 
# I need the whole fucking set (all 3 billion). This is maddening... 
# Apropos aside with Jackie Huband: there may be a way to pare the solution set... Look at first line, say...

# Check these...
length(matchedsolutions.l) # 38
# but only $aa and $ab have solutions!!!
lengths(matchedsolutions.l)
matchedsolutions.l[1] # first set
length(matchedsolutions.l$aa) # there are 20146 matches in this set!!?
length(matchedsolutions.l$ab) # 117 matches

# What to do?
# Generate the 3 billion solutions!


### EARLIER EFFORTS ### ------

# The following attempt to create a master list of the split file solutions isn't working...
# allhyphenatedsolutions.l<-list()
# start <- proc.time()
# for(i in 1:length(beadsolutions.l)){
#   allhyphenatedsolutions.l[names(beadsolutions.l[i])]<-genPos(beadsolutions.l[[i]])
# }
# print(proc.time() - start)
# # user  system elapsed 
# # 778.803  35.804 896.410 
# 
# str(allhyphenatedsolutions.l) # should be 38 elements, 100,000 solutions in each, each solution a list of 8
# 
# #Remember that we are working with a list of lists (the following are made up)...
# solution1.l<-list(list(c(9),c(3,5,9),c(2,7,10),c(3,9,10),c(2,9),integer(0)))
# solution2.l<-list(list(integer(0),c(3,5,9),c(2,3,5),c(2,9,10),c(9),c(2,4)))
# solution3.l<-list(list(c(8,9),c(3,5,9),c(2,7,10),integer(0),c(3,9,10),c(2,9)))
# allthesolutions.l<-list(solution1.l,solution2.l,solution3.l)
# str(allthesolutions.l)

### Match Solution
# Here is Clay's original function. It works to match exact [?] solutions -- I think, but I'm worried. 
# checkSol <- function(x, y){ # x is the poem, y is the solution
#   tmp1 <- lapply(x, unlist)
#   tmp2 <- lapply(y, unlist)
#   all(mapply(function(x,y)all(x %in% y), x = tmp1, y = tmp2))
# }

# # Try out the function with a test solution
# # The function maps poems to solutions. If there is a hyphen in a poem, there has to be one in the
# # solution, but the inverse need not be true for a match. 
# testsolution.l<-list(list(2,c(4,5),7,c(6,9),c(6,9),c(3,8,9,10),5,5))
# str(testsolution.l)
# checkSol(testsolution.l,ebbhypoctavepositions)

# Brad wrote this clumsy, failing function:
# matchedsolutions<-NULL
# findSolutions<-function(poem,puzzsolutions){ 
#   # give the function an octave and a 100,000 solution split?
#   # try with poem<-ebbhypoctavepositions.l and puzzsolution<-testhyphenatedsolutions.l
#   for(i in 1:length(puzzsolutions)){
#     solution<-puzzsolutions[[i]] # set solution to ith solution, each line a list of positions
#     solutionname<-names(puzzsolutions[i]) # keep track of which solution is tested
#     matches<-NULL
#     for(j in 1:lengths(poem)){ # loop over every line of the poem and match
#       poemline<-poem[[1]][[j]] # working with only one poem, its jth line
#       puzzline<-solution[[j]]
#       if(all(poemline %in% puzzline)){
#         # note integer(0) %in% c(2,5) returns a logical(0) and all(logical(0)) is T
#         matches<-c(matches,T)
#       } else {
#         matches<-c(matches,F)
#       }
#     }
#     if(sum(matches)==lengths(poem)){
#       matchedsolution<-solutionname
#       matchedsolutions<-c(matchedsolutions,matchedsolution)
#     } else {
#       print(paste(solutionname,"is not a solution"))
#     }
#   }
#   return(matchedsolutions)
# }


# ### Clay's handling to multiple poems with checkSol() and output checking ### ----
# 
# # This worked nicely when I was checking 17 sestets against 2339 sestet solutions.
# # I am worried it may not be working with my octaves...
# #Quick check on test set of solutions:
#   output <- matrix(NA, nrow = length(testhyphenatedsolutions.l), ncol = length(ebbhypoctavepositions))
# for(i in 1:length(ebbhypoctavepositions)){
#   for(j in 1:length(testhyphenatedsolutions.l)){
#     output[j,i] <- checkSol(ebbhypoctavepositions, testhyphenatedsolutions.l[[j]])
#   }
# }
# 
# # Any TRUE?
# any(output) # NO!!!! I've tested/checked hundreds of thousands now!
# 
# ### Process EBB octave against all 38 sets of 100,000 solutions (3 million solutions) ### --
# # Now loop through EBB octave and allhyphenatedsolutions.l and store results in a matrix
# 
# for(i in 1:length(beadsolutions.l)){
#   testhyphenatedsolutions.l<-genPos(beadsolutions.l[[i]])
#   output <- matrix(NA, nrow = length(testhyphenatedsolutions.l), ncol = length(ebbhypoctavepositions))
#   for(j in 1:length(ebbhypoctavepositions)){
#     for(k in 1:length(testhyphenatedsolutions.l)){
#       output[k,j] <- checkSol(ebbhypoctavepositions, testhyphenatedsolutions.l[[k]])
#     }
#   }
#   if(any(output)){
#     paste("Solution(s)! For",names(beadsolutions.l[i]))
#     which(output, arr.ind = TRUE)
#   } else {
#     paste("No solutions for",names(beadsolutions.l[i]))
#   }
# }
# 
# 
# # If there are any solutions, they will be stored in the matrix as TRUE
# output <- matrix(NA, nrow = length(hyphenatedsolutions.l[4]), ncol = length(ebbhypoctavepositions))
# for(i in 1:length(ebbhypoctavepositions)){
#   for(j in 1:length(hyphenatedsolutions.l[4])){
#     output[j,i] <- checkSol(ebbhypoctavepositions, hyphenatedsolutions.l[4][[j]])
#   }
# }
# 
# # Any TRUE?
# any(output)
# 
# # Which ones? Use arr.ind = TRUE to list the row and column number;
# # row lists the solution number
# which(output, arr.ind = TRUE)
# 
# 
# hyphenatedsolutions.l[4][[1]]
# ebbhypoctavepositions

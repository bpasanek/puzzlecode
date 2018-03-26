// polycube:  A polyomino and polycube puzzle solver.
// Copyright 2011 Matthew T. Busche.
//
// This program is free software:  you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation, either version 3 of the License, or (at your option)
// any later version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
// more details.
//
// You should have received a copy of the GNU General Public License along
// with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "FileConfig.hpp"
#include "NamedPiece.hpp"

void FileConfig::add_options(boost::program_options::options_description& desc)
{
    using boost::lexical_cast;
    desc.add_options()

        ("bruijn,b", boost::program_options::value<int>(&bruijn),
            (std::string("\nSets the number of remaining pieces when the EMCH "
            "tiling algorithm is turned off and de Bruijn's algorithm "
            "is turned on.  A bruijn setting of zero turns de Bruijn's "
            "algorithm off.  The default value is ") +
            lexical_cast<std::string>(bruijn) + ".\n").c_str())

        ("emch,e", boost::program_options::value<int>(&emch),
            (std::string("\nSets the number of remaining pieces when the MCH "
            "algorithm is turned off and the Estimated-MCH algorithm "
            "is turned on.  Setting emch to zero or to any value less than "
            "or equal to the bruijn setting disables Estimated-MCH "
            "altogether.  The default value is ") +
            lexical_cast<std::string>(emch) + ".\n").c_str())

        ("format,f", boost::program_options::value<OutputFormatConfig>(&outputFormat),
            std::string("\nSelects the format of solution and "
            "trace output.  The configuration argument may be up to two "
            "characters.  The overall format of each solution or trace "
            "output is specified by one character:\n"
            "   B - Brief (default)\n"
            "   F - Full\n"
            "   S - Sub-puzzle\n"
            "Brief output only displays placed pieces.  Full output is in the "
            "form of complete puzzle definitions which may be reapplied as "
            "input.  Sub-puzzle output is like full output, but placed pieces "
            "are encoded as stationary, which is useful when combined with "
            "the -g and -R options to break one large puzzle into smaller "
            "subpuzzles.\n\n"
            "The output format of pieces is specfied by one character:\n"
            "   L - Layout (default)\n"
            "   C - Coordinate\n"
            "These two output formats are identical to the puzzle piece "
            "input formats of the layout (L) and coordinate (C) "
            "directives.\n").c_str())

        ("fitFilter,F", boost::program_options::value<int>(&fitFilter)->implicit_value(FILTER_ONCE),
            (std::string("\nSet the minimum number of remaining pieces for "
            "the invocation of fit-based filtering of puzzle piece images.  "
            "A fit filter operation examines each remaining row in the "
            "DLX matrix to see if using that row would result in a puzzle "
            "configuration where some hole can no longer be filled or "
            "some piece can no longer be placed.  If this occurs, "
            "the row is temporarily removed.  Setting fitFilter to the "
            "special fixed value ") + lexical_cast<std::string>(FILTER_ONCE) +
            " is equivalent to setting to the number of pieces in the puzzle "
            "and results in fit-based filtering to be applied just once "
            "before the search begins.  A setting of "  +
            lexical_cast<std::string>(FILTER_OFF) + " disables fit-based "
            "filtering altogether.  Note that filtering is implemented on "
            "the DLX matrix and so decreasing the setting below the maximum "
            "of the mch, emch and bruijn settings has no affect.  The default "
            "setting is " +
            lexical_cast<std::string>(fitFilter) + ".\n").c_str())

        ("goal,g", boost::program_options::value<int>(&goal),
            (std::string("\nEach time the number of remaining pieces equals "
            "the goal, a solution is declared and processed, and "
            "a backtrack is forced (even if additional pieces could be "
            "placed).  The default setting is ") +
            lexical_cast<std::string>(goal) + ".  Setting the goal to "
            "non-zero values is useful for breaking down "
            "a large puzzle into many smaller puzzles.  See also the "
            "-f and -R options.\n").c_str())

        ("info,i", boost::program_options::value<bool>(&info)->implicit_value(true),
            (std::string("\nIf set to true, input configuration, puzzle "
            "properties, and search statistics are output.  By default "
            "this feature is ") + (info ? "on" : "off") + ".\n").c_str())

        ("mch,m", boost::program_options::value<int>(&mch),
            (std::string("\nSets the number of remaining pieces when the DLX "
            "algorithm is turned off and the MCH algorithm "
            "is turned on.  Setting mch to zero or to any value less than "
            "or equal to the emch setting disables MCH altogether.  "
            "The default value is ") +
            lexical_cast<std::string>(mch) + ".\n").c_str())

        ("order,o", boost::program_options::value<OrderingHeuristicConfig>(&orderingHeuristicConfig),
            std::string("\nConfigure DLX ordering heuristics used to pick "
            "which hole to fill or piece to place at each step of the DLX "
            "algorithm.  Different heuristics may be specified for use "
            "when there are different numbers of pieces left to place in "
            "the puzzle box.  The ordering specification (arg) has the "
            "following format:\n\n"
            "    N1[@A1][=R1][:N2[@A2]=R2[:...]]\n\n"
            "where N1, N2, ... are names of available heuristics; "
            "A1, A2, ... are arguments which, depending on the heuristic, "
            "may or may not be required; and R1, R2, ... are the number of "
            "remaining pieces causing the heuristic to be enabled  "
            "(replacing the previously enabled heuristic).  One heuristic "
            "may be specified without an R setting which causes it to "
            "be the first heuristic enabled.  There are currently five "
            "heuristics to choose from:  f, l, a, A and R.  All of these "
            "heuristics will pick a column with zero or one fits over any "
            "other column (with a zero fit column taking precedence over "
            "a one fit column).  Assuming all columns have more than "
            "one fits, then the heuristics have these behaviors:\n\n"
            "    f: \tPicks the DLX column (hole or piece) with minimum fits.  "
            "This heuristic takes no arguments.\n\n"
            "    l@a\t,b,c: Picks the GridPoint that minimizes the linear sum "
            "a*x + b*y + c*z.\n\n"
            "    a[@\ti[,xc,yc]]: Picks the GridPoint with minimum angle from "
            "origin (xc, yc) in the X-Y plane relative to the reference angle i "
            "(which should be given in degrees -- not radians).  The default "
            "values for (xc, yc) are the mid point of the puzzle.  The default "
            "value for i is zero.\n\n"
            "    A[@\ti[,xc,yc]]: Identical to a, but maximizes the angle "
            "instead.\n\n"
            "    R[@\txc,yc,zc]: Picks the GridPoint with maximum distance from "
            "(xc, yc, zc).  The default value for (xc, yc, zc) is the center "
            "of the puzzle.\n\n"
            "In addition, these shorthand notations are available:\n\n"
            "    x: \tEquivalent to l@1,0,0\n"
            "    y: \tEquivalent to l@0,1,0\n"
            "    z: \tEquivalent to l@0,0,1\n"
            "    X: \tEquivalent to l@-1,0,0\n"
            "    Y: \tEquivalent to l@0,-1,0\n"
            "    Z: \tEquivalent to l@0,0,-1\n"
            "  xyz: \tEquivalent to l@1,1,1\n\n"
            "The default heuristic is f.\n").c_str())

        ("parityBacktrack,p", boost::program_options::value<bool>(&parityBacktrack)->implicit_value(true),
            (std::string("\nIf set, parity constraint violations are checked "
            "after each piece placement.  If the check fails, the algorithm "
            "will immediately backtrack.  These checks are implemented in "
            "all three basic algorithms:  dlx, mch and debruijn.  "
            "Unlike parity filtering, a parity backtrack check is "
            "extremely fast and adds little processing overhead "
            "to the algorithms.  If the puzzle was parity filtered immediately "
            "prior to placing the subject piece, then this parity check cannot "
            "fail (since all images causing a parity check failure would have "
            "already been filtered from the DLX matrix.  By default "
            "this feature is ") + (parityBacktrack ? "on" : "off") + ".\n").c_str())

        ("parityFilter,P", boost::program_options::value<int>(&parityFilter)->implicit_value(FILTER_ONCE),
            (std::string("\nSet the minimum number of remaining pieces for "
            "the invocation of parity-based filtering of puzzle piece images.  "
            "A parity filter operation examines each remaining row in the "
            "DLX matrix to see if using that row would result in a parity "
            "violation for the puzzle as a whole.  If a violation would "
            "occur, the row is temporarily removed.  Setting parityFilter "
            "to the special fixed value ") +
            lexical_cast<std::string>(FILTER_ONCE) +
            " is equivalent setting it to the number of pieces in the puzzle "
            "and results in parity-based filtering to be applied just once "
            "before the search begins.  A setting of "  +
            lexical_cast<std::string>(FILTER_OFF) + " disables parity-based "
            "filtering altogether.  Note that filtering is implemented on "
            "the DLX matrix and so decreasing the setting below the maximum "
            "of the mch, emch and bruijn settings has no affect.  The default "
            "setting is " +
            lexical_cast<std::string>(parityFilter) + ".\n").c_str())

        ("quiet,q", boost::program_options::value<bool>(&quiet)->implicit_value(true),
            (std::string("\nIf set to true, solutions are not displayed.  "
            "This is useful if you're only interested in the run time or "
            "statistical information output by the info option.  By default "
            "this feature is ") + (quiet ? "on" : "off") + ".\n").c_str())

        ("redundancyFilter,r", boost::program_options::value<std::string>(&redundancyFilter)->implicit_value(REDUNDANCY_FILTER_AUTO_NAME),
            (std::string("\nSet the name of the piece the Solver limits "
            "rotations and/or translations to reduce or potentially "
            "eliminate rotationally redundant solutions without loss "
            "of unique solutions.  The special value '" +
            lexical_cast<std::string>(REDUNDANCY_FILTER_AUTO_NAME) +
            "' causes the solver to pick a piece for you.  The special " +
            "value '" + lexical_cast<std::string>(REDUNDANCY_FILTER_OFF_NAME) +
            "' turns this feature off.  The redundancyFilter piece must have "
            "a shape that is unique from other pieces in the puzzle.  "
            "The default value is '") +
            lexical_cast<std::string>(redundancyFilter) + "'.\n").c_str())

        ("redundancyFilterFirst,R", boost::program_options::value<bool>(&redundancyFilterFirst)->implicit_value(true),
            (std::string("\nIf set to true, the DLX column corresponding to "
            "the piece selected for rotational redundancy filtering is "
            "forcibly selected to be processed first.  This option is "
            "meaningless if not combined with the -r option.  This option "
            "is useful when also combined the -f, and -g options to "
            "try to make a set of generated puzzle subproblems "
            "rotationally unique.  By default this feature is ") +
            (redundancyFilterFirst ? "on" : "off") + ".\n").c_str())

        ("sample,s", boost::program_options::value<MonteCarloConfig>(&monteCarlo),
            std::string("\nEnables Monte Carlo sampling of the search space.  "
            "The configuration argument is of the form T,R,S where T is "
            "the number of trials to perform, R is the number of remaining "
            "pieces that triggers the start and end of a trial, and S is the "
            "64 bit seed value for the Mersenne Twister 19937 random number "
            "generator.  When enabled, prior to the execution of any "
            "backtracking algorithm, but after initial application of "
            "requested image filters, the nodes under each DLX column are "
            "reordered randomly so that the order of the search executed by "
            "DLX will be randomized.  The first time a piece is removed from "
            "the puzzle when exactly R pieces remain (prior to the removal) "
            "the Monte Carlo trial is ended.  DLX then completely unwinds and "
            "the matrix is randomly ordered again to start the next trial.  "
            "Statistics output at the end of the program will be for "
            "all trials, so you must divide by T to get per trial "
            "statistics.  The conditionals to terminate a trial are only "
            "implemented in the DLX algorithm, so R must be greater than "
            "or equal to the enabling threshold of the MCH, EMCH and "
            "de Bruijn algorithms.  (See mch, emch and bruijn.)\n").c_str())

        ("trace,t", boost::program_options::value<int>(&trace)->implicit_value(1),
            (std::string("\nIf trace is set to some positive number K, then "
            "after each piece placement, if there are at least K-1 pieces "
            "remaining to be placed, then the state of the puzzle is "
            "displayed.  Also, after each piece removal if there are at least "
            "K pieces remaining to be placed, then the state of the puzzle is "
            "displayed again.  If K is negative, then the state of the puzzle is "
            "displayed only after a piece is placed and only if there are "
            "exactly -(K-1) pieces left to place.  If K is zero, then "
            "trace output is disabled.  The default value is ") +
            lexical_cast<std::string>(trace) + ".\n").c_str())

        ("unique,u", boost::program_options::value<bool>(&unique)->implicit_value(true),
            (std::string("\nIf enabled only rotationally unique solutions are "
            "output.  Activating this option enables a solution filter that "
            "compares each solution found to all rotations of all previously "
            "discovered solutions (via binary search).  If the new solution is "
            "found to be identical to some rotation of some previously "
            "discovered solution, then the solution will still increment the "
            "total solution counter, but is otherwise discarded.  If the "
            "solution is found to be unique, then all rotations of that "
            "solution are calculated and added to the list of solutions "
            "filtered against.  This filter is forcibly disabled if the "
            "puzzle has no symmetric rotations, or if the redundancyFilter "
            "is enabled and it successfully expunges redundant solutions "
            "from the search space.  By default this feature is ") +
             (unique ? "on" : "off") + ".\n\n"
            "Note:  \tThe filter uses " + lexical_cast<std::string>(sizeof(PIECEID_T)) +
            " byte" + (sizeof(PIECEID_T) > 1 ? "s" : "") +
            " per GridPoint in the puzzle cuboid per unique soltuion per "
            "symmetric rotation, plus additional memory for STL vector "
            "overhead.  So if a puzzle has hundreds of millions of unique "
            "solutions, use of this filter may cause you trouble.\n\n").c_str())

        ("volumeBacktrack,v", boost::program_options::value<int>(&volumeBacktrack),
            (std::string("\nAfter placing a piece, if the number of remaining "
            "pieces equals at least the volumeBacktrack setting, then "
            "the remaining puzzle space is examined for volume constraint "
            "violations.  If an isolated sub-space is found that cannot "
            "possibly be filled by the remaining pieces (because no "
            "combination of those pieces gives a total volume "
            "that matches the volume of the sub-space), then the search "
            "algorithm immediately backtracks. Unlike volume filtering, "
            "volume backtracks do not require the use of DLX.  A setting "
            "of 0 disables volume-based backtrack altogether.  The default "
            "setting is ") + lexical_cast<std::string>(volumeFilter) + ".\n\n"
            "WARNING:  the memory required by the internal volume monitor "
            "grows geometrically with the number of different piece "
            "sizes.\n\n").c_str())

        ("volumeFilter,V", boost::program_options::value<int>(&volumeFilter)->implicit_value(FILTER_ONCE),
            (std::string("\nSet the minimum number of remaining pieces for "
            "the invocation of volume-based filtering of puzzle piece images.  "
            "A volume filter operation examines each remaining row in the "
            "DLX matrix to see if using that row would result in the "
            "parititioning of the remaining puzzle space with one of "
            "these new subspaces having a volume that cannot possibly "
            "be filled with the remaining pieces of the puzzle.  If this "
            "occurs, the row is temporarily removed.  Setting volumeFilter to "
            "the special fixed value ") +
            lexical_cast<std::string>(FILTER_ONCE) +
            " is equivalent to setting to the number of pieces in the puzzle "
            "and results in volume-based filtering to be applied just once "
            "before the search begins.  A setting of "  +
            lexical_cast<std::string>(FILTER_OFF) + " disables volume-based "
            "filtering altogether.  Note that filtering is implemented on "
            "the DLX matrix and so decreasing the setting below the maximum "
            "of the mch, emch and bruijn settings has no affect.  The default "
            "setting is " +
            lexical_cast<std::string>(volumeFilter) + ".\n\n"
            "WARNING:  the memory required by the internal volume monitor "
            "grows geometrically with the number of different piece "
            "sizes.\n\n").c_str())

    ;
}

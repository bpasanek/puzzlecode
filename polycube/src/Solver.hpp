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

#ifndef SOLVER_HPP
#define SOLVER_HPP

#include <algorithm>
#include <iostream>
#include <iomanip>
#include "time.h"
#include "MonteCarloConfig.hpp"
#include "OrderingHeuristicConfig.hpp"
#include "OrderingHeuristic.hpp"
#include "PuzzleSolverInterface.hpp"

class Solver
{
    private:

        // ####################################################
        // CONFIG SETTINGS
        // ####################################################

        /** The solver is setup to slower but smarter techniques when there
         ** are still many pieces to place in the puzzle solution space, but
         ** to use a faster, but dumber tiling technique when there are fewer
         ** pieces remaining to be placed.  The last bruijn pieces are
         ** placed using a fast and dumb bruijn algorithm.
         **/
        int bruijn;

        /** Specifies the format of solution and trace output.
         **/ 
        OutputFormatConfig outputFormat;

        /** Sets the number of remaining pieces when the MCH algorithm is
         ** turned off and the Estimated-MCH algorithm is turned on.  Setting
         ** emch to zero or to any value less than or equal to the bruijn
         ** setting disables MCH altogether.
         **/
        int emch;

        /** The minimum number of remaining pieces for fit filters.
         **/
        int fitFilter;

        /** Each time the number of remaining pieces equals the goal, a
         ** solution is declared and processed, and a backtrack is forced
         ** (even if additional pieces could be placed).
         **/
        int goal;

        /** If true, various informational messages are printed.  These
         ** include puzzle input information; and solution, and performance
         ** statistics.
         **/
        bool info;

        /** Sets the number of remaining pieces when the DLX algorithm is
         ** turned off and the MCH algorithm is turned on.  Setting mch to
         ** zero or to any value less than or equal to the emch setting
         ** disables MCH altogether.
         **/
        int mch;

        MonteCarloConfig monteCarlo;

        OrderingHeuristicConfig orderingHeuristicConfig;

        /** If true, a parity constraint check is made after each piece
         ** placement.
         **/
        bool parityBacktrack;

        /** The minimum number of remaining pieces for parity filters.
         **/
        int parityFilter;

        /** If true, solutions are not output.
         **/
        bool quiet;

        /** The name of a piece in pieceList which the solver constrains
         ** rotations and/or translations to eliminate rotationally equivalent
         ** solutions during the search.  The value REDUNDANCY_FILTER_AUTO_NAME
         ** causes the solver to make a good choice for which piece to limit
         ** rotations.  The value REDUNDANCY_FILTER_OFF_NAME turns off
         ** constraing processing (and usually leads to more redundant
         ** solutions being found only to have them filtered out.)
         **/
        std::string redundancyFilterName;

        /** The index of the piece whose images are filtered to eliminate
         ** rotational redundancy among solutions.  The value
         ** REDUNDANCY_FILTER_OFF_INDEX turns this feature off.  This field is
         ** not directly settable by the user, but is rather derived from the
         ** user settable field constrainName during initialization.
         **/
        int redundancyFilterIndex;

        /** If set to true, the DLX column corresponding to the piece selected
         ** for rotational redundancy filtering is forcibly selected to be
         ** processed first.
         **/
        bool redundancyFilterFirst;

        /** If trace is set to some number K, then after each piece placement,
         ** if there are at least K pieces remaining to be placed, then the
         ** state of the puzzle is displayed.  Also, after each piece removal
         ** if there are at least K+1 pieces remaining to be placed, then the
         ** state of the puzzle is displayed.  If K is negative, then the state
         ** of the puzzle is displayed only after a piece is placed and there
         ** are exactly -K pieces left to place.  A zero value turns this
         ** feature off.
         **/
        int trace;

        /** If true, rotated copies of previously discovered rotationally
         ** unique solutions are maintained, and only rotationlly unique
         ** solutions are output.
         **/
        bool unique;

        /** The minimum number of remaining pieces for volume backtracks.
         **/
        int volumeBacktrack;

        /** The minimum number of remaining pieces for volume filters.
         **/
        int volumeFilter;

        // ####################################################################
        // CONTROL VARIABLES.
        //
        // These variables are all just simple translations of configuration
        // variables or combinations of configuration variables that simplify
        // for the algorithms the decision making process of whether or not to
        // invoke some image filter, backtrack processing, trace, etc.
        // ####################################################################

        /** Same as volumeBacktrack but a 0 value which means OFF is
         ** tranlated to a number greater than the number of pieces
         ** in the puzzle, simplifying the conditional used
         ** to determine if a volume check should be made.
         **/
        int volumeBacktrackControl;

        /** Same as fitFilter but special constants FILTER_OFF and FILTER_ONCE
         ** are translated to actual piece counts for easier processing.  This
         ** field is not directly settable by the user, but is rather derived
         ** from the user settable field fitFilter during initialization.
         **/
        int fitFilterControl;

        /** Same as parityFilter but special constants FILTER_OFF and
         ** FILTER_ONCE are translated to actual piece counts for easier
         ** processing.  This field is not directly settable by the user, but
         ** is rather derived from the user settable field parityFilter during
         ** initialization.
         **/
        int parityFilterControl;

        /** Same as volumeFilter but special constants FILTER_OFF and
         ** FILTER_ONCE are translated to actual piece counts for easier
         ** processing.  This field is not directly settable by the user, but
         ** is rather derived from the user settable field volumeFilter during
         ** initialization.
         **/
        int volumeFilterControl;

        /** This is just a convenience lookup to determine if the image set
         ** should be filtered after image placement:
         **
         **   filterControl = min(fitFilterControl, volumeFilterControl, parityFilterControl)
         **/
        int filterControl;

        /** The minimum number of remaining pieces needed to continue
         ** maintaining the parity state.  Once the number of remaining pieces
         ** falls below this threshold, neither parity filters nor parity
         ** backtracks are used and it becomes pointless to continue to
         ** maintain the parity state.  Parity state updates are so simple we
         ** could conceivably be better off by just always updating the parity
         ** state; but I've kept this control for the aesthetics of symmetry
         ** with volumePlaceControl.
         **/
        int parityPlaceControl;

        /** The minimum of volumeFilterControl and volumeBacktrackControl.
         ** This variable is used to determine whether to continue maintaining
         ** the GridPoint fill state as pieces are placed.  Once the number of
         ** remaining pieces falls below this threshold, neither volume filters
         ** nor volume backtracks are used and it becomes pointless to
         ** continue to maintain the fill state.
         **/
        int volumePlaceControl;

        /** Nominally this will have the same value as unique; however, if
         ** either the Puzzle has no symmetric rotations, or if the Puzzle has
         ** determined that the redundancyFilter eliminates all rotationally
         ** redundant solutions from the search space; then uniqueControl will
         ** necessarily be false.
         **/
        int uniqueControl;

        /** Flag is set when a Monte Carlo trial is ended, forcing DLX to
         ** unwind.
         **/
        bool endTrialControl;

        // ####################################################
        // OTHER STUFF
        // ####################################################

        /** If the soltuion filter is enabled, then each time a rotationally
         ** unique solution is found, it is rotated by all rotations in the
         ** Puzzle's symmetric rotation list.  Each of these rotated solutions
         ** are then added to allSolutionList.  Each time a solution is found,
         ** it is checked against allSolutionList to determine if it is
         ** rotationally unique from previously discovered solutions.  This
         ** list and all associated processing is forcibly disabled if the
         ** Puzzle determines that no rotationally redundant solutions will be
         ** found.
         **/
        std::set<std::vector<PIECEID_T>, StateComp> allSolutionList;

        std::vector<OrderingHeuristic*> orderingHeuristicList;

        /** This is just used as an iteration variable for monte-carlo
         ** anlaysis, but I made it a class member so I could print the
         ** current trial number as part of the status dump (which can be
         ** triggered asynchronously via unix signal 10).
         **/
        int monteCarloTrial;

        // ####################################################
        // THE PUZZLE AND SOME OF IT'S IMPORTANT PROPERTIES
        // ####################################################

        /** An interface wrapper around the Puzzle object upon which this
         ** solver is operating.
         **/
        PuzzleSolverInterface puzzle;

        /** The number of individual mobile pieces in the puzzle.  (This is
         ** the same as numMobileShapes if all mobile pieces have a unique
         ** shape.)  Copied here from puzzle for convenience.
         **/
        int numMobilePieces;

        /** The array of indicees of mobile pieces that are still free.  This
         ** is reset each time Dlx is turned off and one of the tiling
         ** algorithms (mch or bruijn) is turned on.
         **/
        int* remainingShapeList;

        /** remainingShapeCount[p] gives the number of copies of the shape
         ** with id p that is still free.  This is reset each time Dlx is
         ** turned off and one of the tiling algorithms (mch or bruijn) is
         ** turned on.
         **/
        int* remainingShapeCount;

        // ####################################################
        // STATISTICS
        // ####################################################

        unsigned long long traceCount;
        long long  solutionCount;
        long long  redundantCount;
        long long* parityBacktrackCount;
        long long* parityFilterCount;
        long long* volumeBacktrackCount;
        long long* volumeFilterCount;
        long long* fitFilterCount;
        long long* attemptsList;
        long long* fitsList;

        PerformanceMeter* solveMeter;
        PerformanceMeter* initMeter;
        PerformanceMeter* algoMeter;
        PerformanceMeter* filterMeter;
        PerformanceMeter* cleanupMeter;

    public:

        Solver();
        int getBruijn() const;
        void setBruijn(int bruijn);
        int getEmch() const;
        void setEmch(int emch);
        const OutputFormatConfig& getOutputFormat() const;
        void setOutputFormat(const OutputFormatConfig& outputFormat);
        int getFitFilter() const;
        void setFitFilter(int fitFilter);
        int getGoal() const;
        void setGoal(int goal);
        bool getInfo() const;
        void setInfo(bool info);
        int getMch() const;
        void setMch(int mch);
        const MonteCarloConfig& getMonteCarlo() const;
        void setMonteCarlo(const MonteCarloConfig& monteCarlo);
        const OrderingHeuristicConfig& getOrderingHeuristicConfig() const;
        void setOrderingHeuristicConfig(const OrderingHeuristicConfig& orderingHeuristicConfig);
        bool getParityBacktrack() const;
        void setParityBacktrack(bool parityBacktrack);
        int getParityFilter() const;
        void setParityFilter(int parityFilter);
        int getQuiet() const;
        void setQuiet(bool quiet);
        const std::string& getRedundancyFilterName() const;
        void setRedundancyFilterName(const std::string& redundancyFilterName);
        bool getRedundancyFilterFirst() const;
        void setRedundancyFilterFirst(bool redundancyFilterFirst);
        int getTrace();
        void setTrace(int trace);
        bool getUnique() const;
        void setUnique(bool unique);
        int getVolumeBacktrack() const;
        void setVolumeBacktrack(int volumeBacktrack);
        int getVolumeFilter() const;
        void setVolumeFilter(int volumeFilter);
        void solve(Puzzle& puzzle);

    private:

        void init();
        void cleanup();
        int translateFilter(int filterLevel);
        static std::string formatFilter(int filterSetting);
        void enableSigHandler();
        void disableSigHandler();
        void processSigRequest();
        void allocRemainingShapeList();
        void deleteRemainingShapeList();
        void initControlVariables();
        void initOrderingHeuristics();
        void deleteOrderingHeuristics();
        void initStats();
        void deleteStats();
        int redundancyFilterNameToIndex();
        void dumpInfo();
        void solve();
        void solveOnce();
        int dlxFilter(int numRemainingPieces, int parityOfLastPiecePlaced);
        void dlxUnfilter(int numUnfilter);

        // These next five functions together form the basic top-level DLX
        // search algorithm.  They call eachother in a recursive chain:
        //
        //   solveDlx_outter calls
        //   solveDlx_parityBacktrack which calls
        //   solveDlx_volumeBacktrack which calls
        //   solveDlx_coverAndFilter which calls
        //   solveDlx_inner which calls
        //   solveDlx_outter
        //
        // It could easily all be written as single recursive function but
        // this approach has less branching and is perhaps easier to follow.

        void solveDlx_outter(DlxHead* target, int numRemainingPieces);
        void solveDlx_parityBacktrack(DlxNode* r, int numRemainingPieces);
        void solveDlx_volumeBacktrack(DlxNode* r, int numRemainingPieces);
        void solveDlx_coverAndFilter(DlxNode* r, int numRemainingPieces);
        void solveDlx_inner(DlxNode* r, int numRemainingPieces);

        /** This function is called each time DLX is turned off and the mch
         ** and/or bruijn algorithms are turned on.  It transforms the current
         ** state of the DLX data model into a data model more well suited for
         ** the MCH and Bruijn algorithms and then calls either solveMch_outter
         ** or solveBruijn_outter (depending on configuration settings).
         **/
        void solveTile();

        void solveMch_outter(GridPoint* target, int numRemainingShapes, int numRemainingPieces);
        void solveMch_parityBacktrack(Image* i, int numRemainingShapes, int numRemainingPieces);
        void solveMch_volumeBacktrack(Image* i, int numRemainingShapes, int numRemainingPieces);
        void solveMch_inner(int numRemainingShapes, int numRemainingPieces);

        void solveBruijn_outter(GridPoint* target, int numRemainingShapes, int numRemainingPieces);
        void solveBruijn_parityBacktrack(GridPoint* target, Image* i, int numRemainingShapes, int numRemainingPieces);
        void solveBruijn_volumeBacktrack(GridPoint* target, Image* i, int numRemainingShapes, int numRemainingPieces);

        void processSolution();

        /** Checks to see if solution is unique from those in allSolutionList.
         ** If not then the redundant solution count is incremented and false
         ** is returned.  Otherwise, that solution is added to
         ** uniqueSolutionList, all rotations of that solution (that result in
         ** the same puzzle shape) are added to allSolutionList, and true is
         ** returned.
         **/
        bool filterSolution();
        void showTrace();
        void dumpStats();
};


inline int Solver::getBruijn() const
{
    return bruijn;
}

inline void Solver::setBruijn(int bruijn)
{
    this->bruijn = bruijn;
}

inline const OutputFormatConfig& Solver::getOutputFormat() const
{
    return outputFormat;
}

inline void Solver::setOutputFormat(const OutputFormatConfig& outputFormat)
{
    this->outputFormat = outputFormat;
}

inline int Solver::getFitFilter() const
{
    return fitFilter;
}

inline void Solver::setFitFilter(int fitFilter)
{
    this->fitFilter = fitFilter;
}

inline int Solver::getGoal() const
{
    return goal;
}

inline void Solver::setGoal(int goal)
{
    this->goal = goal;
}

inline int Solver::getEmch() const
{
    return emch;
}

inline void Solver::setEmch(int emch)
{
    this->emch = emch;
}

inline bool Solver::getInfo() const
{
    return info;
}

inline void Solver::setInfo(bool info)
{
    this->info = info;
}

inline int Solver::getMch() const
{
    return mch;
}

inline void Solver::setMch(int mch)
{
    this->mch = mch;
}

inline const MonteCarloConfig& Solver::getMonteCarlo() const
{
    return monteCarlo;
}

inline void Solver::setMonteCarlo(const MonteCarloConfig& monteCarlo)
{
    this->monteCarlo = monteCarlo;
}

inline const OrderingHeuristicConfig& Solver::getOrderingHeuristicConfig() const
{
    return orderingHeuristicConfig;
}

inline void Solver::setOrderingHeuristicConfig(const OrderingHeuristicConfig& orderingHeuristicConfig)
{
    this->orderingHeuristicConfig = orderingHeuristicConfig;
}

inline bool Solver::getParityBacktrack() const
{
    return parityBacktrack;
}

inline void Solver::setParityBacktrack(bool parityBacktrack)
{
    this->parityBacktrack = parityBacktrack;
}

inline int Solver::getParityFilter() const
{
    return parityFilter;
}

inline void Solver::setParityFilter(int parityFilter)
{
    this->parityFilter = parityFilter;
}

inline int Solver::getQuiet() const
{
    return quiet;
}

inline void Solver::setQuiet(bool quiet)
{
    this->quiet = quiet;
}

inline const std::string& Solver::getRedundancyFilterName() const
{
    return redundancyFilterName;
}

inline void Solver::setRedundancyFilterName(const std::string& redundancyFilterName)
{
    this->redundancyFilterName = redundancyFilterName;
}

inline bool Solver::getRedundancyFilterFirst() const
{
    return redundancyFilterFirst;
}

inline void Solver::setRedundancyFilterFirst(bool redundancyFilterFirst)
{
    this->redundancyFilterFirst = redundancyFilterFirst;
}

inline int Solver::getTrace()
{
    return trace;
}

inline void Solver::setTrace(int trace)
{
    this->trace = trace;
}

inline bool Solver::getUnique() const
{
    return unique;
}

inline void Solver::setUnique(bool unique)
{
    this->unique = unique;
}

inline int Solver::getVolumeBacktrack() const
{
    return volumeBacktrack;
}

inline void Solver::setVolumeBacktrack(int volumeBacktrack)
{
    this->volumeBacktrack = volumeBacktrack;
}

inline int Solver::getVolumeFilter() const
{
    return volumeFilter;
}

inline void Solver::setVolumeFilter(int volumeFilter)
{
    this->volumeFilter = volumeFilter;
}


inline void Solver::solveDlx_parityBacktrack(DlxNode* r, int numRemainingPieces)
{
    if(numRemainingPieces >= parityPlaceControl)
    {
        puzzle.placeParity(r->image->parity);
        if(!parityBacktrack || puzzle.checkParity())  // TODO:  could modify this conditional to also skip checkParity call if parity filtering was enabled on the previous step.
            solveDlx_volumeBacktrack(r, numRemainingPieces);
        else
            ++parityBacktrackCount[numRemainingPieces];
        puzzle.unplaceParity(r->image->parity);
    }
    else
        solveDlx_volumeBacktrack(r, numRemainingPieces);
}


inline void Solver::solveDlx_volumeBacktrack(DlxNode* r, int numRemainingPieces)
{
    if(numRemainingPieces >= volumePlaceControl)
    {
        puzzle.placeVolume(r->image);
        if(numRemainingPieces < volumeBacktrackControl || puzzle.checkVolume())  // TODO:  could modify this conditional to also skip checkVolume call if volume filtering was enabled on the previous step.
            solveDlx_coverAndFilter(r, numRemainingPieces);
        else
            ++volumeBacktrackCount[numRemainingPieces];
        puzzle.unplaceVolume(r->image);
    }
    else
        solveDlx_coverAndFilter(r, numRemainingPieces);
}


inline void Solver::solveDlx_coverAndFilter(DlxNode* r, int numRemainingPieces)
{
    for(DlxNode* j = r->right; j != r; j = j->right)
        if(--j->head->count == 0)
            puzzle.dlxCover(j->head);

    if(numRemainingPieces >= filterControl)
    {
        int numFilter = dlxFilter(numRemainingPieces, r->image->parity);
        solveDlx_inner(r, numRemainingPieces);
        dlxUnfilter(numFilter);
    }
    else
        solveDlx_inner(r, numRemainingPieces);

    for(DlxNode* j = r->left; j != r; j = j->left)
        if(j->head->count++ == 0)
            puzzle.dlxUncover(j->head);
}


inline void Solver::solveDlx_inner(DlxNode* r, int numRemainingPieces)
{
    // Find the most constrained hole or piece
    //
    DlxHead* root = puzzle.getDlxRoot();
    DlxHead* best = (DlxHead*) root->right;
    OrderingHeuristic* oh = orderingHeuristicList[numRemainingPieces];
    double min = oh->eval(best);
    for(DlxHead* h = (DlxHead*) best->right; h != root; h = (DlxHead*) h->right)
    {
        double e = oh->eval(h);
        if(min > e || (min == e && best->numRow > h->numRow))
        {
            min = e;
            best = h;
        }
    }

    // Only continue if there's at least one row to cover the most
    // constrained column.
    //
    if(best->numRow > 0)
    {
        puzzle.placeGridPointCount(r->image);

        // If the number of remaining pieces has reached the user
        // selected tile range then we want to switch to tiling mode;
        // however, dlx is forcibly used again if  the number of
        // remaining holes is too large to be represented by the
        // puzzlemask_t bitmask OR if the most constrained element has
        // only one row that covers it.  (The latter decision is just
        // an intuitive heuristic:  the cost of a data model morph
        // from dlx to tiling is high, and I want to make sure there
        // are at least two sub-branches before making the switch.
        // Also, if there's only one-subbranch from our current state,
        // DLX can take us to the next step fairly efficiently
        // anyway.)
        //
        if(numRemainingPieces <= mch && puzzle.getNumRemainingGridPoints() <=
                sizeof(puzzlemask_t)*8 && best->numRow > 1)
        {
            solveTile();
        }
        else
        {
            solveDlx_outter(best, numRemainingPieces);
        }

        puzzle.unplaceGridPointCount(r->image);
    }
}


inline void Solver::solveMch_parityBacktrack(Image* i, int numRemainingShapes, int numRemainingPieces)
{
    if(numRemainingPieces >= parityPlaceControl)
    {
        puzzle.placeParity(i->parity);
        if(!parityBacktrack || puzzle.checkParity())
            solveMch_volumeBacktrack(i, numRemainingShapes, numRemainingPieces);
        else
            ++parityBacktrackCount[numRemainingPieces];
        puzzle.unplaceParity(i->parity);
    }
    else
        solveMch_volumeBacktrack(i, numRemainingShapes, numRemainingPieces);
}


inline void Solver::solveMch_volumeBacktrack(Image* i, int numRemainingShapes, int numRemainingPieces)
{
    if(numRemainingPieces >= volumePlaceControl)
    {
        puzzle.placeVolume(i);
        if(numRemainingPieces < volumeBacktrackControl || puzzle.checkVolume())
            solveMch_inner(numRemainingShapes, numRemainingPieces);
        else
            ++volumeBacktrackCount[numRemainingPieces];
        puzzle.unplaceVolume(i);
    }
    else
        solveMch_inner(numRemainingShapes, numRemainingPieces);
}


inline void Solver::solveMch_inner(int numRemainingShapes, int numRemainingPieces)
{
    if(numRemainingPieces == bruijn)
        solveBruijn_outter(puzzle.getNextBruijnHole(puzzle.getDlxRoot()), numRemainingShapes, numRemainingPieces);
    else
        solveMch_outter(puzzle.getMostConstrainedHole(remainingShapeList, numRemainingShapes, numRemainingPieces <= emch), numRemainingShapes, numRemainingPieces);
}


inline void Solver::solveBruijn_parityBacktrack(GridPoint* target, Image* i, int numRemainingShapes, int numRemainingPieces)
{
    if(numRemainingPieces >= parityPlaceControl)
    {
        puzzle.placeParity(i->parity);
        if(!parityBacktrack || puzzle.checkParity())
            solveBruijn_volumeBacktrack(target, i, numRemainingShapes, numRemainingPieces);
        else
            ++parityBacktrackCount[numRemainingPieces];
        puzzle.unplaceParity(i->parity);
    }
    else
        solveBruijn_volumeBacktrack(target, i, numRemainingShapes, numRemainingPieces);
}


inline void Solver::solveBruijn_volumeBacktrack(GridPoint* target, Image* i, int numRemainingShapes, int numRemainingPieces)
{
    if(numRemainingPieces >= volumePlaceControl)
    {
        puzzle.placeVolume(i);
        if(numRemainingPieces < volumeBacktrackControl || puzzle.checkVolume())
            solveBruijn_outter(puzzle.getNextBruijnHole(target), numRemainingShapes, numRemainingPieces);
        else
            ++volumeBacktrackCount[numRemainingPieces];
        puzzle.unplaceVolume(i);
    }
    else
        solveBruijn_outter(puzzle.getNextBruijnHole(target), numRemainingShapes, numRemainingPieces);
}


#endif

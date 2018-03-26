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

#include <iomanip>
#include "Solver.hpp"
#include "boost/lexical_cast.hpp"
#include <algorithm>
#include "OrderingHeuristicFactory.hpp"

#ifndef NOSIGNALS

#include <signal.h>

static bool statsRequest = false;
static bool traceRequest = false;
static bool sigRequest   = false;

void setStatsRequest(int sig)
{
    statsRequest = true;
    sigRequest   = true;
}

void setTraceRequest(int sig)
{
    traceRequest = true;
    sigRequest   = true;
}

#endif

using boost::lexical_cast;

Solver::Solver()
  : 
    bruijn(0),
    emch(0),
    fitFilter(FILTER_OFF),
    goal(0),
    info(false),
    mch(0),
    parityBacktrack(false),
    parityFilter(FILTER_OFF),
    quiet(false),
    redundancyFilterName(REDUNDANCY_FILTER_AUTO_NAME),
    redundancyFilterIndex(REDUNDANCY_FILTER_AUTO_INDEX),
    trace(0),
    unique(false),
    volumeBacktrack(0),
    volumeFilter(FILTER_OFF),
    volumeBacktrackControl(0),
    fitFilterControl(0),
    parityFilterControl(0),
    volumeFilterControl(0),
    filterControl(0),
    parityPlaceControl(0),
    volumePlaceControl(0),
    uniqueControl(false),
    endTrialControl(false),
    puzzle(NULL),
    numMobilePieces(0),
    remainingShapeList(NULL),
    remainingShapeCount(NULL),
    traceCount(0),
    solutionCount(0),
    redundantCount(0),
    parityBacktrackCount(0),
    parityFilterCount(NULL),
    volumeBacktrackCount(NULL),
    volumeFilterCount(NULL),
    fitFilterCount(NULL),
    attemptsList(NULL),
    fitsList(NULL),
    solveMeter(NULL),
    initMeter(NULL),
    algoMeter(NULL),
    filterMeter(NULL),
    cleanupMeter(NULL)
{
}


void Solver::solve(Puzzle& puzzle)
{
    if(solveMeter == NULL)
        solveMeter = PolyPerf::getInstance()->getMeter("solve", "all");
    PerformanceMeasurement mSolve(solveMeter);

    this->puzzle.setPuzzle(&puzzle);
    init();
    solve();
    cleanup();
}


void Solver::init()
{
    if(initMeter == NULL)
        initMeter = PolyPerf::getInstance()->getMeter("solve-init", "solve");
    PerformanceMeasurement mInit(initMeter);

    emch = std::max(emch, bruijn);
    mch = std::max(mch, emch);

    numMobilePieces = puzzle.getNumMobilePieces();
    allocRemainingShapeList();
    initStats();
    redundancyFilterIndex = redundancyFilterNameToIndex();
    redundancyFilterIndex = puzzle.genImageLists(redundancyFilterIndex);
    initControlVariables();

    // Initialize the parity and control monitors only if we intend to use
    // them.  In particular, volume monitor initialization can allocate
    // enormous amounts of memory if puzzle pieces have many different sizes.
    // Thanks to Andrew Juell for discovering this problem.
    //
    if(parityPlaceControl <= numMobilePieces)
        puzzle.initParityMonitor();
    if(volumePlaceControl <= numMobilePieces)
        puzzle.initVolumeMonitor();

    initOrderingHeuristics();
    puzzle.dlxLoad();
    //puzzle.dumpShapes(std::cout);
    if(info)
        dumpInfo();
    enableSigHandler();
}


void Solver::cleanup()
{
    if(cleanupMeter == NULL)
        cleanupMeter = PolyPerf::getInstance()->getMeter("solve-cleanup", "solve");
    PerformanceMeasurement mCleanup(cleanupMeter);

    disableSigHandler();
    if(info)
        dumpStats();
    deleteOrderingHeuristics();
    deleteStats();
    deleteRemainingShapeList();
    numMobilePieces = 0;
    puzzle.setPuzzle(NULL);
}



void Solver::enableSigHandler()
{
#ifndef NOSIGNALS
    struct sigaction act;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;

    act.sa_handler = setStatsRequest;
    sigaction(10, &act, 0);

    act.sa_handler = setTraceRequest;
    sigaction(12, &act, 0);
#endif
}


void Solver::processSigRequest()
{
#ifndef NOSIGNALS
    sigRequest = false;
    if(statsRequest)
    {
        dumpStats();
        std::cout << std::endl << *PolyPerf::getInstance();
        statsRequest = false;
    }
    if(traceRequest)
    {
        showTrace();
        traceRequest = false;
    }
#endif
}


void Solver::disableSigHandler()
{
#ifndef NOSIGNALS
    struct sigaction act;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;

    act.sa_handler = SIG_DFL;
    sigaction(10, &act, 0);

    act.sa_handler = SIG_DFL;
    sigaction(12, &act, 0);

    statsRequest = false;
    traceRequest = false;
    sigRequest   = false;
#endif
}


void Solver::allocRemainingShapeList()
{
    remainingShapeList = makeArray<int>(puzzle.getNumMobileShapes());
    remainingShapeCount = makeArray<int>(puzzle.getNumShapes());
}


void Solver::deleteRemainingShapeList()
{
    deleteArray(remainingShapeList, 0);
    deleteArray(remainingShapeCount, 0);
}


void Solver::initControlVariables()
{
    volumeBacktrackControl = volumeBacktrack > 0 ? volumeBacktrack : numMobilePieces + 1;
    parityFilterControl    = translateFilter(parityFilter);
    volumeFilterControl    = translateFilter(volumeFilter);
    fitFilterControl       = translateFilter(fitFilter);
    parityPlaceControl     = parityBacktrack ? 0 : parityFilterControl;
    volumePlaceControl     = std::min(volumeBacktrackControl, volumeFilterControl);
    filterControl          = std::min(std::min(parityFilterControl, volumeFilterControl), fitFilterControl);

    // Only filter soltuions if the user requested it AND there is the
    // possibility of rotationally redundant soltuions in the search space.
    //
    uniqueControl = unique && puzzle.getRedundancy();
}


void Solver::initOrderingHeuristics()
{
    double xc = ((double) puzzle.getXDim() - 1) / 2;
    double yc = ((double) puzzle.getYDim() - 1) / 2;
    double zc = ((double) puzzle.getZDim() - 1) / 2;
    OrderingHeuristicFactory factory(xc, yc, zc);

    std::map<int,std::string> defList = orderingHeuristicConfig.getDefList();

    // If the defList has no entries, or if the last entry is for an ordering
    // heursitic that is not enabled when the first pieces are placed, then
    // create a default ordering heuristic definition.
    //
    if(defList.size() == 0 || (*defList.rbegin()).first < numMobilePieces)
        defList[numMobilePieces] = "f";
    std::map<int,std::string>::const_iterator i = defList.begin();
    orderingHeuristicList.assign(numMobilePieces+1, (OrderingHeuristic*) NULL);
    for(int n = 1; n <= numMobilePieces; ++n)
    {
        while((*i).first < n)
            ++i;

        orderingHeuristicList[n] = factory.gen((*i).second);
    }
}

int Solver::translateFilter(int filterSetting)
{
    switch(filterSetting)
    {
        case FILTER_OFF:
            return numMobilePieces + 1;
        case FILTER_ONCE:
            return numMobilePieces;
        default:
            return filterSetting;
    }
};


std::string Solver::formatFilter(int filterLevel)
{
    switch(filterLevel)
    {
        case FILTER_OFF:
            return "OFF";
        case FILTER_ONCE:
            return "ONCE";
        default:
            return lexical_cast<std::string>(filterLevel);
    }
}


void Solver::deleteOrderingHeuristics()
{
    for(int i = 1; i < orderingHeuristicList.size(); ++i)
        delete orderingHeuristicList[i];
}


void Solver::initStats()
{
    solutionCount        = 0;
    redundantCount       = 0;
    traceCount           = 0;
    parityBacktrackCount = initArray<long long>(numMobilePieces+1, 0);
    fitFilterCount       = initArray<long long>(numMobilePieces+1, 0);
    parityFilterCount    = initArray<long long>(numMobilePieces+1, 0);
    volumeBacktrackCount = initArray<long long>(numMobilePieces+1, 0);
    volumeFilterCount    = initArray<long long>(numMobilePieces+1, 0);
    fitsList             = initArray<long long>(numMobilePieces,   0);
    attemptsList         = initArray<long long>(numMobilePieces,   0);
}


void Solver::deleteStats()
{
    deleteArray(parityBacktrackCount, 0);
    deleteArray(fitFilterCount, 0);
    deleteArray(parityFilterCount, 0);
    deleteArray(volumeBacktrackCount, 0);
    deleteArray(volumeFilterCount, 0);
    deleteArray(fitsList, 0);
    deleteArray(attemptsList, 0);
}


int Solver::redundancyFilterNameToIndex()
{
    if(redundancyFilterName == REDUNDANCY_FILTER_OFF_NAME)
        return REDUNDANCY_FILTER_OFF_INDEX;

    if(redundancyFilterName == REDUNDANCY_FILTER_AUTO_NAME)
        return REDUNDANCY_FILTER_AUTO_INDEX;

    for(int si = 0; si < puzzle.getNumShapes(); ++si)
    {
        const Shape& s = *puzzle.getShapes()[si];
        for(int ii = 0; ii < s.getNumCopies(); ++ii)
            if(s.getPiece(ii)->getName() == redundancyFilterName)
                return si;
    }

    std::ostringstream errMsg;
    errMsg << "***Redundancy Filter Processing Error:\n\n"
        "Specified filter piece " << lexical_cast<std::string>(redundancyFilterName) << " not found.";
    throw std::runtime_error(errMsg.str());
}


void Solver::dumpInfo()
{
    std::cout << std::endl << "# Puzzle size: " << std::endl <<
        "DIMENSION=" << puzzle.getXDim() << "x" << puzzle.getYDim() <<
        "x" << puzzle.getZDim() << std::endl;

    std::cout << std::endl << "# Shape list:" << std::endl;
    for(int si = 0; si < puzzle.getNumShapes(); ++si)
    {
        std::cout << "SHAPE[" << si << "]=";
        std::cout << *puzzle.getShapes()[si] << std::endl;
    }

    std::cout << std::endl <<
        "# One sided polyomino constraint:" << std::endl <<
        "ONE_SIDE=" << (puzzle.getOneSide() ? "ON" : "OFF") << std::endl;

    std::cout << std::endl <<
        "# Redundancy filter piece name:" << std::endl <<
        "REDUNDANCY_FILTER=" << (redundancyFilterIndex == REDUNDANCY_FILTER_OFF_INDEX ? std::string("OFF") : puzzle.getShapes()[redundancyFilterIndex]->getPiece()->getName()) << std::endl;

    std::cout << std::endl <<
        "# Piece rotations and translations that fit in the solution space:" << std::endl <<
        "BOUNDED=" << puzzle.getBoundedImageCount() << std::endl;

    std::cout << std::endl <<
        "# Piece rotations and translations that don't fit in the solution space:" << std::endl <<
        "UNBOUNDED=" << puzzle.getUnboundedImageCount() << std::endl;

    std::cout << std::endl <<
        "# Are solutions guaranteed to be rotationally unique without filtering?" << std::endl <<
        "ROTATIONALLY_UNIQUE=" << (puzzle.getRedundancy() ? "false" : "true") << std::endl;
}


void Solver::solve()
{
    if(algoMeter == NULL)
        algoMeter = PolyPerf::getInstance()->getMeter("solve-algo", "solve");
    PerformanceMeasurement measure(algoMeter);

    if(parityBacktrack && !puzzle.checkParity())
    {
        ++parityBacktrackCount[numMobilePieces];
        return;
    }

    if(numMobilePieces >= volumeBacktrackControl && !puzzle.checkVolume())
    {
        ++volumeBacktrackCount[numMobilePieces];
        return;
    }

    if(monteCarlo.getNumTrials() > 0)
    {
        Random random(monteCarlo.getSeed());
        for(monteCarloTrial = 0; monteCarloTrial < monteCarlo.getNumTrials(); ++monteCarloTrial)
        {
            puzzle.dlxRandomize(random);
            solveOnce();
            endTrialControl = false;
        }
    }
    else
    {
        solveOnce();
    }
}


void Solver::solveOnce()
{
    int numRemainingPieces = puzzle.getNumRemainingPieces();

    int numFilter = 0;
    if(numRemainingPieces >= filterControl)
        numFilter = dlxFilter(numRemainingPieces, 1);

    // Based on the user selected ordering heuristic, find the next column to
    // process (i.e., find the next hole-to-fill, or piece-to-place).
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

    // Only continue if there's at least one row to cover in the selected
    // column.
    //
    if(best->numRow > 0)
    {
        if(puzzle.getNumRemainingPieces() <= mch &&
                puzzle.getNumRemainingGridPoints() <= sizeof(puzzlemask_t)*8)
        {
            solveTile();
        }
        else
        {
            solveDlx_outter(best, numMobilePieces);
        }
    }

    if(numRemainingPieces >= filterControl)
        dlxUnfilter(numFilter);
}


int Solver::dlxFilter(int numRemainingPieces, int parityOfLastPiecePlaced)
{
    if(filterMeter == NULL)
        filterMeter = PolyPerf::getInstance()->getMeter("solve-filter", "solve-algo");
    PerformanceMeasurement measure(filterMeter);

    int numFilter = 0;

    // Do volume and parity pruning first as these can filter images required
    // for the fit checks below to pass (resulting in more fit pruning).  This
    // is probably rare, but I can definitely construct cases where this
    // happens.

    // It's pointless to look for rows that can be filterd due to parity
    // constraint violations if the new parity is equal to a parity that
    // has already been filtered.
    //
    if(numRemainingPieces >= parityFilterControl && parityOfLastPiecePlaced)
    {
        int np = puzzle.dlxFilterParity();
        numFilter += np;
        parityFilterCount[numRemainingPieces] += np;
    }

    if(numRemainingPieces >= volumeFilterControl)
    {
        int np = puzzle.dlxFilterVolume();
        numFilter += np;
        volumeFilterCount[numRemainingPieces] += np;
    }

    if(numRemainingPieces >= fitFilterControl)
    {
        int np = puzzle.dlxFilterFit();
        numFilter += np;
        fitFilterCount[numRemainingPieces] += np;
    }

    return numFilter;
}


void Solver::dlxUnfilter(int numUnfilter)
{
    for(int i = 0; i < numUnfilter; ++i)
        puzzle.dlxUnfilterRow();
}


void Solver::solveDlx_outter(DlxHead* target, int numRemainingPieces)
{
#ifndef NOSIGNALS
    if(sigRequest)
        processSigRequest();
#endif

    --numRemainingPieces;

    // Only cover the target column if the number-of-copies count for that
    // column hits zero.
    //
    if(--target->count == 0)
        puzzle.dlxCover(target);

    for(DlxNode* r = target->down; r != target && !endTrialControl; r = r->down)
    {
        puzzle.placeStack(r->image);

        ++attemptsList[numRemainingPieces];
        ++fitsList[numRemainingPieces];

        if(trace && (trace > 0 ? numRemainingPieces >= trace-1 : numRemainingPieces == -trace-1))
            showTrace();

        if(numRemainingPieces == goal)
            processSolution();
        else
            solveDlx_parityBacktrack(r, numRemainingPieces);

        puzzle.unplaceStack();

        if(trace > 0 && numRemainingPieces >= trace-1)
            showTrace();

        if(monteCarlo.getRange() == numRemainingPieces + 1)
            endTrialControl = true;
    }

    if(target->count++ == 0)
        puzzle.dlxUncover(target);
}


void Solver::solveTile()
{
    int numRemainingShapes = puzzle.initTile(remainingShapeList, remainingShapeCount);
    int numRemainingPieces = puzzle.getNumRemainingPieces();

    if(numRemainingPieces <= bruijn)
    {
        solveBruijn_outter((GridPoint*) puzzle.getDlxRoot()->right, numRemainingShapes, numRemainingPieces);
    }
    else
    {
        DlxHead* min = (DlxHead*) puzzle.getDlxRoot()->right;
        for(DlxHead* h = (DlxHead*) min->right; h->isGridPoint(); h = (DlxHead*) h->right)
            if(min->numRow > h->numRow)
                min = h;
        solveMch_outter((GridPoint*) min, numRemainingShapes, numRemainingPieces);
    }

    puzzle.cleanupTile(remainingShapeList, numRemainingShapes);
}


void Solver::solveMch_outter(GridPoint* target, int numRemainingShapes, int numRemainingPieces)
{
    if(target == NULL)
        return;

#ifndef NOSIGNALS
    if(sigRequest)
        processSigRequest();
#endif

    --numRemainingPieces;

    register puzzlemask_t occupancyState = puzzle.getOccupancyState();
    for(int si = 0; si < numRemainingShapes; ++si)
    {
        int pi = remainingShapeList[si];

        if(!--remainingShapeCount[pi])
            std::swap(remainingShapeList[si], remainingShapeList[--numRemainingShapes]);

        std::vector<Image*>& imageList = target->mchImageList[pi];
        attemptsList[numRemainingPieces] += imageList.size();

        std::vector<Image*>::iterator end = imageList.end();
        for(std::vector<Image*>::iterator ii = imageList.begin(); ii != end; ++ii)
        {
            Image* i = *ii;

            if(occupancyState & i->layoutMask)
                continue;

            puzzle.placeOccupancy(i);
            puzzle.placeStack(i);

            ++fitsList[numRemainingPieces];

            if(trace && (trace > 0 ? numRemainingPieces >= trace-1 : numRemainingPieces == -trace-1))
                showTrace();

            if(numRemainingPieces == goal)
                processSolution();
            else
                solveMch_parityBacktrack(i, numRemainingShapes, numRemainingPieces);

            puzzle.unplaceStack();
            puzzle.unplaceOccupancy(i);

            if(trace > 0 && numRemainingPieces >= trace-1)
                showTrace();
        }

        if(!remainingShapeCount[pi]++)
            std::swap(remainingShapeList[si], remainingShapeList[numRemainingShapes++]);
    }
}


void Solver::solveBruijn_outter(GridPoint* target, int numRemainingShapes, int numRemainingPieces)
{
    --numRemainingPieces;
    register puzzlemask_t occupancyState = puzzle.getOccupancyState();
    for(int si = 0; si < numRemainingShapes; ++si)
    {
        int pi = remainingShapeList[si];
        if(!--remainingShapeCount[pi])
            std::swap(remainingShapeList[si], remainingShapeList[--numRemainingShapes]);
        std::vector<Image*>& imageList = target->bruijnImageList[pi];
        attemptsList[numRemainingPieces] += imageList.size();
        std::vector<Image*>::iterator end = imageList.end();
        for(std::vector<Image*>::iterator ii = imageList.begin(); ii != end; ++ii)
        {
            Image* i = *ii;

            if(occupancyState & i->layoutMask)
                continue;

            puzzle.placeOccupancy(i);
            puzzle.placeStack(i);

            ++fitsList[numRemainingPieces];

            if(trace && (trace > 0 ? numRemainingPieces >= trace-1 : numRemainingPieces == -trace-1))
                showTrace();

            if(numRemainingPieces == goal)
                processSolution();
            else
                solveBruijn_parityBacktrack(target, i, numRemainingShapes, numRemainingPieces);

            puzzle.unplaceStack();
            puzzle.unplaceOccupancy(i);

            if(trace > 0 && numRemainingPieces >= trace-1)
                showTrace();
        }

        if(!remainingShapeCount[pi]++)
            std::swap(remainingShapeList[si], remainingShapeList[numRemainingShapes++]);
    }
}


void Solver::processSolution()
{
    ++solutionCount;
    if(!uniqueControl || filterSolution())
    {
        if(!quiet)
        {
            std::cout << std::endl << "# --- SOLUTION " << solutionCount - redundantCount << " ---" << std::endl;
            puzzle.printState(std::cout, outputFormat);
        }
    }
}


bool Solver::filterSolution()
{
    std::vector<PIECEID_T> solution;
    puzzle.getNormalizedState(solution);
    if(allSolutionList.find(solution) != allSolutionList.end())
    {
        ++redundantCount;
        return false;
    }

    std::vector<PIECEID_T> rotatedSolution;
    const std::vector<std::vector<int> >& symmetricPermutationList = puzzle.getSymmetricPermutationList();
    for(std::vector<std::vector<int> >::const_iterator ri = symmetricPermutationList.begin(); ri != symmetricPermutationList.end(); ++ri)
    {
        puzzle.rotateAndNormalizeState(solution, *ri, rotatedSolution);
        allSolutionList.insert(rotatedSolution);
    }

    return true;
}


void Solver::showTrace()
{
    std::cout << "TRACE " << ++traceCount << ":" << std::endl;
    puzzle.printState(std::cout, outputFormat);
    std::cout << "END TRACE" << std::endl;
}


void Solver::dumpStats()
{
    double secs = (double) PolyPerf::getInstance()->getMeter("all")->getTime() / 1e6;

    std::cout <<
        std::endl << "# Number of solutions found:" << std::endl <<
        "SOLUTIONS=" << solutionCount << std::endl;

    std::cout <<
        std::endl << "# Number of solutions found per second:" << std::endl <<
        "SOLUTIONS_PER_SEC=";
    if(secs > 0)
        std::cout << std::setprecision(9) << solutionCount / secs;
    else
        std::cout << "*";
    std::cout << std::endl;

    if(!puzzle.getRedundancy() || unique)
    {
        std::cout <<
            std::endl << "# Number of rotationally unique solutions found:" << std::endl <<
            "UNIQUE=" << (solutionCount - redundantCount) << std::endl;

        std::cout <<
            std::endl << "# Number of rotationally unique solutions found per second:" << std::endl <<
            "UNIQUE_PER_SEC=";
        if(secs > 0)
            std::cout << std::setprecision(9) << (solutionCount - redundantCount) / secs;
        else
            std::cout << "*";
        std::cout << std::endl;
        
        std::cout <<
            std::endl << "# Number of rotationally redundant solutions found but filtered out:" << std::endl <<
            "REDUNDANT=" << redundantCount << std::endl;
    }
    else
    {
        std::cout <<
            std::endl << "# Number of rotationally unique solutions found:" << std::endl <<
            "UNIQUE=" << "UNKNOWN" << std::endl;
    }

    if(monteCarlo.getNumTrials() > 0)
    {
        std::cout <<
            std::endl << "# Number of Monte-Carlo trials completed:" << std::endl <<
            "MONTE_CARLO_TRIAL=" << monteCarloTrial << std::endl;
    }

    long long sum = 0;
    for(int ci = 0; ci <= numMobilePieces; ++ci)
        sum += parityBacktrackCount[ci];

    std::cout <<
        std::endl << "# Number of backtracks due to parity constraint:" << std::endl <<
        "PARITY_BACKTRACK_TOTAL=" << sum << std::endl;

    sum = 0;
    for(int ci = 0; ci <= numMobilePieces; ++ci)
        sum += volumeBacktrackCount[ci];

    std::cout <<
        std::endl << "# Number of backtracks due to volume constraint:" << std::endl <<
        "VOLUME_BACKTRACK_TOTAL=" << sum << std::endl;

    sum = 0;
    for(int ci = 0; ci <= numMobilePieces; ++ci)
        sum += fitFilterCount[ci];

    std::cout <<
        std::endl << "# Number of rows filtered due to fit constraint:" << std::endl <<
        "FIT_FILTER_TOTAL=" << sum << std::endl;

    sum = 0;
    for(int ci = 0; ci <= numMobilePieces; ++ci)
        sum += parityFilterCount[ci];

    std::cout <<
        std::endl << "# Number of rows filtered due to parity constraint:" << std::endl <<
        "PARITY_FILTER_TOTAL=" << sum << std::endl;

    sum = 0;
    for(int ci = 0; ci <= numMobilePieces; ++ci)
        sum += volumeFilterCount[ci];

    std::cout <<
        std::endl << "# Number of rows filtered due to volume constraint:" << std::endl <<
        "VOLUME_FILTER_TOTAL=" << sum << std::endl;

    sum = 0;
    for(int ci = 0; ci < numMobilePieces; ++ci)
        sum += attemptsList[ci];

    std::cout << std::endl << "# Number of times pieces were attempted to be placed:" << std::endl <<
        "ATTEMPTS_TOTAL=" << sum << std::endl;

    sum = 0;
    for(int ci = 0; ci < numMobilePieces; ++ci)
        sum += fitsList[ci];

    std::cout << std::endl << "# Number of times pieces fit:" << std::endl <<
        "FITS_TOTAL=" << sum << std::endl;

    std::cout << std::endl << "# Number of backtracks due to parity constraint when N pieces were left to be placed:" << std::endl;
    for(int ci = 1; ci <= numMobilePieces; ++ci)
        std::cout << "PARITY_BACKTRACK[" << std::setw(2) << ci << "]=" <<
            std::setw(14) << parityBacktrackCount[ci] << std::endl;

    std::cout << std::endl << "# Number of backtracks due to volume constraint when N pieces were left to be placed:" << std::endl;
    for(int ci = 1; ci <= numMobilePieces; ++ci)
        std::cout << "VOLUME_BACKTRACK[" << std::setw(2) << ci << "]=" <<
            std::setw(14) << volumeBacktrackCount[ci] << std::endl;

    std::cout << std::endl << "# Number of filters due to parity constraint when N pieces were left to be placed:" << std::endl;
    for(int ci = 1; ci <= numMobilePieces; ++ci)
        std::cout << "PARITY_FILTER[" << std::setw(2) << ci << "]=" <<
            std::setw(14) << parityFilterCount[ci] << std::endl;

    std::cout << std::endl << "# Number of filters due to fit constraint when N pieces were left to be placed:" << std::endl;
    for(int ci = 1; ci <= numMobilePieces; ++ci)
        std::cout << "FIT_FILTER[" << std::setw(2) << ci << "]=" <<
            std::setw(14) << fitFilterCount[ci] << std::endl;

    std::cout << std::endl << "# Number of filters due to volume constraint when N pieces were left to be placed:" << std::endl;
    for(int ci = 1; ci <= numMobilePieces; ++ci)
        std::cout << "VOLUME_FILTER[" << std::setw(2) << ci << "]=" <<
            std::setw(14) << volumeFilterCount[ci] << std::endl;

    std::cout << std::endl << "# Number of placement attempts when N pieces were left to be placed:" << std::endl;
    for(int ci = 1; ci <= numMobilePieces; ++ci)
        std::cout << "ATTEMPTS[" << std::setw(2) << ci << "]=" <<
            std::setw(14) << attemptsList[ci-1] << std::endl;

    std::cout << std::endl << "# Number of fits when N pieces were left to be placed:" << std::endl;
    for(int ci = 1; ci <= numMobilePieces; ++ci)
        std::cout << "FITS[" << std::setw(2)  << ci << "]=    " <<
            std::setw(14) << fitsList[ci-1] << std::endl;
}

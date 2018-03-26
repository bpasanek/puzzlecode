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

#include "Puzzle.hpp"
#include <iomanip>
#include <stdlib.h>
#include <map>
#include <algorithm>


Puzzle::Puzzle(const PuzzleConfig& puzzleConfig)
    :
        xDim(puzzleConfig.xDim),
        yDim(puzzleConfig.yDim),
        zDim(puzzleConfig.zDim),
        numGridPoints(0),
        numRemainingGridPoints(0),
        gridStore(NULL),
        grid(NULL),
        unoccupiedFill(-1),
        pieceFill(0),
        mchCheckList(NULL),
        dlxNode(NULL),
        occupancyState(0),
        stationaryVolume(0),
        shapeMap(NULL),
        pieceMap(NULL),
        longestPieceName(0),
        numPieces(puzzleConfig.stationary.size() + puzzleConfig.mobile.size()),
        numStationaryPieces(puzzleConfig.stationary.size()),
        numMobilePieces(puzzleConfig.mobile.size()),
        numShapes(0),
        numStationaryShapes(0),
        numMobileShapes(0),
        oneSide(puzzleConfig.oneSide),
        mirrors(true),
        redundancy(true),
        redundancyComplexity(false),
        imageStack(NULL),
        imageStackSize(0),
        puzzleInitMeter(NULL),
        puzzleCleanupMeter(NULL),
        dlxLoadMeter(NULL),
        genImageListsMeter(NULL),
        initTileMeter(NULL)
{
    puzzleInitMeter = PolyPerf::getInstance()->getMeter("puzzle-init", "all");
    PerformanceMeasurement measure(puzzleInitMeter);
    imageStack = makeArray<const Image*>(numPieces);
    calcPieceVolumes(puzzleConfig.stationary, puzzleConfig.mobile);
    initAllowedRotationList();
    verifyBounded(puzzleConfig.stationary);
    shapeMap = initArray<Shape*>(numPieces+1, NULL);
    pieceMap = initArray<NamedPiece*>(numPieces+1, NULL);
    initGrid();
    addShapes(puzzleConfig.stationary, true);
    addShapes(puzzleConfig.mobile, false);
    countShapes();
    initMirrors();
    initImageLists();

    // I used to also initialize the parity and volume monitors here,
    // but for puzzles that have pieces of many different sizes, the
    // volume monitor can blow memory; so now the Solver calls these routines
    // directly and only if parity or volume checks are required.

    loadStationaryImages();
    initNeighbors();
    initSymmetricRotationAndPermutationLists();
    dlxClear();
}


Puzzle::~Puzzle()
{
    puzzleCleanupMeter = PolyPerf::getInstance()->getMeter("puzzle-cleanup", "all");
    PerformanceMeasurement measure(puzzleCleanupMeter);
    dlxClear();
    deleteGrid();
    for(int ci = 1; ci < numPieces; ++ci)
        delete pieceMap[ci];
    deleteArray(pieceMap, 0);
    deleteArray(shapeMap, 0);
    for(int si = 0; si < shape.size(); ++si)
        delete shape[si];
    deleteArray(imageStack, 0);
}


void Puzzle::calcPieceVolumes(
        const std::vector<NamedPiece>& stationary,
        const std::vector<NamedPiece>& mobile)
{
    stationaryVolume = 0;
    for(int ci = 0; ci < stationary.size(); ++ci)
        stationaryVolume += stationary[ci].size();

    int mobileVolume = 0;
    for(int ci = 0; ci < mobile.size(); ++ci)
        mobileVolume += mobile[ci].size();

    int totalPieceVolume = stationaryVolume + mobileVolume;

    int puzzleVolume = xDim*yDim*zDim;

    if(puzzleVolume != totalPieceVolume)
    {
        std::ostringstream errMsg;
        errMsg << "***Puzzle Size Error:\n\n"
            "The combined size of all pieces is " <<
            totalPieceVolume << ", but the size of the\n"
            "puzzle is " << puzzleVolume << ".  These two "
            "numbers must be the same.";
        throw std::runtime_error(errMsg.str());
    }
}


void Puzzle::initAllowedRotationList()
{
    if(oneSide)
    {
        const Rotation& z90 = Rotation::z90();
        allowedRotationList.push_back(Rotation::I());
        allowedRotationList.push_back(z90);
        allowedRotationList.push_back(z90.rotate(z90));
        allowedRotationList.push_back(z90.rotate(z90).rotate(z90));
    }
    else
    {
        allowedRotationList = Rotation::getRotationList();
    }
}


void Puzzle::verifyBounded(const std::vector<NamedPiece>& stationary) const
{
    for(int ci = 0; ci < stationary.size(); ++ci)
    {
        const NamedPiece& c = stationary[ci];
        if(!isBounded(c))
        {
            std::ostringstream errMsg;
            errMsg << "***Piece Layout Error:\n\n"
                "Stationary piece " << c.getName() << " falls outside the dimensions of the Puzzle grid space:\n\n" << c;
            throw std::runtime_error(errMsg.str());
        }
    }
}


void Puzzle::initGrid()
{
    numGridPoints = xDim*yDim*zDim;
    numRemainingGridPoints = numGridPoints;
    grid = makeArray<GridPoint>(xDim, yDim, zDim);
    gridStore = &grid[0][0][0];
    mchCheckList = makeArray<GridPoint*>(numGridPoints);

    // Set the id field for each GridPoint.
    //
    for(int gi = 0; gi < numGridPoints; ++gi)
        gridStore[gi].id = gi;

    // Set the point field for each GridPoint.
    //
    for(int x = 0; x < xDim; ++x)
        for(int y = 0; y < yDim; ++y)
            for(int z = 0; z < zDim; ++z)
                grid[x][y][z].set(x, y, z);;
}


void Puzzle::addShapes(const std::vector<NamedPiece>& pieceList, bool stationary)
{
    for(int ci = 0; ci < pieceList.size(); ++ci)
    {
        // Make a copy of the piece:  I don't want to depend on the user
        // keeping their copy around while the puzzle is being solved.
        //
        NamedPiece* c = new NamedPiece(pieceList[ci]);
        pieceMap[c->getId()] = c;
        longestPieceName = std::max<int>(longestPieceName, c->getName().size());
        Shape* s = findShape(*c);
        if(s == NULL)
        {
            shape.push_back(new Shape(*c, shape.size(), allowedRotationList));
            s = (*shape.rbegin());
        }

        shapeMap[c->getId()] = s;
        s->addPiece(c, stationary, grid);
    }
}


Shape* Puzzle::findShape(const Piece& c)
{
    for(int si = 0; si < shape.size(); ++si)
    {
        Shape& s = *shape[si];
        for(int rsi = 0; rsi < s.rotatedPieceList.size(); ++rsi)
        {
            Piece& rs = s.rotatedPieceList[rsi];
            if(rs.isCongruent(c))
                return &s;
        }
    }
    return NULL;
}


void Puzzle::countShapes()
{
    numShapes = shape.size();
    numMobileShapes = 0;
    numStationaryShapes = 0;

    for(int si = 0; si < shape.size(); ++si)
    {
        if(shape[si]->getNumStationaryPieces() > 0)
            ++numStationaryShapes;

        if(shape[si]->getNumMobilePieces() > 0)
            ++numMobileShapes;
    }
}


void Puzzle::initMirrors()
{
    mirrors = true;
    if(oneSide)
    {
        for(int si = 0; si < shape.size(); ++si)
        {
            Shape* s = shape[si];
            if(s->mirrorId != -1)
                continue;

            Shape* m = findMirror(*s);
            if(m != NULL)
            {
                m->mirrorId = s->id;
                s->mirrorId = m->id;
            }

            if(m == NULL || s->getNumCopies() != m->getNumCopies())
                mirrors = false;
        }
    }
}


Shape* Puzzle::findMirror(const Piece& c)
{
    const Rotation& x180 = Rotation::x90().rotate(Rotation::x90());
    Piece m = c.rotate(x180);
    for(int si = 0; si < shape.size(); ++si)
    {
        Shape& s = *shape[si];
        for(int rsi = 0; rsi < s.rotatedPieceList.size(); ++rsi)
        {
            Piece& rs = s.rotatedPieceList[rsi];
            if(rs.isCongruent(m))
                return &s;
        }
    }
    return NULL;
}


void Puzzle::initImageLists()
{
    // Allocate the bruijn and mch image lists.
    //
    for(int gi = 0; gi < numGridPoints; ++gi)
    {
        GridPoint& gp = gridStore[gi];
        gp.bruijnImageList = makeArray<std::vector<Image*> >(numShapes);
        gp.mchImageList = makeArray<std::vector<Image*> >(numShapes);
    }
}


void Puzzle::initParityMonitor()
{
    // If several solvers take a crack at the same puzzle, then
    // this method could be called multiple times; but we only
    // want to initialize the Parity Montior once.
    //
    // Note:  the technique of puzzle reuse is entirely untested.
    //
    if(parityMonitor.isInitialized())
        return;

    // Calculate and set the target parity in the ParityMonitor.
    //
    int targetParity = 0;
    for(int gi = 0; gi < numGridPoints; ++gi)
    {
        GridPoint* g = gridStore + gi;
        targetParity += g->getParity();
    }
    parityMonitor.setTargetParity(targetParity);

    // Load the parity of each puzzle piece into the ParityMonitor.
    //
    for(int si = 0; si < shape.size(); ++si)
        for(int sj = 0; sj < shape[si]->getNumCopies(); ++sj)
            parityMonitor.addElement(shape[si]->getParity());

    // Initialize the ParityMonitor.
    //
    parityMonitor.init();

    // Declare to the parity monitor the list of stationary images already
    // used.
    //
    for(int ii = 0; ii < imageStackSize; ++ii)
        placeParity(imageStack[ii]->parity);
}


void Puzzle::initVolumeMonitor()
{
    // If several solvers take a crack at the same puzzle, then
    // this method could be called multiple times; but we only
    // want to initialize the Volume Montior once.
    //
    // Note:  the technique of puzzle reuse is entirely untested.
    //
    if(volumeMonitor.isInitialized())
        return;

    for(int si = 0; si < shape.size(); ++si)
        for(int ci = 0; ci < shape[si]->getNumCopies(); ++ci)
            volumeMonitor.addElement(shape[si]->size());
    volumeMonitor.init();

    // Declare to the volume monitor the list of stationary images already
    // used.  Can't call placeVolume since the fill for these pieces has
    // already been performed.  Sort of annoying since I'm now calling
    // volumeMonitor.place() from two places.
    //
    for(int ii = 0; ii < imageStackSize; ++ii)
        volumeMonitor.place(imageStack[ii]->layout.size());
}


void Puzzle::loadStationaryImages()
{
    for(int si = 0; si < shape.size(); ++si)
    {
        Shape& s = *shape[si];
        for(int ii = 0; ii < s.stationaryImageList.size(); ++ii)
        {
            Image* image = s.stationaryImageList[ii];
            GridPoint* conflict = fits(image);
            if(conflict)
            {
                std::vector<PIECEID_T> state;
                getState(state);
                const std::string& conflictName = nameAt(state, *conflict);

                std::ostringstream errMsg;
                errMsg << "***Piece Layout Error:\n\n"
                    "Stationary piece " << s.getPiece(ii)->getName() << " overlaps with with stationary piece " << conflictName << " at point (";
                conflict->Point::print(errMsg) << ")";
                throw std::runtime_error(errMsg.str());
            }

            // I used to unconditionally make calls to placeVolume and
            // placeParity here as well, but since parity and volume
            // initialization is now delayed I had to move the task of loading
            // the parity and volumes of stationary images into the monitor
            // initialization routines themselves.  In this way,
            // initialization of the montiors can be avoided altogether if
            // they are not to be used.
            //
            // I do, however, still need to do a fill for stationary pieces as
            // this information is used to detect collisions during image
            // generation.  Sort of annoying since I'm now calling fill
            // from two places.
            //
            fill(image, pieceFill++);

            placeGridPointCount(image);
            placeStack         (image);
        }
    }
}


void Puzzle::initNeighbors()
{
    // Set up the neighbor lists.
    //
    for(int gi = 0; gi < numGridPoints; ++gi)
    {
        GridPoint* gp = gridStore + gi;
        if(gp->fill >= 0)
            continue;

        int x = gp->getX();
        int y = gp->getY();
        int z = gp->getZ();
        checkSetNeighbor(gp, x+1, y, z);
        checkSetNeighbor(gp, x-1, y, z);
        checkSetNeighbor(gp, x, y+1, z);
        checkSetNeighbor(gp, x, y-1, z);
        checkSetNeighbor(gp, x, y, z+1);
        checkSetNeighbor(gp, x, y, z-1);
    }
}


void Puzzle::initSymmetricRotationAndPermutationLists()
{
    Point mag(xDim, yDim, zDim);

    const std::vector<Rotation>& candidateRotationList =
        mirrors ? Rotation::getRotationList() : allowedRotationList;

    // permutation[i] will give the id of the GridPoint that
    // gets rotated to array position i of the rotated gridStore.
    // So, for example, if GridPoint 3 after rotation appears in
    // position 7 of the gridStore, then permutation[7] = 3.
    //
    std::vector<int> permutation(numGridPoints);
    std::vector<PIECEID_T> state;
    std::vector<PIECEID_T> rotatedState;
    for(int ri = 0; ri < candidateRotationList.size(); ++ri)
    {
        const Rotation& r = candidateRotationList[ri];
        Point rMag = mag.rotate(r);
        if(std::abs(rMag.getX()) != xDim)
            continue;
        if(std::abs(rMag.getY()) != yDim)
            continue;
        if(std::abs(rMag.getZ()) != zDim)
            continue;

        //
        // At this point we know we at least have a rotation that results in
        // the puzzle cuboid having the same x, y and z dimensions.
        //

        // Normally the puzzle occupies the first octant of a 3-dimensional
        // rectilinear coordinate system, but after rotation points previously
        // in this octant can move to other octants.  Define tp to be a
        // translation vector to move the rotated puzzle back to the first
        // octant.
        //
        Point tp(
                rMag.getX() < 0 ? xDim-1 : 0,
                rMag.getY() < 0 ? yDim-1 : 0,
                rMag.getZ() < 0 ? zDim-1 : 0);

        // Generate the permutation vector for the candidate rotation.
        //
        for(int x = 0; x < xDim; ++x)
        {
            for(int y = 0; y < yDim; ++y)
            {
                for(int z = 0; z < zDim; ++z)
                {
                    Point p(x, y, z);
                    Point rp = p.rotate(r).translate(tp);
                    int rx = rp.getX();
                    int ry = rp.getY();
                    int rz = rp.getZ();

                    permutation[grid[rx][ry][rz].id] = grid[x][y][z].id;
                }
            }
        }

        getState(state);
        rotateState(state, permutation, rotatedState);
        if(isSymmetric(state, rotatedState, r))
        {
            symmetricRotationList.push_back(r);
            symmetricPermutationList.push_back(permutation);
        }
    }
}


bool Puzzle::isSymmetric(const std::vector<PIECEID_T>& state, std::vector<PIECEID_T>& rotatedState, const Rotation& r)
{
    stateMap.assign(numPieces+1, 0);
    bool rc = false;
    for(int i = 0; i < state.size(); ++i)
    {
        // Don't care what happens to GridPoints that were originally
        // unoccupied.
        //
        if(state[i] == 0)
            continue;

        // oldPiece is the id of the piece that was originally occupying
        // GridPoint i, and newPiece is the id of the piece (if any)
        // occupying that same GridPoint after rotation.
        //
        PIECEID_T oldPiece = state[i];
        PIECEID_T newPiece = rotatedState[i];

        // stateMap[oldPiece] is used to keep track of which piece is
        // occupying the GridPoints previously occupied by oldPiece.  All
        // entries in stateMap are initially zero.  Each time I come
        // across a piece in state for the first time, I mark stateMap
        // with the id of the newPiece that is occupying that GridPoint.
        // If later I find other GridPoints previously held by oldPiece
        // are now either empty or are now held by a different piece, then
        // we have an unresolvable conflict.
        //
        if(stateMap[oldPiece] == 0 && newPiece == 0)
        {
            rc = true;

            // In this case, since stateMap is not yet set, we know we're
            // seeing oldPiece for the first time.  Since rotatedState[i] is
            // not set, there's no piece occupying this GridPoint.  There's a
            // chance that all of the GridPoints previously occupied by
            // oldPiece are now empty.  Here I make an attempt to find a
            // mobile piece with the same shape as oldPiece to fill the
            // GridPoints previously occupied by oldPiece.

            // First find the shape we are looking for.
            //
            Shape* s = shapeMap[oldPiece];

            // If we're in a one-sided puzzle we must be careful: we can't
            // just will-nilly take a piece of the same shape as oldPiece:  if
            // we are investigating any rotation other than a z-axis rotation,
            // we must instead use a piece that's a mirror of that shape.
            //
            if(oneSide && r.id() >= 4)  // z-axis rot id's are 0,1,2,3
            {
                // Shouldn't even be looking at a non-z-axis rotation
                // if there's a piece without a mirror.
                //
                assert(s->mirrorId != -1);

                s = shape[s->mirrorId];
            }

            // If we're out of copies of the needed shape, then the situation
            // is hopeless -- we can never achieve symmetry with this
            // rotation.
            //
            if(shapeCount[s->id] == s->getNumCopies())
                return false;

            // Get the next available piece of the appropriate shape and call
            // it spawn.  Assign spawn to the shapeMap for the oldPiece and
            // then search through state for all copies of oldPiece and mark
            // those same locations in rotatedState with spawn.  If we
            // encounter an occupied hole, then we have a conflict and we are
            // done.
            //
            int spawn = stateMap[oldPiece] = s->getPiece(shapeCount[s->id]++)->getId();
            for(int k = i; k < numGridPoints; ++k)
            {
                if(state[k] == oldPiece)
                {
                    if(rotatedState[k] == 0)
                        rotatedState[k] = spawn;
                    else
                        return false;
                }
            }
        }
        else if(stateMap[oldPiece] == 0)
        {
            // Verify that the newPiece has the same shape as the oldPiece.
            // Need to check this to prevent a large piece from being rotated
            // into the position of a smaller piece -- covering it but not
            // matching it.
            //
            Shape* op = shapeMap[oldPiece];
            Shape* np = shapeMap[newPiece];
            if(op->id != np->id && op->mirrorId != np->id)
                return false;
            stateMap[oldPiece] = newPiece;
        }
        else if(newPiece != stateMap[oldPiece])
            return false;
    }
    if(rc)
        redundancyComplexity = true;
    return true;
}


int Puzzle::genImageLists(int redundancyFilterIndex)
{
    if(genImageListsMeter == NULL)
        genImageListsMeter = PolyPerf::getInstance()->getMeter("genImageLists", "solve-init");
    PerformanceMeasurement measure(genImageListsMeter);

    // We'll want initially generate all images without filtering (even if we
    // know which piece we're going to filter) because we want to calculate
    // the image reduction factor to determine if we've succesffully
    // eliminated all redundancies.
    //
    for(int si = 0; si < shape.size(); ++si)
    {
        Shape& s = *shape[si];
        genImageList(s, false);
    }

    // If the space is completely assymetric, then we can clear the redundancy
    // flag now (and skip the filter processing below).
    //
    if(symmetricRotationList.size() == 1)
        redundancy = false;

    // If no redundancy filter was requested, or if there's no redundancy to
    // eliminate, then just return.
    //
    if(redundancyFilterIndex == REDUNDANCY_FILTER_OFF_INDEX || !redundancy)
        return redundancyFilterIndex == REDUNDANCY_FILTER_AUTO_INDEX ? REDUNDANCY_FILTER_OFF_INDEX : redundancyFilterIndex;

    if(redundancyComplexity)
    {
        std::ostringstream errMsg;
        errMsg << "***Rotational Redundancy Filter Processing Error:\n\n"
            "Puzzle geometries are too complex for this software to attempt rotational\n"
            "redundancy filter processing.  Either turn off redundancy filter processing\n"
            "or try eliminating rotational redundancies manually by breaking the puzzle\n"
            "into subproblems with one or more pieces placed in fixed stationary positions\n"
            "in each subcase.\n";
        throw std::runtime_error(errMsg.str());
    }

    // No support for filtering a piece with multiple mobile copies.
    //
    if(redundancyFilterIndex >= 0 && shape[redundancyFilterIndex]->getNumMobilePieces() > 1)
    {
        std::ostringstream errMsg;
        errMsg << "***Redundancy Filter Processing Error:\n\n"
            "No support for filtering a piece with multiple mobile copies.";
        throw std::runtime_error(errMsg.str());
    }

    if(redundancyFilterIndex == REDUNDANCY_FILTER_AUTO_INDEX)
    {
        double bestReductionFactor = 1.0;
        int bestImageCount = 0;
        int bestIndex = REDUNDANCY_FILTER_OFF_INDEX;

        for(int si = 1; si < shape.size(); ++si)
        {
            if(shape[si]->getNumMobilePieces() > 1)
                continue;

            int oldImageCount = shape[si]->mobileImageList.size();
            Shape s = *shape[si];  // Yes, I really mean to copy here.
            genImageList(s, true);
            int newImageCount = s.mobileImageList.size();
            double reductionFactor = (double) oldImageCount / newImageCount;

            if(reductionFactor > bestReductionFactor ||
                    (reductionFactor == bestReductionFactor &&
                    newImageCount < bestImageCount))
            {
                bestReductionFactor = reductionFactor;
                bestImageCount = newImageCount;
                bestIndex = si;
            }
        }

        // If auto selection of rotational redundancy filtering was requested,
        // but no mobile piece has a unique shape, then throw.
        //
        if(bestIndex == REDUNDANCY_FILTER_OFF_INDEX)
        {
            std::ostringstream errMsg;
            errMsg << "***Redundancy Filter Processing Error:\n\n"
                "No mobile piece has a unique shape.";
            throw std::runtime_error(errMsg.str());
        }

        redundancyFilterIndex = bestIndex;
    }

    // If the user requested AUTO, but all pieces had copies, then it's not
    // possible to filter a piece and the above AUTO processing will result
    // in redundancyFilterIndex == REDUNDANCY_FILTER_OFF_INDEX.  In this case, just return.
    //
    if(redundancyFilterIndex == REDUNDANCY_FILTER_OFF_INDEX)
        return redundancyFilterIndex;

    // If we did the AUTO processing, then this is a bit inefficient since
    // we've already generated the filtered image list for this piece once.
    // But this approach simple and clean and performance matters very little
    // in these initialization routines.
    //
    Shape& s = *shape[redundancyFilterIndex];
    int oldImageCount = s.mobileImageList.size();
    s.clearMobileImageList();
    genImageList(s, true);
    int newImageCount = s.mobileImageList.size();
    double reductionFactor = (double) oldImageCount / newImageCount;

    if(reductionFactor == symmetricRotationList.size())
        redundancy = false;

    return redundancyFilterIndex;
}


void Puzzle::genImageList(Shape& piece, bool redundancyFilter)
{
    // An efficient way to eliminate rotational redundancies with the
    // filtered piece is to simply limit the ways the piece can
    // rotate.  This often eliminates all redundancies, but depending
    // on the shape of the puzzle and on the shape of the piece
    // additional image filtering may still be required below.
    //
    if(redundancyFilter)
    {
        int oldSize = piece.rotationList.size();
        piece.pruneRotationList(symmetricRotationList);
        int newSize = piece.rotationList.size();

        // If the rotation reduction factor is equal to the number of
        // symmetric rotations of the puzzle itself, then we are done
        // with filter processing.
        //
        if((double) oldSize / newSize == symmetricRotationList.size())
            redundancyFilter = false;
    }

    std::vector<PIECEID_T> state;
    std::vector<PIECEID_T> rotatedState;
    std::set<std::vector<PIECEID_T>, StateComp> allStates;

    // Generate all the images of each piece that fit in the grid.
    //
    for(int gi = 0; gi < numGridPoints; ++gi)
    {
        GridPoint* gp = gridStore + gi;
        for(int ri = 0; ri < piece.rotatedPieceList.size(); ++ri)
        {
            const Piece& rs = piece.rotatedPieceList[ri];
            Point tp = *gp - rs.getPoint(0);
            Piece ts = rs.translate(tp);
            if(fits(ts))
            {
                Image* image = new Image(piece, ts, grid);
                if(redundancyFilter)
                {
                    state.assign(numGridPoints, 0);
                    for(int gi = 0; gi < image->layout.size(); ++gi)
                    {
                        // Because only one piece is loaded at a time, just
                        // assign a piece id of 1 and skip the call to
                        // normalizeState.
                        //
                        state[image->layout[gi]->id] = (PIECEID_T) 1;
                    }
                    // normalizeState(state, state);

                    if(allStates.find(state) != allStates.end())
                    {
                        delete image;
                        continue;
                    }

                    for(int spi = 0; spi < symmetricPermutationList.size(); ++spi)
                    {
                        const std::vector<int>& permutation = symmetricPermutationList[spi];
                        rotateState(state, permutation, rotatedState);
                        allStates.insert(rotatedState);
                    }
                }
                piece.mobileImageList.push_back(image);
            }
        }
    }
}


GridPoint* Puzzle::fits(const Image* image) const
{
    for(int gi = 0; gi < image->layout.size(); ++gi)
    {
        GridPoint* g = image->layout[gi];
        if(g->fill >= 0)
            return g;
    }
    return NULL;
}


bool Puzzle::fits(const Piece& s) const
{
    if(!isBounded(s))
        return false;
    for(int pi = 0; pi < s.size(); ++pi)
    {
        const Point& p = s.getPoint(pi);
        if(grid[p.getX()][p.getY()][p.getZ()].fill >= 0)
            return false;
    }
    return true;
}


void Puzzle::dlxClear()
{
    dlxRoot.left  = &dlxRoot;
    dlxRoot.right = &dlxRoot;
    deleteArray(dlxNode, 0);
}


void Puzzle::dlxLoad()
{
    if(dlxLoadMeter == NULL)
        dlxLoadMeter = PolyPerf::getInstance()->getMeter("dlxLoad", "solve-init");
    PerformanceMeasurement measure(dlxLoadMeter);

    // Make an initial pass over all the shapes and calculate the number of
    // nodes we're going to need for the DLX matrix.
    //
    int numNode = 0;
    for(int si = 0; si < shape.size(); ++si)
    {
        Shape* s = shape[si];

        // Skip shapes that have only stationary pieces
        //
        if(s->getNumMobilePieces() == 0)
            continue;

        numNode += s->mobileImageList.size() * (s->size() + 1);
    }

    // Allocate all nodes needed for the matrix.
    //
    dlxNode = makeArray<DlxNode>(numNode);

    // Add a column for each GridPoint not already filled  by a stationary
    // piece.
    //
    for(int gi = 0; gi < numGridPoints; ++gi)
    {
        GridPoint* gp = gridStore + gi;

        // Skip GridPoints filled with stationary pieces
        //
        if(gp->fill >= 0)
            continue;

        dlxAddColumn(gp);
    }

    // Now add columns for the shapes that have at least one mobile piece.
    //
    DlxNode* freeNode = dlxNode;
    for(int si = 0; si < shape.size(); ++si)
    {
        Shape* s = shape[si];

        // Skip shapes that have only stationary pieces
        //
        if(s->getNumMobilePieces() == 0)
            continue;

        // Set the count field used by my modified DLX algorithm that tracks
        // the number of copies of each shape that has been used, only
        // covering the column when the last copy of the shape has been
        // placed.
        //
        s->count = s->getNumMobilePieces();

        dlxAddColumn(s);

        // Also load up a row for each mobile image of shape s.
        //
        for(int ii = 0; ii < s->mobileImageList.size(); ++ii)
        {
            dlxAddRow(s->mobileImageList[ii], freeNode);
            freeNode += s->mobileImageList[ii]->getNumDlxColumn();
        }
    }
    dlxVerify();
}


void Puzzle::dlxAddColumn(DlxHead* dh)
{
    dh->left   = dlxRoot.left;
    dh->right  = &dlxRoot;
    dh->up     = dh;
    dh->down   = dh;
    dh->head   = dh;
    dh->image  = NULL;
    dh->numRow = 0;

    dlxRoot.left->right = dh;
    dlxRoot.left = dh;
}


void Puzzle::dlxAddRow(Image* image, DlxNode* freeNode)
{
    DlxNode* first = freeNode;
    DlxNode* prev = NULL;

    for(int i = 0; i < image->getNumDlxColumn(); ++i)
    {
        DlxNode* curr = freeNode++;
        DlxHead* head = image->getDlxColumn(i);

        curr->up       = head->up;
        curr->down     = head;
        head->up->down = curr;
        head->up       = curr;
        head->numRow++;

        if(prev != NULL)
        {
            curr->left     = prev;
            prev->right    = curr;
        }

        curr->head = head;
        curr->image = image;
        prev = curr;
    }

    // Circularly link up the first and last nodes in the row.
    //
    prev->right = first;
    first->left = prev;
}


void Puzzle::dlxRandomize(Random& random)
{
    for(DlxHead* h = (DlxHead*) dlxRoot.right; h != &dlxRoot; h = (DlxHead*) h->right)
    {
        std::map<double, DlxNode*> nodeList;
        for(DlxNode* n = h->down; n != h; n = n->down)
            nodeList[random.gen()] = n;

        h->down = h;
        h->up = h;

        for(std::map<double, DlxNode*>::iterator i = nodeList.begin(); i != nodeList.end(); ++i)
        {
            DlxNode* n = (*i).second;

            n->up       = h->up;
            n->down     = h;
            h->up->down = n;
            h->up       = n;
        }
    }
    dlxVerify();
}


int Puzzle::dlxFilterFit()
{
    int oldFilterListSize = filterList.size();
    for(DlxHead* h = (DlxHead*) dlxRoot.left; !h->isGridPoint(); h = (DlxHead*) h->left)
        for(DlxNode* r = h->down; r != h; r = r->down)
            if(!dlxCheckFit(r))
                dlxFilterRow(r);

    return filterList.size() - oldFilterListSize;
}


bool Puzzle::dlxCheckFit(DlxNode* row)
{
    bool status = true;

    if(--row->head->count == 0)
        dlxCover(row->head);
    for(DlxNode* j = row->right; j != row; j = j->right)
        if(--j->head->count == 0)
            dlxCover(j->head);

    for(DlxHead* h = (DlxHead*) dlxRoot.right; h != &dlxRoot; h = (DlxHead*) h->right)
    {
        if(h->numRow == 0)
        {
            status = false;
            break;
        }
    }

    for(DlxNode* j = row->left; j != row; j = j->left)
        if(j->head->count++ == 0)
            dlxUncover(j->head);
    if(row->head->count++ == 0)
        dlxUncover(row->head);

    return status;
}


int Puzzle::dlxFilterParity()
{
    int oldFilterListSize = filterList.size();
    for(DlxHead* h = (DlxHead*) dlxRoot.left; !h->isGridPoint(); h = (DlxHead*) h->left)
    {
        Shape* s = (Shape*) h;
        int p = s->getParity();
        if(p == 0)
            continue;

        dlxFilterParity(s, p);
        dlxFilterParity(s, -p);
    }
    return filterList.size() - oldFilterListSize;
}


void Puzzle::dlxFilterParity(Shape* s, int p)
{
    if(!parityMonitor.placeCheckParity(p))
        for(DlxNode* node = s->down; node != s; node = node->down)
            if(node->image->parity == p)
                dlxFilterRow(node);
}


int Puzzle::dlxFilterVolume()
{
    int oldFilterListSize = filterList.size();
    for(DlxHead* h = (DlxHead*) dlxRoot.left; !h->isGridPoint(); h = (DlxHead*) h->left)
    {
        for(DlxNode* r = h->down; r != h; r = r->down)
        {
            placeVolume(r->image);
            if(!checkVolume())
                dlxFilterRow(r);
            unplaceVolume(r->image);
        }
    }
    return filterList.size() - oldFilterListSize;
}


void Puzzle::dlxVerify()
{
    for(DlxHead* h = (DlxHead*) dlxRoot.right; h != &dlxRoot; h = (DlxHead*) h->right)
    {
        assert(h->left->right == h);
        assert(h->right->left == h);
        dlxVerifyColumn(h);
    }
}

void Puzzle::dlxVerifyColumn(DlxHead* head)
{
    int numRow = 0;
    for(DlxNode* n = head->down; n != head; n = n->down)
    {
        assert(n->up->down == n);
        assert(n->down->up == n);
        assert(n->left->right == n);
        assert(n->right->left == n);
        dlxVerifyRow(n);
        ++numRow;
    }
    assert(head->numRow == numRow);
}

void Puzzle::dlxVerifyRow(DlxNode* node)
{
    for(DlxNode* n = node->right; n != node; n = n->right)
    {
        assert(n->up->down == n);
        assert(n->down->up == n);
        assert(n->left->right == n);
        assert(n->right->left == n);
        assert(n->image == node->image);
    }
}


int Puzzle::initTile(int* remainingShapeList, int* remainingShapeCount)
{
    if(initTileMeter == NULL)
        initTileMeter = PolyPerf::getInstance()->getMeter("initTile", "solve-algo");

    PerformanceMeasurement measure(initTileMeter);

    int numShapes = loadRemainingShapesList(remainingShapeList, remainingShapeCount);

    occupancyState = 0;

    // Iterate over all remaining GridPoints and assign each a bit.
    //
    puzzlemask_t bit = 1;
    for(DlxHead* head = (DlxHead*) dlxRoot.right;
            head->isGridPoint(); head = (DlxHead*) head->right)
    {
        ((GridPoint*) head)->bit = bit;
        bit <<= 1;
    }

    // For each remaining shape...
    //
    for(DlxHead* head = (DlxHead*) dlxRoot.left;
            !head->isGridPoint(); head = (DlxHead*) head->left)
    {
        // ...for each image of that shape...
        //
        for(register DlxNode* node = head->down; node != head; node = node->down)
        {
            Image* image = node->image;

            // Get the FIRST Gridpoint in that shape.  This is always the de
            // Bruijn tiling node.
            //
            register DlxNode* n = node->right;
            register GridPoint* gp = (GridPoint*) n->head;

            // Initialize the image's layout mask with the bit of it's first
            // GridPoint.
            //
            image->layoutMask = gp->bit;

            // Add the image to both the bruijn and mch image lists.
            //
            gp->bruijnImageList[image->shape.id].push_back(image);
            gp->mchImageList[image->shape.id].push_back(image);

            // Now iterate over the remaining GridPoints for that image.
            //
            for(n = n->right; n != node; n = n->right)
            {
                gp = (GridPoint*) n->head;

                // Or in the bit of the current GridPoint to the layoutMask.
                //
                image->layoutMask |= gp->bit;

                // Add the image only to the mch image list at this GridPoint.
                //
                gp->mchImageList[image->shape.id].push_back(image);
            }
        }
    }
    return numShapes;
}


int Puzzle::loadRemainingShapesList(int* remainingShapeList, int* remainingShapeCount)
{
    int numRemainingShapes = 0;
    for(DlxHead* head = (DlxHead*) dlxRoot.left;
            !head->isGridPoint(); head = (DlxHead*) head->left)
    {
        int id = ((Shape*) head)->id;
        *remainingShapeList++ = id;
        remainingShapeCount[id] = head->count;
        ++numRemainingShapes;
    }
    return numRemainingShapes;
}


void Puzzle::cleanupTile(int* remainingShapeList, int numRemainingShapes)
{
    for(DlxHead* head = (DlxHead*) dlxRoot.right;
            head->isGridPoint(); head = (DlxHead*) head->right)
    {
        GridPoint* gp = (GridPoint*) head;
        gp->bit = 0;
        for(int si = 0; si < numRemainingShapes; ++si)
        {
            gp->bruijnImageList[remainingShapeList[si]].clear();
            gp->mchImageList[remainingShapeList[si]].clear();
        }
    }
}


GridPoint* Puzzle::getMostConstrainedHole(
        const int* remainingShapeList,
        int numRemainingShapes,
        bool estimate)
{
    // First load mchCheckList with the set of GridPoints we want to
    // get exact fit counts on.  If estimate is false, then it gets
    // loaded with every unoccupied GridPoint.  If estimate is true,
    // then only those nodes that have the fewest number of
    // neighboring holes are loaded to the list.
    //
    int checkListSize = 0;
    int minNeighborHoles = 7;
    //register puzzlemask_t ostate = occupancyState;
    for(DlxHead* head = (DlxHead*) dlxRoot.right;
            head->isGridPoint(); head = (DlxHead*) head->right)
    {
        GridPoint* gp = (GridPoint*) head;
        if(gp->bit & occupancyState)
            continue;

        if(estimate)
        {
            int numNeighborHoles = 0;
            for(std::vector<GridPoint*>::iterator ni = gp->neighborList.begin();
                    ni != gp->neighborList.end(); ++ni)
            {
                // If nbit is clear, then the neighbor was filled by
                // dlx prior to the enabling of mch.
                //
                puzzlemask_t nbit = (*ni)->bit;
                if(nbit && !(nbit & occupancyState))
                    ++numNeighborHoles;
            }

            if(numNeighborHoles > minNeighborHoles)
                continue;

            if(numNeighborHoles < minNeighborHoles)
            {
                checkListSize = 0;
                minNeighborHoles = numNeighborHoles;
            }
        }
        mchCheckList[checkListSize++] = gp;
    }

    int minFits = 0x7FFFFFFF;
    GridPoint* best = NULL;
    for(int gi = 0; gi < checkListSize; ++gi)
    {
        GridPoint* gp = mchCheckList[gi];
        int numFits = countFits(gp, minFits, remainingShapeList, numRemainingShapes);
        if(numFits < minFits)
        {
            minFits = numFits;
            if(minFits == 0)
            {
                best = NULL;
                break;
            }
            best = gp;
        }
    }
    return best;
}


int Puzzle::countFits(
    GridPoint* gp,
    int minFits,
    const int* remainingShapes,
    int numRemainingShapes) const
{
    //register puzzlemask_t ostate = occupancyState;
    int numFits = 0;
    for(int si = 0; si < numRemainingShapes; ++si)
    {
        std::vector<Image*>& imageList = gp->mchImageList[remainingShapes[si]];
        for(std::vector<Image*>::iterator ii = imageList.begin(); ii != imageList.end(); ++ii)
        {
            if(!((*ii)->layoutMask & occupancyState))
                if(++numFits > minFits)
                    return numFits;
        }
    }
    return numFits;
}


int Puzzle::getUnboundedImageCount() const
{
    int ic = 0;
    for(int si = 0; si < shape.size(); ++si)
        ic += shape[si]->rotationList.size() * (numGridPoints - stationaryVolume);
    return ic - getBoundedImageCount();
}


int Puzzle::getBoundedImageCount() const
{
    int bic = 0;
    for(int si = 0; si < shape.size(); ++si)
        bic += shape[si]->stationaryImageList.size() + shape[si]->mobileImageList.size();
    return bic;
}


bool Puzzle::checkVolume()
{
    // The statement below is a simple trick to get unoccupiedFill to
    // alternately take on values -1 and -2.
    //
    int newFill = -3 - unoccupiedFill;

    // Examine each grid point.  If it's fill data member is in the
    // unoccupied state, then find the volume of that sub-space,
    //
    bool status = true;
    GridPoint* gpend = gridStore + numGridPoints;
    for(GridPoint* gp = gridStore; gp != gpend; ++gp)
        if(gp->fill == unoccupiedFill && !volumeMonitor.checkVolume(fillVolume(gp, newFill)))
            status = false;
    unoccupiedFill = newFill;
    return status;
}


void Puzzle::getState(std::vector<PIECEID_T>& state) const
{
    state.assign(numGridPoints, 0);
    shapeCount.assign(shape.size(), 0);
    for(int ii = 0; ii < imageStackSize; ++ii)
    {
        const Image* i = imageStack[ii];
        const Shape& s = i->shape;
        i->piece = s.getPiece(shapeCount[s.id]);
        for(int gi = 0; gi < i->layout.size(); ++gi)
            state[i->layout[gi]->id] = i->piece->getId();
        ++shapeCount[s.id];
    }
}


void Puzzle::getNormalizedState(std::vector<PIECEID_T>& state) const
{
    getState(state);
    normalizeState(state, state);
}


void Puzzle::rotateState(const std::vector<PIECEID_T>& state,
        const std::vector<int>& permutation, std::vector<PIECEID_T>& rotatedState) const
{
    rotatedState.clear();
    rotatedState.reserve(state.size());
    for(int si = 0; si < state.size(); ++si)
        rotatedState.push_back(state[permutation[si]]);
}


void Puzzle::rotateAndNormalizeState(const std::vector<PIECEID_T>& state,
        const std::vector<int>& permutation, std::vector<PIECEID_T>& rotatedState) const
{
    rotateState(state, permutation, rotatedState);
    normalizeState(rotatedState, rotatedState);
}


void Puzzle::normalizeState(const std::vector<PIECEID_T>& state, std::vector<PIECEID_T>& normalizedState) const
{
    stateMap.assign(numPieces+1, 0);
    normalizedState.resize(state.size());
    int x = 0;
    for(int i = 0; i < state.size(); ++i)
    {
        if(state[i] == 0)
            continue;

        if(stateMap[state[i]] == 0)
            stateMap[state[i]] = ++x;

        normalizedState[i] = stateMap[state[i]];
    }
}


const std::string& Puzzle::nameAt(const std::vector<PIECEID_T>& state, const Point& p) const
{
    return nameAt(state, p.getX(), p.getY(), p.getZ());
}


const std::string& Puzzle::nameAt(const std::vector<PIECEID_T>& state, int x, int y, int z) const
{
    NamedPiece* piece = pieceMap[state[x*(yDim*zDim) + y*(zDim) + z]];
    if(piece == NULL)
        return OPEN_HOLE_NAME;
    else
        return piece->getName();
}


void Puzzle::dumpFill(std::ostream& os) const
{
    for(int y = yDim - 1; y >= 0; --y)
    {
        for(int z = 0; z < zDim; ++z)
        {
            for(int x = 0; x < xDim; ++x)
            {
                os << std::setw(4) << grid[x][y][z].fill;
            }
            os << "     ";
        }
        os << std::endl;
    }
}


void Puzzle::printState(
        std::ostream& os,
        const OutputFormatConfig& outputFormat) const
{
    std::vector<PIECEID_T> state;
    getState(state);

    PuzzleFormat zf = outputFormat.getPuzzleFormat();
    PieceFormat  cf = outputFormat.getPieceFormat();

    if(zf != BRIEF)
    {
        os << "D:xDim=" << xDim << ":yDim=" << yDim << ":zDim=" << zDim;
        if(oneSide)
            os << ":oneSide";
        os << std::endl;

        for(int si = 0; si < shape.size(); ++si)
        {
            Shape* s = shape[si];
            for(int pi = shapeCount[si]; pi < s->pieceList.size(); ++pi)
            {
                NamedPiece* p = s->pieceList[pi];
                os << "C:name=" << p->getName() << ":type=M:";
                p->printLayout(os);
                os << std::endl;
            }
        }
    }

    int nsp = (zf == SUBPUZZLE) ? imageStackSize : numStationaryPieces;

    if(cf == COORDINATE)
    {
        for(int ii = 0; ii < imageStackSize; ++ii)
        {
            const Image* i = imageStack[ii];
            os << "C:name=" << i->piece->getName() << ":type=" <<
                (ii < nsp ? "S" : "M") << ":";
            i->printLayout(os) << std::endl;
        }
    }
    else
    {
        if(zf != BRIEF)
        {
            os << "L";
            if(nsp > 0)
                os << ":stationary=";
            for(int ii = 0; ii < nsp; ++ii)
            {
                if(ii != 0)
                    os << " ";
                os << imageStack[ii]->piece->getName();
            }
            os << std::endl;
        }

        for(int y = yDim - 1; y >= 0; --y)
        {
            for(int z = 0; z < zDim; ++z)
            {
                if(z > 0)
                    os << ", ";
                for(int x = 0; x < xDim; ++x)
                    os << std::setw(longestPieceName+1) << nameAt(state, x, y, z);
            }
            os << std::endl;
        }

        if(zf != BRIEF)
        {
            os << "~L" << std::endl;
        }
    }

    if(zf != BRIEF)
    {
        os << "~D"<< std::endl;
    }
}


void Puzzle::deleteGrid()
{
    deleteArray(mchCheckList, 0);
    deleteArray(grid, 0, 0, 0);
}

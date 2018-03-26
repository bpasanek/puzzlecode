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

#ifndef PUZZLESOLVERINTERFACE_HPP
#define PUZZLESOLVERINTERFACE_HPP

#include "Puzzle.hpp"

class Solver;

/** This class defines the interface between the Solver and the Puzzle.  The
 ** public interface on Puzzle is intended for calls from the application
 ** (i.e., calls from main) and is quite small, but the Solver requires access
 ** to numerous methods on the Puzzle -- around 50 methods:  everything from
 ** placing and unplacing pieces, to various row pruning calls, to methods to
 ** switch the puzzles active data model from one supporting DLX to one
 ** supporting the MCH/Bruijn algorithms, to methods to dump the Puzzle's
 ** current configuration.  It might make sense to simply declare the Puzzle a
 ** friend of the Solver, but I wanted to codify the exact set of Puzzle
 ** methods the Solver actually uses and make it more clear that the Solver
 ** never directly manipulates Puzzle private data.
 **
 ** Aside from the clarity provided by this interface definition, this class
 ** is otherwise unnecessary fluff, but I don't think it adds any processing
 ** overhead so long as compiler optimization is turned on -- all the inlining
 ** should be compiled away.  Indeed, I don't think there's even an extra
 ** pointer dereference since I keep a PuzzleSolverInterface object as an
 ** actual data member of the Solver.  (So from the compiler's point of view,
 ** the Puzzle pointer should be directly accessible by the Solver's this
 ** pointer.)
 **/
class PuzzleSolverInterface
{
    private:
        Puzzle* puzzle;

        PuzzleSolverInterface(Puzzle* puzzle = NULL);
        void setPuzzle(Puzzle* puzzle);

        int getXDim() const;
        int getYDim() const;
        int getZDim() const;
        int getNumGridPoints() const;
        int getNumShapes() const;
        int getNumStationaryShapes() const;
        int getNumMobileShapes() const;
        int getNumPieces() const;
        int getNumStationaryPieces() const;
        int getNumMobilePieces() const;
        bool getOneSide() const;
        void initParityMonitor();
        void initVolumeMonitor();
        const std::vector<NamedPiece>& getStationaryPieces() const;
        const std::vector<Shape*>& getShapes() const;
        const std::vector<std::vector<int> >& getSymmetricPermutationList() const;
        int genImageLists(int redundancyFilterIndex);
        bool getRedundancy() const;
        DlxHead* getDlxRoot();
        void dlxLoad();
        void dlxRandomize(Random& random);
        int dlxFilterFit();
        int dlxFilterParity();
        int dlxFilterVolume();
        void dlxUnfilterRow();
        void dlxCover(DlxHead* c);
        void dlxUncover(DlxHead* c);
        int initTile(int* remainingShapeList, int* remainingShapeCount);
        void cleanupTile(int* remainingShapeList, int numRemainingShapes);
        GridPoint* getMostConstrainedHole(
                const int* remainingShapeList,
                int numRemainingShapes,
                bool estimate);
        int getUnboundedImageCount() const;
        int getBoundedImageCount() const;
        puzzlemask_t getOccupancyState() const;
        void placeOccupancy(const Image* image);
        void unplaceOccupancy(const Image* image);
        void placeParity(int parity);
        void unplaceParity(int parity);
        void placeVolume(const Image* image);
        void unplaceVolume(const Image* image);
        void placeGridPointCount(const Image* image);
        void unplaceGridPointCount(const Image* image);
        void placeStack(const Image* image);
        void unplaceStack();
        bool checkParity();
        bool checkVolume();
        int getNumRemainingPieces();
        int getNumRemainingGridPoints();
        GridPoint* getNextBruijnHole(DlxHead* lastBruijnHole) const;
        void getNormalizedState(std::vector<PIECEID_T>& state) const;
        void rotateAndNormalizeState(const std::vector<PIECEID_T>& state, const std::vector<int>& permutation, std::vector<PIECEID_T>& rotatedState);
        void printState(
            std::ostream& os,
            const OutputFormatConfig& outputFormat) const;

    friend class Solver;
};


inline PuzzleSolverInterface::PuzzleSolverInterface(Puzzle* puzzle)
    : puzzle(puzzle)
{
}

inline void PuzzleSolverInterface::setPuzzle(Puzzle* puzzle)
{
    this->puzzle = puzzle;
}

inline int PuzzleSolverInterface::getXDim() const
{
    return puzzle->getXDim();
}

inline int PuzzleSolverInterface::getYDim() const
{
    return puzzle->getYDim();
}

inline int PuzzleSolverInterface::getZDim() const
{
    return puzzle->getZDim();
}

inline int PuzzleSolverInterface::getNumGridPoints() const
{
    return puzzle->getNumGridPoints();
}


inline int PuzzleSolverInterface::getNumShapes() const
{
    return puzzle->getNumShapes();
}


inline int PuzzleSolverInterface::getNumStationaryShapes() const
{
    return puzzle->getNumStationaryShapes();
}


inline int PuzzleSolverInterface::getNumMobileShapes() const
{
    return puzzle->getNumMobileShapes();
}


inline int PuzzleSolverInterface::getNumPieces() const
{
    return puzzle->getNumPieces();
}


inline int PuzzleSolverInterface::getNumStationaryPieces() const
{
    return puzzle->getNumStationaryPieces();
}


inline int PuzzleSolverInterface::getNumMobilePieces() const
{
    return puzzle->getNumMobilePieces();
}


inline bool PuzzleSolverInterface::getOneSide() const
{
    return puzzle->getOneSide();
}

inline void PuzzleSolverInterface::initParityMonitor()
{
    puzzle->initParityMonitor();
}

inline void PuzzleSolverInterface::initVolumeMonitor()
{
    puzzle->initVolumeMonitor();
}

inline const std::vector<Shape*>& PuzzleSolverInterface::getShapes() const
{
    return puzzle->getShapes();
}

inline const std::vector<std::vector<int> >& PuzzleSolverInterface::getSymmetricPermutationList() const
{
    return puzzle->getSymmetricPermutationList();
}

inline int PuzzleSolverInterface::genImageLists(int redundancyFilterIndex)
{
    return puzzle->genImageLists(redundancyFilterIndex);
}

inline bool PuzzleSolverInterface::getRedundancy() const
{
    return puzzle->getRedundancy();
}

inline DlxHead* PuzzleSolverInterface::getDlxRoot()
{
    return puzzle->getDlxRoot();
}

inline void PuzzleSolverInterface::dlxLoad()
{
    puzzle->dlxLoad();
}

inline void PuzzleSolverInterface::dlxRandomize(Random& random)
{
    return puzzle->dlxRandomize(random);
}

inline int PuzzleSolverInterface::dlxFilterFit()
{
    return puzzle->dlxFilterFit();
}

inline int PuzzleSolverInterface::dlxFilterParity()
{
    return puzzle->dlxFilterParity();
}

inline int PuzzleSolverInterface::dlxFilterVolume()
{
    return puzzle->dlxFilterVolume();
}

inline void PuzzleSolverInterface::dlxUnfilterRow()
{
    puzzle->dlxUnfilterRow();
}

inline void PuzzleSolverInterface::dlxCover(DlxHead* c)
{
    puzzle->dlxCover(c);
}

inline void PuzzleSolverInterface::dlxUncover(DlxHead* c)
{
    puzzle->dlxUncover(c);
}

inline int PuzzleSolverInterface::initTile(int* remainingShapeList, int* remainingShapeCount)
{
    return puzzle->initTile(remainingShapeList, remainingShapeCount);
}

inline void PuzzleSolverInterface::cleanupTile(int* remainingShapeList, int numRemainingShapes)
{
    puzzle->cleanupTile(remainingShapeList, numRemainingShapes);
}

inline GridPoint* PuzzleSolverInterface::getMostConstrainedHole(
        const int* remainingShapeList,
        int numRemainingShapes,
        bool estimate)
{
    return puzzle->getMostConstrainedHole(remainingShapeList, numRemainingShapes, estimate);
}

inline int PuzzleSolverInterface::getUnboundedImageCount() const
{
    return puzzle->getUnboundedImageCount();
}

inline int PuzzleSolverInterface::getBoundedImageCount() const
{
    return puzzle->getBoundedImageCount();
}

inline puzzlemask_t PuzzleSolverInterface::getOccupancyState() const
{
    return puzzle->getOccupancyState();
}

inline void PuzzleSolverInterface::placeOccupancy(const Image* image)
{
    puzzle->placeOccupancy(image);
}

inline void PuzzleSolverInterface::unplaceOccupancy(const Image* image)
{
    puzzle->unplaceOccupancy(image);
}

inline void PuzzleSolverInterface::placeParity(int parity)
{
    puzzle->placeParity(parity);
}

inline void PuzzleSolverInterface::unplaceParity(int parity)
{
    puzzle->unplaceParity(parity);
}

inline void PuzzleSolverInterface::placeVolume(const Image* image)
{
    puzzle->placeVolume(image);
}

inline void PuzzleSolverInterface::unplaceVolume(const Image* image)
{
    puzzle->unplaceVolume(image);
}

inline void PuzzleSolverInterface::placeGridPointCount(const Image* image)
{
    puzzle->placeGridPointCount(image);
}

inline void PuzzleSolverInterface::unplaceGridPointCount(const Image* image)
{
    puzzle->unplaceGridPointCount(image);
}

inline void PuzzleSolverInterface::placeStack(const Image* image)
{
    puzzle->placeStack(image);
}

inline void PuzzleSolverInterface::unplaceStack()
{
    puzzle->unplaceStack();
}

inline bool PuzzleSolverInterface::checkParity()
{
    return puzzle->checkParity();
}

inline bool PuzzleSolverInterface::checkVolume()
{
    return puzzle->checkVolume();
}

inline int PuzzleSolverInterface::getNumRemainingPieces()
{
    return puzzle->getNumRemainingPieces();
}

inline int PuzzleSolverInterface::getNumRemainingGridPoints()
{
    return puzzle->getNumRemainingGridPoints();
}

inline GridPoint* PuzzleSolverInterface::getNextBruijnHole(DlxHead* lastBruijnHole) const
{
    return puzzle->getNextBruijnHole(lastBruijnHole);
}

inline void PuzzleSolverInterface::getNormalizedState(std::vector<PIECEID_T>& state) const
{
    puzzle->getNormalizedState(state);
}

inline void PuzzleSolverInterface::rotateAndNormalizeState(const std::vector<PIECEID_T>& state, const std::vector<int>& permutation, std::vector<PIECEID_T>& rotatedState)
{
    puzzle->rotateAndNormalizeState(state, permutation, rotatedState);
}

inline void PuzzleSolverInterface::printState(
    std::ostream& os,
    const OutputFormatConfig& outputFormat) const
{
    puzzle->printState(os, outputFormat);
}

#endif

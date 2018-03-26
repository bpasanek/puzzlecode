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

#ifndef PUZZLE_HPP
#define PUZZLE_HPP

#include <string>
#include <vector>
#include <set>
#include <ostream>
#include <sstream>
#include <assert.h>
#include <iomanip>
#include "Constants.hpp"
#include "OutputFormatConfig.hpp"
#include "PuzzleConfig.hpp"
#include "PolyPerf.hpp"
#include "Rotation.hpp"
#include "Random.hpp"
#include "ParityMonitor.hpp"
#include "VolumeMonitor.hpp"
#include "Image.hpp"
#include "DlxHead.hpp"


inline int compareVectors(const std::vector<PIECEID_T>& lhs, const std::vector<PIECEID_T>& rhs)
{
    int minSize = std::min(lhs.size(), rhs.size());
    for(int pi = 0; pi < minSize; ++pi)
    {
        if(lhs.at(pi) < rhs.at(pi))
            return -1;
        if(lhs.at(pi) > rhs.at(pi))
            return 1;
    }
    if(lhs.size() < rhs.size())
        return -1;
    if(lhs.size() > rhs.size())
        return 1;
    return 0;
}

class StateComp
{
    public:
        bool operator() (const std::vector<PIECEID_T>& lhs, const std::vector<PIECEID_T>& rhs) const
        {
            return compareVectors(lhs, rhs) < 0;
        }
};

class PuzzleSolverInterface;

// Some terminolgy:
//   puzzle cuboid -- The three dimensional space having 6 rectangular sides
//                    (cuboid) in which all puzzle pieces (both mobile and
//                    stationary) are to be contained.
//
class Puzzle
{
    private:
        static const puzzlemask_t ONE = 1;

        // The dimensions of the puzzle cuboid.
        //
        int xDim;
        int yDim;
        int zDim;

        /** The number of GridPoints (I.e., The number of holes in the puzzle
         ** after all stationary pieces are placed.)
         **/
        int numGridPoints;

        /** The number of open GridPoints.  This starts out with the same value
         ** as numGridPoints, but is reduced each time a piece is placed by
         ** the size of the piece (and increases by a like amount when that
         ** piece is unplaced).
         **/
        int numRemainingGridPoints;

        /** After stationary pieces are loaded into the puzzle box, an array of
         ** GridPoints is allocated that has a size equal to the number of
         ** remaining holes in the box.  (I.e., I don't allocate GridPoints for
         ** holes that are already filled.)  GridPoints are sorted by x, y, z
         ** priority so that any subset of the gridStore will already be
         ** in the correct tiling order.
         **/
        GridPoint* gridStore;

        /** grid[x][y][z] is a pointer to the GridPoint object at position
         ** (x, y, z) or NULL if no GridPoint exists at that position.  (A
         ** GridPoint is only instantiated for each hole remaining the puzzle
         ** cuboid AFTER all stationary pieces are placed.)
         **/
        GridPoint*** grid;

        /** The current value used to represent an unoccupied space in the
         ** GridPoint fill field.  The value flip-flops between -1 and -2.
         ** See GridPoint::fill for details.
         **/
        int unoccupiedFill;

        /** The fill value used for the next piece placement.  Starts at zero
         ** and increments each time a piece is placed.  The value should
         ** correspond to the index of the piece in the image stack.  I think
         ** I could actually just hardcode this at zero (or any non-negative
         ** value) and everything would work the same, but I think there's
         ** some aesthetic value to having the fill value for a GridPoint
         ** being the image stack index of the piece that's occupying the
         ** point.  During volume filters I never actually place each
         ** prospective piece on the image stack, so I track the pieceFill as
         ** a separate value.
         **/
        int pieceFill;

        /** For the case of estimated MCH, this list is loaded with the
         ** pointers to open Gridpoints that have the fewest open neighbors.
         ** Only the grid points in this list are then checked for exact fit
         ** counts.
         **/
        GridPoint** mchCheckList;

        /** List of rows filtered from the DLX matrix for reasons of parity
         ** violations, volume violations and/or fit violations.
         **/
        std::vector<DlxNode*> filterList;

        /** Donald Knuth's dancing link's data model.
         **/
        DlxHead dlxRoot;

        /** Just a big array of DlxNode objects used to construct the Dlx
         ** sparse matrix.
         **/
        DlxNode* dlxNode;

        /** 64 bit field identifying which GridPoints in the search grid
         ** are occupied.  If there are sufficient bits in this field,
         ** then the maping between bits in this field and GridPoints is
         ** determined up front, but for larger puzzles the mapping
         ** must be redetermined each time the number of remaining
         ** GridPoints falls below the size of this field.  Of course
         ** one could use a variable length bit-field to avoid this
         ** complexity, but at the cost of increased state check times.
         ** But the whole bit-field approach breaks down if the size of this
         ** mask gets too large:  you'd be better off distributing the
         ** occupancy state as a bit in each GridPoint object and instead
         ** iterating over the cells of each prospective image.
         **/
        puzzlemask_t occupancyState;

        /** The total volume of all stationary pieces.
         **/
        puzzlemask_t stationaryVolume;

        /** List of unique shapes.  Both mobile and stationary pieces are
         ** merged to this one list, with pieces having the same shape
         ** collapsing to a single entry in the list.
         **/
        std::vector<Shape*> shape;

        /** An array of pointers to Shapes mapping from the id
         ** of a NamedPiece to it's Shape.
         **/
        Shape** shapeMap;

        /** An array of pointers to heap allocated NamedPieces mapping from
         ** the id of a NamedPiece to the NamedPiece itself.
         **/
        NamedPiece** pieceMap;

        /** The length of the longest piece name -- used for text-graphic output.
         **/
        int longestPieceName;

        /** The total number of pieces (both mobile and stationary) counting
         ** duplicates.
         **/
        int numPieces;

        /** The total number of stationary pieces counting duplicates.
         **/
        int numStationaryPieces;

        /** The total number of mobile pieces counting duplicates.
         **/
        int numMobilePieces;

        /** The total number of unique shapes (both mobile and stationary).
         **/
        int numShapes;

        /** The number of unique shapes among stationary pieces.
         **/
        int numStationaryShapes;

        /** The number of unique shapes among mobile pieces.
         **/
        int numMobileShapes;

        /** Simply the list of allowed rotations for shapes.
         ** Normally this list simply contains all 24 rotations, but if oneSide
         ** is set, this list contains only the 4 possible rotations about
         ** the z axis.
         **/
        std::vector<Rotation> allowedRotationList;

        /** A configuration parameter which if set true constrains the mobile
         ** puzzle pieces to only rotate about the z-axis.  This is useful
         ** for solving one-sided polyomino puzzles, etc.
         **/
        bool oneSide;

        /** False iff the puzzle is one-sided AND at least one shape doesn't
         ** have a mirror or if that mirror has a different number of copies.
         ** This field is used to determine if a solution to a one-sided
         ** polyomino puzzle when flipped up-side-down (and rotated about
         ** the z-axis as necesssary) can possibly produce another solution to
         ** the puzzle.  This information, in-turn, affects the generated
         ** symmetric rotations for the puzzle.  The symmetric rotations of
         ** the puzzle are used to determine what rotations must be performed
         ** on a solution to determine if it is unique.  Note that the
         ** configuration of stationary pieces may also prevent a one-sided
         ** puzzle from being flipped to produce a new solution, so mirrors
         ** being true is a necessary but not sufficient condition for
         ** rotations other than z-axis rotations to be in the
         ** symmetricRotationsList.
         **/
        bool mirrors;

        /** Set to true if there's any possibility of the solver finding
         ** rotationally redundant solutions for this puzzle based on the
         ** image set generated.  Redundancy is initialized with a true value,
         ** but depending on the results of image generation may be set false.
         ** If it is false the Solver disables rotational redundancy checks of
         ** solutions and disables the storage of solution fingerprints.
         ** Since large puzzles can have very large numbers of solutions,
         ** this can be quite important.
         **/
        bool redundancy;

        /** Set to true if the puzzle loaded with only stationary pieces has
         ** at least one rotation that is distinguishable from it's original
         ** form, but for which a subset of the mobile pieces can be placed in
         ** the puzzle in such a way that that same rotation is made
         ** indistinguishable from the puzzle's original form.  All puzzle
         ** solutions satisfying this configuration produce new valid
         ** solutions under the rotation in question.  For such puzzles it is
         ** much harder to correctly filter the rotations or translations of
         ** mobile pieces so as to eliminate rotationally redundant solutions
         ** from the algorithm search.  It may be possible to break the puzzle
         ** into subproblems that are each more-easily filtered, but I have
         ** not yet tried to formulate how this might work.  For now I simply
         ** disable rotational redundancy filter processing for these puzzles.
         ** Redundant solutions can still be filtered out via the solution
         ** filter, but that does require sufficient memory to maintain the
         ** list of puzzle solutions.
         **/
        bool redundancyComplexity;

        /** Ordered list of images of pieces placed in the puzzle.  Stationary
         ** pieces are always listed first.  Any time Dlx or Tiling places an
         ** image in the puzzle, the image is pushed on this stack.  When
         ** either algorithm removes an image, it is popped from this stack.
         ** (I originally had this as an STL vector, but changed it to a
         ** dynamically allocated array to avoid the bounds checks in
         ** the calls to push_back and pop_back made in the inner loop of
         ** the de Bruijn algorithm.)
         **/
        const Image** imageStack;
        int imageStackSize;

        /** The list of rotations that when acted upon a puzzle solution may
         ** produce a new valid solution.  This list is calculated AFTER
         ** stationary pieces are loaded.  In order for a rotation to appear
         ** on this list, the rotated puzzle must not only have the same x, y
         ** and z dimensions; but there must also exist some configuration of
         ** some subset of the mobile pieces (along with the stationary pieces
         ** which are already loaded) where the rotation acting upon this
         ** partially loaded puzzle produces an identical image.  If no such
         ** configuration exists, then that rotation when acted upon a puzzle
         ** solution can't possibly produce a new valid solution since
         ** stationary pieces would not occupy the positions to which they are
         ** constrained.
         **/
        std::vector<Rotation> symmetricRotationList;

        /** Any rotation of the grid by any rotation found in
         ** symmetricRotationList results in a new grid of exactly the same
         ** shape, but with mobile pieces rotated.  A symmetric permutation
         ** gives the id translations between the old and new grid point
         ** positions under one of these rotations and enables efficient
         ** rotations of solutions found during the search.  The
         ** symmetricPermutationList lists the permutations for each rotation
         ** in symmetricRotationList.
         **/
        std::vector<std::vector<int> > symmetricPermutationList;

        /** ParityMonitor object for tracking parity constraints on partial
         ** solutions.
         **/
        ParityMonitor parityMonitor;

        /** Volume object for tracking volume constraints when and if the open
         ** puzzle space becomes partitioned.
         **/
        VolumeMonitor volumeMonitor;

        /** Used by both initSymmetricRotationAndPermutationLists and
         ** normalizeState to renumber the id's used to identify pieces in
         ** puzzle state vectors.
         **/
        mutable std::vector<int> stateMap;

        /** Used by getState and initSymmetricRotationAndPermutationLists
         ** to track the number of instances of each shape that have been used
         ** as it translates non-specific shapes from the image stack to the
         ** names of specific pieces.
         **/
        mutable std::vector<int> shapeCount;

        PerformanceMeter* puzzleInitMeter;
        PerformanceMeter* puzzleCleanupMeter;
        PerformanceMeter* dlxLoadMeter;
        PerformanceMeter* genImageListsMeter;
        PerformanceMeter* initTileMeter;

    public:

        Puzzle(const PuzzleConfig& config);
        ~Puzzle();

        int                        getXDim()                    const;
        int                        getYDim()                    const;
        int                        getZDim()                    const;
        int                        getNumGridPoints()           const;
        int                        getNumPieces()               const;
        int                        getNumStationaryPieces()     const;
        int                        getNumMobilePieces()         const;
        int                        getNumShapes()               const;
        int                        getNumStationaryShapes()     const;
        int                        getNumMobileShapes()         const;
        bool                       getOneSide()                 const;

        const std::vector<Shape*>& getShapes()                  const;
        void                       dumpShapes(std::ostream& os) const;

    private:

        void calcPieceVolumes(
                const std::vector<NamedPiece>& stationary,
                const std::vector<NamedPiece>& mobile);

        void initAllowedRotationList();

        bool isBounded(int x, int y, int z) const;

        bool isBounded(const Point& p) const;

        bool isBounded(const Piece& s) const;

        void verifyBounded(const std::vector<NamedPiece>& pieceList) const;

        void initStationaryVolume(const std::vector<NamedPiece>& pieceList);

        void initGrid();

        /** Takes the user provided list of NamedPieces, pieceList, and
         ** adds them to the Puzzle's list of Shapes creating new Shapes as
         ** necessary.
         **/
        void addShapes(const std::vector<NamedPiece>& pieceList, bool stationary);

        Shape* findShape(const Piece& c);

        void countShapes();

        /** Initializes the mirrorId field of each shape with the id of it's
         ** mirror if any.  Also sets the Puzzle's mirrors field
         ** appropriately.
         **/
        void initMirrors();

        void initImageLists();

        Shape* findMirror(const Piece& c);

        void initParityMonitor();

        void initVolumeMonitor();

        void loadStationaryImages();

        void initNeighbors();

        void checkSetNeighbor(GridPoint* gp, int x, int y, int z);

        GridPoint* getGridPoint(int gridIndex);

        void verifyPuzzleSize() const;

        /** As a rule, this software does not consider two solutions unique if
         ** you can take one and simply rotate it in space to produce the
         ** other.  The objective of this function is to find the set of
         ** rotations that when acted upon a solution to the puzzle could
         ** possibly produce another valid puzzle solution.  Of the entire
         ** application, this method is likely the most difficult to
         ** understand, so read carefully.  Specifically, for each rotation r
         ** of the puzzle cuboid that produces a new cuboid of exactly the
         ** same x, y and z dimensions, this method examines the puzzle after
         ** stationary pieces have been loaded to determine if there exists
         ** some placement of some subset (including the empty subset) of
         ** mobile pieces that when rotated along with the stationary pieces
         ** by r produces a new configuration where the positions previously
         ** occupied by stationary pieces are now occupied by pieces (either
         ** mobile or stationary) with exactly the same shape; so that the new
         ** configuration satisifies all the stationary piece requirements for
         ** the puzzle.  If such a configuration is found, then r is added to
         ** symmetricRotationList.  If there exists at least one such rotation
         ** where a mobile piece was required to achieve the symmetry, then
         ** the puzzle as a whole is declared to have redundancyComplexity.
         ** See the redundancyComplexity flag for more information.
         **
         ** Also, for each rotation added to symmetricRotationList, this method
         ** also loads symmetricPermutationList with a vector of ints that is
         ** the permutation of grid point indicees that effects the rotation.
         **
         ** If this is a one-sided puzzle and at least one piece doesn't have
         ** a mirror, then only the 4 rotations about the z axis are
         ** considered when looking for symmetries.  (If there are one-sided
         ** pieces without mirrors, then flipping the puzzle upside down will
         ** result in the appearance of invalid pieces and so the resultant
         ** shape can't possibly be a solution.)  In the case that this is a
         ** one-sided puzzle and all pieces do have mirrors, then flipping the
         ** puzzle up-side-down could create a valid solution, and so we do
         ** allow these rotations when looking for symmetries.
         **/
        void initSymmetricRotationAndPermutationLists();

        /** Returns true iff there exists some placement of some subset
         ** (including the empty subset) of mobile pieces in rotatedState such
         ** that every occupied position of state is also occupied in
         ** rotatedState by a piece of the same shape.  If symmetry is
         ** achieved, but only through the use of mobile pieces, then
         ** isSymmetric has the side-affect of setting redundancyComplexity to
         ** true.  (Note that it is not neccessary after augmentation that
         ** state and rotatedState be geometrically identical:  I'm only
         ** interested in determining if given the stationary piece
         ** placements, then does there exist some placement of mobile pieces
         ** such that when the puzzle is rotated by r, all the grid positions
         ** previously occupied by stationary pieces are now occupied by
         ** mobile pieces of the same shape.  If there are, then puzzle
         ** solutions when rotated by r could conceivably form another valid
         ** puzzle solution.)  This method is a helper method to- and is only
         ** called by- initSymmetricAndPermutationLists.
         **/
        bool isSymmetric(const std::vector<PIECEID_T>& state, std::vector<PIECEID_T>& rotatedState, const Rotation& r);

        const std::vector<std::vector<int> >& getSymmetricPermutationList() const;

        /** Generate the image lists for each shape and load them to the DLX
         ** data structure.  The images of the shape with id matching
         ** redundancyFilterIndex are filtered so as to reduce or (if
         ** possible) eliminate rotationally redundant puzzle solutions.  An
         ** attempt to filter a piece that has multiple mobile copies will
         ** result in exception.  For the case of one-sided polyomino puzzles,
         ** you may choose a piece that has a mirror, but this software is not
         ** able to filter such a piece in such a way as to eliminate
         ** solutions that are upside-down mirrors of eachother
         ** (assuming the puzzle has upside-down symmetry.)
         **
         ** If redundancyFilterIndex is set to REDUNDANCY_FILTER_OFF_INDEX,
         ** then no piece is filtered.  If redundancyFilterIndex is set to
         ** REDUNDANCY_FILTER_AUTO_INDEX, then the results of filtering
         ** each shape is examined in turn and the piece having the highest
         ** reduction factor of produced images when the piece is filtered
         ** compared to when it is not filtered is chosen as the piece
         ** to rotationally constrain.  If two or more pieces tie in this
         ** first comparison measure, then the piece with the fewest number of
         ** total images is chosen.  The id of the piece actually filtered is
         ** is returned or, if no piece is rotationally constrained,
         ** REDUNDANCY_FILTER_OFF_INDEX is returned.
         **
         ** If the image reduction factor for the filtered piece is equal to
         ** the number of symmetric rotations of the puzzle then the
         ** filtering process will eliminate all rotationally redundant
         ** solutions and the redundacy flag is set to false.
         **/
        int genImageLists(int redundancyFilterIndex);

        /** Generates all possible rotated and translated images of the piece
         ** that are bounded to the Puzzle's Gridpoints and stores them in
         ** piece.imageList.  If the redundancyFilter flag is set, then images that
         ** can be rotated with the puzzle as a whole to produce an overall
         ** puzzle appearance that is identical to the appearance of the
         ** puzzle given by some other image already added to imageList
         ** (during the processing of this same call) is filtered out.  Note
         ** that some images can be rotated with the puzzle and effect no
         ** change in the overall puzzle appearance.  (For example assume the
         ** puzzle is a 3x3x3 cube and the subject piece is a 1x1x1 cube; and
         ** consider the image of the piece placed in the center of the puzzle
         ** cube:  all rotations of this configuration are indistinguishable
         ** from the non-rotated configuration.)  Such images alone in the
         ** puzzle are rotationally redundant with themselves.  These images
         ** ARE included in the returned imageList.
         **/
        void genImageList(Shape& piece, bool redundancyFilter);

        /** Check to see if image fits in the grid based on grid fill states.
         ** Returns a GridPoint of conflict if the image does not fit.  This
         ** method is only used for validating that user defined stationary
         ** pieces do not overlap.
         **/
        GridPoint* fits(const Image* image) const;

        /** Check to see if piece fits in the grid based on grid dimension
         ** and grid fill states.  This method is used during mobile piece
         ** image generation to see if a particular rotation and translation
         ** of a mobile piece both fits within the bounds of the puzzle
         ** cuboid and does not conflict with the placement of stationary
         ** pieces.
         **/
        bool fits(const Piece& piece) const;

        bool getRedundancy() const;

        DlxHead* getDlxRoot();

        void dlxClear();

        void dlxLoad();

        void dlxAddColumn(DlxHead* dh);

        void dlxAddRow(Image* image, DlxNode* freeNode);

        void dlxRandomize(Random& random);

        int dlxFilterFit();

        /** Temporarily places row using DLX algortihms, checks to see if any
         ** remaining columns in the matrix have zero remaining rows and
         ** then restores the matrix to it's previous state.  Returns true
         ** if all columns had at least one row; and false otherwise.
         **/
        bool dlxCheckFit(DlxNode* row);

        /** Filters all images of all remaining pieces which, if used, would
         ** produce a parity violation.  
         **/
        int dlxFilterParity();

        /** Internal helper method to the above dlxFilterParity().  Determines
         ** if placing images of the  given piece with the given parity would
         ** result in a parity violation. If a violation would occur, all
         ** images of that piece with the given parity are filtered.
         **/
        void dlxFilterParity(Shape* piece, int p);

        int dlxFilterVolume();

        void dlxFilterRow(DlxNode* row);

        void dlxUnfilterRow();

        void dlxCover(DlxHead* c);

        void dlxUncover(DlxHead* c);

        void dlxVerify();

        void dlxVerifyColumn(DlxHead* head);

        void dlxVerifyRow(DlxNode* node);

        /** This is an initialization that must be called each time DLX is
         ** disabled and tiling is enabled.  It performs a few actions (and
         ** perhaps should be split into separate method calls for clarity):
         ** 
         ** 1) Packs the indicees of the remaining shapes into the
         **    caller provided array remainingShapeList.  The
         **    remainingShapeList is used in a few places during tiling to
         **    avoid searching over all shapes in the puzzle.  Perhaps
         **    most importantly, it is used to choose all permutational
         **    orderings of the remaining shapes.  See the mch and bruijn
         **    algorithms for more information.
         **
         ** 2) Assigns each open grid point a bit number (in support of
         **    bit oriented placement checks).
         **
         ** 3) ORs these bits together as appropriate for each remaining
         **    image in the DLX matrix and stores this field as the images
         **    layoutMask (again, in support of bit-mask oriented placement
         **    checks.)
         **
         ** 4) Repacks these images into arrays stored at each gridpoint
         **    so that the list of remaining images for a particular
         **    piece is directly accessible.
         **
         ** 5) returns the number of remaining shapes.
         **/
        int initTile(int* remainingShapeList, int* remainingShapeCount);

        /** Internal support function for initTile above.  Packs the indicees
         ** of those shapes still having images into the low range of
         ** remainingShpaeList.  Also load the counts for those images, but
         ** note that remainingShapeCount is indexed by the piece index --
         ** it's not packed like remainingShapeList.  Also set numShapes which
         ** is the number of entries that got packed into remainingShapeList.
         ** Returns the number of remaining shapes.
         **/
        int loadRemainingShapesList(int* remainingShapeList, int* remainingShapeCount);

        /** Clears the assigned bit number of each remaining GridPoint.  Also
         ** clears the mch and bruijn image lists at each remaining GridPoint.
         **/
        void cleanupTile(int* remainingShapeList, int numRemainingShapes);

        GridPoint* getMostConstrainedHole(
                const int* remainingShapeList,
                int numRemainingShapes,
                bool estimate);

        int countFits(
            GridPoint* gp,
            int minFits,
            const int* remainingShapes,
            int numRemainingShapes) const;

        /** The number of images of mobile pieces that failed to be created
         ** because they fell outside the puzzle box or conflicted with the
         ** position of a stationary piece.
         **/
        int getUnboundedImageCount() const;

        /** The number of images generated for all pieces (mobile +
         ** stationary) that fit in the puzzle space.
         **/
        int getBoundedImageCount() const;

        puzzlemask_t getOccupancyState() const;

        // Different algorithmic techniques require different state
        // information to be maintained as pieces are placed and
        // unplaced.  Rather than always perform all the state
        // updates with ever place/unplace operation (which would
        // be simpler), I've instead broken out the facilities
        // to update each piece of state information allowing
        // algorithms to skip the update of state information that
        // they do not require.

        /** Sets the bits in occupancyState that are set in image's layoutMask.
         **/
        void placeOccupancy(const Image* image);

        /** Unsets the bits in occupancyState that are set in image's layoutMask.
         **/
        void unplaceOccupancy(const Image* image);

        /** Updates the parity monitor with the parity of the image.
         **/
        void placeParity(int parity);

        /** Reverse updates the parity monitor with the parity of the image.
         **/
        void unplaceParity(int parity);

        /** Updates the volume monitor with the volume of the image.
         **/
        void placeVolume(const Image* image);

        /** Reverse updates the volume monitor with the volume of the image.
         **/
        void unplaceVolume(const Image* image);

        /** Decreases numRemainingGridPoints by the size of the image.  This is
         ** only needed by DLX to determine when the puzzle has gotten
         ** sufficiently small to model by the occupancyState bit mask
         ** (permitting the switch to mch and bruijn algorithms as desired).
         **/
        void placeGridPointCount(const Image* image);

        /** Increases numRemainingGridPoints by the size of the image.
         **/
        void unplaceGridPointCount(const Image* image);

        /** Pushes the image to the image stack.
         **/
        void placeStack(const Image* image);

        /** Pops the last image pushed to the image stack.
         **/
        void unplaceStack();

        bool checkParity();

        void fill(const Image* image, int fillValue);

        void unfill(const Image* image);

        bool checkVolume();

        int fillVolume(GridPoint* gp, int fillValue);

        /** Returns the number of pieces still to be placed in the puzzle.
         **/
        int getNumRemainingPieces();

        /** Returns the number of open GridPoints remaining in the puzzle.
         ** Note: this value is only maintained while DLX is active.
         ** MCH and Bruijn algorithms have no use for this information
         ** and so the processing required to maintain it is skipped
         ** by those algorithms.
         **/
        int getNumRemainingGridPoints();

        /** Traverses the DLX header linked list in the forward direction
         ** until it finds an unoccupied GridPoint and returns it.  The first
         ** call to this method is made by  passing in the DLX root node
         ** (which isn't a GridPoint at all).  Each subsequent hole filled is
         ** picked by passing in the last hole returned.  Calling this when
         ** there are no GridPoints left in the DLX matrix will cause the
         ** DlxRoot node to be cast disasterously into a GridPoint.  Don't do
         ** that.
         **/
        GridPoint* getNextBruijnHole(DlxHead* lastBruijnHole) const;

        /** Overwrites the caller supplied vector so that on return
         ** state[x*(yDim*zDim) + y*(zDim) + z] will be the integer id of the
         ** piece occupying position (x,y,z) in the Puzzle's grid space or
         ** will have a zero value if no piece occupies that position.  The
         ** length of the state variable upon return will equal the number of
         ** GridPoints in the puzzle.  Note that if two or more puzzle pieces
         ** share the same shape, then swapping the positions of two pieces
         ** with the same shape in the puzzle will produce different values
         ** for the Puzzle state.  If a single unique identifier for the
         ** geometry of the Puzzle is desired, call getNormalizedState
         ** instead.
         **/
        void getState(std::vector<PIECEID_T>& state) const;

        /** A call to:
         **
         **     getNormalizedState(state);
         **
         ** Is equivalent to:
         **
         **    getState(state);
         **    normalizeState(state, state);
         **/
        void getNormalizedState(std::vector<PIECEID_T>& state) const;


        /** Rotates state using the given permutation matrix.
         **/
        void rotateState(const std::vector<PIECEID_T>& state,
                const std::vector<int>& permutation, std::vector<PIECEID_T>& rotatedState) const;

        /** A call to:
         **
         **     rotateAndNormalizeState(state, permutation, rotatedState);
         **
         ** Is equivalent to:
         **
         **     rotateState(state, permutation, rotatedState);
         **     normalizeState(rotatedState, rotatedState);
         **
         **/
        void rotateAndNormalizeState(const std::vector<PIECEID_T>& state,
                const std::vector<int>& permutation, std::vector<PIECEID_T>& rotatedState) const;

        /** Copies the entries in state to normalizedState, but renumbers them
         ** as it copies so as to produce a unique identifier for the puzzle
         ** state.  Any zero entries are left unchanged; the first non-zero
         ** piece identifier appearing in state is renumbered 1; the second
         ** non-zero piece is renumbered 2, etc.  Here is a sample input and
         ** output result:
         **
         **     state:           '2'  '2'  '1'   0   '1'  '2'  '1'  0
         **     normalizedState:  1    1    2    0    2    1    2   0
         **
         ** So, piece 2 got numbered 1 since it appears first; piece 1 got
         ** numbered 2 since it appears second; and the zero entries are
         ** simply copied over unchanged.  (Zero is handled differently since
         ** it represents open spaces in the puzzle and when comparing two
         ** state vectors, you never want a set of holes to be mistaken for an
         ** actual piece that happens to have the same geometry.)
         **/
        void normalizeState(const std::vector<PIECEID_T>& state,
                std::vector<PIECEID_T>& normalizedState) const;


        /** Takes a Puzzle state variable and returns the name of the piece
         ** loaded at Point p or 0 if no piece is loaded at that location.
         **/
        const std::string& nameAt(const std::vector<PIECEID_T>& state, const Point& p) const;

        /** Takes a Puzzle state variable and returns the name of the piece
         ** loaded at coordinates (x, y, z) or 0 if no piece is loaded at that
         ** location.
         **/
        const std::string& nameAt(const std::vector<PIECEID_T>& state, int x, int y, int z) const;

        void dumpFill(std::ostream& os) const;

        void printState(
            std::ostream& os,
            const OutputFormatConfig& outputFormat) const;

        void deleteGrid();

        friend class PuzzleSolverInterface;
};


// ***************
// *** INLINES ***
// ***************

inline int  Puzzle::getXDim() const
{
    return this->xDim;
}


inline int  Puzzle::getYDim() const
{
    return this->yDim;
}


inline int  Puzzle::getZDim() const
{
    return this->zDim;
}


inline int Puzzle::getNumGridPoints() const
{
    return this->numGridPoints;
}


inline int Puzzle::getNumPieces() const
{
    return this->numPieces;
}

inline int Puzzle::getNumStationaryPieces() const
{
    return this->numStationaryPieces;
}

inline int Puzzle::getNumMobilePieces() const
{
    return this->numMobilePieces;
}


inline int Puzzle::getNumShapes() const
{
    return this->numShapes;
}

inline int Puzzle::getNumStationaryShapes() const
{
    return this->numStationaryShapes;
}

inline int Puzzle::getNumMobileShapes() const
{
    return this->numMobileShapes;
}


inline bool Puzzle::getOneSide() const
{
    return this->oneSide;
}


inline const std::vector<Shape*>& Puzzle::getShapes() const
{
    return shape;
}


inline void Puzzle::dumpShapes(std::ostream& os) const
{
    for(int si = 0; si < shape.size(); ++si)
        shape[si]->printAll(os) << std::endl;
}


inline bool Puzzle::isBounded(int x, int y, int z) const
{
    if(x < 0 || x >= xDim)
        return false;
    if(y < 0 || y >= yDim)
        return false;
    if(z < 0 || z >= zDim)
        return false;
    return true;
}


inline bool Puzzle::isBounded(const Point& p) const
{
    return isBounded(p.getX(), p.getY(), p.getZ());
}


inline bool Puzzle::isBounded(const Piece& s) const
{
    for(int pi = 0; pi < s.size(); ++pi)
        if(!isBounded(s.getPoint(pi)))
            return false;
    return true;
}


inline void Puzzle::checkSetNeighbor(GridPoint* gp, int x, int y, int z)
{
    if(isBounded(x, y, z) && grid[x][y][z].fill < 0)
        gp->neighborList.push_back(&grid[x][y][z]);
}


inline GridPoint* Puzzle::getGridPoint(int gridIndex)
{
    return gridStore + gridIndex;
}


inline const std::vector<std::vector<int> >& Puzzle::getSymmetricPermutationList() const
{
    return symmetricPermutationList;
}


inline bool Puzzle::getRedundancy() const
{
    return redundancy;
}


inline DlxHead* Puzzle::getDlxRoot()
{
    return &dlxRoot;
}


inline void Puzzle::dlxFilterRow(DlxNode* row)
{
    register DlxNode* n = row;
    do
    {
        n->up->down = n->down;
        n->down->up = n->up;
        --n->head->numRow;
        n = n->right;
    } while(n != row);
    filterList.push_back(row);
}


inline void Puzzle::dlxUnfilterRow()
{
    DlxNode* row = *filterList.rbegin();
    filterList.pop_back();
        
    register DlxNode* n = row;
    do
    {
        ++n->head->numRow;
        n->down->up = n;
        n->up->down = n;
        n = n->left;
    } while(n != row);
}


inline void Puzzle::dlxCover(DlxHead* c)
{
    c->right->left = c->left;
    c->left->right = c->right;
    for(register DlxNode* i = c->down; i != c; i = i->down)
    {
        for(register DlxNode* j = i->right; j != i; j = j->right)
        {
            j->up->down = j->down;
            j->down->up = j->up;
            --j->head->numRow;
        }
    }
}


inline void Puzzle::dlxUncover(DlxHead* c)
{
    for(register DlxNode* i = c->up; i != c; i = i->up)
    {
        for(register DlxNode* j = i->left; j != i; j = j->left)
        {
            ++j->head->numRow;
            j->down->up = j;
            j->up->down = j;
        }
    }
    c->left->right = c;
    c->right->left = c;
}


inline puzzlemask_t Puzzle::getOccupancyState() const
{
    return occupancyState;
}


inline void Puzzle::placeOccupancy(const Image* image)
{
    occupancyState |= image->layoutMask;
}


inline void Puzzle::unplaceOccupancy(const Image* image)
{
    occupancyState &= ~image->layoutMask;
}


inline void Puzzle::placeParity(int parity)
{
    parityMonitor.place(parity);
}


inline void Puzzle::unplaceParity(int parity)
{
    parityMonitor.unplace(parity);
}


inline void Puzzle::placeVolume(const Image* image)
{
    volumeMonitor.place(image->layout.size());
    fill(image, pieceFill++);
}


inline void Puzzle::unplaceVolume(const Image* image)
{
    unfill(image);
    --pieceFill;
    volumeMonitor.unplace(image->layout.size());
}


inline void Puzzle::placeGridPointCount(const Image* image)
{
    numRemainingGridPoints -= image->layout.size();
}


inline void Puzzle::unplaceGridPointCount(const Image* image)
{
    numRemainingGridPoints += image->layout.size();
}


inline void Puzzle::placeStack(const Image* image)
{
    imageStack[imageStackSize++] = image;
}



inline void Puzzle::unplaceStack()
{
    --imageStackSize;
}


inline bool Puzzle::checkParity()
{
    return parityMonitor.checkParity();
}


inline void Puzzle::fill(const Image* image, int fillValue)
{
    for(std::vector<GridPoint*>::const_iterator gi = image->layout.begin(); gi != image->layout.end(); ++gi)
    {
        (*gi)->fill = fillValue;
    }
}


inline void Puzzle::unfill(const Image* image)
{
    for(std::vector<GridPoint*>::const_iterator gi = image->layout.begin(); gi != image->layout.end(); ++gi)
    {
        (*gi)->fill = unoccupiedFill;
    }
}


inline int Puzzle::fillVolume(GridPoint* gp, int fillValue)
{
    if(gp->fill != unoccupiedFill)
        return 0;
    int volume = 1;
    gp->fill = fillValue;
    for(std::vector<GridPoint*>::iterator ni = gp->neighborList.begin(); ni != gp->neighborList.end(); ++ni)
        volume += fillVolume(*ni, fillValue);
    return volume;
}


inline int Puzzle::getNumRemainingPieces()
{
    return numPieces - imageStackSize;
}


inline int Puzzle::getNumRemainingGridPoints()
{
    return numRemainingGridPoints;
}

inline GridPoint* Puzzle::getNextBruijnHole(DlxHead* lastBruijnHole) const
{
    while(((GridPoint*) (lastBruijnHole = (DlxHead*) lastBruijnHole->right))->bit & occupancyState);
    return (GridPoint*) lastBruijnHole;
}


#endif

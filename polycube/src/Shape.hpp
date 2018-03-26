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

#ifndef SHAPE_PIECE_HPP
#define SHAPE_PIECE_HPP

#include <string>
#include "DlxHead.hpp"
#include "NamedPiece.hpp"

class GridPoint;

class Shape : public Piece, public DlxHead
{
    public:
        /** A simple fixed identifier for the shape useful for finding this
         ** shape in the shape array.
         **/
        int id;

        /** The mirrorId is initialized to -1.  It is only set and used
         ** for the case of one-sided polyomino puzzles.  If a shape is
         ** indistinguishable from itself when flipped up-side-down, then
         ** mirrorId is set to id.  (The shape is it's own mirror.)  If
         ** instead, when flipped up-side-down, it looks like another shape in
         ** the available shape set, then mirrorId is set to the id of that
         ** other shape.  Otherwise, if the shape has no mirror in the
         ** available shape set, mirrorId is left at the -1 value.
         **/
        int mirrorId;

        /** pieceList holds the list of NamedPieces that share this shape.
         ** Because stationary pieces are always loaded first, the first
         ** stationaryImageList.size() entries in this list are stationary
         ** pieces.  (I'm not sure I use this fact anywhere.  Hopefully not.)
         **/
        std::vector<NamedPiece*> pieceList;

        /** The subset of rotations among the caller-defined
         ** allowedRotationList that produce a unique shape.  If this piece is
         ** constrained, this rotationList could be reduced to eliminate the
         ** discovery of rotationally redundant solutions by the search
         ** algorithm.
         **/
        std::vector<Rotation> rotationList;

        /** The set of shapes produced by rotating this shape by each
         ** rotation in rotationList.
         **/
        std::vector<Piece> rotatedPieceList;

        /** The list of images of fixed position pieces that have this shape.
         **/
        std::vector<Image*> stationaryImageList;

        /** List of images of mobile pieces that have this shape.  The content
         ** of this imageList is heavily dependent upon the shape of the
         ** Puzzle and upon the placement constraints on pieces that share
         ** this shape.  Because of this, this class provides no facilities
         ** for the maintenance of this list beyond that of deleting and
         ** clearing the contents of the imageList upon destruction.
         **/
        std::vector<Image*> mobileImageList;

        Shape(
                const Piece& piece,
                int id,
                const std::vector<Rotation>& allowedRotationList = Rotation::getRotationList());

        Shape(const Shape& shape);

        ~Shape();

        /** This method constrains the set of rotations allowed on this shape
         ** so as to reduce or eliminate rotationally redundant puzzle
         ** solutions produced by the back-tracking algorithm without loss of
         ** rotationally unique puzzle solutions.
         **
         ** The user provided input, symmetricRotationList is loaded with the
         ** set of rotations which when acted on the Puzzle while loaded with
         ** stationary shapes produce an identical shape.  The result is
         ** a pruning of the rotationList and rotatedPieceList data members
         ** on this object which will reduce or eliminate the discovery
         ** of rotationally symmetric solutions during the search.
         **/
        void pruneRotationList(const std::vector<Rotation>& symmetricRotationList);

        /** Deletes all entries in the stationaryImageList and then clears the
         ** list.
         **/
        void clearStationaryImageList();

        /** Deletes all entries in the mobileImageList and then clears the
         ** list.
         **/
        void clearMobileImageList();

        NamedPiece* getPiece(int i = 0) const;

        int getNumCopies() const;

        int getNumStationaryPieces() const;

        int getNumMobilePieces() const;

        /** Adds the piece to the pieceList.  If the piece is stationary, then
         ** it's image is added to the stationary Image list as well.  The
         ** grid argument is required if stationary is true.
         **/
        void addPiece(NamedPiece* piece,
                bool stationary = false,
                GridPoint*** grid = NULL);

        std::ostream& print(std::ostream& os) const;

        virtual std::ostream& printAll(std::ostream& os) const;

    private:
        void initRotationLists(const std::vector<Rotation>& allowedRotationList);

};


inline NamedPiece* Shape::getPiece(int i) const
{
    return pieceList[i];
}


inline int Shape::getNumCopies() const
{
    return pieceList.size();
}


inline int Shape::getNumStationaryPieces() const
{
    return stationaryImageList.size();
}


inline int Shape::getNumMobilePieces() const
{
    return pieceList.size() - stationaryImageList.size();
}


inline std::ostream& operator << (std::ostream& os, const Shape& shape)
{
    return shape.print(os);
}

#endif

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

#include "Shape.hpp"
#include "Image.hpp"

using namespace std;

Shape::Shape(
        const Piece& piece,
        int id,
        const vector<Rotation>& allowedRotationList)
    : Piece(piece), id(id), mirrorId(-1)
{
    initRotationLists(allowedRotationList);
}


Shape::Shape(const Shape& shape)
    :
        Piece(shape),
        id(shape.id),
        mirrorId(shape.mirrorId),
        pieceList(shape.pieceList),
        rotationList(shape.rotationList),
        rotatedPieceList(shape.rotatedPieceList)
{
    for(int ii = 0; ii < shape.stationaryImageList.size(); ++ii)
    {
        this->stationaryImageList.push_back(new Image(*shape.stationaryImageList[ii]));
    }
}


Shape::~Shape()
{
    clearStationaryImageList();
    clearMobileImageList();
}


void Shape::pruneRotationList(
        const vector<Rotation>& symmetricRotationList)
{
    // Define a mapping from Rotation id to pointer to Rotation
    // setting only those map entries corresponding to a Rotation that
    // appears in rotationList.
    //
    const Rotation* tList[24];
    for(int ri = 0; ri < 24; ++ri)
        tList[ri] = NULL;

    for(int ri = 0; ri < rotationList.size(); ++ri)
        tList[rotationList[ri].id()] = &rotationList[ri];

    rotationList.clear();
    rotatedPieceList.clear();

    const Rotation* r;
    for(int ri = 0; ri < 24; ++ri)
    {
        if((r = tList[ri]) == NULL)
            continue;

        rotationList.push_back(*r);
        rotatedPieceList.push_back(rotate(*r));

        for(int sri = 0; sri < symmetricRotationList.size(); ++sri)
        {
            const Rotation& sr = symmetricRotationList[sri];
            const Rotation& rr = r->rotate(sr);
            tList[rr.id()] = NULL;
        }
    }
}


void Shape::addPiece(NamedPiece* piece, bool stationary, GridPoint*** grid)
{
    pieceList.push_back(piece);
    if(stationary)
        stationaryImageList.push_back(new Image(*this, *piece, grid));
}


void Shape::clearStationaryImageList()
{
    for(int ii = 0; ii < stationaryImageList.size(); ++ii)
        delete stationaryImageList[ii];
    stationaryImageList.clear();
}


void Shape::clearMobileImageList()
{
    for(int ii = 0; ii < mobileImageList.size(); ++ii)
        delete mobileImageList[ii];
    mobileImageList.clear();
}


void Shape::initRotationLists(const vector<Rotation>& allowedRotationList)
{
    rotationList.clear();
    rotatedPieceList.clear();

    for(int ri = 0; ri < allowedRotationList.size(); ++ri)
    {
        const Rotation& r = allowedRotationList.at(ri);
        Piece rs = rotate(r);
        bool unique = true;
        for(int rsi = 0; rsi < rotatedPieceList.size(); ++rsi)
        {
            if(rotatedPieceList.at(rsi).isCongruent(rs))
            {
                unique = false;
                break;
            }
        }
        if(unique)
        {
            rotationList.push_back(r);
            rotatedPieceList.push_back(rs);
        }
    }
}


ostream& Shape::print(ostream& os) const
{
    Piece::print(os);
    os << ":";
    DlxHead::print(os);
    os << ":" << "id=" << id <<
        ":numCopies=" << getNumCopies() <<
        ":numStationary=" << getNumStationaryPieces() <<
        ":numMobile=" << getNumMobilePieces() <<
        ":numStationaryImages=" << stationaryImageList.size() <<
        ":numMobileImages=" << mobileImageList.size() <<
        ":pieceList={";
    for(int ci = 0; ci < pieceList.size(); ++ci)
    {
        if(ci > 0)
            os << ",";
        os << pieceList[ci]->getName();
    }
    os << "}";
    return os;
}

ostream& Shape::printAll(ostream& os) const
{
    Piece::print(os) << ":id=" << id << endl;
    os << "    pieceList={";
    for(int ci = 0; ci < pieceList.size(); ++ci)
    {
        if(ci > 0)
            os << ",";
        os << pieceList[ci]->getName();
    }
    os << "    }" << endl;

    os << "    rotationList={" << endl;
    for(int ri = 0; ri < rotationList.size(); ++ri)
    {
        os << "        " << rotationList[ri];
        if(ri != rotationList.size() - 1)
            os << ",";
        os << endl;
    }
    os << "    }" << endl;

    os << "    rotatedPieceList={" << endl;
    for(int ci = 0; ci < rotatedPieceList.size(); ++ci)
    {
        os << "        " << rotatedPieceList[ci];
        if(ci != rotatedPieceList.size() - 1)
            os << ",";
    }
    os << "    }" << endl;

    os << "    stationaryImageList={" << endl;
    for(int ii = 0; ii < stationaryImageList.size(); ++ii)
    {
        os << "        " << *stationaryImageList[ii];
        if(ii != stationaryImageList.size() - 1)
            os << ",";
        os << endl;
    }
    os << "    }" << endl;
    
    os << "    mobileImageList={" << endl;
    for(int ii = 0; ii < mobileImageList.size(); ++ii)
    {
        os << "        " << *mobileImageList[ii];
        if(ii != mobileImageList.size() - 1)
            os << ",";
        os << endl;
    }
    os << "    }" << endl;
    
    return os;
}

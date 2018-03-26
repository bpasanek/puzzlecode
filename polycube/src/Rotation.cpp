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

#include "Rotation.hpp"
#include "makeArray.hpp"

//      y
//
//      |
//      |
//      |
//      |
//      |
//      +--------- x
//     /
//    /
//   /
//
// z

RotationImpl*   Rotation::implList     = NULL;
std::vector<Rotation>* Rotation::rotationList = NULL;
const Rotation* Rotation::rotationTable[24][24];
const Rotation* Rotation::_I           = NULL;
const Rotation* Rotation::_x90         = NULL;
const Rotation* Rotation::_y90         = NULL;
const Rotation* Rotation::_z90         = NULL;

const std::vector<Rotation>& Rotation::getRotationList()
{
    if(rotationList == NULL)
    {
        int I[3][3]   = { { 1,  0,  0},
                          { 0,  1,  0},
                          { 0,  0,  1} };

        int x90[3][3] = { { 1,  0,  0},    //  x =  x
                          { 0,  0, -1},    //  y = -z
                          { 0,  1,  0} };  //  z =  y

        int y90[3][3] = { { 0,  0,  1},    //  x =  z
                          { 0,  1,  0},    //  y =  y
                          {-1,  0,  0} };  //  z = -x

        int z90[3][3] = { { 0, -1,  0},    //  x = -y
                          { 1,  0,  0},    //  y =  x
                          { 0,  0,  1} };  //  z =  z


        implList = makeArray<RotationImpl>(24);

        // Assign each RotationImpl a local variable.
        //

        int m = 0;
        RotationImpl& rI        = implList[m++];
        RotationImpl& rz90      = implList[m++];
        RotationImpl& rz180     = implList[m++];
        RotationImpl& rz270     = implList[m++];

        RotationImpl& rx90      = implList[m++];
        RotationImpl& rx90z90   = implList[m++];
        RotationImpl& rx90z180  = implList[m++];
        RotationImpl& rx90z270  = implList[m++];

        RotationImpl& rx180     = implList[m++];
        RotationImpl& rx180z90  = implList[m++];
        RotationImpl& rx180z180 = implList[m++];
        RotationImpl& rx180z270 = implList[m++];

        RotationImpl& rx270     = implList[m++];
        RotationImpl& rx270z90  = implList[m++];
        RotationImpl& rx270z180 = implList[m++];
        RotationImpl& rx270z270 = implList[m++];

        RotationImpl& ry90      = implList[m++];
        RotationImpl& ry90z90   = implList[m++];
        RotationImpl& ry90z180  = implList[m++];
        RotationImpl& ry90z270  = implList[m++];

        RotationImpl& ry270     = implList[m++];
        RotationImpl& ry270z90  = implList[m++];
        RotationImpl& ry270z180 = implList[m++];
        RotationImpl& ry270z270 = implList[m++];

        // Now initialize each RotationImpl as appropriate.
        //
        m = 0;
        rI.set(m++, "RI", I);
        rz90.set(m++, "R+z90", z90);
        rz180.set(m++, "R+z180", rz90, rz90);
        rz270.set(m++, "R+z270", rz180, rz90);

        rx90.set(m++, "R+x90", x90);
        rx90z90.set(m++, "R+x90+z90", rx90, rz90);
        rx90z180.set(m++, "R+x90+z180", rx90, rz180);
        rx90z270.set(m++, "R+x90+z270", rx90, rz270);

        rx180.set(m++, "R+x180", rx90, rx90);
        rx180z90.set(m++, "R+x180+z90", rx180, rz90);
        rx180z180.set(m++, "R+x180+z180", rx180, rz180);
        rx180z270.set(m++, "R+x180+z270", rx180, rz270);

        rx270.set(m++, "R+x270", rx180, rx90);
        rx270z90.set(m++, "R+x270+z90", rx270, rz90);
        rx270z180.set(m++, "R+x270+z180", rx270, rz180);
        rx270z270.set(m++, "R+x270+z270", rx270, rz270);

        ry90.set(m++, "R+y90", y90);
        ry90z90.set(m++, "R+y90+z90", ry90, rz90);
        ry90z180.set(m++, "R+y90+z180", ry90, rz180);
        ry90z270.set(m++, "R+y90+z270", ry90, rz270);

        ry270.set(m++, "R+y270", rx180z180, ry90);
        ry270z90.set(m++, "R+y270+z90", ry270, rz90);
        ry270z180.set(m++, "R+y270+z180", ry270, rz180);
        ry270z270.set(m++, "R+y270+z270", ry270, rz270);


        // Initialize rotationList.

        rotationList = new std::vector<Rotation>();
        for(int m = 0; m < 24; ++m)
            rotationList->push_back(Rotation(implList + m));

        _I   = &rotationList->at(rI.id);
        _x90 = &rotationList->at(rx90.id);
        _y90 = &rotationList->at(ry90.id);
        _z90 = &rotationList->at(rz90.id);

        // Initialize rotationTable.
        //
        // I frequently combine two rotations to get a composite rotation.
        // But every possible combination of the 24 rotations listed results
        // in one of these 24 rotations.  Rather than perform the matrix
        // multiplication every time; I instead perform all possible 24x24
        // rotation multiplications here and cache the results as a 24x24
        // array of rotation matrix ids.
        //
        int tmp[3][3];

        for(int i = 0; i < 24; ++i)
        {
            for(int j = 0; j < 24; ++j)
            {
                RotationImpl::matrixRotate(implList[i].m, implList[j].m, tmp);

                for(int k = 0; k < 24; ++k)
                {
                    if(RotationImpl::matrixEquals(implList[k].m, tmp))
                    {
                        rotationTable[i][j] = &rotationList->at(k);
                        break;
                    }
                }
            }
        }
    }
    return *rotationList;
}

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

#ifndef ROTATION_HPP
#define ROTATION_HPP

#include <ostream>
#include <vector>
#include <string>
#include "RotationImpl.hpp"
#include "Printable.hpp"

class Point;

/** The only data member of Rotation is a single pointer to an object of type
 ** RotationImpl.  Typically this kind of thing is done to keep implementation
 ** details out of the user-facing header file, but that's not at all my
 ** motivation in this case.  Because Rotations are constrained to 90 degree
 ** increments in three dimensions, there are only 24 possible values for
 ** Rotation objects.  My application frequently combines two Rotations
 ** together.  To make this faster I index the 24 possible Rotation values
 ** (i.e., the rotation id) and then simply lookup the resultant Rotation
 ** value from a 24x24 rotation lookup table.  The return value from this
 ** rotate function is a const reference to a statically allocated Rotation
 ** (so no objects are actually allocated when you rotate two Rotation
 ** objects).  Now I suppose I could eliminate all public constructors so that
 ** only 24 Rotation objects are ever constructed (forcing the user to only
 ** store const-refs to Rotation objects, but such restrictions are often
 ** troublesome.  Instead, I do provide a copy constructor, but the copy
 ** constructor only makes a shallow copy.  (I.e., only the single pointer to
 ** the RotaitonImpl is copied -- the 3x3 rotation matrix, the string name and
 ** the integer id are not copied.)  This ensures all Rotation operations are
 ** always fast.
 **
 ** In hind-sight, this was probably all over kill:  I only ever rotate
 ** things during the one-time Puzzle initialization sequence.  The
 ** performance optizations here are surely only noise but for all but
 ** the simplest of puzzles.
 **/
class Rotation : public Printable
{
    private:
        static RotationImpl* implList;
        static std::vector<Rotation>* rotationList;
        static const Rotation* rotationTable[24][24];

        static const Rotation* _I;
        static const Rotation* _x90;
        static const Rotation* _y90;
        static const Rotation* _z90;

        RotationImpl const * impl;
        Rotation(RotationImpl const * impl);

    public:
        static const std::vector<Rotation>& getRotationList();
        static const Rotation& I();
        static const Rotation& x90();
        static const Rotation& y90();
        static const Rotation& z90();

        /** Warning:  this constructor sets the Rotation's implementation
         ** to NULL, but a default constructor is useful for instantiating
         ** arrays of Rotations and in situations where 2-step 
         ** initialization is convenient.
         **/
        Rotation();
        Rotation(const Rotation& r);
        Rotation(int id);
        const Rotation& rotate(const Rotation& r) const;
        int id() const;
        const std::string& name() const;
        void operator = (const Rotation& r);
        bool operator == (const Rotation& r) const;
        bool operator < (const Rotation& r) const;

        std::ostream& print(std::ostream& os) const;
        friend class Point;
};

inline const Rotation& Rotation::I()
{
    getRotationList();
    return *_I;
}

inline const Rotation& Rotation::x90()
{
    getRotationList();
    return *_x90;
}

inline const Rotation& Rotation::y90()
{
    getRotationList();
    return *_y90;
}

inline const Rotation& Rotation::z90()
{
    getRotationList();
    return *_z90;
}

inline Rotation::Rotation()
    : impl(NULL)
{
}

inline Rotation::Rotation(RotationImpl const * impl)
    : impl(impl)
{
}

inline Rotation::Rotation(const Rotation& r)
    : impl(r.impl)
{
}

inline Rotation::Rotation(int id)
    : impl(getRotationList()[id].impl)
{
}

inline const Rotation& Rotation::rotate(const Rotation& r) const
{
    return *rotationTable[this->impl->id][r.impl->id];
}

inline int Rotation::id() const
{
    return this->impl->id;
}

inline const std::string& Rotation::name() const
{
    return this->impl->name;
}

inline void Rotation::operator = (const Rotation& r)
{
    this->impl = r.impl;
}

inline bool Rotation::operator == (const Rotation& r) const
{
    return this->impl == r.impl;
}

inline bool Rotation::operator < (const Rotation& r) const
{
    return this->impl->id < r.impl->id;
}

inline std::ostream& Rotation::print(std::ostream& os) const
{
    return impl->print(os);
}

#endif

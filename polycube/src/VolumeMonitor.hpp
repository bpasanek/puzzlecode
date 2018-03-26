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

#ifndef VOLUMEMONITOR_HPP
#define VOLUMEMONITOR_HPP

#include <string>
#include <vector>
#include <set>
#include <map>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <stdexcept>
#include <boost/lexical_cast.hpp>
#include "Printable.hpp"
#include "makeArray.hpp"


class VolumeMonitor;

class VolumeState : public Printable
{
    private:
        std::string   name;
        VolumeState** place;
        VolumeState** unplace;
        bool*         possible;

    public:
        VolumeState() : place(NULL), unplace(NULL), possible(NULL)
        {
        }

        ~VolumeState()
        {
            deleteArray(place, 0);
            deleteArray(unplace, 0);
            deleteArray(possible, 0);
        }

        std::ostream& print(std::ostream& os) const { return os << name; }

        friend class VolumeMonitor;
};


/** Given a polycube or polyomino puzzle, if after placing a piece the puzzle
 ** is partitioned into two or more isolated sub-spaces, it is possible that
 ** one or more of these sub-spaces can not be filled because the volume
 ** of the sub-space is not achievable by any subset of the remaining pieces.
 ** If all pieces are the same size, then this is easy to check:  if V is the
 ** volume of a subspace and S is the constant piece size then if V%S != 0
 ** the puzzle is no longer solvable.  But if the pieces have varying size the
 ** problem is slighly more difficult.  This class provides the functionality
 ** to perform volume checks for the general case.
 **/
class VolumeMonitor : public Printable
{
    private:

        std::vector<int> volumeSetBucketCounter;

        // The maximum volume of any individual piece in the puzzle.
        //
        int maxPieceVolume;

        // Multiplicative weights of the array-like indicees used to
        // index into the stateStore array.
        //
        std::vector<int> indexWeight;

        int maxVolumeByAllPieces;

        VolumeState* state;

        // Pointer to the allocated VolumeStates.
        //
        VolumeState* stateStore;

    public:

        VolumeMonitor();
        ~VolumeMonitor();
        void addElement(int volume);
        void init();
        void place(int volume);
        bool checkVolume(int targetVolume);
        void unplace(int volume);
        void clear();
        bool isInitialized();
        std::ostream& print(std::ostream& os) const;

    private:

        void initState(int p, std::vector<int>& indicees, const std::set<int>& base);
        VolumeState* getState(std::vector<int>& indicees) const;
        std::string genStateName(std::vector<int>& indicees) const;
        void addVolumeSets(const std::set<int>& set1, const std::set<int>& set2, std::set<int>& result) const;
        bool* genPossible(const std::set<int>& s) const;
};


inline VolumeMonitor::VolumeMonitor()
    :
        maxPieceVolume(0),
        maxVolumeByAllPieces(0),
        state(NULL),
        stateStore(NULL)
{
}


inline VolumeMonitor::~VolumeMonitor()
{
    clear();
}


inline void VolumeMonitor::addElement(int volume)
{
    if(isInitialized())
        throw std::runtime_error("VolumeMonitor::addElement():  Cannot add elements after init has been called.  Use clear to clear the state of the Volume object and start again.");

    if(volume <= 0)
        throw std::runtime_error("VolumeMonitor::addElement():  Volumes must be strictly positive.");

    volumeSetBucketCounter.resize(std::max((unsigned) volumeSetBucketCounter.size(), (unsigned) (volume + 1)), 0);
    ++volumeSetBucketCounter[volume];
}


inline void VolumeMonitor::init()
{
    maxPieceVolume = volumeSetBucketCounter.size() - 1;
    maxVolumeByAllPieces = 0;
    int numStates = 1;
    std::vector<int> indicees(maxPieceVolume+1, 0);
    indexWeight = std::vector<int>(maxPieceVolume+1, 0);
    for(int i = 1; i < volumeSetBucketCounter.size(); ++i)
    {
        if(volumeSetBucketCounter[i] > 0)
        {
            indexWeight[i] = numStates;
            numStates *= (volumeSetBucketCounter[i] + 1);
            maxVolumeByAllPieces += i*volumeSetBucketCounter[i];
        }
    }

    stateStore = makeArray<VolumeState>(numStates);

    std::set<int> base;
    base.insert(0);
    initState(0, indicees, base);
    state = getState(volumeSetBucketCounter);
}


inline void VolumeMonitor::place(int volume)
{
    state = state->place[volume];
}


inline bool VolumeMonitor::checkVolume(int targetVolume)
{
    return state->possible[targetVolume];
}


inline void VolumeMonitor::unplace(int volume)
{
    state = state->unplace[volume];
}


inline void VolumeMonitor::clear()
{
    state = NULL;
    deleteArray(stateStore, 0);
    maxVolumeByAllPieces = 0;
    indexWeight.clear();
    maxPieceVolume   = 0;
    volumeSetBucketCounter.clear();
}


inline bool VolumeMonitor::isInitialized()
{
    return stateStore != NULL;
}

inline std::ostream& VolumeMonitor::print(std::ostream& os) const
{
    if(state == NULL)
        return os << "NULL";
    else
        return os << *state;
}


inline void VolumeMonitor::initState(int p, std::vector<int>& indicees, const std::set<int>& base)
{
    while(++p < volumeSetBucketCounter.size() && volumeSetBucketCounter[p] == 0)
        ;

    if(p == volumeSetBucketCounter.size())
    {
        VolumeState* s = getState(indicees);
        s->name = genStateName(indicees);
        s->possible = genPossible(base);
        s->place    = initArray<VolumeState*>(maxPieceVolume+1, NULL);
        s->unplace  = initArray<VolumeState*>(maxPieceVolume+1, NULL);
        for(int i = 0; i < volumeSetBucketCounter.size(); ++i)
        {
            if(volumeSetBucketCounter[i] > 0)
            {
                indicees[i] -= 1;
                s->place[i] = getState(indicees);
                indicees[i] += 2;
                s->unplace[i] = getState(indicees);
                indicees[i] -= 1;
            }
        }
        return;
    }

    std::set<int> delta;
    delta.insert(0);
    delta.insert(p);
    std::set<int> s1 = base;
    std::set<int> s2;

    std::set<int>* current = &s2;
    std::set<int>* next    = &s1;

    for(int i = 0; i <= volumeSetBucketCounter[p]; ++i)
    {
        indicees[p] = i;
        initState(p, indicees, *next);

        std::swap(current, next);
        next->clear();
        addVolumeSets(*current, delta, *next);
    }
}


inline VolumeState* VolumeMonitor::getState(std::vector<int>& indicees) const
{
    int i = 0;
    for(int p = 0; p < indicees.size(); ++p)
    {
        if(volumeSetBucketCounter[p] > 0)
        {
            if(indicees[p] < 0 || indicees[p] > volumeSetBucketCounter[p])
                return NULL;

            i += indicees[p]*indexWeight[p];
        }
    }
    return stateStore + i;
}


inline std::string VolumeMonitor::genStateName(std::vector<int>& indicees) const
{
    using boost::lexical_cast;

    std::string name;
    for(int p = 0; p < indicees.size(); ++p)
    {
        if(volumeSetBucketCounter[p] > 0)
        {
            if(name != "")
                name += ",";
            name += lexical_cast<std::string>(indicees[p]) + "V" + lexical_cast<std::string>(p);
        }
    }
    return name;
}


inline void VolumeMonitor::addVolumeSets(const std::set<int>& set1, const std::set<int>& set2, std::set<int>& result) const
{
    for(std::set<int>::iterator i1 = set1.begin(); i1 != set1.end(); ++i1)
        for(std::set<int>::iterator i2 = set2.begin(); i2 != set2.end(); ++i2)
            result.insert(*i1 + *i2);
}


inline bool* VolumeMonitor::genPossible(const std::set<int>& s) const
{
    bool* possible = initArray<bool>(maxVolumeByAllPieces + 1, false);
    for(std::set<int>::iterator i = s.begin(); i != s.end(); ++i)
        possible[*i] = true;
    return possible;
}

#endif

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

#ifndef PARITYMONITOR_HPP
#define PARITYMONITOR_HPP

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


class ParityMonitor;

class ParityState : public Printable
{
    private:
        std::string   name;
        ParityState** place;
        ParityState** unplace;
        bool*         possible;

    public:
        ParityState() : place(NULL), unplace(NULL), possible(NULL)
        {
        }

        std::ostream& print(std::ostream& os) const { return os << name; }

        friend class ParityMonitor;
};


/** Given a polycube or polyomino puzzle, certain arrangements of various
 ** subsets of the puzzle pieces can be shown to have no solutions due to the
 ** inability of the remaining pieces to achieve the parity of the remaining
 ** holes in the puzzle space.  ParityMonitor identifies these situations
 ** effectively lopping off fruitless branches of the search space.
 **
 ** How to calculate the parity of your puzzle space:
 **    1. Assign each hole in your puzzle a coordinate (x, y, z).
 **    2. If for a particular hole the sum of it's coordinates (x+y+z) is
 **       odd, then color that hole black and define it's parity to be 1;
 **       otherwise, color that hole white and define it's parity to be -1.
 **    3. The parity of the puzzle space is simply the sum of the parity
 **       of all of it's holes.
 **
 ** Here's how to use the class:
 **
 **    1. Instantiate a ParityMonitor object.
 **    2. Invoke targetParity passing in the parity of the puzzle solution
 **       space.
 **    3. For each puzzle piece invoke addElement passing in the magnitude
 **       (absolute value) of the parity of the puzzle piece.
 **    4. Invoke init.  This causes the ParityMonitor object to generate the
 **       internal data structures that enable fast parity checking.
 **    5. Each time you place a piece in the puzzle, invoke place(parity)
 **       passing in the parity of the piece that was placed in the puzzle.
 **    6. After placing a piece, invoke checkParity() to determine if the
 **       puzzle is still solvable.
 **    7. Each time you remove a piece from the puzzle, invoke unplace(parity)
 **       passing in the parity of the piece removed from the puzzle.
 **
 ** The processing overhead for parity checks are negligible compared to DLX.
 ** Steps 1 through 4 are one-time setup actions.  Steps 5, and 7 only require
 ** an add, an array lookup, a pointer dereference, and an increment.  Step 6
 ** amounts to nothing more than a multidimensional array lookup.  (Because
 ** the dimensionality of the array is dependent upon the puzzle pieces, this
 ** array lookup is implemented with a for loop where each iteration of the
 ** loop resolves one array index.)  Anyway, it's quite fast compared to other
 ** search algorithm processing.
 **/
class ParityMonitor : public Printable
{
    private:

        // Parity of remaining holes in the grid.  It is initialized with
        // the parity of the holes remaining in the puzzle after stationary
        // pieces are loaded.
        //
        int targetParity;

        std::vector<int> paritySetBucketCounter;

        // The maximum parity of any individual piece in the puzzle.
        //
        int maxPieceParity;

        // Multiplicative weights of the array-like indicees used to
        // index into the stateStore array.
        //
        std::vector<int> indexWeight;

        // Bound on lower index ever needed for the possible array.  (Used
        // with maxPossibleIndex to size the possible arrays and also to
        // offset the zero position of these arrays allowing the lookup of
        // negative target parities without translation.)
        //
        int minPossibleIndex;

        // Bound on maximum index ever needed for the possible array.
        // (Used with minPossibleIndex to size the possible arrays.)
        //
        int maxPossibleIndex;

        ParityState* state;

        // The number of ParityStates in the stateStore array.
        //
        int numStates;

        // Pointer to the allocated ParityStates.
        //
        ParityState* stateStore;

    public:

        ParityMonitor();
        ~ParityMonitor();
        int getTargetParity();
        void setTargetParity(int targetParity);
        void addElement(int parity);
        void init();
        void place(int parity);
        bool checkParity() const;
        void unplace(int parity);

        /** Returns what would be returned by a checkParity() call if it were
         ** preceded by a place(parity) call, but without actually placing
         ** the piece (and leaving the internal state of the monitor unmodified).
         ** Thus the following two code snipits are equivalent (so long as
         ** the if block doesn't depend on the state of parityMonitor).
         **
         ** // IMPLEMENTATION 1
         ** //
         ** parityMonitor.place(p);
         ** if(parityMonitor.checkParity()) {
         **     // whatever
         ** }
         ** parityMonitor.unplace(p);
         **
         ** // IMPLEMENTATION 2
         ** //
         ** if(parityMonitor(placeCheckParity(p)) {
         **     // whatever
         ** }
         **/
        bool placeCheckParity(int parity) const;

        void clear();
        bool isInitialized();
        std::ostream& print(std::ostream& os) const;

    private:

        void initState(int p, std::vector<int>& indicees, const std::set<int>& base);
        ParityState* getState(std::vector<int>& indicees) const;
        std::string genStateName(std::vector<int>& indicees) const;
        void addParitySets(const std::set<int>& set1, const std::set<int>& set2, std::set<int>& result) const;
        bool* genPossible(const std::set<int>& set) const;
};


inline ParityMonitor::ParityMonitor()
    :
        targetParity(0),
        maxPieceParity(0),
        minPossibleIndex(0),
        maxPossibleIndex(0),
        state(NULL),
        numStates(0),
        stateStore(NULL)
{
}


inline ParityMonitor::~ParityMonitor()
{
    clear();
}


inline int ParityMonitor::getTargetParity()
{
    return this->targetParity;
}


inline void ParityMonitor::setTargetParity(int targetParity)
{
    if(isInitialized())
        throw std::runtime_error("ParityMonitor::targetParity():  Cannot set target parity after init has been called.  Use clear to clear the state of the Parity object and start again.");

    this->targetParity = targetParity;
}


inline void ParityMonitor::addElement(int parity)
{
    if(isInitialized())
        throw std::runtime_error("ParityMonitor::addElement():  Cannot add elements after init has been called.  Use clear to clear the state of the Parity object and start again.");

    parity = abs(parity);

    paritySetBucketCounter.resize(std::max((unsigned) paritySetBucketCounter.size(), (unsigned) (parity + 1)), 0);
    ++paritySetBucketCounter[parity];
}


inline void ParityMonitor::init()
{
    maxPieceParity = paritySetBucketCounter.size() - 1;
    int maxParityByAllPieces = 0;
    numStates = 1;
    std::vector<int> indicees(maxPieceParity+1, 0);
    indexWeight = std::vector<int>(maxPieceParity+1, 0);
    for(int i = 1; i < paritySetBucketCounter.size(); ++i)
    {
        if(paritySetBucketCounter[i] > 0)
        {
            indexWeight[i] = numStates;
            numStates *= (paritySetBucketCounter[i] + 1);
            maxParityByAllPieces += i*paritySetBucketCounter[i];
        }
    }

    // The parity of the remaining holes can vary from (targetParity -
    // maxParityByAllPieces) to (targetParity + maxParityByAllPieces).
    // That's the range of parity indexes that might be used to dip into a
    // possible array during a call to checkParity().  But the actual
    // achievable parities (that are always loaded into the possible arrays in
    // full during initialization -- even though some entries may never be
    // used) range from -maxParityByAllPieces to maxParityByAllPieces.  These
    // two considerations give rise to the lop-sided min and max indexes
    // calculated below.
    //
    minPossibleIndex = std::min(targetParity - maxParityByAllPieces, -maxParityByAllPieces);
    maxPossibleIndex = std::max(targetParity + maxParityByAllPieces, maxParityByAllPieces);
    stateStore = makeArray<ParityState>(numStates);

    std::set<int> base;
    base.insert(0);
    initState(0, indicees, base);
    state = getState(paritySetBucketCounter);
}


inline void ParityMonitor::place(int parity)
{
    targetParity -= parity;
    state = state->place[parity];
}


inline bool ParityMonitor::checkParity() const
{
    return state->possible[targetParity];
}


inline void ParityMonitor::unplace(int parity)
{
    state = state->unplace[parity];
    targetParity += parity;
}


inline bool ParityMonitor::placeCheckParity(int parity) const
{
    return state->place[parity]->possible[targetParity - parity];
}


inline void ParityMonitor::clear()
{
    state = NULL;
    for(int i = 0; i < numStates; ++i)
    {
        deleteArray(stateStore[i].possible + minPossibleIndex, 0);
        deleteArray(stateStore[i].place - maxPieceParity, 0);
        deleteArray(stateStore[i].unplace - maxPieceParity, 0);
    }
    deleteArray(stateStore, 0);

    numStates = 0;
    maxPossibleIndex = 0;
    minPossibleIndex = 0;
    indexWeight.clear();
    maxPieceParity   = 0;
    paritySetBucketCounter.clear();
    targetParity = 0;
}


inline bool ParityMonitor::isInitialized()
{
    return stateStore != NULL;
}


inline std::ostream& ParityMonitor::print(std::ostream& os) const
{
    if(state == NULL)
        return os << "NULL";
    else
        return os << *state;
}


inline void ParityMonitor::initState(int p, std::vector<int>& indicees, const std::set<int>& base)
{
    while(++p < paritySetBucketCounter.size() && paritySetBucketCounter[p] == 0)
        ;

    if(p == paritySetBucketCounter.size())
    {
        ParityState* s = getState(indicees);
        s->name = genStateName(indicees);
        s->possible = genPossible(base);
        s->place    = initArray<ParityState*>(maxPieceParity*2+1, NULL) + maxPieceParity;
        s->unplace  = initArray<ParityState*>(maxPieceParity*2+1, NULL) + maxPieceParity;
        s->place[0] = s;
        s->unplace[0] = s;
        for(int i = 1; i < paritySetBucketCounter.size(); ++i)
        {
            if(paritySetBucketCounter[i] > 0)
            {
                indicees[i] -= 1;
                s->place[i] = s->place[-i] = getState(indicees);
                indicees[i] += 2;
                s->unplace[i] = s->unplace[-i] = getState(indicees);
                indicees[i] -= 1;
            }
        }
        return;
    }

    std::set<int> delta;
    delta.insert(-p);
    delta.insert(p);
    std::set<int> s1 = base;
    std::set<int> s2;

    std::set<int>* current = &s2;
    std::set<int>* next    = &s1;

    for(int i = 0; i <= paritySetBucketCounter[p]; ++i)
    {
        indicees[p] = i;
        initState(p, indicees, *next);

        std::swap(current, next);
        next->clear();
        addParitySets(*current, delta, *next);
    }
}


inline ParityState* ParityMonitor::getState(std::vector<int>& indicees) const
{
    int i = 0;
    for(int p = 1; p < indicees.size(); ++p)
    {
        if(paritySetBucketCounter[p] > 0)
        {
            if(indicees[p] < 0 || indicees[p] > paritySetBucketCounter[p])
                return NULL;

            i += indicees[p]*indexWeight[p];
        }
    }
    return stateStore + i;
}


inline std::string ParityMonitor::genStateName(std::vector<int>& indicees) const
{
    using boost::lexical_cast;

    std::string name;
    for(int p = 1; p < indicees.size(); ++p)
    {
        if(paritySetBucketCounter[p] > 0)
        {
            if(name != "")
                name += ",";
            name += lexical_cast<std::string>(indicees[p]) + "P" + lexical_cast<std::string>(p);
        }
    }
    return name;
}


inline void ParityMonitor::addParitySets(const std::set<int>& set1, const std::set<int>& set2, std::set<int>& result) const
{
    for(std::set<int>::iterator i1 = set1.begin(); i1 != set1.end(); ++i1)
        for(std::set<int>::iterator i2 = set2.begin(); i2 != set2.end(); ++i2)
            result.insert(*i1 + *i2);
}


inline bool* ParityMonitor::genPossible(const std::set<int>& set) const
{
    bool* possible = initArray<bool>(maxPossibleIndex - minPossibleIndex + 1, false) - minPossibleIndex;
    for(std::set<int>::iterator i = set.begin(); i != set.end(); ++i)
        possible[*i] = true;
    return possible;
}

#endif

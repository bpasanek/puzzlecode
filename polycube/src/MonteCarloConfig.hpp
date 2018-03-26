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

#ifndef MONTECARLOCONFIG_HPP
#define MONTECARLOCONFIG_HPP

#include "Printable.hpp"
#include "Scanable.hpp"

class MonteCarloConfig : public Printable, public Scanable
{
    private:
        int numTrials;
        int range;
        long seed;

    public:
        MonteCarloConfig();
        std::ostream& print(std::ostream& os) const;
        std::istream& scan(std::istream& is);

        int getNumTrials() const;
        void setNumTrials(int numTrials);

        int getRange() const;
        void setRange(int range);

        long getSeed() const;
        void setSeed(long seed);

    private:
        void throwSyntax(const std::string& spec);
};


inline MonteCarloConfig::MonteCarloConfig()
    :
        numTrials(0),
        range(0),
        seed(0)
{
}

inline int MonteCarloConfig::getNumTrials() const
{
    return numTrials;
}


inline void MonteCarloConfig::setNumTrials(int numTrials)
{
    this->numTrials = numTrials;;
}


inline int MonteCarloConfig::getRange() const
{
    return range;
}


inline void MonteCarloConfig::setRange(int range)
{
    this->range = range;
}


inline long MonteCarloConfig::getSeed() const
{
    return seed;
}


inline void MonteCarloConfig::setSeed(long seed)
{
    this->seed = seed;
}


#endif

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

#include "MonteCarloConfig.hpp"
#include <sstream>
#include <stdexcept>
#include "boost/lexical_cast.hpp"


std::ostream& MonteCarloConfig::print(std::ostream& os) const
{
    return os << numTrials << "," << range << "," << seed;
}

std::istream& MonteCarloConfig::scan(std::istream& is)
{
    using boost::lexical_cast;

    std::string spec;
    is >> spec;

    int comma = spec.find(',');
    if(comma == -1)
        throwSyntax(spec);
    numTrials = lexical_cast<int>(spec.substr(0, comma));

    std::string s = spec.substr(comma + 1);
    comma = s.find(',');
    if(comma == -1)
        throwSyntax(spec);
    range = lexical_cast<int>(s.substr(0, comma));

    s = s.substr(comma + 1);
    seed = lexical_cast<int>(s);

    return is;
}

void MonteCarloConfig::throwSyntax(const std::string& spec)
{
    std::ostringstream errMsg;
    errMsg <<
        "***Monte Carlo Configuration Syntax Error:  '" << spec << "'\n\n"
        "Monte Carlo configuration settings must have the format T,R,S\n"
        "where T is the integer number of trials and R is the test range\n"
        "(the number of remaining pieces that triggers the start and\n"
        "end of a trial), and S is the seed value for the random number\n"
        "generator (a 64 signed bit integer).\n";

    throw std::runtime_error(errMsg.str());
}

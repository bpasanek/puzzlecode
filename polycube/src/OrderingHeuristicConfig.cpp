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

#include "OrderingHeuristicConfig.hpp"
#include <sstream>
#include <stdexcept>
#include "boost/lexical_cast.hpp"
#include "boost/tokenizer.hpp"
#include "limits.h"
#include "util.hpp"

using namespace std;

const map<int,string>& OrderingHeuristicConfig::getDefList() const
{
    return defList;
}


void OrderingHeuristicConfig::setDefList(const map<int,string>& defList)
{
    this->defList = defList;
}


ostream& OrderingHeuristicConfig::print(ostream& os) const
{
    bool first = true;
    for(map<int,string>::const_reverse_iterator i = defList.rbegin(); i != defList.rend(); ++i)
    {
        if(!first)
            os << ":";
        else
            first = false;

        os << (*i).second;
    }
    return os;
}


istream& OrderingHeuristicConfig::scan(istream& is)
{
    using boost::lexical_cast;
    typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
    typedef boost::char_separator<char> char_separator;

    string spec;
    is >> spec;

    tokenizer specTok(spec, char_separator(":"));
    for(tokenizer::iterator i = specTok.begin(); i != specTok.end(); ++i)
    {
        string ohSpec = *i;
        tokenizer ohSpecTok(ohSpec, char_separator("="));
        int c = countAll(ohSpecTok.begin(), ohSpecTok.end());
        if(c > 2)
            throwSyntax(spec);

        string ohDef = *ohSpecTok.begin();
        int numRemainingPieces = INT_MAX;
        if(c == 2)
            numRemainingPieces = lexical_cast<int>(*++ohSpecTok.begin());
        defList[numRemainingPieces] = ohDef;
    }
    return is;
}


void OrderingHeuristicConfig::throwSyntax(const string& spec)
{
    ostringstream errMsg;
    errMsg <<
        "***Ordering Heuristic Configuration Syntax Error:  '" << spec << "'\n\n"
        "Ordering heuristic configuration settings must have the format\n"
        "O[(ARGS)][=R] where O is the name of an ordering heuristic, ARGS\n"
        "arguments for the heuristic, and R is the range (the number of\n"
        "remaining pieces to begin using this heuristic).  If R is not\n"
        "specified then O becomes default heuristic and will be used until\n"
        "the number of remaining pieces reaches the enabling threshold\n"
        "for another heuristic.\n";

    throw runtime_error(errMsg.str());
}

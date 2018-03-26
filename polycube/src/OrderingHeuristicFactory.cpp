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

#include "OrderingHeuristicFactory.hpp"
#include "AngularOrderingHeuristic.hpp"
#include "RadialOrderingHeuristic.hpp"
#include "LinearOrderingHeuristic.hpp"
#include "FitOrderingHeuristic.hpp"
#include "Constants.hpp"
#include "util.hpp"
#include <sstream>
#include <stdexcept>
#include "boost/lexical_cast.hpp"
#include "boost/tokenizer.hpp"

using namespace std;

OrderingHeuristicFactory::OrderingHeuristicFactory(double defaultXc, double defaultYc, double defaultZc)
    :
        defaultXc(defaultXc), defaultYc(defaultYc), defaultZc(defaultZc)
{
}


OrderingHeuristic* OrderingHeuristicFactory::gen(const std::string& def)
{
    using boost::lexical_cast;

    typedef boost::tokenizer<boost::char_separator<char> > Tokenizer;
    typedef boost::char_separator<char> CharSeparator;

    string name = def;
    string args = "";

    int atSymbol = def.find('@');
    if(atSymbol != -1)
    {
        name = def.substr(0, atSymbol);
        args = def.substr(atSymbol + 1);
    }


    if(args != "" && (name == "f" || name == "x" || name == "X" || name == "y" || name == "Y" || name == "z" || name == "Z" || name == "xyz"))
    {
            ostringstream errMsg;
            errMsg <<
                "***Ordering Heuristic Configuration Syntax Error:  '" << def << "'\n\n"
                "Ordering heuristic " << name << " takes no arguments.\n";

            throw runtime_error(errMsg.str());
    }

    if(name == "f")
        return new FitOrderingHeuristic();

    if(name == "x")
       return new LinearOrderingHeuristic(1, 0, 0);

    if(name == "X")
        return new LinearOrderingHeuristic(-1, 0, 0);

    if(name == "y")
        return new LinearOrderingHeuristic(0, 1, 0);

    if(name == "Y")
        return new LinearOrderingHeuristic(0, -1, 0);

    if(name == "z")
        return new LinearOrderingHeuristic(0, 0, 1);

    if(name == "Z")
        return new LinearOrderingHeuristic(0, 0, -1);

    if(name == "xyz")
        return new LinearOrderingHeuristic(1, 1, 1);

    Tokenizer argTok(args, CharSeparator(","));
    int c = countAll(argTok.begin(), argTok.end());
    Tokenizer::iterator i = argTok.begin();

    if(name == "l")
    {
        if(c != 3)
        {
            ostringstream errMsg;
            errMsg <<
                "***Linear Ordering Heuristic Configuration Syntax Error:  '" << def << "'\n\n"
                "The args for a linear ordering heuristic configuration settings\n"
                "must be three comma separated floating point numbers.\n";

            throw runtime_error(errMsg.str());
        }

        double a = lexical_cast<double>(*i++);
        double b = lexical_cast<double>(*i++);
        double c = lexical_cast<double>(*i++);
        return new LinearOrderingHeuristic(a, b, c);
    }

    if(name == "a" || name == "A")
    {
        double initialAngle = 0.0;
        if(i != argTok.end())
            initialAngle = lexical_cast<double>(*i++) / 180.0 * PI;

        double xc = defaultXc;
        if(i != argTok.end())
            xc = lexical_cast<double>(*i++);

        double yc = defaultYc;
        if(i != argTok.end())
            yc = lexical_cast<double>(*i++);

        return new AngularOrderingHeuristic(initialAngle, xc, yc, name == "A");
    }

    if(name == "R")
    {
        double xc = defaultXc;
        if(i != argTok.end())
            xc = lexical_cast<double>(*i++);

        double yc = defaultYc;
        if(i != argTok.end())
            yc = lexical_cast<double>(*i++);

        double zc = defaultZc;
        if(i != argTok.end())
            zc = lexical_cast<double>(*i++);

        return new RadialOrderingHeuristic(xc, yc, zc);
    }

    ostringstream errMsg;
    errMsg <<
        "***Unknown Ordering Heuristic:  '" << def << "'\n\n"
        "Known heuristics are:  a, A, f, R, x, X, y, Y, z, Z xyz.\n";

    throw runtime_error(errMsg.str());
}

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

#include "PerformanceRegistry.hpp"
#include "PerformanceMeter.hpp"
#include <sstream>
#include <stdexcept>

PerformanceMeter* PerformanceRegistry::getMeter(const std::string& name)
{
    PerformanceMeter* pm;
    std::map<std::string,PerformanceMeter*>::iterator i = registry.find(name);
    if(i != registry.end())
        pm = (*i).second;
    else
        registry[name] = pm = new PerformanceMeter(name, NULL);
    return pm;
}

PerformanceMeter* PerformanceRegistry::getMeter(const std::string& name, const std::string& parentName)
{
    PerformanceMeter* pm;
    std::map<std::string,PerformanceMeter*>::iterator i = registry.find(name);
    if(i != registry.end())
    {
        pm = (*i).second;
        if(pm->getParent() == NULL)
        {
            PerformanceMeter* parent = getMeter(parentName);
            pm->setParent(parent);
            parent->addChild(pm);
        }
    }
    else
    {
        PerformanceMeter* parent = getMeter(parentName);
        registry[name] = pm = new PerformanceMeter(name, parent);
        parent->addChild(pm);
    }
    return pm;
}

std::ostream& PerformanceRegistry::print(std::ostream& os) const
{
    os <<
        "NAME                          :      TOTAL TIME                    : PERCENTAGE                 :  NUM EVENTS :     AVG TIME   \n"
        "_______________________________________________________________________________________________________________________________\n\n";

    for(std::map<std::string,PerformanceMeter*>::const_iterator i = registry.begin();
        i != registry.end(); ++i)
    {
        if((*i).second->getParent() == NULL)
            os << *(*i).second;
    }
    return os;
}

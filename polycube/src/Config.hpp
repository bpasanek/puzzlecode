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

#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "FileConfig.hpp"
#include "CmdLineConfig.hpp"
#include "HiddenConfig.hpp"

/** Config binds the three sets of program options defined in classes
 ** FileConfig, CmdLineConfig, and HiddenConfig into a single class via
 ** mulitple inheritance.
 **/
class Config : public FileConfig, public CmdLineConfig, public HiddenConfig
{
    public:
        std::ostream& print(std::ostream& os) const;
        void validate() const;
};


inline std::ostream& Config::print(std::ostream& os) const
{
    FileConfig::print(os);
    CmdLineConfig::print(os);
    HiddenConfig::print(os);
    return os;
}


inline std::ostream& operator << (std::ostream& os, const Config& config)
{
    return config.print(os);
}

#endif

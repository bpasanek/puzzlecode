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

#ifndef HIDDENCONFIG_HPP
#define HIDDENCONFIG_HPP

#include <boost/program_options.hpp>
#include "Printable.hpp"

class HiddenConfig : public Printable
{
    private:
        std::vector<std::string> inputFileList;

    public:
        HiddenConfig();
        const std::vector<std::string>& getInputFileList() const;
        void setInputFileList(const std::vector<std::string>& inputFileList);
        void add_options(boost::program_options::options_description& desc);
        std::ostream& print(std::ostream& os) const;
};


inline HiddenConfig::HiddenConfig()
{
}

inline const std::vector<std::string>& HiddenConfig::getInputFileList() const
{
    return inputFileList;
}

inline void HiddenConfig::setInputFileList(const std::vector<std::string>& inputFileList)
{
    this->inputFileList = inputFileList;
}

inline void HiddenConfig::add_options(boost::program_options::options_description& desc)
{
    desc.add_options()
        ("inputFile", boost::program_options::value< std::vector<std::string> >(&inputFileList), "input file")
    ;
}

inline std::ostream& HiddenConfig::print(std::ostream& os) const
{
    os << "inputFileList={";
    for(int i = 0; i < getInputFileList().size(); ++i)
        os << getInputFileList()[i];
    return os << "}";
}

#endif

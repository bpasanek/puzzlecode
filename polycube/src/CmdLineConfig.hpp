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

#ifndef CMDLINECONFIG_HPP
#define CMDLINECONFIG_HPP

#include <boost/program_options.hpp>
#include "Printable.hpp"

/** Program options settable only on the commmand line.
 **/
class CmdLineConfig : public Printable
{
        std::string conf;
        bool        help;
        bool        version;

    public:

        CmdLineConfig();
        const std::string& getConf() const;
        void setConf(const std::string& value);
        bool getHelp() const;
        void setHelp(bool value);
        bool getVersion() const;
        void setVersion(bool value);
        void add_options(boost::program_options::options_description& desc);
        std::ostream& print(std::ostream& os) const;
};


inline CmdLineConfig::CmdLineConfig()
{
    conf    = "";
    help    = false;
    version = false;
}


inline const std::string& CmdLineConfig::getConf() const
{
    return conf;
}


inline void CmdLineConfig::setConf(const std::string& value)
{
    conf = value;
}


inline bool CmdLineConfig::getHelp() const
{
    return help;
}


inline void CmdLineConfig::setHelp(bool value)
{
    help = value;
}


inline bool CmdLineConfig::getVersion() const
{
    return version;
}


inline void CmdLineConfig::setVersion(bool value)
{
    version = value;
}


inline void CmdLineConfig::add_options(boost::program_options::options_description& desc)
{
    using boost::lexical_cast;

    desc.add_options()
        ("conf",    boost::program_options::value<std::string> (&conf),                   "Set configuration file name.\n")
        ("help",    boost::program_options::value<bool> (&help)->implicit_value(true),    "Print program synopsis and exit.\n")
        ("version", boost::program_options::value<bool> (&version)->implicit_value(true), "Print version information and exit.\n")
    ;
}


inline std::ostream& CmdLineConfig::print(std::ostream& os) const
{
    return os <<
        "conf="    << getConf()    << "\n" <<
        "help="    << getHelp()    << "\n" <<
        "version=" << getVersion() << std::endl;
}

#endif

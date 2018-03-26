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

#ifndef FILECONFIG_HPP
#define FILECONFIG_HPP

#include <boost/lexical_cast.hpp>
#include <boost/program_options.hpp>
#include "Constants.hpp"
#include "Printable.hpp"
#include "MonteCarloConfig.hpp"
#include "OutputFormatConfig.hpp"
#include "OrderingHeuristicConfig.hpp"

/** List of options allowed in configuration files and the command line.
 **/
class FileConfig : public Printable
{
    private:

        int                     bruijn;
        int                     emch;
        OutputFormatConfig      outputFormat;
        int                     fitFilter;
        int                     goal;
        bool                    info;
        int                     mch;
        MonteCarloConfig        monteCarlo;
        OrderingHeuristicConfig orderingHeuristicConfig;
        bool                    parityBacktrack;
        int                     parityFilter;
        bool                    quiet;
        std::string             redundancyFilter;
        bool                    redundancyFilterFirst;
        int                     trace;
        bool                    unique;
        int                     volumeBacktrack;
        int                     volumeFilter;

    public:

        FileConfig();
        void add_options(boost::program_options::options_description& desc);
        int  getBruijn() const;
        void setBruijn(int bruijn);
        int  getEmch() const;
        void setEmch(int emch);
        const OutputFormatConfig& getOutputFormat() const;
        void setOutputFormat(const OutputFormatConfig& outputFormat);
        int  getFitFilter() const;
        void setFitFilter(int fitFilter);
        int  getGoal() const;
        void setGoal(int goal);
        bool getInfo() const;
        void setInfo(bool info);
        int  getMch() const;
        void setMch(int mch);
        const MonteCarloConfig& getMonteCarlo() const;
        void setMonteCarlo(const MonteCarloConfig& monteCarlo);
        const OrderingHeuristicConfig& getOrderingHeuristicConfig() const;
        void setOrderingHeuristicConfig(const OrderingHeuristicConfig& orderingHeuristicConfig);
        bool getParityBacktrack() const;
        void setParityBacktrack(bool parityBacktrack);
        int  getParityFilter() const;
        void setParityFilter(int parityFilter);
        bool getQuiet() const;
        void setQuiet(bool quiet);
        const std::string& getRedundancyFilter() const;
        void setRedundancyFilter(const std::string& redundancyFilter);
        bool getRedundancyFilterFirst() const;
        void setRedundancyFilterFirst(bool redundancyFilterFirst);
        int  getTrace() const;
        void setTrace(int trace);
        bool getUnique() const;
        void setUnique(bool unique);
        int  getVolumeBacktrack() const;
        void setVolumeBacktrack(int volumeBacktrack);
        int  getVolumeFilter() const;
        void setVolumeFilter(int volumeFilter);
        std::ostream& print(std::ostream& os) const;
};

inline FileConfig::FileConfig()
    :
        bruijn(0),
        emch(0),
        fitFilter(FILTER_OFF),
        goal(0),
        info(false),
        mch(0),
        parityBacktrack(false),
        parityFilter(FILTER_OFF),
        quiet(false),
        redundancyFilter(REDUNDANCY_FILTER_OFF_NAME),
        redundancyFilterFirst(false),
        trace(0),
        unique(false),
        volumeBacktrack(0),
        volumeFilter(FILTER_OFF)
{
}

inline int FileConfig::getBruijn() const
{
    return bruijn;
}

inline void FileConfig::setBruijn(int bruijn)
{
    this->bruijn = bruijn;
}

inline int FileConfig::getEmch() const
{
    return emch;
}

inline void FileConfig::setEmch(int emch)
{
    this->emch = emch;
}

inline const OutputFormatConfig& FileConfig::getOutputFormat() const
{
    return outputFormat;
}

inline void FileConfig::setOutputFormat(const OutputFormatConfig& outputFormat)
{
    this->outputFormat = outputFormat;
}

inline int FileConfig::getFitFilter() const
{
    return fitFilter;
}

inline void FileConfig::setFitFilter(int fitFilter)
{
    this->fitFilter = fitFilter;
}

inline int FileConfig::getGoal() const
{
    return goal;
}

inline void FileConfig::setGoal(int goal)
{
    this->goal = goal;
}

inline bool FileConfig::getInfo() const
{
    return info;
}

inline void FileConfig::setInfo(bool info)
{
    this->info = info;
}

inline int FileConfig::getMch() const
{
    return mch;
}

inline void FileConfig::setMch(int mch)
{
    this->mch = mch;
}

inline const MonteCarloConfig& FileConfig::getMonteCarlo() const
{
    return monteCarlo;
}

inline void FileConfig::setMonteCarlo(const MonteCarloConfig& monteCarlo)
{
    this->monteCarlo = monteCarlo;
}

inline const OrderingHeuristicConfig& FileConfig::getOrderingHeuristicConfig() const
{
    return orderingHeuristicConfig;
}

inline void FileConfig::setOrderingHeuristicConfig(const OrderingHeuristicConfig& orderingHeuristicConfig)
{
    this->orderingHeuristicConfig = orderingHeuristicConfig;
}

inline bool FileConfig::getParityBacktrack() const
{
    return parityBacktrack;
}

inline void FileConfig::setParityBacktrack(bool parityBacktrack)
{
    this->parityBacktrack = parityBacktrack;
}

inline int FileConfig::getParityFilter() const
{
    return parityFilter;
}

inline void FileConfig::setParityFilter(int parityFilter)
{
    this->parityFilter = parityFilter;
}

inline bool FileConfig::getQuiet() const
{
    return quiet;
}

inline void FileConfig::setQuiet(bool quiet)
{
    this->quiet = quiet;
}

inline const std::string& FileConfig::getRedundancyFilter() const
{
    return redundancyFilter;
}

inline void FileConfig::setRedundancyFilter(const std::string& redundancyFilter)
{
    this->redundancyFilter = redundancyFilter;
}

inline bool FileConfig::getRedundancyFilterFirst() const
{
    return redundancyFilterFirst;
}

inline void FileConfig::setRedundancyFilterFirst(bool redundancyFilterFirst)
{
    this->redundancyFilterFirst = redundancyFilterFirst;
}

inline int FileConfig::getTrace() const
{
    return trace;
}

inline void FileConfig::setTrace(int trace)
{
    this->trace = trace;
}

inline bool FileConfig::getUnique() const
{
    return unique;
}

inline void FileConfig::setUnique(bool unique)
{
    this->unique = unique;
}

inline int FileConfig::getVolumeBacktrack() const
{
    return volumeBacktrack;
}

inline void FileConfig::setVolumeBacktrack(int volumeBacktrack)
{
    this->volumeBacktrack = volumeBacktrack;
}

inline int FileConfig::getVolumeFilter() const
{
    return volumeFilter;
}

inline void FileConfig::setVolumeFilter(int volumeFilter)
{
    this->volumeFilter = volumeFilter;
}

inline std::ostream& FileConfig::print(std::ostream& os) const
{
    return os <<
        "bruijn="                  << getBruijn()                  << "\n" <<
        "emch="                    << getEmch()                    << "\n" <<
        "outputFormat="            << getOutputFormat()            << "\n" <<
        "fitFilter="               << getFitFilter()               << "\n" <<
        "goal="                    << getGoal()                    << "\n" <<
        "info="                    << getInfo()                    << "\n" <<
        "mch="                     << getMch()                     << "\n" <<
        "orderingHeuristicConfig=" << getOrderingHeuristicConfig() << "\n" <<
        "parityBacktrack="         << getParityBacktrack()         << "\n" <<
        "parityFilter="            << getParityFilter()            << "\n" <<
        "quiet="                   << getQuiet()                   << "\n" <<
        "redundancyFilter="        << getRedundancyFilter()        << "\n" <<
        "redundancyFilterFirst="   << getRedundancyFilterFirst()   << "\n" <<
        "sample="                  << getMonteCarlo()              << "\n" <<
        "trace="                   << getTrace()                   << "\n" <<
        "unique="                  << getUnique()                  << "\n" <<
        "volumeBacktrack="         << getVolumeBacktrack()         << "\n" <<
        "volumeFilter="            << getVolumeFilter()            << std::endl;
}

#endif

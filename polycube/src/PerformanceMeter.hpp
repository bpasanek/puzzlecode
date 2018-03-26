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

#ifndef PERFORMANCEMETER_HPP
#define PERFORMANCEMETER_HPP

#include <sys/time.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <iostream>
#include <iomanip>
#include "Printable.hpp"
#include "PerformanceRegistry.hpp"
#include <sstream>
#include <stdexcept>


/** PerformanceMeter, PerformanceMeasure and PerformanceRegistry are just some
 ** quick-and-dirty classes I threw together to measure how much time is spent
 ** in different parts of the program.  To use this class, you simply acquire
 ** a PerformanceMeter from some PerformanceRegistry; then invoke start
 ** whenever you want to start measuring elapsed time for some task, and
 ** invoke stop when you've reached the end of the task.  You can repeat
 ** these operations over and over and the Meter will aggregate the total
 ** time and will also keep track of the number invocations of start-and-stop.
 ** Calling print will format this information on an ostream.
 **
 ** A PerformanceMeter has a name, and can also have a parent PerformanceMeter.
 ** When the PerformanceMeter is printed, it includes the percentage of time
 ** this meter consumed relative to the parent.  Printing a PerformanceRegistry
 ** causes all meters to be displayed at once with parent-child relationships
 ** shown through indentation.
 **
 ** On my computer the cost of a gettimeofday call is almost a microsecond
 ** (really huge), so care must be taken not to start or stop time
 ** measurements anywhere such overhead is unacceptable.
 **
 ** See also PerformanceMeasure which is the preferred way of invoking
 ** start and stop on a PerformanceMeter.
 **/
class PerformanceMeter : public Printable
{
    private:

        std::string name;
        PerformanceMeter* parent;
        std::vector<PerformanceMeter*> children;
        bool active;
        struct timeval startTime;
        long long microseconds;
        long long numMeasurements;

        PerformanceMeter(const std::string& name, PerformanceMeter* parent);
        void setParent(PerformanceMeter* parent);
        void addChild(PerformanceMeter* child);

    public:

        void start();
        void stop();
        bool isRunning() const;
        const std::string& getName() const;
        long long getNumEvents() const;

        /** Returns the total number of microseconds this performance meter
         ** has been running.
         **/
        long long getTime() const;

        /** Returns the total number of microseconds this performance meter
         ** has been running.  If the meter is stopped, then it simply returns
         ** the aggregated time stored in the meter.  If the meter is running
         ** the currently elapsed time for the active metering event is added
         ** with the stored total for past metering events and that sum is
         ** returned.  In this case, to calculate the elapsed time of the
         ** currently running meter event, the internally stored start time is
         ** substracted from the current time.  If the currentTime argument is
         ** non-zero, then this value is used as the current time in this
         ** calculation.  If the currentTime is zero, then a system call is
         ** first made to gettimeofday and the results are loaded to
         ** currentTime and the calculation of the total elapsed time proceeds
         ** as before.  The caller can then use the updated value in
         ** currentTime for subsequent related getTime calls on other meters.
         **/
        long long getTime(long long& currentTime) const;
        PerformanceMeter* getParent() const;
        void clear();
        std::ostream& print(std::ostream& os) const;
        std::ostream& print(std::ostream& os, long long currentTime) const;

        friend class PerformanceRegistry;
};


inline PerformanceMeter::PerformanceMeter(const std::string& name, PerformanceMeter* parent)
    :
        name(name),
        parent(parent),
        active(false),
        microseconds(0),
        numMeasurements(0)
{
}


inline void PerformanceMeter::setParent(PerformanceMeter* parent)
{
    this->parent = parent;
}


inline void PerformanceMeter::addChild(PerformanceMeter* child)
{
    children.push_back(child);
}


inline void PerformanceMeter::start()
{
    if(!active)
    {
        gettimeofday(&startTime, NULL);
        ++numMeasurements;
        active = true;
        if(parent && !parent->isRunning())
        {
            std::ostringstream errMsg;
            errMsg << "***Performance Meter Exception:\n\n"
                "Attempt to start meter " << name << " when it's parent (" << parent->getName() << ") is not running." << std::endl;
            throw std::runtime_error(errMsg.str());
        }
    }
    else
    {
        std::ostringstream errMsg;
        errMsg << "***Performance Meter Exception:\n\n"
            "Attempt to start meter " << name << " when it was already started." << std::endl;
        throw std::runtime_error(errMsg.str());
    }
}


inline void PerformanceMeter::stop()
{
    if(active)
    {
        struct timeval stopTime;
        gettimeofday(&stopTime, NULL);
        microseconds += (stopTime.tv_sec - startTime.tv_sec) * 1000000LL +
            (stopTime.tv_usec - startTime.tv_usec);
        active = false;
        if(parent && !parent->isRunning())
        {
            std::ostringstream errMsg;
            errMsg << "***Performance Meter Exception:\n\n"
                "Parent meter " << parent->getName() << " was apparently stopped before prior to stop request made on meter " << name << "." << std::endl;
            throw std::runtime_error(errMsg.str());
        }
    }
    else
    {
        std::ostringstream errMsg;
        errMsg << "***Performance Meter Exception:\n\n"
            "Attempt to stop meter " << name << " when it was already stopped." << std::endl;
        throw std::runtime_error(errMsg.str());
    }
}


inline bool PerformanceMeter::isRunning() const
{
    return active;
}


inline const std::string& PerformanceMeter::getName() const
{
    return name;
}


inline long long PerformanceMeter::getNumEvents() const
{
    return numMeasurements;
}


inline long long PerformanceMeter::getTime() const
{
    if(active)
    {
        long long currentTime = 0LL;
        return getTime(currentTime);
    }
    return microseconds;
}


inline long long PerformanceMeter::getTime(long long& currentTime) const
{
    if(active)
    {
        if(currentTime == 0)
        {
            struct timeval ct;
            gettimeofday(&ct, NULL);
            currentTime = ct.tv_sec * 1000000LL + ct.tv_usec;
        }
        return microseconds + currentTime - (startTime.tv_sec * 1000000LL + startTime.tv_usec);
    }
    return microseconds;
}


inline PerformanceMeter* PerformanceMeter::getParent() const
{
    return parent;
}


inline void PerformanceMeter::clear()
{
    microseconds = 0;
    numMeasurements = 0;
    for(int i = 0; i < children.size(); ++i)
        children[i]->clear();
}


inline std::ostream& PerformanceMeter::print(std::ostream& os) const
{
    return print(os, 0LL);
}


inline std::ostream& PerformanceMeter::print(std::ostream& os, long long currentTime) const
{
    int indent = 0;
    for(PerformanceMeter* p = parent; p != NULL; p = p->getParent())
        indent += 4;

    long long t = getTime(currentTime);

    os << std::fixed << std::setprecision(6) <<
        std::setw(indent) << "" << std::setw(30-indent) << std::left << name << ":" <<
        std::setw(16+indent) << std::right << ((double) t / 1000000) <<
        std::setw(21-indent) << ":";
    if(parent)
    {
        double pct = 0.0;
        if(t)
            pct = ((double) t / parent->getTime()) * 100;
        os << std::setw(7+indent) << std::setprecision(3) <<
            pct << "%" << std::setw(21-indent) << ":";
    }
    else
    {
        os << " *" << std::setw(27) << ":";
    }

    os << std::setw(13) << numMeasurements << ":" <<
        std::setw(16) << std::setprecision(3) << (numMeasurements > 0 ?
        ((double) t / numMeasurements / 1000000) : 0.00) << std::endl;

    for(int i = 0; i < children.size(); ++i)
        children[i]->print(os, currentTime);

    return os;
}


#endif

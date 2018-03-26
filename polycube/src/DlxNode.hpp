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

#ifndef DLX_NODE_HPP
#define DLX_NODE_HPP

#include <iostream>
#include "Printable.hpp"

class DlxHead;
class Image;

class DlxNode : public Printable
{
    public:
        DlxNode* left;
        DlxNode* right;
        DlxNode* up;
        DlxNode* down;
        DlxHead* head;
        Image*   image;

        DlxNode();
        virtual ~DlxNode();
        std::ostream& print(std::ostream& os) const;
};

inline DlxNode::DlxNode()
    :
        left(NULL),
        right(NULL),
        up(NULL),
        down(NULL),
        image(NULL)
{
}

inline DlxNode::~DlxNode()
{
}

inline std::ostream& DlxNode::print(std::ostream& os) const
{
    return os;
}

#endif

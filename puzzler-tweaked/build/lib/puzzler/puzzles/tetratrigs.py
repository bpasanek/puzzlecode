#!/usr/bin/env python
# $Id: tetratrigs.py 600 2015-02-24 20:21:02Z goodger $

# Author: David Goodger <goodger@python.org>
# Copyright: (C) 1998-2015 by David J. Goodger
# License: GPL 2 (see __init__.py)

"""
Concrete tetratrig puzzles.
"""

from puzzler.puzzles.polytrigs import Tetratrigs


class TetratrigsElongatedHex11x3(Tetratrigs):

    """ solutions."""

    width = 15
    height = 7

    def coordinates(self):
        holes = set([(6, 4, 0), (7, 2, 0)])
        for coord in self.coordinates_elongated_hexagon(11, 3):
            if coord not in holes:
                yield coord

    def customize_piece_data(self):
        self.piece_data['P04'][-1]['flips'] = None
        self.piece_data['O04'][-1]['flips'] = None
        self.piece_data['O04'][-1]['rotations'] = (1,)

    def build_matrix(self):
        """"""
        keys = sorted(self.pieces.keys())
        o_coords, o_aspect = self.pieces['O04'][0]
        translated = o_aspect.translate((6, 3, 0))
        self.build_matrix_row('O04', translated)
        keys.remove('O04')
        self.build_regular_matrix(keys)

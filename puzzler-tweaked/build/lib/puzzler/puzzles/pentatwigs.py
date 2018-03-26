#!/usr/bin/env python
# -*- coding: utf-8 -*-
# $Id: pentatwigs.py 600 2015-02-24 20:21:02Z goodger $

# Author: David Goodger <goodger@python.org>
# Copyright: (C) 1998-2015 by David J. Goodger
# License: GPL 2 (see __init__.py)

"""
Concrete pentatwig puzzles.
"""

from puzzler.puzzles.polytwigs import Pentatwigs, OneSidedPentatwigs


class PentatwigsTriangle(Pentatwigs):

    """56 solutions"""

    height = 6
    width = 6

    def coordinates(self):
        return self.coordinates_triangle(5)

    def customize_piece_data(self):
        self.piece_data['R5'][-1]['flips'] = None
        self.piece_data['R5'][-1]['rotations'] = (0, 1)


class PentatwigsTriangle2(PentatwigsTriangle):

    """
    6 solutions

    Puzzle design by Colin F. Brown.
    """

    holes = set(((1,2,0), (2,1,1), (2,2,2)))
    extras = set(((0,0,2), (0,5,1), (5,0,0)))

    def coordinates(self):
        for coord in self.coordinates_triangle(5):
            if coord not in self.holes:
                yield coord
        for coord in self.extras:
            yield self.coordinate_offset(*coord, offset=None)


class PentatwigsTriangle3(PentatwigsTriangle2):

    """
    2 solutions

    Puzzle design by Colin F. Brown.
    """

    holes = set(((1,1,2), (1,3,1), (3,1,0)))


class Pentatwigs5x3(Pentatwigs):

    """194 solutions"""

    height = 4
    width = 6

    def coordinates(self):
        return self.coordinates_bordered(5, 3)

    def customize_piece_data(self):
        self.piece_data['R5'][-1]['rotations'] = (0, 1, 2)


class PentatwigsChevron3x3(Pentatwigs):

    """115 solutions"""

    height = 6
    width = 6

    def coordinates(self):
        return self.coordinates_chevron(3, 3)

    def customize_piece_data(self):
        self.piece_data['R5'][-1]['flips'] = None


class PentatwigsChevron5x2(Pentatwigs):

    """241 solutions"""

    height = 4
    width = 7

    def coordinates(self):
        return self.coordinates_chevron(5, 2)

    def customize_piece_data(self):
        self.piece_data['R5'][-1]['flips'] = None


class PentatwigsTrapezoid6x3(Pentatwigs):

    """184 solutions"""

    height = 4
    width = 7

    def coordinates(self):
        return self.coordinates_trapezoid(6, 3)

    def customize_piece_data(self):
        self.piece_data['R5'][-1]['flips'] = None


class PentatwigsHexagonRing(Pentatwigs):

    """0 solutions"""

    height = 6
    width = 6

    def coordinates(self):
        hole = set(self.coordinates_hexagon_unbordered(2, offset=(1, 1, 0)))
        for coord in self.coordinates_hexagon(3):
            if coord in hole:
                continue
            yield coord

    def customize_piece_data(self):
        self.piece_data['R5'][-1]['flips'] = None
        self.piece_data['R5'][-1]['rotations'] = None


class PentatwigsStaggeredRectangle5x3(Pentatwigs):

    """145 solutions"""

    height = 6
    width = 6

    svg_rotation = 0

    def coordinates(self):
        return self.coordinates_vertically_staggered_rectangle(5, 3)

    def customize_piece_data(self):
        self.piece_data['R5'][-1]['flips'] = None


class PentatwigsWaveStaggeredRectangle5x3(Pentatwigs):

    """
    202 solutions

    Puzzle design by Colin F. Brown.
    """

    height = 6
    width = 6

    holes = set(((3,4,0), (3,4,2), (4,3,1)))

    extras = set(((3,0,0), (3,0,1), (4,0,2)))

    svg_rotation = 0

    def coordinates(self):
        coords = set(self.coordinates_vertically_staggered_rectangle(5, 3))
        coords.difference_update(self.holes)
        coords.update(
            set(self.coordinate_offset(*coord, offset=None)
                for coord in self.extras))
        return sorted(coords)

    def customize_piece_data(self):
        self.piece_data['R5'][-1]['rotations'] = (0, 1, 2)


class PentatwigsButterfly1(Pentatwigs):

    """8 solutions"""

    height = 6
    width = 7

    hole = set(((2,3,0), (3,2,0), (3,2,1), (3,2,2), (3,3,2)))

    svg_rotation = -60

    def coordinates(self):
        for coord in self.coordinates_butterfly(4, 3):
            if coord not in self.hole:
                yield coord

    def customize_piece_data(self):
        self.piece_data['R5'][-1]['flips'] = None
        self.piece_data['R5'][-1]['rotations'] = (0,1,2)


class PentatwigsButterfly_X(PentatwigsButterfly1):

    """0 solutions"""

    hole = set(((2,4,0), (2,4,2), (3,1,0), (4,0,1), (4,1,2)))


class PentatwigsTrefoil(Pentatwigs):

    """
    4 solutions, all equivalent to the solutions to Pentatwigs3Congruent1.

    Puzzle design by Colin F. Brown.
    """

    height = 6
    width = 6

    hole = set((
        (2,0,0), (2,0,1), (3,0,2), (2,1,0),
        (0,4,1), (0,5,0), (0,5,2), (1,4,2),
        (4,2,1), (4,3,0), (5,2,1), (5,2,2)))

    svg_rotation = 0

    def coordinates(self):
        for coord in self.coordinates_hexagon(3):
            if coord not in self.hole:
                yield coord

    def customize_piece_data(self):
        self.piece_data['R5'][-1]['flips'] = None
        self.piece_data['R5'][-1]['rotations'] = (0, 1)


class Pentatwigs3Congruent1(Pentatwigs):

    """
    4 solutions

    Puzzle design by Colin F. Brown.
    """

    height = 7
    width = 7

    coords = [
        (0, 2, 0), (0, 2, 1), (0, 3, 0), (0, 3, 1), (0, 3, 2),
        (0, 4, 0), (0, 4, 2), (1, 1, 0), (1, 1, 1), (1, 2, 0),
        (1, 2, 1), (1, 2, 2), (1, 3, 0), (1, 3, 1), (1, 3, 2),
        (2, 1, 1), (2, 1, 2), (2, 2, 0), (2, 2, 1), (2, 2, 2),

        (1, 5, 0), (1, 5, 1), (1, 6, 0), (1, 6, 2), (2, 4, 0),
        (2, 4, 1), (2, 4, 2), (2, 5, 0), (2, 5, 1), (2, 5, 2),
        (2, 6, 0), (2, 6, 2), (3, 4, 0), (3, 4, 1), (3, 4, 2),
        (3, 5, 0), (3, 5, 1), (3, 5, 2), (4, 4, 1), (4, 4, 2),
        
        (4, 0, 0), (4, 0, 1), (4, 1, 0), (4, 1, 1), (4, 1, 2),
        (4, 2, 0), (4, 2, 1), (4, 2, 2), (5, 0, 0), (5, 0, 1),
        (5, 0, 2), (5, 1, 0), (5, 1, 1), (5, 1, 2), (5, 2, 0),
        (5, 2, 2), (6, 0, 1), (6, 0, 2), (6, 1, 1), (6, 1, 2)]

    svg_rotation = 0

    def coordinates(self):
        for coord in self.coords:
            yield self.coordinate_offset(*coord, offset=None)

    def customize_piece_data(self):
        self.piece_data['R5'][-1]['rotations'] = (0, 1)


class PentatwigsTrefoil_x1(PentatwigsTrefoil):

    """0 solutions"""

    hole = set((
        (2,0,0), (2,0,1), (3,0,2), (2,2,0),
        (0,4,1), (0,5,0), (0,5,2), (2,3,2),
        (3,2,1), (4,3,0), (5,2,1), (5,2,2)))


class PentatwigsTrefoil_x2(Pentatwigs):

    """0 solutions (design by Colin F. Brown)"""

    height = 6
    width = 6

    holes = set(((1,2,1), (2,4,0), (4,1,2)))

    def coordinates(self):
        coords = set(
            list(self.coordinates_hexagon(2, offset=(0,1,0)))
            + list(self.coordinates_hexagon(2, offset=(1,2,0)))
            + list(self.coordinates_hexagon(2, offset=(2,0,0))))
        coords -= self.holes
        return sorted(coords)


class PentatwigsTrefoilRing_x(Pentatwigs):

    """0 solutions (design by Colin F. Brown)"""

    height = 6
    width = 6

    holes = set(((2,2,1), (2,3,0), (3,2,2)))

    def coordinates(self):
        coords = set(
            list(self.coordinates_hexagon(2, offset=(0,1,0)))
            + list(self.coordinates_hexagon(2, offset=(1,2,0)))
            + list(self.coordinates_hexagon(2, offset=(2,0,0))))
        coords -= self.holes
        return sorted(coords)


class PentatwigsMobiusStrip1(Pentatwigs):

    """
    6 solutions?

    Puzzle design by Colin F. Brown.
    """

    height = 5
    width = 12

    holes = set(((0,0,0), (0,0,1), (0,1,2), (8,0,1), (8,1,1), (8,1,2), (7,2,0)))

    def coordinates(self):
        for coord in self.coordinates_bordered(8, 2):
            if coord not in self.holes:
                yield coord


class Pentatwigs4x4_1(Pentatwigs):

    """
    1 solution

    Design by Colin F. Brown.
    """

    width = 5
    height = 5

    holes = set(((1,3,2), (2,2,2), (3,1,2)))

    svg_rotation = 60

    def coordinates(self):
        for coord in self.coordinates_bordered(4, 4):
            if coord not in self.holes:
                yield coord

    def customize_piece_data(self):
        self.piece_data['R5'][-1]['flips'] = None
        self.piece_data['R5'][-1]['rotations'] = (0, 1, 2)


class Pentatwigs4x4_2(Pentatwigs4x4_1):

    """8 solutions"""

    holes = set(((1,1,2), (2,2,2), (3,3,2)))


class PentatwigsElongatedRoundedRectangle1(Pentatwigs):

    """
    21 solutions

    Design by Colin F. Brown.
    """

    width = 11
    height = 6

    extras = ((0,5,0), (10,0,0))

    svg_rotation = 0

    def coordinates(self):
        coords = set(self.coordinates_rounded_rectangle(9, 2, offset=(1,0,0)))
        for x, y, z in self.extras:
            coords.add(self.coordinate_offset(x, y, z, None))
        return sorted(coords)

    def customize_piece_data(self):
        self.piece_data['R5'][-1]['flips'] = None
        self.piece_data['R5'][-1]['rotations'] = (0, 1, 2)


class PentatwigsRosetteCluster1(Pentatwigs):

    """
    9 solutions

    Design by Colin F. Brown.
    """

    width = 8
    height = 6

    extras = ((3,2,0), (3,3,0))
    holes = set(((2,3,0), (4,2,0)))

    svg_rotation = 0

    def coordinates(self):
        coords = (
            set(list(self.coordinates_hexagon(2, offset=(0,2,0)))
                + list(self.coordinates_hexagon(2, offset=(4,0,0))))
            - self.holes)
        for x, y, z in self.extras:
            coords.add(self.coordinate_offset(x, y, z, None))
        return sorted(coords)

    def customize_piece_data(self):
        self.piece_data['R5'][-1]['flips'] = None
        self.piece_data['R5'][-1]['rotations'] = (0, 1, 2)


class PentatwigsRosetteCluster2(PentatwigsRosetteCluster1):

    """
    11 solutions

    Design by Colin F. Brown.
    """

    holes = set(((0,4,0), (6,1,0)))


class PentatwigsTwoHexagons(Pentatwigs):

    """0 solutions"""

    width = 8
    height = 6

    svg_rotation = 0

    def coordinates(self):
        coords = (
            set(list(self.coordinates_hexagon(2, offset=(0,2,0)))
                + list(self.coordinates_hexagon(2, offset=(4,0,0)))))
        return sorted(coords)


class PentatwigsSnowflake(Pentatwigs):

    """0 solutions"""

    width = 6
    height = 6

    hex_offsets = ((0,2,0), (0,4,0), (2,0,0), (2,4,0), (4,0,0), (4,2,0))

    def coordinates(self):
        coords = set(self.coordinates_hexagon(2, offset=(1,1,0)))
        for offset in self.hex_offsets:
            for coord in self.coordinates_bordered(1, 1, offset=offset):
                coords.add(coord)
        return sorted(coords)


class PentatwigsX1(PentatwigsButterfly1):

    """32 solutions"""

    hole = set(((1,5,0), (1,5,2), (2,4,1), (2,5,0), (2,5,2)))

    svg_rotation = 30

    def customize_piece_data(self):
        self.piece_data['R5'][-1]['flips'] = None


class PentatwigsSemiregularHexagonRing_x(Pentatwigs):

    """0 solutions (design by Colin F. Brown)"""

    height = 6
    width = 6

    def coordinates(self):
        coords = (
            set(self.coordinates_semiregular_hexagon(4, 2))
            - set(self.coordinates_triangle_unbordered(3, offset=(1,1,0))))
        return sorted(coords)


class PentatwigsElongatedHexagonRing_x(Pentatwigs):

    """0 solutions (design by Colin F. Brown)"""

    width = 7
    height = 4

    def coordinates(self):
        coords = (
            set(self.coordinates_elongated_hexagon(5, 2))
            - set(self.coordinates_unbordered(4, 1, offset=(1,1,0))))
        return sorted(coords)


class PentatwigsElongatedHexagon5x2_1(Pentatwigs):

    """2 solutions (design by Colin F. Brown)"""

    width = 7
    height = 4

    holes = set(((1,1,1), (3,1,1), (5,1,1)))

    def coordinates(self):
        coords = set(self.coordinates_elongated_hexagon(5, 2)) - self.holes
        return sorted(coords)

    def customize_piece_data(self):
        self.piece_data['R5'][-1]['flips'] = None
        self.piece_data['R5'][-1]['rotations'] = (0, 1, 2)
 

class PentatwigsHexagram_x(Pentatwigs):

    """0 solutions (design by Colin F. Brown)"""

    width = 7
    height = 7

    extras = ((0,5,0), (2,2,2), (2,6,1), (5,0,1), (5,5,2), (6,2,0))

    def coordinates(self):
        coords = set(self.coordinates_hexagram(2, offset=(1,1,0)))
        coords.update(
            [self.coordinate_offset(x, y, z, None) for x, y, z in self.extras])
        return sorted(coords)


class OneSidedPentatwigsTrapezoid12x2(OneSidedPentatwigs):

    """many solutions"""

    height = 3
    width = 13

    def coordinates(self):
        return self.coordinates_trapezoid(12, 2)

    def customize_piece_data(self):
        OneSidedPentatwigs.customize_piece_data(self)
        self.piece_data['R5'][-1]['flips'] = None


class OneSidedPentatwigsInsetRectangle7x4_1(OneSidedPentatwigs):

    """many solutions"""

    height = 8
    width = 8

    svg_rotation = 0

    hole = (3,4,0)

    def coordinates(self):
        for coord in self.coordinates_inset_rectangle(7, 4):
            if coord != self.hole:
                yield coord


class OneSidedPentatwigsInsetRectangle7x4_2(
    OneSidedPentatwigsInsetRectangle7x4_1):

    """many solutions"""

    hole = (3,5,0)


class OneSidedPentatwigsInsetRectangle7x4_3(
    OneSidedPentatwigsInsetRectangle7x4_1):

    """many solutions"""

    hole = (0,5,0)


class OneSidedPentatwigsInsetRectangle7x4_4(
    OneSidedPentatwigsInsetRectangle7x4_1):

    """many solutions"""

    hole = (2,4,0)


class OneSidedPentatwigsElongatedHexagon8x2_1(OneSidedPentatwigs):

    """many solutions"""

    width = 10
    height = 4

    hole = set(((4,2,1),))

    def coordinates(self):
        coords = set(self.coordinates_elongated_hexagon(8, 2))
        coords -= self.hole
        return sorted(coords)


class OneSidedPentatwigsElongatedHexagon8x2_2(
    OneSidedPentatwigsElongatedHexagon8x2_1):

    """many solutions"""

    hole = set(((1,1,1),))


class OneSidedPentatwigsElongatedHexagon3x4_1(OneSidedPentatwigs):

    """many solutions"""

    width = 7
    height = 8

    def coordinates(self):
        coords = set(self.coordinates_elongated_hexagon(3, 4))
        coords -= set(self.coordinates_trapezoid_unbordered(
            3, 2, offset=(1,4,0))) 
        coords -= set(self.coordinates_inverted_trapezoid_unbordered(
            3, 2, offset=(2,1,0)))
        return sorted(coords)


class OneSidedPentatwigsElongatedHexagon3x4_2(OneSidedPentatwigs):

    """many solutions"""

    width = 7
    height = 8

    def coordinates(self):
        coords = set(self.coordinates_elongated_hexagon(3, 4))
        coords -= set(self.coordinates_trapezoid_unbordered(
            3, 2, offset=(3,0,0))) 
        coords -= set(self.coordinates_inverted_trapezoid_unbordered(
            3, 2, offset=(0,5,0)))
        return sorted(coords)


class OneSidedPentatwigsRosettes1(OneSidedPentatwigs):

    """
    many solutions

    design by `Peter F. Esser <http://polyforms.eu/>`_
    """

    width = 11
    height = 11

    def coordinates(self):
        coords = set(
            list(self.coordinates_hexagon(2, offset=(2,0,0)))
            + list(self.coordinates_hexagon(2, offset=(5,0,0))))
        coords.update(set(
            self.coordinates_hexagon(2, offset=(-1,0,0))).intersection(
            self.coordinates_hexagon(2)))
        coords.update(set(
            self.coordinates_hexagon(2, offset=(7,0,0))).intersection(
            self.coordinates_hexagon(2, offset=(8,0,0))))
        return sorted(coords)


class OneSidedPentatwigsCross1(OneSidedPentatwigs):

    """
    many solutions

    design by `Peter F. Esser <http://polyforms.eu/>`_
    """

    width = 8
    height = 7

    svg_rotation = 0

    def coordinates(self):
        coords = set(
            list(self.coordinates_inset_rectangle(7, 3))
            + list(self.coordinates_inset_rectangle(3, 5, offset=(2,0,0))))
        return sorted(coords)


class OneSidedPentatwigsPeanut1(OneSidedPentatwigs):

    """
    many solutions

    design by `Peter F. Esser <http://polyforms.eu/>`_
    """

    width = 10
    height = 7

    hex_offsets = (
        (0,3,0), (1,2,0), (1,3,0), (2,2,0), (5,0,0), (5,1,0), (6,0,0))

    holes = set(((0,5,0), (2,4,0), (6,2,0), (8,1,0)))

    svg_rotation = 0

    def coordinates(self):
        coords = set()
        for offset in self.hex_offsets:
            coords.update(set(self.coordinates_hexagon(2, offset=offset)))
        coords -= self.holes
        return sorted(coords)


class OneSidedPentatwigsPeanut2(OneSidedPentatwigsPeanut1):

    """many solutions"""

    holes = set(((2,3,0), (2,5,0), (6,1,0), (6,3,0)))


class OneSidedPentatwigsPeanut3(OneSidedPentatwigsPeanut1):

    """many solutions"""

    holes = set(((3,3,0), (3,4,0), (5,2,0), (5,3,0)))


class OneSidedPentatwigsPeanut4(OneSidedPentatwigsPeanut1):

    """many solutions"""

    holes = set(((1,4,0), (1,5,0), (7,1,0), (7,2,0)))

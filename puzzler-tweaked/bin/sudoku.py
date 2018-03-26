#!/usr/bin/env python
# $Id: sudoku.py 244 2008-11-07 05:52:26Z goodger $

"""
Front end for 9x9 Sudoku solver. 
"""

if __file__.startswith('bin/') or __file__.startswith('bin\\'):
    import os
    import sys
    # enable access to the puzzle package via the parent directory:
    sys.path.insert(0, os.path.dirname(sys.path[0]))

from puzzler import sudoku

sudoku.run_from_command_line(sudoku.Sudoku9x9)

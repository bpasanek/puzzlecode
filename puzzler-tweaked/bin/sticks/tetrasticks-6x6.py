#!/usr/bin/env python
# $Id: tetrasticks-6x6.py 571 2012-10-06 21:14:57Z goodger $

"""
1795 solutions total:

* 72 solutions omitting H
* 382 omitting J
* 607 omitting L
* 530 omitting N
* 204 omitting Y

All are perfect solutions (i.e. no pieces cross).
"""

import puzzler
from puzzler.puzzles.tetrasticks import Tetrasticks6x6

puzzler.run(Tetrasticks6x6)

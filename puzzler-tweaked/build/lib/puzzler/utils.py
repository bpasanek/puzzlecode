#!/usr/bin/env python
# $Id: utils.py 600 2015-02-24 20:21:02Z goodger $

# Author: David Goodger <goodger@python.org>
# Copyright: (C) 1998-2015 by David J. Goodger
# License: GPL 2 (see __init__.py)

"""
Common utility functions.
"""

import locale


def thousands(number):
    return locale.format('%d', number, grouping=True)

def plural_s(number):
    if number == 1:
        return ''
    else:
        return 's'

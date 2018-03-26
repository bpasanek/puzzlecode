.. -*- coding: utf-8 -*-

===============================
 README |---| Polyform Puzzler
===============================

:Author: David Goodger <goodger@python.org>
:Date: $Date: 2015-02-24 14:21:02 -0600 (Tue, 24 Feb 2015) $
:Web site: http://puzzler.sourceforge.net/
:Copyright: |c| 1998-2015 by David J. Goodger
:License: `GPL 2 <COPYING.html>`__

**Polyform Puzzler** is a set of solvers for many polyform puzzles
(like Pentominoes and Soma Cubes), and a software toolkit for
exploring & solving polyform puzzles.  It consists of a set of
front-end applications for specific polyform puzzles and a Python
library that does the heavy lifting.  New polyforms and new puzzles
can easily be defined and added.  Python 2.4 or higher is required.

**Polyform Puzzler** also contains a Sudoku puzzle solver using the
same (versatile!) engine.

.. contents::


Quick-Start
===========

This section is for those who understand computers well and who want
to get up & running quickly.  If you don't know what a "shell" is, or
if you don't understand any of the following steps, go to
"Installation_" below.  Full details are in the following sections.

1. Get and install the latest release of Python, available from

       http://www.python.org/

   Python 2.5 or later; Python 2.7.2 or later is recommended.

2. Use the latest Polyform Puzzler code.  Get the code from Subversion
   or from the snapshot:

       http://puzzler.sourceforge.net/puzzler-snapshot.tgz

   See Snapshots_ below for details.

3. Unpack the snapshot tarball (archive file) in a temporary directory
   (**not** directly in Python's ``site-packages``).

   For example, on Unix, GNU/Linux, or Mac OS X, in a shell type::::

       tar xzf puzzler-snapshot.tgz

   On Windows, use the WinZip program (or equivalent) to unpack the
   archive file.

4. In a shell/terminal, move (``cd``) into the temporary directory::

       cd puzzler

There are two ways to proceed from here: `install it`_, or `just run
it`_:


Install It
----------

This option installs the Polyform Puzzler library into Python's
system-wide standard library.

5. Run ``install.py`` with admin rights.  On Windows systems it may be
   sufficient to double-click ``install.py``.  On Unix, GNU/Linux, or
   Mac OS X, type::

        su
        (enter admin password)
        ./install.py

   See Installation_ below for details.

6. Use a front-end application from the "bin" subdirectory.  For
   example::

       cd bin
       ./pentominoes3x20.py            (Unix/Mac)
       python pentominoes3x20.py       (Windows)

   See Usage_ below for details.


Just Run It
-----------

This option allows you to use the "puzzler" package without installing
it permanently.  Note that you will only be able to use the "puzzler"
package from one location (where you unpacked it, not from arbitrary
locations on your system), unless you `install it`_, or you set your
``PYTHONPATH`` environment variable.

5. In the top-level directory (containing the "puzzler", "docs", and
   "bin" directories), you can run the various front-ends.

   See Usage_ below for details.
 
   * **Unix/Mac users:**

     Enter the following command::

         PYTHONPATH= bin/pentominoes3x20.py

     The ``PYTHONPATH=`` part tells Python to look in the current
     directory for modules and packages, so it can find the "puzzler"
     package (directory).

   * **Windows users:**

     Enter the following commands::

         set PYTHONPATH=;
         python bin\pentominoes3x20.py

     The ``set PYTHONPATH=;`` command tells Python to look in the
     current directory for modules and packages, so it can find the
     "puzzler" package (directory).

     If the Python executable is not on your ``PATH``, you will
     have to specify its location, e.g.::

         C:\Python24\python bin\pentominoes3x20.py


Snapshots
=========

We recommend that you always use the current snapshot, which is
usually updated within an hour of changes being committed to the
repository:

    http://puzzler.sourceforge.net/puzzler-snapshot.tgz

To keep up to date on the latest developments, either download fresh
copies of the snapshots regularly, or use the `Subversion
repository`_:

    svn co https://puzzler.svn.sourceforge.net/svnroot/puzzler/trunk/puzzler

.. _Subversion repository: https://sourceforge.net/svn/?group_id=7049


Project Files & Directories
===========================

* README.txt: You're reading it.

* COPYING.txt: Copyright and license details.

* GPL2.txt: The GNU General Public License, version 2.

* setup.py: Installation script.  See "Installation" below.

* install.py: Quick & dirty installation script.  Just run it.  For
  any kind of customization or help though, setup.py must be used.

* puzzler: The project source directory, installed as a Python
  package.

* bin: Polyform puzzler front-end applications directory.

* docs: The project documentation directory.  All project
  documentation is in reStructuredText_ format, and can be converted
  to HTML and other formats using Docutils_.

  - FAQ.txt: Frequently Asked Questions (with answers!).
  - puzzles.txt: List of puzzles implemented and counts of solutions
  - extend.txt: How to extend Polyform Puzzler
  - history.txt: Detailed log of changes.
  - todo.txt: To do list.

.. _reStructuredText: http://docutils.sourceforge.net/rst.html
.. _Docutils: http://docutils.sourceforge.net


Installation
============

OS-specific installation instructions follow.  For non-standard
installations (i.e. installing to non-standard places, like your home
directory), please see `Installing Python Modules
<http://docs.python.org/inst/inst.html>`_.


GNU/Linux, BSDs, Unix, Mac OS X, etc.
-------------------------------------

1. Open a shell (terminal).

2. Expand the ``.tgz`` archive into a temporary directory (**not**
   directly into Python's ``site-packages``)::

       tar xzf puzzler-snapshot.tgz

   (The archive file name may not be exactly as shown above.)

3. Go to the directory created by expanding the archive::

       cd <archive_directory_path>

   It contains a distutils setup file "setup.py".

4. Install the package (you may need root permissions to complete this
   step)::

       python setup.py install

   If the Python executable isn't on your ``PATH``, you'll have to
   specify the complete path, such as /usr/local/bin/python.

   You can also just run install.py; it does the same thing.


Windows
-------

1. Double-click on the ``.tgz`` archive; this should open the archive
   in WinZip.  (If you don't have WinZip or equivalent installed,
   please download and install it.)  Extract the archive's contents
   into a temporary directory (**not** directly into Python's
   ``site-packages``).

       tar xzf puzzler-snapshot.tgz

   (The archive file name may not be exactly as shown above.)

2. The extracted folder contains distutils setup files ``setup.py``
   and ``install.py``.  Just double-click ``install.py`` and you're
   done.

   If you have any trouble, try the following steps:

3. Open a DOS Box (Command Shell, MS-DOS Prompt, or whatever they're
   calling it these days).

4. Go to the directory created by expanding the archive::

       cd <archive_directory_path>

5. Install the package::

       <path_to_python.exe>\python setup.py install


Optional Acceleration With Psyco
--------------------------------

Note that Psyco does not seem to accelerate the X2 algorithm, only the
DLX algorithm.

If your computer has an Intel/AMD 386-compatible processor (under any
OS), you can use `Psyco <http://psyco.sourceforge.net/>`_ to
accelerate processing by 1.5 to 3 times.  Support is already built
into Polyform Puzzler (a few trivial lines in
``puzzler/exact_cover_dlx.py``), so just install Psyco_ to enjoy the
speed boost!


Usage
=====

After unpacking and installing the Polyform Puzzler package, the
applications in the ``bin`` directory can be used to solve puzzles.

All of the puzzle applications support several command-line options.
Use the "-h" or "--help" option to see a complete list.


GNU/Linux, BSDs, Unix, Mac OS X, etc.
-------------------------------------

For example, type the following commands (in a shell) to solve the
3x20 pentominoes puzzle::

    cd <archive_directory_path>
    bin/pentominoes-3x20.py


Windows
-------

For example, type the following commands (in a command prompt window)
to solve the 3x20 pentominoes puzzle::

    cd <archive_directory_path>
    python bin\pentominoes-3x20.py

If you just double-click on the puzzle application, it will run, but
the output window will disappear as soon as the puzzle finishes.

You may want to redirect the output to a file, since the Windows
command prompt limits the number of output lines it remembers.  Do
this::

    python bin\pentominoes-3x20.py > output.txt

(Choose any name you like for your output file.)

Alternatively, right-click on the puzzle application file, choose
"Edit with IDLE", then choose "Run script" from the "Edit" menu (or
hit Ctrl+F5).


Sudoku
======

To solve a 9x9 Sudoku puzzle, use the ``bin/sudoku.py`` front end
command as described the Usage_ section above.  You must supply a
Sudoku starting position, either by providing (on the command line)
the name of the file containing the position, or by typing in the
starting position at the prompt.  Examples:

* Supply the name of a file containing the starting position::

      bin/sudoku.py start.txt

* Supply the starting position at the prompt::

      bin/sudoku.py

      Enter a 9x9 Sudoku starting position: either 9 lines of 9 columns
      or 1 big line, "." or "0" for empty squares, spaces optional.
      Ctrl-D (on Linux/Mac), Ctrl-Z + Enter (on Windows) to end:

      ... enter starting position here ...

Starting positions must use periods (".") or zeros ("0") to represent
empty squares, and must be in one of the following formats:

* 9 lines of 9 columns, with or without spaces between digits::

     4 . . . . . 8 . 5
     . 3 . . . . . . .
     . . . 7 . . . . .
     . 2 . . . . . 6 .
     . . . . 8 . 4 . .
     . . . . 1 . . . .
     . . . 6 . 3 . 7 .
     5 . . 2 . . . . .
     1 . 4 . . . . . .

     4.....8.5
     .3.......
     ...7.....
     .2.....6.
     ....8.4..
     ....1....
     ...6.3.7.
     5..2.....
     1.4......

  Blank lines and spaces may be used to separate blocks::

     4.. ... 8.5
     .3. ... ...
     ... 7.. ...

     .2. ... .6.
     ... .8. 4..
     ... .1. ...

     ... 6.3 .7.
     5.. 2.. ...
     1.4 ... ...

* Dividing-line characters may be used to clarify the diagram ("- | +"
  are simply ignored)::

     4 . . | . . . | 8 . 5
     . 3 . | . . . | . . .
     . . . | 7 . . | . . .
     ------+-------+------
     . 2 . | . . . | . 6 .
     . . . | . 8 . | 4 . .
     . . . | . 1 . | . . .
     ------+-------+------
     . . . | 6 . 3 | . 7 .
     5 . . | 2 . . | . . .
     1 . 4 | . . . | . . .

* All on one line, with or without spaces between digits::

    4.....8.5.3..........7......2.....6.....8.4......1.......6.3.7.5..2.....1.4......

    4.....8.5 .3....... ...7..... .2.....6. ....8.4.. ....1.... ...6.3.7. 5..2..... 1.4......


Getting Help
============

If you have questions or need assistance with Polyform Puzzler, please
post a message to the Puzzler-Users mailing list
(puzzler-users@lists.sourceforge.net).  Please subscribe_ if possible;
messages from non-subscribers will be held for approval.

`Bug reports`_, patches_, and other contributions are welcome!

.. _subscribe:
   https://lists.sourceforge.net/lists/listinfo/puzzler-users
.. _Bug reports:
   http://sourceforge.net/tracker/?group_id=7049&atid=107049
.. _patches:
   http://sourceforge.net/tracker/?group_id=7049&atid=307049

.. |---| unicode:: U+2014  .. em dash
   :trim:
.. |c| unicode:: U+00A9 .. copyright sign
.. |x| unicode:: U+00D7 .. multiplication sign
   :trim:


..
   Local Variables:
   mode: indented-text
   indent-tabs-mode: nil
   sentence-end-double-space: t
   fill-column: 70
   End:

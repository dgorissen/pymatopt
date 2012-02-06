Motivation
==========

This code demonstrates how you can pass handles to python functions directly to Matlab code. In this case the goal was to use Matlab's optimizers to optimize an objective function written in python.  The bridge is written in C using ctypes/mex and does not rely on sockets or COM.  An example optimizing the banana function is included.

The full motivation and more background is described in `this blog post <http://www.dirkgorissen.com/2012/02/06/bridging-python-and-matlab-passing-callbacks-and-optimization/>`_.

Usage
======

Download the library and run the Makefile.  On Windows you can just run the mex/mbuild commands from the makefile directly in the Windows/Matlab shell.  Once everything is built follow the instructions in the generated readme.txt file about setting up LD_LIBRARY_PATH. Once that is done simply run the included python file.  This should optimize the banana function using fmincon with one linear inequality constraint and one nonlinear one.

The code should be rather easy to understand and extend to more solvers. I welcome any patches or suggestions.

License
=======

`LGPL v3 <http://www.gnu.org/licenses/lgpl.html/>`_.: 
# PDE Solver Generator

I wrote this tool to help with my capstone project at Oregon State University.

You can describe systems of partial differential equations using a simple custom syntax. 
This description is then translated into C++ code which compiles into an executable which solves that system using CVODES.

There are two executables involved: the generator and the solver. The generator reads in a text file which describes the
PDE system, and outputs a header file which is used to build the solver.

## Build & Usage

To build the executables, make a new folder within the project root and call `cmake .. -G "Unix Makefiles"` from inside.
You can use something other than Unix Makefiles at your own volition, but you need to make sure you call any resulting
executables from directly within your new folder. These instructions will proceed assuming you will use `make`.

You can build the generator with `make generator`.

You can run the example system through the generator by calling `./generator ../system.txt`. The second parameter is
the name of the file which describes the PDE system. Information on the syntax of this file can be found in the docs
folder. 

After calling this executable, the contents of `generated/system.h` will be replaced by code describing the
new system. Calling `make solver` will build this generated code into a solver executable which uses CVODES
to solve the PDE system. The solution will be sent to stdout as a table of points in csv format
representing a graph of the solution.

all: mex lib main exe

mex:
	# Compile the MEX-file which allows calling back into the C code
	mex -largeArrayDims evalfunction.c
	mex -largeArrayDims evalconfunction.c

lib:
	# Compile the library using the MATLAB Compiler
	mcc -W 'lib:liboptimize' -T link:lib doOptim.m

main:
	# Compiler the "driver" library called by python
	mbuild -output liboptimmatlab optimmatlab.c optimmatlab.exports -L. -loptimize

exe:
	# Compiler the "driver" library as an executable
	mbuild optimmatlab.c -L. -loptimize

clean:
	rm -f *~ *.log *.mexa64 *.pyc *.o *.so liboptimize.* optimmatlab
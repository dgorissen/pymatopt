#include "mex.h"

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {
    /* Function signature for the function handle */
    double (*fun)(double*);
    /* Get the actual function handle back */
    fun = (double (*)(double*)) *((mwSize*) mxGetData(prhs[0]));
    /* Call the C function through the handle */
    plhs[0] = mxCreateDoubleScalar(fun(mxGetPr(prhs[1])));
}
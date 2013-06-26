#include <math.h>
#include <stdio.h>
#include "liboptimize.h"
#include "util.h"

/* Simple test function to be optimized */
double bananafunction(double *x) {
    printf("  Calling C obj fun with (%f,%f)\n",x[0],x[1]);
    double term1;
    double term2;
    term1 = 100.0 * pow(x[1] - pow(x[0],2.0), 2.0);
    term2 = pow(1.0 - x[0],2.0);
    return term1 + term2;
}

/* Call matlab's fmincon */
int matlab_fmincon(
	       double* x,		    /* result: optimum point */
	       double* fval,		    /* result: optimum function value */
               int ndim,		    /* number of dimensions (variables) */
               double (*objfun)(double *),  /* objective function */
	       double* x0,                  /* starting point */
	       int nIneq,		    /* number of linear inequalities */
	       double* A,		    /* linear inequalities A*X <= B*/
	       double* B,		    
	       int nEq,			    /* number of linear equalities */
	       double* Aeq,		    /* linear equalities Aeq*X = B*/
	       double* Beq,
	       bool hasBounds,              /* are there any bounds */ 
	       double* LB,		    /* lower bounds */
	       double* UB,		    /* upper bounds */
	       int nC,			    /* number of constraints */
	       double* (*C)(double *),      /* non linear constraints */
	       int nCeq,		    /* number of equality constraints */
	       double* (*Ceq)(double *)     /* non linear equality constraints */
	      ){

    /** Intialize MCR and library **/
    const char* ops = "-nodesktop";
    /*const char* ops = "";*/
    if (!mclInitializeApplication(&ops, 1)) {
        printf("Error Initializing Application\n");
        return -1;
    }
    if (!liboptimizeInitialize()) {
        printf("Error Initializing Library\n");
        return -1;
    }

    printf("Setting up matlab optimizer for a %i var problem\n",ndim);
    
    /** The objective function **/
    /* Create a numeric datatype large enough to hold a pointer */
    mxArray *of = mxCreateNumericMatrix(1,1,mxUINT64_CLASS,mxREAL);
    
    /* Save the pointer to the objective function as numeric */
    *((mwSize*)mxGetData(of)) = (mwSize) objfun;
    printf("  Objective function set\n");
    
    /** The starting point **/
    mxArray *x0arr = makeArray(x0,ndim,1);
    printf("  Starting point set x0[0]=%f x0[1]=%f\n",x0[0],x0[1]);

    /** linear constraints **/
    mxArray *Aarr = makeArray(A,nIneq,ndim);
    mxArray *Barr = makeArray(B,nIneq,1);
    printf("  %i inequality constraints set\n",nIneq);

    /** linear equality constraints **/
    mxArray *Aeqarr = makeArray(Aeq,nEq,ndim);
    mxArray *Beqarr = makeArray(Beq,nEq,1);
    printf("  %i equality constraints set\n",nEq);
    
    /** variable bounds **/
    int nb = ndim;
    if (!hasBounds) nb = 0;
    mxArray *LBarr = makeArray(LB,nb,1);
    mxArray *UBarr = makeArray(UB,nb,1);
    printf("  %i variable bounds set\n",nb);
    
    /** nonlinear constraints **/
    mxArray *cf;
    mxArray *ncf = makeScalar(nC);
    if (nC > 0) {
	cf = mxCreateNumericMatrix(1,1,mxUINT64_CLASS,mxREAL);
        *((mwSize*)mxGetData(cf)) = (mwSize) C;
    }else{
        cf = mxCreateNumericMatrix(0, 1, mxDOUBLE_CLASS, mxREAL);
    }
    printf("  %i nonlinear constraints set\n",nC);

    /** nonlinear equality constraints **/
    mxArray *ecf;
    mxArray *necf = makeScalar(nCeq);
    if (nCeq > 0) {
	ecf = mxCreateNumericMatrix(1,1,mxUINT64_CLASS,mxREAL);
        *((mwSize*)mxGetData(ecf)) = (mwSize) Ceq;
    }else{
        ecf = mxCreateNumericMatrix(0, 1, mxDOUBLE_CLASS, mxREAL);
    }
    printf("  %i nonlinear equality constraints set\n",nCeq);

    /** Call the matlab function **/
    mxArray *xarr=NULL;
    mxArray *fvalarr=NULL;

    printf("Calling the matlab library\n");
    
    mlfDoOptim(2,&xarr,&fvalarr,of,x0arr,Aarr,Barr,Aeqarr,Beqarr,LBarr,UBarr,ncf,cf,necf,ecf);
    
    printf("Returning results\n");
    
    /** Get/display the results **/
    double* xa = mxGetPr(xarr);
    double* fvala = mxGetPr(fvalarr);
    int i;
    for(i=0;i<ndim;++i) x[i] = xa[i];
    for(i=0;i<1;++i) fval[i] = fvala[i];
    
    printf("Cleaning up\n");

    /** Cleanup **/
    mxDestroyArray(of);
    mxDestroyArray(x0arr);
    mxDestroyArray(Aarr);
    mxDestroyArray(Barr);
    mxDestroyArray(Aeqarr);
    mxDestroyArray(Beqarr);
    mxDestroyArray(LBarr);
    mxDestroyArray(UBarr);
    mxDestroyArray(cf);
    mxDestroyArray(ecf);
    mxDestroyArray(ncf);
    mxDestroyArray(necf);
    mxDestroyArray(xarr);
    mxDestroyArray(fvalarr);
    
    liboptimizeTerminate();
    mclTerminateApplication();
    
    return 0;
}


int main(void) {
  int ndim = 2;
  double (*objfun)(double *) = &bananafunction;
  double x0[2] = {1.1,2.2};
  int nIneq = 1;
  double A[2] = {1,1};
  double B[2] = {1.5};
  int nEq = 0;
  double* Aeq = NULL;
  double* Beq = NULL;
  int nBounds = 0;
  double* LB = NULL;
  double* UB = NULL;
  int nC = 0;
  double* (*C)(double *) = NULL;
  int nCeq = 0;
  double* (*Ceq)(double *) = NULL;
  
  double x[ndim];
  double fval;
  
  int res = matlab_fmincon(x,&fval,ndim,objfun,x0,nIneq,A,B,nEq,Aeq,Beq,nBounds,LB,UB,nC,C,nCeq,Ceq);  
  
  printf("Optimum at %f, %f with value: %e\n",x[0],x[1],fval);

  return res;
}

#!/usr/bin/python2.7

from ctypes import *
from os import path

# simple test function
def banana(x):
    term1 = 100.0 * (x[1] - x[0]**2.0) ** 2.0
    term2 = (1.0 - x[0])**2.0
    return term1 + term2

# example non linear constraint
def nonlincon(x):
  return x[0]**2 - x[1]**2

 
def fmincon(of,x0,A=[],B=[],Aeq=[],Beq=[],LB=[],UB=[],C=[],Ceq=[],minimize=True):
  # assume the library is in the same directory
  lib=path.join(path.dirname(path.abspath(__file__)),'liboptimmatlab.so')
  lo = cdll.LoadLibrary(lib)

  # starting point
  ndim = len(x0)
  x0 = (c_double * ndim)(*x0)

  # objective function
  def objfun(x):
    xx = [float(x[i]) for i in range(ndim)] 
    res = of(xx)
    res = res if minimize else -res
    return res
    
  OBJFUN = CFUNCTYPE(c_double, POINTER(c_double))
  objfun = OBJFUN(objfun)

  # inequality constraints
  nIneq = len(B)
  A2 = (c_double * ndim)(*A) if A else None #TODO flatten matrix & check col/row major
  B2 = (c_double * nIneq)(*B) if B else None
  
  # equality constraints
  nEq = len(Beq)
  Aeq2 = (c_double * ndim)(*Aeq) if Aeq else None #TODO flatten matrix & check col/row major
  Beq2 = (c_double * nEq)(*Beq) if Beq else None
  
  # variable bounds
  assert len(LB) == len(UB), "The number of lower/upper bounds must agree with the number of variables"
  assert len(LB) == ndim if len(LB) > 0 else True, "The number of lower/upper bounds must agree with the number of variables"
  
  nBounds = len(LB)
  LB2 = (c_double * ndim)(*LB) if nBounds > 0 else None
  UB2 = (c_double * ndim)(*UB) if nBounds > 0 else None
  
  # non linear constraints
  nC = len(C)
  nCeq = len(Ceq)
  
  # Matlab expects a single function handle that processes all constraints
  # Since we have a list of ocnstraints, just need to wrap them here
  def cfun(xx):
    # this copy is annoying but things break without it
    x = [float(xx[i]) for i in range(ndim)]
    # call each of the constraints on the point
    res = [c(x) for c in C]
    # convert to a ctypes double array
    data = (c_double * nC)(*res)
    # cast to a void pointer (see below)
    dp = cast(data,c_void_p)
    return dp.value

  def eqcfun(xx):
    x = [float(xx[i]) for i in range(ndim)]
    res = [c(x) for c in Ceq]
    data = (c_double * nCeq)(*res)
    dp = cast(data,c_void_p)
    return dp.value

  # returing POINTER(c_double) is not possible so use c_void_p
  CONFUN = CFUNCTYPE(c_void_p, POINTER(c_double))
  confun = CONFUN(cfun) if C else None
  eqconfun = CONFUN(eqcfun) if Ceq else None

  # result arrays, passed by reference
  x = (c_double * ndim)()
  fval = c_double(0)

  retcode = lo.matlab_fmincon(byref(x),byref(fval),ndim,objfun,x0,nIneq,A2,B2,nEq,Aeq2,Beq2,nBounds > 0,LB2,UB2,nC,confun,nCeq,eqconfun);  
  
  if retcode < 0:
    raise Exception("Failed to solve")
  else:
    return map(float,x),fval.value

if __name__ == "__main__":
  x,fval = fmincon(banana,[1.99,1.55],A=[1.99,1.55],B=[1.5],C=[nonlincon])
  print "Final result is ",fval," at ", x

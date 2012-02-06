/* Given an array of double, create a corresponding mxArray */
mxArray* makeArray(double* d, int nrow, int ncol) {
      mxArray *arr = mxCreateNumericMatrix(nrow, ncol, mxDOUBLE_CLASS, mxREAL);

      double* data = mxGetPr(arr);
      int i;
      for(i=0;i<nrow*ncol;++i){
	data[i] = d[i];
      } 
      return arr;
}

mxArray* makeScalar(double d) {
      mxArray *arr = mxCreateDoubleScalar(d);
      return arr;
}

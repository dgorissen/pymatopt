function [x,fval] = doOptim(f, x0, A, B, Aeq, Beq, LB, UB, nc, c, nceq, ceq)
    
    % Create an anynomous function to allow the MEX-file to be called by
    % the optimizer and be able to pass the function handle to the C function
    fWrapper = @(x) evalfunction(f,x);

    % do the same for the constraint functions
       function [con,coneq] = confun(x)
	  con = [];
	  coneq = [];

	  if ~isempty(c)
	    con = evalconfunction(c,nc,x);
	  end

	  if ~isempty(ceq)
	    coneq = evalconfunction(ceq,nceq,x);
	  end
       end

    % Call fmincon
    opts = optimoptions('fmincon', 'Algorithm','active-set');
    opts = optimoptions(opts,'Display','iter');
    %opts = optimset(opts,'MaxFunEvals',maxfun);
    opts = optimoptions(opts,'PlotFcns' ,{@optimplotx @optimplotfval,@optimplotfunccount,@optimplotconstrviolation,@optimplotstepsize});

    [x,fval] = fmincon(fWrapper,x0,A,B,Aeq,Beq,LB,UB,@confun,opts);
end
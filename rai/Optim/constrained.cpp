/*  ------------------------------------------------------------------
    Copyright (c) 2011-2020 Marc Toussaint
    email: toussaint@tu-berlin.de

    This code is distributed under the MIT License.
    Please see <root-path>/LICENSE for details.
    --------------------------------------------------------------  */

#include "constrained.h"

//==============================================================================
//
// PhaseOneProblem
//

void PhaseOneProblem::initialize(arr& x) {
  arr phi;
  P->evaluate(phi, NoArr, x);
  dim_eq=dim_ineq=0;
  double gmax=0.;
  for(uint i=0; i<phi.N; i++) {
    if(featureTypes.elem(i)==OT_ineq) {
      dim_ineq++;
      gmax = rai::MAX(gmax, phi.elem(i));
    }
    if(featureTypes.elem(i)==OT_eq) {
      dim_eq++;
    }
  }
  x.append(gmax);
}

void PhaseOneProblem::evaluate(arr& meta_phi, arr& meta_J, const arr& meta_x) {
  CHECK_EQ(meta_x.N, dimension+1, "");
  arr x = meta_x({0, -2});
  double s = meta_x(-1);

  arr phi, J;
  P->evaluate(phi, J, x);

  meta_phi = phi;
  meta_phi.append(-s);

  for(uint i=0; i<phi.N; i++) if(P->featureTypes.elem(i)==OT_ineq) {
      meta_phi(i) = phi(i) - s; //subtract slack!
    }

  if(!!meta_J) {
    meta_J = J;
    meta_J.append(zeros(meta_J.d1));
    meta_J(-1, -1) = -1.;
  }
}

//==============================================================================
//
// Solvers
//

const char* MethodName[]= { "NoMethod", "SquaredPenalty", "AugmentedLagrangian", "LogBarrier", "AnyTimeAugmentedLagrangian", "SquaredPenaltyFixed"};

//==============================================================================

OptConstrained::OptConstrained(arr& _x, arr& _dual, const shared_ptr<MathematicalProgram>& P, rai::OptOptions _opt, std::ostream* _logFile)
  : L(P, _opt, _dual), newton(_x, L, _opt, _logFile), dual(_dual), opt(_opt), logFile(_logFile) {

  if(opt.boundedNewton){
    arr lo, up;
    P->getBounds(lo, up);
    if(lo.N || up.N) newton.setBounds(lo, up);
  }

  if(opt.constrainedMethod==rai::logBarrier){
    L.useLB=true;
  }

  newton.options.verbose = rai::MAX(opt.verbose-1, 0);

  if(opt.verbose>0) cout <<"***** optConstrained: method=" <<MethodName[opt.constrainedMethod] <<" bounds: " <<(opt.boundedNewton?"yes":"no") <<endl;

  if(logFile) {
    (*logFile) <<"{ optConstraint: " <<its <<", mu: " <<L.mu <<", nu: " <<L.nu <<", L_x: " <<newton.fx <<", errors: ["<<L.get_costs() <<", " <<L.get_sumOfGviolations() <<", " <<L.get_sumOfHviolations() <<"], lambda: " <<L.lambda <<" }," <<endl;
  }
}

bool OptConstrained::step() {
  newton.logFile = logFile;
  L.logFile = logFile;

  if(!newton.evals) newton.reinit(newton.x);

  if(opt.verbose>0) {
    cout <<"** optConstr. it=" <<its
         <<(earlyPhase?'e':'l')
         <<" mu=" <<L.mu <<" nu=" <<L.nu <<" muLB=" <<L.muLB;
    if(newton.x.N<5) cout <<" \tlambda=" <<L.lambda;
    cout <<endl;
  }

  arr x_old = newton.x;

  //check for no constraints
  bool newtonOnce=false;
  if(L.get_dimOfType(OT_ineq)==0 && L.get_dimOfType(OT_ineqB)==0 && L.get_dimOfType(OT_eq)==0) {
    if(opt.verbose>0) cout <<"** optConstr. NO CONSTRAINTS -> run Newton once and stop" <<endl;
    newtonOnce=true;
  }

  if(L.lambda.N) CHECK_EQ(L.lambda.N, L.phi_x.N, "");

  //run newton on the Lagrangian problem
  if(newtonOnce || opt.constrainedMethod==rai::squaredPenaltyFixed) {
    newton.run();
  } else {
    double org_stopTol = newton.options.stopTolerance;
    double org_stopGTol = newton.options.stopGTolerance;
    if(!its){
      newton.options.stopTolerance *= 3.;
      newton.options.stopGTolerance *= 3.;
    }
    if(earlyPhase){
      newton.options.stopTolerance *= 10.;
      newton.options.stopGTolerance *= 10.;
    }
    if(opt.constrainedMethod==rai::anyTimeAula)  newton.run(20);
    else                                         newton.run();
    newton.options.stopTolerance = org_stopTol;
    newton.options.stopGTolerance = org_stopGTol;
  }

  if(L.lambda.N) CHECK_EQ(L.lambda.N, L.phi_x.N, "the evaluation (within newton) changed the phi-dimensionality");

  if(opt.verbose>0) {
    cout <<"** optConstr. it=" <<its
         <<(earlyPhase?'e':'l')
         <<' ' <<newton.evals
         <<" f(x)=" <<L.get_costs()
         <<" \tg_compl=" <<L.get_sumOfGviolations()
         <<" \th_compl=" <<L.get_sumOfHviolations()
         <<" \t|x-x'|=" <<absMax(x_old-newton.x);
    if(newton.x.N<5) cout <<" \tx=" <<newton.x;
    cout <<endl;
  }

  //check for squaredPenaltyFixed method
  if(opt.constrainedMethod==rai::squaredPenaltyFixed) {
    if(opt.verbose>0) cout <<"** optConstr. squaredPenaltyFixed stops after one outer iteration" <<endl;
    return true;
  }

  //check for newtonOnce
  if(newtonOnce) {
    return true;
  }

  //check for squaredPenaltyFixed method
  if(opt.constrainedMethod==rai::squaredPenaltyFixed) {
    if(opt.verbose>0) cout <<"** optConstr. squaredPenaltyFixed stops after one outer iteration" <<endl;
    return true;
  }

  its++;

  //stopping criteron
  if(its>=2 && absMax(x_old-newton.x) < (earlyPhase?5.:1.)*opt.stopTolerance) {
    if(opt.verbose>0) cout <<"** optConstr. StoppingCriterion Delta<" <<opt.stopTolerance <<endl;
    if(earlyPhase) earlyPhase=false;
    else {
      if(opt.stopGTolerance<0.
          || L.get_sumOfGviolations() + L.get_sumOfHviolations() < opt.stopGTolerance)
        return true;
    }
  }
//  if(its>=2 && newtonStop==OptNewton::stopTinySteps) {
//    if(opt.verbose>0) cout <<"** optConstr. StoppingCriterion TinySteps<" <<opt.stopTolerance <<endl;
//    if(earlyPhase) earlyPhase=false;
//    else {
//      if(opt.stopGTolerance<0.
//          || L.get_sumOfGviolations() + L.get_sumOfHviolations() < opt.stopGTolerance)
//        return true;
//    }
//  }
  if(opt.stopEvals>0 && newton.evals>=opt.stopEvals) {
    if(opt.verbose>0) cout <<"** optConstr. StoppingCriterion MAX EVALS" <<endl;
    return true;
  }
  if(opt.stopIters>0 && newton.its>=opt.stopIters) {
    if(opt.verbose>0) cout <<"** optConstr. StoppingCriterion MAX ITERS" <<endl;
    return true;
  }
  if(opt.stopOuters>0 && its>=opt.stopOuters) {
    if(opt.verbose>0) cout <<"** optConstr. StoppingCriterion MAX OUTERS" <<endl;
    return true;
  }

  double L_x_before = newton.fx;

  //upate Lagrange parameters
  L.autoUpdate(opt, &newton.fx, newton.gx, newton.Hx);

  if(!!dual) dual=L.lambda;

  if(logFile) {
    (*logFile) <<"{ optConstraint: " <<its <<", mu: " <<L.mu <<", nu: " <<L.nu <<", L_x_beforeUpdate: " <<L_x_before <<", L_x_afterUpdate: " <<newton.fx <<", errors: ["<<L.get_costs() <<", " <<L.get_sumOfGviolations() <<", " <<L.get_sumOfHviolations() <<"], lambda: " <<L.lambda <<" }," <<endl;
  }

  return false;
}

uint OptConstrained::run() {
//  earlyPhase=true;
  while(!step());
//  newton.beta *= 1e-3;
//  step();
  return newton.evals;
}

OptConstrained::~OptConstrained() {
}


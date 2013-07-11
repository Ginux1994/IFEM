// $Id$
//==============================================================================
//!
//! \file NonLinSIM.h
//!
//! \date Jun 1 2010
//!
//! \author Knut Morten Okstad / SINTEF
//!
//! \brief Nonlinear solution driver for isogeometric FEM simulators.
//!
//==============================================================================

#ifndef _NONLIN_SIM_H
#define _NONLIN_SIM_H

#include "MultiStepSIM.h"
#include "SIMenums.h"

class TimeStep;
class TimeDomain;


/*!
  \brief Nonlinear solution driver for isogeometric FEM simulators.
  \details This class contains data and methods for computing the nonlinear
  solution to a FE problem based on splines/NURBS basis functions, through
  Newton-Raphson iterations.
*/

class NonLinSIM : public MultiStepSIM
{
public:
  //! \brief Enum describing the norm used for convergence checks.
  enum CNORM { NONE, L2, ENERGY };
  //! \brief Enum describing reference norm options.
  enum NormOp { MAX, ALL };

  //! \brief The constructor initializes default solution parameters.
  //! \param sim Pointer to the spline FE model
  //! \param[in] n Which type of iteration norm to use in convergence checks
  NonLinSIM(SIMbase& sim, CNORM n = ENERGY);
  //! \brief Empty destructor.
  virtual ~NonLinSIM() {}

  //! \brief Initializes the primary solution vectors.
  //! \param[in] nSol Number of consequtive solutions stored
  //! \param[in] initVal Initial values of the primary solution
  virtual void init(size_t nSol = 2, const RealArray& initVal = RealArray());

  //! \brief Sets the initial guess in the Newton-Raphson iterations.
  //! \param[in] value Initial values of the primary solution
  void setInitialGuess(const RealArray& value);

  //! \brief Advances the time/load step one step forward.
  //! \param param Time stepping parameters
  //! \param[in] updateTime If \e false, the time parameters are not incremented
  virtual bool advanceStep(TimeStep& param, bool updateTime = true);

  //! \brief Solves the nonlinear equations by Newton-Raphson iterations.
  //! \param[in] zero_toler Truncate norm values smaller than this to zero
  //! \param[in] outPrec Number of digits after the decimal point in norm print
  SIM::ConvStatus solve(double zero_toler = 1.0e-8,
                        std::streamsize outPrec = 0);

  //! \brief Solves the nonlinear equations by Newton-Raphson iterations.
  //! \param param Time stepping parameters
  //! \param[in] mode Solution mode to use for this step
  //! \param[in] energyNorm If \e true, integrate energy norm of the solution
  //! \param[in] zero_tolerance Truncate norm values smaller than this to zero
  //! \param[in] outPrec Number of digits after the decimal point in norm print
  SIM::ConvStatus solveStep(TimeStep& param,
                            SIM::SolutionMode mode = SIM::STATIC,
                            bool energyNorm = false,
                            double zero_tolerance = 1.0e-8,
                            std::streamsize outPrec = 0);

  //! \brief Computes and prints some solution norm quantities.
  //! \param[in] time Parameters for nonlinear/time-dependent simulations
  //! \param[in] energyNorm If \e true, integrate energy norm of the solution
  //! \param[in] zero_tolerance Truncate norm values smaller than this to zero
  //! \param[in] outPrec Number of digits after the decimal point in norm print
  virtual bool solutionNorms(const TimeDomain& time,
			     bool energyNorm = false,
			     double zero_tolerance = 1.0e-8,
			     std::streamsize outPrec = 0);

protected:
  //! \brief Checks whether the nonlinear iterations have converged or diverged.
  virtual SIM::ConvStatus checkConvergence(TimeStep& param);
  //! \brief Updates configuration variables (solution vector) in an iteration.
  virtual bool updateConfiguration(TimeStep& param);
  //! \brief Performs line search to accelerate convergence.
  virtual bool lineSearch(TimeStep& param);

public:
  //! \brief Parses a data section from an input stream.
  //! \param[in] keyWord Keyword of current data section to read
  //! \param is The file stream to read from
  virtual bool parse(char* keyWord, std::istream& is);

  //! \brief Parses a data section from an XML document.
  //! \param[in] elem The XML element to parse
  virtual bool parse(const TiXmlElement* elem);

protected:
  CNORM  iteNorm; //!< The norm type used to measure the residual
  NormOp refNopt; //!< Reference norm option
  double refNorm; //!< Reference norm value used in convergence checks
  double convTol; //!< Convergence tolerance
  double rTol;    //!< Relative convergence tolerance
  double aTol;    //!< Absolute convergence tolerance
  double divgLim; //!< Relative divergence limit
  double eta;     //!< Line search tolerance
  double alpha;   //!< Iteration acceleration parameter (for line search)
  int    maxit;   //!< Maximum number of iterations in a time/load step
  int    nupdat;  //!< Number of iterations with updated tangent
  int    prnSlow; //!< How many DOFs to print out on slow convergence
};

#endif

// $Id$
//==============================================================================
//!
//! \file NonlinearElasticityUL.h
//!
//! \date Sep 21 2010
//!
//! \author Knut Morten Okstad / SINTEF
//!
//! \brief Integrand implementations for nonlinear elasticity problems.
//!
//==============================================================================

#ifndef _NONLINEAR_ELASTICITY_UL_H
#define _NONLINEAR_ELASTICITY_UL_H

#include "Elasticity.h"


/*!
  \brief Class representing the integrand of the nonlinear elasticity problem.
  \details This class implements an Updated Lagrangian formulation. It inherits
  most of the Elasticity methods, but reimplements the \a kinematics method
  for calculating the deformation gradient and the associated Green-Lagrange
  strain tensor. The \a evalInt and \a evalBou methods are also reimplemented
  to account for the updated geometry.
*/

class NonlinearElasticityUL : public Elasticity
{
public:
  //! \brief The default constructor invokes the parent class constructor.
  //! \param[in] n Number of spatial dimensions
  //! \param[in] lop Load option (0=on initial length, 1=on updated length)
  NonlinearElasticityUL(unsigned short int n = 3, char lop = 0);
  //! \brief Empty destructor.
  virtual ~NonlinearElasticityUL() {}

  //! \brief Prints out problem definition to the given output stream.
  virtual void print(std::ostream& os) const;

  //! \brief Defines the solution mode before the element assembly is started.
  //! \param[in] mode The solution mode to use
  virtual void setMode(SIM::SolutionMode mode);

  //! \brief Evaluates the integrand at an interior point.
  //! \param elmInt The local integral object to receive the contributions
  //! \param[in] detJW Jacobian determinant times integration point weight
  //! \param[in] N Basis function values
  //! \param[in] dNdX Basis function gradients
  //! \param[in] X Cartesian coordinates of current integration point
  virtual bool evalInt(LocalIntegral*& elmInt, double detJW,
		       const Vector& N, const Matrix& dNdX,
		       const Vec3& X) const;

  //! \brief Evaluates the integrand at a boundary point.
  //! \param elmInt The local integral object to receive the contributions
  //! \param[in] detJW Jacobian determinant times integration point weight
  //! \param[in] N Basis function values
  //! \param[in] dNdX Basis function gradients
  //! \param[in] X Cartesian coordinates of current integration point
  //! \param[in] normal Boundary normal vector at current integration point
  //!
  //! \details This method is reimplemented in this class to account for
  //! possibly with-rotated traction fields (non-conservative loads).
  //! For uni-directional (conservative) loads, it is similar to the
  //! \a LinearElasticity::evalBou method.
  virtual bool evalBou(LocalIntegral*& elmInt, double detJW,
                       const Vector& N, const Matrix& dNdX,
                       const Vec3& X, const Vec3& normal) const;

  //! \brief Returns a pointer to an Integrand for solution norm evaluation.
  //! \note The Integrand object is allocated dynamically and has to be deleted
  //! manually when leaving the scope of the pointer variable receiving the
  //! returned pointer value.
  virtual NormBase* getNormIntegrand(AnaSol* = 0) const;

  //! \brief Calculates some kinematic quantities at current point.
  //! \param[in] dNdX Basis function gradients at current point
  //! \param[out] F Deformation gradient at current point
  //! \param[out] E Green-Lagrange strain tensor at current point
  virtual bool kinematics(const Matrix& dNdX, Tensor& F, SymmTensor& E) const;

protected:
  //! \brief Calculates the deformation gradient at current point.
  //! \param[in] dNdX Basis function gradients at current point
  //! \param[out] F Deformation gradient at current point
  bool formDefGradient(const Matrix& dNdX, Tensor& F) const;

protected:
  mutable Matrix dNdx; //!< Basis function gradients in current configuration
  mutable Matrix CB;   //!< Result of the matrix-matrix product C*B

private:
  char loadOp; //!< Load option

  friend class ElasticityNormUL;
};


/*!
  \brief Class representing the integrand of the elasticity energy norm.
  \details This class reimplements the \a evalInt method to use the strain
  energy density value returned by the nonlinear constitutive model.
  It also forwards the mixed interface methods to the corresponding single-field
  methods of the parent class.
*/

class ElasticityNormUL : public ElasticityNorm
{
public:
  //! \brief The only constructor initializes its data members.
  //! \param[in] p The linear elasticity problem to evaluate norms for
  ElasticityNormUL(NonlinearElasticityUL& p) : ElasticityNorm(p) {}
  //! \brief Empty destructor.
  virtual ~ElasticityNormUL() {}

  //! \brief Initializes current element for numerical integration (mixed).
  //! \param[in] MNPC1 Nodal point correspondance for the displacement field
  virtual bool initElement(const std::vector<int>& MNPC1,
                           const std::vector<int>&, size_t)
  {
    return this->ElasticityNorm::initElement(MNPC1);
  }
  //! \brief Initializes current element for boundary integration (mixed).
  //! \param[in] MNPC1 Nodal point correspondance for the displacement field
  virtual bool initElementBou(const std::vector<int>& MNPC1,
			      const std::vector<int>&, size_t)
  {
    return this->ElasticityNorm::initElement(MNPC1);
  }

  //! \brief Evaluates the integrand at an interior point.
  //! \param elmInt The local integral object to receive the contributions
  //! \param[in] detJW Jacobian determinant times integration point weight
  //! \param[in] N Basis function values
  //! \param[in] dNdX Basis function gradients
  //! \param[in] X Cartesian coordinates of current integration point
  virtual bool evalInt(LocalIntegral*& elmInt, double detJW,
		       const Vector& N, const Matrix& dNdX,
		       const Vec3& X) const;
  //! \brief Evaluates the integrand at an interior point (mixed).
  //! \param elmInt The local integral object to receive the contributions
  //! \param[in] detJW Jacobian determinant times integration point weight
  //! \param[in] N1 Basis function values for the displacement field
  //! \param[in] dN1dX Basis function gradients for the displacement field
  //! \param[in] X Cartesian coordinates of current integration point
  virtual bool evalInt(LocalIntegral*& elmInt, double detJW,
                       const Vector& N1, const Vector&,
                       const Matrix& dN1dX, const Matrix&,
                       const Vec3& X) const
  {
    return this->evalInt(elmInt,detJW,N1,dN1dX,X);
  }

  //! \brief Evaluates the integrand at a boundary point (mixed).
  //! \param elmInt The local integral object to receive the contributions
  //! \param[in] detJW Jacobian determinant times integration point weight
  //! \param[in] N1 Basis function values for the displacement field
  //! \param[in] dN1dX Basis function gradients for the displacement field
  //! \param[in] X Cartesian coordinates of current integration point
  //! \param[in] normal Boundary normal vector at current integration point
  virtual bool evalBou(LocalIntegral*& elmInt, double detJW,
                       const Vector& N1, const Vector&,
                       const Matrix& dN1dX, const Matrix&,
                       const Vec3& X, const Vec3& normal) const
  {
    return this->ElasticityNorm::evalBou(elmInt,detJW,N1,dN1dX,X,normal);
  }
};

#endif

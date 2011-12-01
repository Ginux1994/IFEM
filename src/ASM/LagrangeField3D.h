// $Id$
//==============================================================================
//!
//! \file LagrangeField3D.h
//!
//! \date Jun 8 2011
//!
//! \author Runar Holdahl / SINTEF
//!
//! \brief Class for Lagrange-based finite element scalar fields in 3D.
//!
//==============================================================================

#ifndef _LAGRANGE_FIELD_3D_H
#define _LAGRANGE_FIELD_3D_H

#include "Field.h"

class ASMs3DLag;


/*!
  \brief Class for Lagrange-based finite element scalar fields in 3D.

  \details This class implements the methods required to evaluate a 3D Lagrange
  scalar field at a given point in parametrical or physical coordinates.
*/

class LagrangeField3D : public Field
{
public:
  //! \brief The constructor sets the number of space dimensions and fields.
  //! \param[in] patch The spline patch on which the field is to be defined
  //! \param[in] v Array of control point field values
  //! \param[in] name Name of field
  LagrangeField3D(const ASMs3DLag* patch, const RealArray& v,
		  const char* name = NULL);
  //! \brief Empty destructor.
  virtual ~LagrangeField3D() {}

  // Methods to evaluate the field
  //==============================

  //! \brief Computes the value in a given node/control point.
  //! \param[in] node Node number
  double valueNode(size_t node) const;

  //! \brief Computes the value at a given local coordinate.
  //! \param[in] fe Finite element definition
  double valueFE(const FiniteElement& fe) const;

  //! \brief Computes the value at a given global coordinate.
  //! \param[in] x Global/physical coordinate for point
  double valueCoor(const Vec3& x) const;

  //! \brief Computes the gradient for a given local coordinate.
  //! \param[in] fe Finite element
  //! \param[out] grad Gradient of solution in a given local coordinate
  bool gradFE(const FiniteElement& fe, Vector& grad) const;

  //! \brief Computes the gradient for a given global/physical coordinate.
  //! \param[in] x Global coordinate
  //! \param[out] grad Gradient of solution in a given global coordinate
  bool gradCoor(const Vec3& x, Vector& grad) const;

protected:
  Matrix coord; //!< Matrix of nodel coordinates
  int n1; //!< Number of nodes in first parameter direction
  int n2; //!< Number of nodes in second parameter direction
  int n3; //!< Number of nodes in third parameter direction
  int p1; //!< Element order in first parameter direction
  int p2; //!< Element order in second parameter direction
  int p3; //!< Element order in third parameter direction
};

#endif

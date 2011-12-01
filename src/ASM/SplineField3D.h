// $Id$
//==============================================================================
//!
//! \file SplineField3D.h
//!
//! \date Mar 28 2011
//!
//! \author Runar Holdahl / SINTEF
//!
//! \brief Class for spline-based finite element scalar fields in 3D.
//!
//==============================================================================

#ifndef _SPLINE_FIELD_3D_H
#define _SPLINE_FIELD_3D_H

#include "Field.h"

class ASMs3D;

namespace Go {
  class SplineVolume;
}


/*!
  \brief Class for spline-based finite element scalar fields in 3D.

  \details This class implements the functions required to evaluate a 3D
  spline scalar field at a given point in parametrical or physical coordinates.
*/

class SplineField3D : public Field
{
public:
  //! \brief The constructor sets the number of space dimensions and fields.
  //! \param[in] patch The spline patch on which the field is to be defined
  //! \param[in] v Array of control point field values
  //! \param[in] name Name of spline field
  SplineField3D(const ASMs3D* patch, const RealArray& v,
                const char* name = NULL);
  //! \brief Empty destructor.
  virtual ~SplineField3D() {}

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
  const Go::SplineVolume* basis; //!< Spline basis description
  const Go::SplineVolume* vol;   //!< Spline geometry description
};

#endif

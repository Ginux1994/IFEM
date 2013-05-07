// $Id$
//==============================================================================
//!
//! \file GlbForceVec.h
//!
//! \date Dec 13 2012
//!
//! \author Knut Morten Okstad / SINTEF
//!
//! \brief Storage of a global nodal force vector for a FEM problem.
//!
//==============================================================================

#ifndef _GLB_FORCE_VEC_H
#define _GLB_FORCE_VEC_H

#include "GlobalIntegral.h"
#include "MatVec.h"
#include <map>

class LocalIntegral;
class Vec3;
class SAM;


/*!
  \brief Class for storage of a global nodal force vector with assembly methods.
*/

class GlbForceVec : public GlobalIntegral
{
public:
  //! \brief The constructor only sets its reference to the SAM object.
  GlbForceVec(const SAM& _sam) : sam(_sam) {}
  //! \brief Empty destructor.
  virtual ~GlbForceVec() {}

  //! \brief Initializes the global node map and allocates the force vector.
  //! \param[in] globalNodes The global node numbers that will have force terms
  //! \param[in] nfc Number of force components
  bool initNodeMap(const std::vector<int>& globalNodes, size_t nfc);

  //! \brief Initializes the global nodal force vector to zero.
  virtual void initialize(bool = false);
  //! \brief Finalizes the global nodal force vector after element assembly.
  virtual bool finalize(bool = false);

  //! \brief Adds a set of element nodal forces into the global nodal forces.
  //! \param[in] elmObj Pointer to the element nodal forces to add into \a *this
  //! \param[in] elmId Global number of the element associated with \a *elmObj
  virtual bool assemble(const LocalIntegral* elmObj, int elmId);

  //! \brief Returns the global nodal force vector for a specified node.
  //! \param[in] node 1-based global node number to return the forces for
  Vec3 getForce(int node) const;

private:
  const SAM&           sam;     //!< Data for FE assembly management
  Matrix               F;       //!< Global nodal forces
  std::map<int,size_t> nodeMap; //!< Maps from global node number to force index
};

#endif

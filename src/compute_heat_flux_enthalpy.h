/* -*- c++ -*- ----------------------------------------------------------
   LAMMPS - Large-scale Atomic/Molecular Massively Parallel Simulator
   https://www.lammps.org/, Sandia National Laboratories
   LAMMPS development team: developers@lammps.org

   Copyright (2003) Sandia Corporation.  Under the terms of Contract
   DE-AC04-94AL85000 with Sandia Corporation, the U.S. Government retains
   certain rights in this software.  This software is distributed under
   the GNU General Public License.

   See the README file in the top-level LAMMPS directory.
------------------------------------------------------------------------- */

#ifdef COMPUTE_CLASS
// clang-format off
ComputeStyle(heat/flux/enthalpy,ComputeHeatFluxEnthalpy);
// clang-format on
#else

#ifndef LMP_COMPUTE_HEAT_FLUX_ENTHALPY_H
#define LMP_COMPUTE_HEAT_FLUX_ENTHALPY_H

#include "compute.h"

namespace LAMMPS_NS {

class ComputeHeatFluxEnthalpy : public Compute {
 public:
  ComputeHeatFluxEnthalpy(class LAMMPS *, int, char **);
  ~ComputeHeatFluxEnthalpy() override;
  void init() override;
  void compute_vector() override;

 private:
  char *id_ke, *id_pe, *id_stress;
  class Compute *c_ke, *c_pe, *c_stress;
};

}    // namespace LAMMPS_NS

#endif
#endif

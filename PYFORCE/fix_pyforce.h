/* -*- c++ -*- ----------------------------------------------------------
   LAMMPS - Large-scale Atomic/Molecular Massively Parallel Simulator
   http://lammps.sandia.gov, Sandia National Laboratories
   Steve Plimpton, sjplimp@sandia.gov

   Copyright (2003) Sandia Corporation.  Under the terms of Contract
   DE-AC04-94AL85000 with Sandia Corporation, the U.S. Government retains
   certain rights in this software.  This software is distributed under
   the GNU General Public License.

   See the README file in the top-level LAMMPS directory.
------------------------------------------------------------------------- */

#ifdef FIX_CLASS

FixStyle(pyforce,FixPyforce)

#else

#ifndef LMP_FIX_PYFORCE_H
#define LMP_FIX_PYFORCE_H

#include "fix.h"

namespace LAMMPS_NS {

class FixPyforce : public Fix {
 public:
  FixPyforce(class LAMMPS *, int, char **);
  int setmask();
  void init();
  void setup(int);
  void post_force(int);
  void post_force_respa(int, int, int);
  double compute_vector(int);

 private:
  int force_flag;
  double foriginal[3],foriginal_all[3],ftmp[3];
  char **typenames;
};

}

#endif
#endif

/* ERROR/WARNING messages:

E: Illegal ... command

Self-explanatory.  Check the input script syntax and compare to the
documentation for the command.  You can use -echo screen as a
command-line option when running LAMMPS to see the offending line.

E: Fix freeze requires atom attribute torque

The atom style defined does not have this attribute.

E: More than one fix freeze

Only one of these fixes can be defined, since the granular pair
potentials access it.

*/

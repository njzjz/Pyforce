/* ----------------------------------------------------------------------
   LAMMPS - Large-scale Atomic/Molecular Massively Parallel Simulator
   http://lammps.sandia.gov, Sandia National Laboratories
   Steve Plimpton, sjplimp@sandia.gov

   Copyright (2003) Sandia Corporation.  Under the terms of Contract
   DE-AC04-94AL85000 with Sandia Corporation, the U.S. Government retains
   certain rights in this software.  This software is distributed under
   the GNU General Public License.

   See the README file in the top-level LAMMPS directory.
------------------------------------------------------------------------- */

#include <cstring>
#include "fix_pyvelocity.h"
#include "atom.h"
#include "update.h"
#include "modify.h"
#include "comm.h"
#include "respa.h"
#include "error.h"
#include <fstream>
#include <iostream>
#include <cstdlib>

using namespace std;
using namespace LAMMPS_NS;
using namespace FixConst;

/* ---------------------------------------------------------------------- */

FixPyvelocity::FixPyvelocity(LAMMPS *lmp, int narg, char **arg) :
  Fix(lmp, narg, arg)
{
    int ntypes = atom->ntypes;
    if (narg < ntypes+3)
      error->all(FLERR,"Inlegal pyvelocity command: element names do not match atom types. Example: fix 1 all pyvelocity C H O");
    typenames = new char*[ntypes+1];
    for (int itype = 1; itype <= ntypes; itype++) {
      int n = strlen(arg[itype]) + 1;
      typenames[itype] = new char[n];
      strcpy(typenames[itype],arg[itype+2]);
    }
  if(comm->me>0)
	  error->one(FLERR,"MPI is not supported by Pyvelocity now!");
  vector_flag = 1;
  size_vector = 3;
  global_freq = 1;
  extvector = 1;

  velocity_flag = 0;
  voriginal[0] = voriginal[1] = voriginal[2] = 0.0;
}

/* ---------------------------------------------------------------------- */

int FixPyvelocity::setmask()
{
  int mask = 0;
  mask |= END_OF_STEP;
  mask |= END_OF_STEP_RESPA;
  return mask;
}

/* ---------------------------------------------------------------------- */

void FixPyvelocity::init()
{
  // error if more than one pyvelocity fix
  // because accessed by pair style granular and fix gran/diag

  int count = 0;
  for (int i = 0; i < modify->nfix; i++)
    if (strcmp(modify->fix[i]->style,"pyvelocity") == 0) count++;
  if (count > 1) error->all(FLERR,"More than one fix pyvelocity");
}

/* ---------------------------------------------------------------------- */

void FixPyvelocity::setup(int vflag)
{
  if (strstr(update->integrate_style,"verlet"))
    end_of_step(vflag);
  else {
    int nlevels_respa = ((Respa *) update->integrate)->nlevels;
    for (int ilevel = 0; ilevel < nlevels_respa; ilevel++) {
      ((Respa *) update->integrate)->copy_flevel_f(ilevel);
      end_of_step_respa(vflag,ilevel,0);
      ((Respa *) update->integrate)->copy_f_flevel(ilevel);
    }
  }
}

/* ---------------------------------------------------------------------- */

void FixPyvelocity::end_of_step(int vflag)
{
  int *mask = atom->mask;
  int nlocal = atom->nlocal;
  double **x = atom->x;
  double **v = atom->v;
  int *type= atom->type;
  if (igroup == atom->firstgroup) nlocal = atom->nfirst;

  ofstream xout("comb.xyz");
  xout<<nlocal<<"\nfix_pyvelocity by Jinzhe Zeng\n";
  for (int i = 0; i < nlocal; i++)
	  if (mask[i] & groupbit) {
		xout<<typenames[type[i]]<<" "<<x[i][0]<<" "<<x[i][1]<<" "<<x[i][2]<<"\n";
	  }
  xout.close();

  ofstream vout("velocity_before");
  for (int i = 0; i < nlocal; i++)
	  if (mask[i] & groupbit) {
		vout<<v[i][0]<<" "<<v[i][1]<<" "<<v[i][2]<<"\n";
	  }
  vout.close();

  std::system("python -u velocity.py");
  
  voriginal[0] = voriginal[1] = voriginal[2] = 0.0;
  velocity_flag = 0;
  
  ifstream vin("velocity_after");
  if (! vin.is_open())
	error->all(FLERR,"cannot find velocity_after");
  double vtmp[nlocal*3];
  int i=0;
  while(!vin.eof())
    vin >> vtmp[i++];

  for (int i = 0; i < nlocal; i++)
    if (mask[i] & groupbit) {
      voriginal[0] += v[i][0];
      voriginal[1] += v[i][1];
      voriginal[2] += v[i][2];
      v[i][0] = vtmp[3*i];
      v[i][1] = vtmp[3*i+1];
      v[i][2] = vtmp[3*i+2];
    }
}

/* ---------------------------------------------------------------------- */

void FixPyvelocity::end_of_step_respa(int vflag, int ilevel, int iloop)
{
  end_of_step(vflag);
}

/* ----------------------------------------------------------------------
   return components of total velocity on fix group before velocity was changed
------------------------------------------------------------------------- */

double FixPyvelocity::compute_vector(int n)
{
  // only sum across procs one time

  if (velocity_flag == 0) {
    MPI_Allreduce(voriginal,voriginal_all,3,MPI_DOUBLE,MPI_SUM,world);
    velocity_flag = 1;
  }
  return voriginal_all[n];
}

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
#include "fix_pyforce.h"
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

FixPyforce::FixPyforce(LAMMPS *lmp, int narg, char **arg) :
  Fix(lmp, narg, arg)
{
    int ntypes = atom->ntypes;
    if (narg < ntypes+3)
      error->all(FLERR,"Inlegal pyforce command: element names do not match atom types. Example: fix 1 all pyforce C H O");
    typenames = new char*[ntypes+1];
    for (int itype = 1; itype <= ntypes; itype++) {
      int n = strlen(arg[itype]) + 1;
      typenames[itype] = new char[n];
      strcpy(typenames[itype],arg[itype+2]);
    }

  vector_flag = 1;
  size_vector = 3;
  global_freq = 1;
  extvector = 1;

  force_flag = 0;
  foriginal[0] = foriginal[1] = foriginal[2] = 0.0;

}

/* ---------------------------------------------------------------------- */

int FixPyforce::setmask()
{
  int mask = 0;
  mask |= POST_FORCE;
  mask |= POST_FORCE_RESPA;
  return mask;
}

/* ---------------------------------------------------------------------- */

void FixPyforce::init()
{
  // error if more than one pyforce fix
  // because accessed by pair style granular and fix gran/diag

  int count = 0;
  for (int i = 0; i < modify->nfix; i++)
    if (strcmp(modify->fix[i]->style,"pyforce") == 0) count++;
  if (count > 1) error->all(FLERR,"More than one fix pyforce");

}

/* ---------------------------------------------------------------------- */

void FixPyforce::setup(int vflag)
{
  if (strstr(update->integrate_style,"verlet"))
    post_force(vflag);
  else {
    int nlevels_respa = ((Respa *) update->integrate)->nlevels;
    for (int ilevel = 0; ilevel < nlevels_respa; ilevel++) {
      ((Respa *) update->integrate)->copy_flevel_f(ilevel);
      post_force_respa(vflag,ilevel,0);
      ((Respa *) update->integrate)->copy_f_flevel(ilevel);
    }
  }
}

/* ---------------------------------------------------------------------- */

void FixPyforce::post_force(int vflag)
{
  double **f = atom->f;
  int *mask = atom->mask;
  int nlocal = atom->nlocal;
  double **x = atom->x;
  int *type= atom->type;
  if (igroup == atom->firstgroup) nlocal = atom->nfirst;

  ofstream fout("comb.xyz");
  fout<<nlocal<<"\nfix_pyforce by Jinzhe Zeng\n";
  for (int i = 0; i < nlocal; i++)
	  if (mask[i] & groupbit) {
		fout<<typenames[type[i]]<<" "<<x[i][0]<<" "<<x[i][1]<<" "<<x[i][2]<<"\n";
	  }
  fout.close();

  std::system("python -u force.py");
  
  foriginal[0] = foriginal[1] = foriginal[2] = 0.0;
  force_flag = 0;
  
  ifstream fin("force.dat");
  if (! fin.is_open())
	error->all(FLERR,"cannot find force.dat");
  double ftmp[nlocal*3];
  int i=0;
  while(!fin.eof())
    fin >> ftmp[i++];

  for (int i = 0; i < nlocal; i++)
    if (mask[i] & groupbit) {
      foriginal[0] += f[i][0];
      foriginal[1] += f[i][1];
      foriginal[2] += f[i][2];
      f[i][0] = ftmp[3*i];
      f[i][1] = ftmp[3*i+1];
      f[i][2] = ftmp[3*i+2];
    }
}

/* ---------------------------------------------------------------------- */

void FixPyforce::post_force_respa(int vflag, int ilevel, int iloop)
{
  post_force(vflag);
}

/* ----------------------------------------------------------------------
   return components of total force on fix group before force was changed
------------------------------------------------------------------------- */

double FixPyforce::compute_vector(int n)
{
  // only sum across procs one time

  if (force_flag == 0) {
    MPI_Allreduce(foriginal,foriginal_all,3,MPI_DOUBLE,MPI_SUM,world);
    force_flag = 1;
  }
  return foriginal_all[n];
}

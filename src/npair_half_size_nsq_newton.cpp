// clang-format off
/* ----------------------------------------------------------------------
   LAMMPS - Large-scale Atomic/Molecular Massively Parallel Simulator
   https://www.lammps.org/, Sandia National Laboratories
   LAMMPS development team: developers@lammps.org

   Copyright (2003) Sandia Corporation.  Under the terms of Contract
   DE-AC04-94AL85000 with Sandia Corporation, the U.S. Government retains
   certain rights in this software.  This software is distributed under
   the GNU General Public License.

   See the README file in the top-level LAMMPS directory.
------------------------------------------------------------------------- */

#include "npair_half_size_nsq_newton.h"

#include "atom.h"
#include "atom_vec.h"
#include "domain.h"
#include "error.h"
#include "molecule.h"
#include "group.h"
#include "my_page.h"
#include "neigh_list.h"

using namespace LAMMPS_NS;

/* ---------------------------------------------------------------------- */

NPairHalfSizeNsqNewton::NPairHalfSizeNsqNewton(LAMMPS *lmp) : NPair(lmp) {}

/* ----------------------------------------------------------------------
   size particles
   N^2 / 2 search for neighbor pairs with full Newton's 3rd law
   pair added to list if atoms i and j are both owned and i < j
   if j is ghost only me or other proc adds pair
   decision based on itag,jtag tests
------------------------------------------------------------------------- */

void NPairHalfSizeNsqNewton::build(NeighList *list)
{
  int i,j,jh,n,bitmask,which,imol,iatom,moltemplate;
  tagint itag,jtag,tagprev;
  double xtmp,ytmp,ztmp,delx,dely,delz,rsq;
  double radi,radsum,cutsq;
  int *neighptr;

  double **x = atom->x;
  double *radius = atom->radius;
  tagint *tag = atom->tag;
  int *type = atom->type;
  int *mask = atom->mask;
  tagint *molecule = atom->molecule;
  tagint **special = atom->special;
  int **nspecial = atom->nspecial;
  int nlocal = atom->nlocal;
  int nall = nlocal + atom->nghost;
  if (includegroup) {
    nlocal = atom->nfirst;
    bitmask = group->bitmask[includegroup];
  }

  int *molindex = atom->molindex;
  int *molatom = atom->molatom;
  Molecule **onemols = atom->avec->onemols;
  if (molecular == Atom::TEMPLATE) moltemplate = 1;
  else moltemplate = 0;

  int history = list->history;
  int *ilist = list->ilist;
  int *numneigh = list->numneigh;
  int **firstneigh = list->firstneigh;
  MyPage<int> *ipage = list->ipage;

  int mask_history = 1 << HISTBITS;

  int inum = 0;
  ipage->reset();

  for (i = 0; i < nlocal; i++) {
    n = 0;
    neighptr = ipage->vget();

    itag = tag[i];
    xtmp = x[i][0];
    ytmp = x[i][1];
    ztmp = x[i][2];
    radi = radius[i];
    if (moltemplate) {
      imol = molindex[i];
      iatom = molatom[i];
      tagprev = tag[i] - iatom - 1;
    }

    // loop over remaining atoms, owned and ghost

    for (j = i+1; j < nall; j++) {
      if (includegroup && !(mask[j] & bitmask)) continue;

      if (j >= nlocal) {
        jtag = tag[j];
        if (itag > jtag) {
          if ((itag+jtag) % 2 == 0) continue;
        } else if (itag < jtag) {
          if ((itag+jtag) % 2 == 1) continue;
        } else {
          if (x[j][2] < ztmp) continue;
          if (x[j][2] == ztmp) {
            if (x[j][1] < ytmp) continue;
            if (x[j][1] == ytmp && x[j][0] < xtmp) continue;
          }
        }
      }

      if (exclude && exclusion(i,j,type[i],type[j],mask,molecule)) continue;

      delx = xtmp - x[j][0];
      dely = ytmp - x[j][1];
      delz = ztmp - x[j][2];
      rsq = delx*delx + dely*dely + delz*delz;
      radsum = radi + radius[j];
      cutsq = (radsum+skin) * (radsum+skin);

      if (rsq <= cutsq) {
        jh = j;
        if (history && rsq < radsum*radsum)
          jh = jh ^ mask_history;

        if (molecular != Atom::ATOMIC) {
          if (!moltemplate)
            which = find_special(special[i],nspecial[i],tag[j]);
          else if (imol >= 0)
            which = find_special(onemols[imol]->special[iatom],
                                 onemols[imol]->nspecial[iatom],
                                 tag[j]-tagprev);
          else which = 0;
          if (which == 0) neighptr[n++] = jh;
          else if (domain->minimum_image_check(delx,dely,delz))
            neighptr[n++] = jh;
          else if (which > 0) neighptr[n++] = jh ^ (which << SBBITS);
        } else neighptr[n++] = jh;
      }
    }

    ilist[inum++] = i;
    firstneigh[i] = neighptr;
    numneigh[i] = n;
    ipage->vgot(n);
    if (ipage->status())
      error->one(FLERR,"Neighbor list overflow, boost neigh_modify one");
  }

  list->inum = inum;
}

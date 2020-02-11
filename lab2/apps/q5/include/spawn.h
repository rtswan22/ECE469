#ifndef __USERPROG__
#define __USERPROG__

#define H20 0
#define SO4 1
#define H2 2
#define O2 3
#define SO2 4
#define H2SO4 5
#define MOLECULE_TYPES 6
#define INJECTION_TYPES 2

#define R_2H2O 0
#define R_SO4 1
#define R_H2_O2_SO2 2
#define REACTION_TYPES 3

typedef struct sems_and_counts {
  sem_t s_procs_completed;
  sem_t molecules[MOLECULE_TYPES];
  int inject_count[INJECTION_TYPES];
  int react_count[REACTION_TYPES];
} sems_and_counts;

#define INJECT_H2O "inject_H2O.dlx.obj"
#define INJECT_SO4 "inject_SO4.dlx.obj"
#define REACT_2H2O "react_2H2O.dlx.obj"
#define REACT_SO4 "react_SO4.dlx.obj"
#define REACT_H2_O2_SO2 "react_H2_O2_SO2.dlx.obj"

#endif

#ifndef _BPJ_REITREI4_GRID_H
#define _BPJ_REITREI4_GRID_H

#include "ray4shapes.h"
#include <cstdio>

#ifdef DEBUG
extern bool isDebug;
inline void debugOn() {
#ifdef VERBOSE
  printf("* Debug on\n");
#endif
  isDebug = 1;
}
inline void debugOff() {
#ifdef VERBOSE
  printf("* Debug off\n"); 
#endif
  isDebug = 0;
}
#endif

class Ray4Grid {
 protected:
  Vect4 Cn, Sz;
  Ray4ObjectSet outer;
  Ray4ObjectSet* sets;
  int n[3];
  Ray4Object* box;

  void getGridBounds(Vect4* L, Vect4* U) const;
  bool escaped(int cellno[3]) const;
  void getCellNo(const Vect4& v, int* cellno) const;
  void getCellBounds(int cellno[3], Vect4* L, Vect4* U) const;
  Ray4ObjectSet& getSet(int cellno[3]) const;

 public:
  Ray4Grid();
  ~Ray4Grid();
  void setup(const Ray4ObjectSet& objects);
  Ray4Intersection intersect(const Vect4& O, const Vect4& D) const;
};

#endif

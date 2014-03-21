#ifndef _BPJ_REITREI5_GRID_H
#define _BPJ_REITREI5_GRID_H

#include "ray5shapes.h"
#include <cstdio>

class Ray5Grid {
 protected:
  Vect4 Cn, Sz;//Center and size?
  Ray5ObjectSet outer;
  Ray5ObjectSet* sets;
  int n[3];
  Ray5Object* box;

  void getGridBounds(Vect4* L, Vect4* U) const;
  bool escaped(int cellno[3]) const;
  void getCellNo(const Vect4& v, int* cellno) const;
  void getCellBounds(int cellno[3], Vect4* L, Vect4* U) const;
  Ray5ObjectSet& getSet(int cellno[3]) const;

 public:
  Ray5Grid();
  ~Ray5Grid();
  void setup(const Ray5ObjectSet& objects);
  Ray5Intersection intersect(const Vect4& O, const Vect4& D, TraceMode mode = TRACE_NORMAL) const;
};

#endif

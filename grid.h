#ifndef _BPJ_REITREI_GRID_H
#define _BPJ_REITREI_GRID_H

#include "ray5shapes.h"
#include <cstdio>

class Grid {  
public:
  typedef int CellNumber[3];

protected:
  Vect4 corner, size;
  Ray5Box box;
  Ray5ObjectSet outer;
  std::vector<Ray5ObjectSet> sets;
  CellNumber count;

  void getGridCorners(Vect4& lower, Vect4& upper) const;
  void getCellCorners(Vect4& lower, Vect4& upper, CellNumber cell) const;
  bool isInGrid(CellNumber cell) const;
  void getCellNumber(const Vect4& v, CellNumber cell) const;
  Ray5ObjectSet& getObjects(CellNumber cell);
  const Ray5ObjectSet& getObjects(CellNumber cell) const;

 public:
  Grid();
  Grid(const Ray5ObjectSet& objects);

  Ray5Intersection intersect(const Vect4& O, const Vect4& D, TraceMode mode = TRACE_NORMAL) const;
};

#endif

#ifndef _BPJ_REITREI_GRID_H
#define _BPJ_REITREI_GRID_H

#include "shapes.h"

class Grid {  
public:
  typedef int CellNumber[3];

protected:
  Vect3 corner, size;
  Box box;
  ObjectSet outer;
  std::vector<ObjectSet> sets;
  CellNumber count;

  void getGridCorners(Vect3& lower, Vect3& upper) const;
  void getCellCorners(Vect3& lower, Vect3& upper, CellNumber cell) const;
  bool isInGrid(CellNumber cell) const;
  void getCellNumber(const Vect3& v, CellNumber cell) const;
  ObjectSet& getObjects(CellNumber cell);
  const ObjectSet& getObjects(CellNumber cell) const;

 public:
  Grid();
  Grid(const ObjectSet& objects);

  Intersection intersect(const Vect3& O, const Vect3& D, TraceMode mode = TRACE_NORMAL) const;
};

#endif

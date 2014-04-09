#include "grid.h"

void Grid::getGridCorners(Vect4& lower, Vect4& upper) const {
  lower = upper = corner;
  for (int i = 0; i < 3; i++)
    upper[i] += (count[i] + 1) * size[i];
}

void Grid::getCellCorners(Vect4& lower, Vect4& upper, CellNumber cell) const {
  for (int i = 0; i < 3; i++) {
    lower[i] = corner[i] + size[i] * cell[i];
    upper[i] = lower[i] + size[i];
  }
}

bool Grid::isInGrid(CellNumber cell) const {
  for (int i = 0; i < 3; i++)
    if (cell[i] < 0 || cell[i] >= count[i]) return 0;
  return 1;
}

void Grid::getCellNumber(const Vect4& v, CellNumber cell) const {
  Real x;
  for (int i = 0; i < 3; i++) {
    x = v[i] - corner[i];//Translate v to grid-space
    cell[i] = (int)floor(x / size[i]);
  }
}

ObjectSet& Grid::getObjects(CellNumber cell) {
  return sets[cell[2] * count[0] * count[1] + cell[1] * count[0] + cell[0]];
}

const ObjectSet& Grid::getObjects(CellNumber cell) const {
  return sets[cell[2] * count[0] * count[1] + cell[1] * count[0] + cell[0]];
}

Grid::Grid() {count[0] = count[1] = count[2] = 0;}
 
Grid::Grid(const ObjectSet& objects) {
  /*
   * Determine bounding box extrema, size, and reference corner
   */ 
  Vect4 L, U;
  objects.getBounds(&L, &U);
  L = L - Vect4(1, 1, 1) * EPS;
  U = U + Vect4(1, 1, 1) * EPS;
  corner = L;
  size = U - L;
 
  /*
   * Construct bounding box object
   */
  box.scale((U - L) / 2);
  box.translate((U + L) / 2);

  /*
   * Construct cells and determine grid dimensions
   */
  int n = objects.countBounded();
  if (n) {
    Real s = pow(size[0] * size[1] * size[2] / n, 1.0 / 3);
    for (int i = 0; i < 3; i++) {
      count[i] = (int)ceil(size[i] / s);
      size[i] /= count[i];
    }
  }
  else count[0] = count[1] = count[2] = 1;
  sets.resize(count[0] * count[1] * count[2]);

  /*
   * Insert objects into appropriate cells
   */

  Vect4 Lobj, Uobj;
  CellNumber lbound, ubound, cell;
  for (int i = 0; i < objects.count(); i++) {
    if (objects[i]->infBounds()) {
      outer.add(objects[i]);
      continue;
    }
    
    objects[i]->getBounds(&Lobj, &Uobj);
    
    getCellNumber(Lobj, lbound);
    getCellNumber(Uobj, ubound);
    for (cell[2] = lbound[2]; cell[2] <= ubound[2]; cell[2]++)
      for (cell[1] = lbound[1]; cell[1] <= ubound[1]; cell[1]++)
	for (cell[0] = lbound[0]; cell[0] <= ubound[0]; cell[0]++)
	  getObjects(cell).add(objects[i]);
  }
}

Intersection Grid::intersect(const Vect4& O, const Vect4& D, TraceMode mode) const {
  Intersection best = outer.intersect(O, D, mode), test;  
  CellNumber cell; //Current cell
  Real t0, t1;//t values for entry and exit wrt cell

  getCellNumber(O, cell);  

  /*
   * If origin of ray is outside the grid, determine if the ray will hit the box
   */

  if (isInGrid(cell)) {
    t1 = 0.0;
  }
  else {
    Box box = this->box;//uf delete this when constness is restored to Object class
    test = box.intersects(O, D);
    if (test.t < 0) return best;

    getCellNumber(test.P + D * EPS, cell);
    t1 = test.t;
  }

  Vect4 lower, upper; //Cell corners
  Real t;
  int axis = 0;
  do {
    /*
     * Compute t-values in and out of the cell
     */
    t0 = t1;//t in is the previous t out
    t1 = 1e99;//t out will be computed next

    getCellCorners(lower, upper, cell);
    for (int i = 0; i < 3; i++) {
      //determine t of cell exit on axis i
      if (D[i] < 0) t = (lower[i] - O[i]) / D[i];
      else if (D[i] > 0) t = (upper[i] - O[i]) / D[i];
      else continue;

      if (t < t1) {
	axis = i;
	t1 = t;
      }
    }

    //Remember that in order to be a valid hit, the intersection has to be inside the cell of interest
    test = getObjects(cell).intersect(O, D, mode);
    if (test.nearerThan(best) && test.t < t1 + EPS)
      return test;

    if (D[axis] < 0) cell[axis]--;
    else cell[axis]++;
    
  } while (isInGrid(cell));
  
  return best;
}

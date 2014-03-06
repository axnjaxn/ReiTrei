#include "ray4grid.h"

bool isDebug;

void Ray4Grid::getGridBounds(Vect4* L, Vect4* U) const {
  *L = *U = Cn;
  for (int i = 0; i < 3; i++)
    (*U)[i] += (n[i] + 1) * Sz[i];
}

bool Ray4Grid::escaped(int cellno[3]) const {
  for (int i = 0; i < 3; i++)
    if (cellno[i] < 0 || cellno[i] >= n[i]) return 1;
  return 0;
}

void Ray4Grid::getCellNo(const Vect4& v, int* cellno) const {
  REAL x;
  for (int i = 0; i < 3; i++) {
    x = v[i] - Cn[i];
    x = x - fmod(x, Sz[i]);
    cellno[i] = (int)(x / Sz[i] + 0.5);
  }
}

void Ray4Grid::getCellBounds(int cellno[3], Vect4* L, Vect4* U) const {
  *L = Vect4();
  for (int i = 0; i < 3; i++)
    (*L)[i] = Cn[i] + Sz[i] * cellno[i];
  *U = *L + Sz;
}

Ray4ObjectSet& Ray4Grid::getSet(int cellno[3]) const {
  return sets[cellno[2] * n[0] * n[1] + cellno[1] * n[0] + cellno[0]];
}

Ray4Grid::Ray4Grid() {
  sets = NULL;
  box = NULL;
}

Ray4Grid::~Ray4Grid() {
  if (sets) delete [] sets;
  if (box) delete box;
}

void Ray4Grid::setup(const Ray4ObjectSet& objects) {
  //Get grid dimensions
  Vect4 L, U;
  objects.getBounds(&L, &U);

  L = L - Vect4(1, 1, 1) * EPS;
  U = U + Vect4(1, 1, 1) * EPS;

  //Memoize for faster initial collision
  box = new Ray4Box();
  box->scale((U - L) / 2);
  box->translate((U + L) / 2);
  
  L = L - Vect4(1, 1, 1) * EPS;
  U = U + Vect4(1, 1, 1) * EPS;

  //Set up grid cells
  Cn = L;
  Sz = U - L;

  int N = objects.countBounded();

  if (N) {
    REAL s = pow(Sz[0] * Sz[1] * Sz[2] / N, 1.0 / 3);
    for (int i = 0; i < 3; i++) n[i] = int(Sz[i] / s + 0.5);
    
    for (int i = 0; i < 3; i++) Sz[i] = Sz[i] / n[i];
  }
  else n[0] = n[1] = n[2] = 1;
  sets = new Ray4ObjectSet [n[0] * n[1] * n[2]];

  //Bin objects
  Vect4 Lobj, Uobj;
  for (int i = 0; i < objects.count(); i++) {
    if (objects[i]->infBounds()) {
      outer.add(objects[i]);
      continue;
    }
    
    objects[i]->getBounds(&Lobj, &Uobj);
    int c[3], c0[3], c1[3];
    getCellNo(Lobj, c0);
    getCellNo(Uobj, c1);
    for (c[2] = c0[2]; c[2] <= c1[2]; c[2]++)
      for (c[1] = c0[1]; c[1] <= c1[1]; c[1]++)
	for (c[0] = c0[0]; c[0] <= c1[0]; c[0]++) 
	  getSet(c).add(objects[i]);
  }

  /*
#ifdef VERBOSE
  for (int i = 0; i < n[0] * n[1] * n[2]; i++)
    printf("Set %d: %d objects.\n", i, sets[i].count());
  printf("Outer: %d objects.\n", outer.count());
  fflush(0);
#endif
  */
}

Ray4Intersection Ray4Grid::intersect(const Vect4& O, const Vect4& D) const {
  Ray4Intersection hit = outer.intersect(O, D), test;
  Vect4 v = O, L, U;
  int cellno[3];
  getCellNo(v, cellno);
  REAL t0, t1;
  
  if (escaped(cellno)) {
    Ray4Intersection in = box->intersects(O, D);
    t1 = in.t;
    if (t1 < 0) return hit;

    v = in.P;
    getCellNo(v, cellno);
  }


  REAL t;
  int d = 0;
  do {
    t0 = t1;
    
    test = getSet(cellno).intersect(O, D);

    getCellBounds(cellno, &L, &U);
    t1 = -1;
    for (int i = 0; i < 3; i++) {
      if (D[i] < 0)
	t = (L[i] - O[i]) / D[i];
      else if (D[i] > 0)
	t = (U[i] - O[i]) / D[i];
      else continue;

      if (t1 == -1 || t < t1) {
	d = i;
	t1 = t;
      }
    }

    if (D[d] < 0) cellno[d]--;
    else cellno[d]++;
    
    v = O + t1 * D;
    
    if (test.nearerThan(hit) && test.t >= t0 && test.t <= t1 + EPS) hit = test;
  } while (!escaped(cellno) && t0 != t1);
  
  return hit;
}

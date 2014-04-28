#include "object.h"

Vect3 Modifier::pointToObject(const Vect3& v) const {return N.mult(v, 1);}

Vect3 Modifier::directionToObject(const Vect3& v) const {return N.mult(v);}

Vect3 Modifier::normalToObject(const Vect3& v) const {return M.transpose().mult(v).unit();}

Vect3 Modifier::pointToWorld(const Vect3& v) const {return M.mult(v, 1);}

Vect3 Modifier::directionToWorld(const Vect3& v) const {return M.mult(v);}

Vect3 Modifier::normalToWorld(const Vect3& v) const {return N.transpose().mult(v).unit();}

Intersection Object::intersects(const Vect3& O, const Vect3& D) const {
  Intersection result = intersectsUnit(pointToObject(O), directionToObject(D));
  if (result.t > 0) {
    result.P = pointToWorld(result.P);
    result.N = normalToWorld(result.N);
  }
  return result;
}

void Object::getBounds(Vect3* lower, Vect3* upper) {
  Vect3 corners[8];
  corners[0] = pointToWorld(Vect3(-1, -1, -1));
  corners[1] = pointToWorld(Vect3(-1, -1, 1));
  corners[2] = pointToWorld(Vect3(-1, 1, -1));
  corners[3] = pointToWorld(Vect3(-1, 1, 1));
  corners[4] = pointToWorld(Vect3(1, -1, -1));
  corners[5] = pointToWorld(Vect3(1, -1, 1));
  corners[6] = pointToWorld(Vect3(1, 1, -1));
  corners[7] = pointToWorld(Vect3(1, 1, 1));
  
  Vect3 U, L;
  U = L = corners[0];
  for (int i = 1; i < 8; i++) {
    for (int j = 0; j < 3; j++) {
      if (corners[i][j] > U[j]) U[j] = corners[i][j];
      if (corners[i][j] < L[j]) L[j] = corners[i][j];
    }
  }
  
  *upper = U; *lower = L;
}


void ObjectSet::release() {
  for (int i = 0; i < objects.size(); i++) delete objects[i];
  objects.clear();
}

int ObjectSet::countBounded() const {
  int sum = 0;
  for (int i = 0; i < count(); i++) if (!objects[i]->infBounds()) sum++;
  return sum;
}

Intersection ObjectSet::intersect(const Vect3& O, const Vect3& D, TraceMode mode) const {
  Intersection nearest, next;
    
  for (int i = 0; i < count(); i++) {
    next = objects[i]->intersects(O, D);
    if (next.nearerThan(nearest)) {
      if ((mode == TRACE_NORMAL  && !objects[i]->material.invisible)
	  || (mode == TRACE_SHADOW && !objects[i]->material.shadowless)) nearest = next;
    }
  }
    
  return nearest;
}

void ObjectSet::getBounds(Vect3* L, Vect3* U) const {
  Vect3 l, u, li, ui;
  bool initial = 1;
  for (int i = 0; i < count(); i++) {
    if (objects[i]->infBounds()) continue;
    objects[i]->getBounds(&li, &ui);
    if (initial) {
      l = li;
      u = ui;
      initial = 0;
    }
    else
      for (int j = 0; j < 3; j++) {
	if (li[j] < l[j]) l[j] = li[j];
	if (ui[j] > u[j]) u[j] = ui[j];
      }
  }
  *L = l;
  *U = u;
}

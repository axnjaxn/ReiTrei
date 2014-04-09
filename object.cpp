#include "object.h"

Intersection Object::intersects(const Vect4& O, const Vect4& D) const {
  Intersection result = intersectsUnit(transformPoint(N, O), transformDirection(N, D));
  if (result.t > 0) {
    result.P = transformPoint(M, result.P);
    result.N = transformDirection(N.transpose(), result.N);
  }
  return result;
}

void Object::getBounds(Vect4* lower, Vect4* upper) {
  Vect4 corners[8];
  corners[0] = transformPoint(M, Vect4(-1, -1, -1));
  corners[1] = transformPoint(M, Vect4(-1, -1, 1));
  corners[2] = transformPoint(M, Vect4(-1, 1, -1));
  corners[3] = transformPoint(M, Vect4(-1, 1, 1));
  corners[4] = transformPoint(M, Vect4(1, -1, -1));
  corners[5] = transformPoint(M, Vect4(1, -1, 1));
  corners[6] = transformPoint(M, Vect4(1, 1, -1));
  corners[7] = transformPoint(M, Vect4(1, 1, 1));
  
  Vect4 U, L;
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

Intersection ObjectSet::intersect(const Vect4& O, const Vect4& D, TraceMode mode) const {
  Intersection nearest, next;
    
  for (int i = 0; i < count(); i++) {
    next = objects[i]->intersects(O, D);
    if (next.nearerThan(nearest)) {
      if (mode == TRACE_NORMAL 
	  || (mode == TRACE_SHADOW && !objects[i]->material.shadowless)) nearest = next;
    }
  }
    
  return nearest;
}

void ObjectSet::getBounds(Vect4* L, Vect4* U) const {
  Vect4 l, u, li, ui;
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

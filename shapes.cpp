#include "shapes.h"
#include <cmath>

Intersection Sphere::intersectsUnit(const Vect3& O, const Vect3& D) const {
  Real a = dot(D, D) / 2;
  Real b = dot(D, O);
  Real c = (dot(O, O) - 1) / 2;

  Real discriminant = b * b - 4 * a * c;
  if (discriminant < 0) return Intersection();
  else discriminant = sqrt(discriminant);

  Real t1 = (-b + discriminant) / (2 * a);
  Real t2 = (-b - discriminant) / (2 * a);
  Real t = (t1 > 0 && (t2 <= 0 || t1 < t2))? t1 : t2;
  if (t <= 0) return Intersection();

  Vect3 P = O + t * D;
  return Intersection(this, t, P, P);
}  

Intersection Box::intersectsUnit(const Vect3& O, const Vect3& D) const {
  Real Tmin[3], Tmax[3];
  for (int i = 0; i < 3; i++)
    if (D[i] > 0) {
      Tmin[i] = (-1 - O[i]) / D[i];
      Tmax[i] = (1 - O[i]) / D[i];
    }
    else if (D[i] < 0) {
      Tmin[i] = (1 - O[i]) / D[i];
      Tmax[i] = (-1 - O[i]) / D[i];
    }
    else {
      Tmin[i] = (-1 - O[i]) / EPS;
      Tmax[i] = (1 - O[i]) / EPS;
    }
    
  Real t0;
  if (Tmin[0] > Tmin[1]) t0 = Tmin[0];
  else t0 = Tmin[1];
  if (Tmin[2] > t0) t0 = Tmin[2];
    
  Real t1;
  if (Tmax[0] < Tmax[1]) t1 = Tmax[0];
  else t1 = Tmax[1];
  if (Tmax[2] < t1) t1 = Tmax[2];

  if (t0 >= t1) return Intersection();

  Real t = (t0 > 0 && (t1 <= 0 || t0 < t1))? t0 : t1;
  if (t <= 0) return Intersection();

  Vect3 P = O + t * D;
  Vect3 N;
  for (int i = 0; i < 3; i++)
    if (P[i] >= 1 - EPS) N[i] = 1;
    else if (P[i] <= -1 + EPS) N[i] = -1;

  return Intersection(this, t, P, N);
}

Intersection Cone::intersectsUnit(const Vect3& O, const Vect3& D) const {
  Intersection best, hit;
  Real t;
  Vect3 N;
  //Test to see if the caps are hit
  if (has_caps && dot(D, Vect3(0, 1, 0)) != 0.0) {
    N = Vect3(0, 1, 0);
    t = dot(N - O, N) / dot(D, N);
    hit = Intersection(this, t, O + t * D, N);
    if (t >= 0 && sqDistance(hit.P, N) < 1.0) best = hit;
    
    N = Vect3(0, -1, 0);
    t = dot(N - O, N) / dot(D, N);
    hit = Intersection(this, t, O + t * D, N);
    if (t >= 0 && sqDistance(hit.P, N) < 1.0 && hit.nearerThan(best))
      best = hit;
  }

  Vect3 Oxz(O[0], 0, O[2]);
  Vect3 Dxz(D[0], 0, D[2]);
  Vect3 P;

  Real a = dot(Dxz, Dxz);
  Real b = 2 * dot(Oxz, Dxz);
  Real c = dot(Oxz, Oxz) - 1;
  Real discriminant = b * b - 4 * a * c;
  if (discriminant < 0.0) return best;
  discriminant = sqrt(discriminant);

  t = (-b + discriminant) / (2 * a);
  P = O + t * D;
  N = Vect3(P[0], 0, P[2]);
  hit = Intersection(this, t, P, N);
  if (t >= 0 && P[1] >= -1 && P[1] <= 1 && hit.nearerThan(best)) best = hit;

  t = (-b - discriminant) / (2 * a);
  P = O + t * D;
  N = Vect3(P[0], 0, P[2]);
  hit = Intersection(this, t, P, N);
  if (t >= 0 && P[1] >= -1 && P[1] <= 1 && hit.nearerThan(best)) best = hit;

  return best;
}

Intersection Plane::intersectsUnit(const Vect3& O, const Vect3& D) const {
  Real t = dot(A - O, N) / dot(D, N);

  if (t > 0) return Intersection(this, t, O + t * D, N);
  else return Intersection();
}

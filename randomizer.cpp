#include "randomizer.h"
#include <ctime>

Randomizer::Randomizer() {seed(0);}

void Randomizer::timeSeed() {seedno = (int)time(NULL); reseed();}

void Randomizer::seed(int i) {seedno = i; reseed();}

void Randomizer::reseed() {srand(seedno);}

void Randomizer::advanceSeed() {seedno++; reseed();}

float Randomizer::uniform() {
#ifdef NO_RAND_MAX
  return (rand() & 0x7FFF) / 32768.0;
#else
  return (rand() & RAND_MAX) / (float)(RAND_MAX + 1);
#endif
}

Vect4 Randomizer::randomSpherical(float radius) {
  Real theta = 2 * PI * uniform(), phi = PI * uniform();
  Real ct = cos(theta), st = sin(theta), cp = cos(phi), sp = sin(phi);
  return Vect4(st * cp, st * sp, ct, 0.0) * radius;
}

Randomizer randomizer;

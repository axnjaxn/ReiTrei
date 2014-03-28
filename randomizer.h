#ifndef _BPJ_REITREI_RANDOMIZER_H
#define _BPJ_REITREI_RANDOMIZER_H

#include "mat4.h"

class Randomizer {
private:
  inline Randomizer(const Randomizer&) { }
  inline Randomizer& operator=(const Randomizer&) {return *this;}

  int seedno;
  
public:
  Randomizer();

  void timeSeed();
  void seed(int i);
  void reseed();
  void advanceSeed();

  float uniform();
  Vect4 randomSpherical(float radius);
};

extern Randomizer randomizer;

#endif

#ifndef _BPJ_REITREI5_SCREEN_H
#define _BPJ_REITREI5_SCREEN_H

#include "mat4.h"
#include <vector>

class Ray5Screen {
 protected:
  Vect4* buffer;
  int w, h;

 public:
  Ray5Screen();
  Ray5Screen(const std::vector<Ray5Screen>& v);
  Ray5Screen(const Ray5Screen& screen);
  ~Ray5Screen();
  
  Ray5Screen& operator=(const Ray5Screen& screen);

  inline int width() const {return w;}
  inline int height() const {return h;}

  void setDimensions(int w, int h);
  inline void setColor(int r, int c, const Vect4& color) {buffer[r * w + c] = color;}
  inline const Vect4& getColor(int r, int c) const {return buffer[r * w + c];}
};

#endif

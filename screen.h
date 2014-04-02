#ifndef _BPJ_REITREI_SCREEN_H
#define _BPJ_REITREI_SCREEN_H

#include "mat4.h"
#include <vector>

class Screen {
 protected:
  Vect4* buffer;
  int w, h;

 public:
  Screen();
  Screen(const std::vector<Screen>& v);
  Screen(const Screen& screen);
  ~Screen();
  
  Screen& operator=(const Screen& screen);

  inline int width() const {return w;}
  inline int height() const {return h;}

  void setDimensions(int w, int h);
  inline void setColor(int r, int c, const Vect4& color) {buffer[r * w + c] = color;}
  inline const Vect4& getColor(int r, int c) const {return buffer[r * w + c];}

  void saveBMP(const char* filename) const;

  Screen differenceMap() const;
};

#endif

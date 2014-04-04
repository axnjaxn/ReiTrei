#ifndef _BPJ_REITREI_SCREEN_H
#define _BPJ_REITREI_SCREEN_H

/*
 * This needs to be renamed texture
 */

#include "mat4.h"
#include <string>
#include <vector>

class Screen {
 protected:
  Vect4* buffer;
  int w, h;

  void saveBMP(const char* filename) const;

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

  Screen differenceMap() const;

#ifndef NO_MAGICK
  inline Screen(const std::string& fn) {buffer = NULL; load_filename(fn);}
  void load_filename(const std::string& fn);
#endif

  void save_filename(const std::string& fn) const;
};

#endif

#ifndef _BPJ_REITREI_TEXTURE_H
#define _BPJ_REITREI_TEXTURE_H

/*
 * This needs to be renamed texture
 */

#include "mat4.h"
#include <string>
#include <vector>

class Texture {
 protected:
  Vect3* buffer;
  int w, h;

  void saveBMP(const char* filename) const;

 public:
  Texture();
  Texture(const std::vector<Texture>& v);
  Texture(const Texture& screen);
  ~Texture();
  
  Texture& operator=(const Texture& screen);

  inline int width() const {return w;}
  inline int height() const {return h;}

  void setDimensions(int w, int h);
  inline void setColor(int r, int c, const Vect3& color) {buffer[r * w + c] = color;}
  inline const Vect3& getColor(int r, int c) const {return buffer[r * w + c];}

  Texture differenceMap() const;

#ifndef NO_MAGICK
  inline Texture(const std::string& fn) {buffer = NULL; load_filename(fn);}
  void load_filename(const std::string& fn);
#endif

  void save_filename(const std::string& fn) const;
};

#endif

#ifndef _BPJ_REITREI5_SCREEN_H
#define _BPJ_REITREI5_SCREEN_H

//uf audit

#include <SDL/SDL.h>
#include "mat4.h"

class Ray5Screen {
 protected:
  class BufferEntry {
  public:
    Vect4 color;
    float depth;

    BufferEntry() {depth = 0.0;}
  };

  //uf move to ByteImage
  SDL_Surface* target;
  int w, h;
  BufferEntry* buffer;

 public:
  Ray5Screen();
  Ray5Screen(const Ray5Screen& screen);
  ~Ray5Screen();
  
  Ray5Screen& operator=(const Ray5Screen& screen);

  inline int width() const {return w;}
  inline int height() const {return h;}
  inline SDL_Surface* getTarget() const {return target;}//deprecated

  void setTargetSurface(SDL_Surface* target);//deprecated
  void setDimensions(int w, int h);
  inline void setColor(int r, int c, const Vect4& color) {buffer[r * w + c].color = color;}
  inline void setDepth(int r, int c, float depth) {buffer[r * w + c].depth = depth;}

  void drawScanline(int r);
  void drawSurface();
};

#endif

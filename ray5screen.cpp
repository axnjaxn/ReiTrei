#include "ray5screen.h"

Ray5Screen::Ray5Screen() {
  target = NULL;
  w = h = 0;
  buffer = NULL;
}

Ray5Screen::Ray5Screen(const Ray5Screen& screen) {
  *this = screen;
}

Ray5Screen::~Ray5Screen() {
  if (buffer) delete [] buffer;
}

Ray5Screen& Ray5Screen::operator=(const Ray5Screen& screen) {
  buffer = NULL;
  setTargetSurface(screen.target);
  setDimensions(screen.w, screen.h);
  memcpy(buffer, screen.buffer, w * h * sizeof(BufferEntry));
}

void Ray5Screen::setTargetSurface(SDL_Surface* target) {this->target = target;}

void Ray5Screen::setDimensions(int w, int h) {
  if (buffer) delete [] buffer;

  this->w = w; this->h = h;
  buffer = new BufferEntry [w * h];
}

inline Uint8 toByte(Real r) {
  if (r > 1.0) return 255;
  else return (Uint8)(255 * r);
}

void Ray5Screen::drawScanline(int r) {
  if (SDL_MUSTLOCK(target)) SDL_LockSurface(target);

  Uint32* bufp = (Uint32*)target->pixels;
  Vect4 color;
  for (int c = 0; c < w; c++) {
    color = buffer[r * w + c].color;
    bufp[r * target->w + c] = SDL_MapRGB(target->format, toByte(color[0]), toByte(color[1]), toByte(color[2]));
  } 

  if (SDL_MUSTLOCK(target)) SDL_UnlockSurface(target);
}

void Ray5Screen::drawSurface() {for (int r = 0; r < h; r++) drawScanline(r);}

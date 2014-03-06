#include "ray5screen.h"

Ray5Screen::Ray5Screen() {
  target = NULL;
  w = h = 0;
  ambient = diffuse = NULL;
}

Ray5Screen::Ray5Screen(const Ray5Screen& screen) {
  *this = screen;
}

Ray5Screen::~Ray5Screen() {
  if (ambient) delete [] ambient;
  if (diffuse) delete [] diffuse;
}

Ray5Screen& Ray5Screen::operator=(const Ray5Screen& screen) {
  ambient = diffuse = NULL;
  setTargetSurface(screen.target);
  setDimensions(screen.w, screen.h);
  for (int i = 0; i < w * h; i++) {
    ambient[i] = screen.ambient[i];
    diffuse[i] = screen.diffuse[i];
  }
}

void Ray5Screen::setTargetSurface(SDL_Surface* target) {this->target = target;}

void Ray5Screen::setDimensions(int w, int h) {
  if (ambient) delete [] ambient;
  if (diffuse) delete [] ambient;

  this->w = w; this->h = h;
  ambient = new Vect4 [w * h];
  diffuse = new Vect4 [w * h];
}

void Ray5Screen::setAmbient(int r, int c, const Vect4& color) {
  ambient[r * w + c] = color;
}

void Ray5Screen::setDiffuse(int r, int c, const Vect4& color) {
  diffuse[r * w + c] = color;
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
    color = diffuse[r * w + c] + ambient[r * w + c];
    bufp[r * target->w + c] = SDL_MapRGB(target->format, toByte(color[0]), toByte(color[1]), toByte(color[2]));
  } 

  if (SDL_MUSTLOCK(target)) SDL_UnlockSurface(target);
}

void Ray5Screen::drawSurface() {for (int r = 0; r < h; r++) drawScanline(r);}

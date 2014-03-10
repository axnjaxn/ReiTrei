#include "ray5screen.h"

Ray5Screen::Ray5Screen() {
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
  setDimensions(screen.w, screen.h);
  memcpy(buffer, screen.buffer, w * h * sizeof(Vect4));
}

void Ray5Screen::setDimensions(int w, int h) {
  if (buffer) delete [] buffer;

  this->w = w; this->h = h;
  buffer = new Vect4 [w * h];
}

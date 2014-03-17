#include "ray5screen.h"
#include <cstring>

Ray5Screen::Ray5Screen() {
  w = h = 0;
  buffer = NULL;
}

Ray5Screen::Ray5Screen(const std::vector<Ray5Screen>& v) {
  buffer = NULL;
  
  if (v.empty()) {
    w = h = 0;
    return;
  }

  setDimensions(v[0].w, v[1].h);
  for (int i = 0; i < v.size(); i++)
    for (int j = 0; j < w * h; j++)
      buffer[j] = buffer[j] + v[i].buffer[j];
  for (int i = 0; i < w * h; i++)
    buffer[i] = buffer[i] / v.size();
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

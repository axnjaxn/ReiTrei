#include "screen.h"
#include <cstring>

Screen::Screen() {
  w = h = 0;
  buffer = NULL;
}

Screen::Screen(const std::vector<Screen>& v) {
  buffer = NULL;
  
  if (v.empty()) {
    w = h = 0;
    return;
  }

  setDimensions(v[0].w, v[0].h);
  for (int i = 0; i < v.size(); i++)
    for (int j = 0; j < w * h; j++)
      buffer[j] = buffer[j] + v[i].buffer[j];
  for (int i = 0; i < w * h; i++)
    buffer[i] = buffer[i] / v.size();
}

Screen::Screen(const Screen& screen) {
  *this = screen;
}

Screen::~Screen() {
  if (buffer) delete [] buffer;
}

Screen& Screen::operator=(const Screen& screen) {
  buffer = NULL;
  setDimensions(screen.w, screen.h);
  memcpy(buffer, screen.buffer, w * h * sizeof(Vect4));
  return *this;
}

void Screen::setDimensions(int w, int h) {
  if (buffer) delete [] buffer;

  this->w = w; this->h = h;
  buffer = new Vect4 [w * h];
}

Screen Screen::differenceMap() const {
  Screen map;
  map.setDimensions(w, h);

  Vect4 Gx, Gy, G;
  for (int r = 1; r < h - 1; r++)
    for (int c = 1; c < w - 1; c++) {
      Gx = getColor(r, c + 1) - getColor(r, c - 1);
      Gy = getColor(r + 1, c) - getColor(r - 1, c);
      for (int i = 0; i < 3; i++) {
	G[i] = sqrt(Gx[i] * Gx[i] + Gy[i] * Gy[i]);
	if (G[i] > 0.25) G[i] = 1.0;
	else G[i] = 0.0;
      }
      map.setColor(r, c, G);
    }
  
  return map;
}

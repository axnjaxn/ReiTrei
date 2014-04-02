#include "screen.h"
#include <cstring>
#include <cstdio>
extern "C" {
#include <stdint.h>
}

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

inline uint8_t toByte(Real r) {return (r < 0)? 0 : (r > 1.0)? 255 : (uint8_t)(255 * r);}

void Screen::saveBMP(const char* filename) const {
  FILE* fp = fopen(filename, "wb");

  //BMP standard header: 14 bytes
  uint8_t stdhead[14];
  stdhead[0] = 0x42;
  stdhead[1] = 0x4D;
  *(uint32_t*)(stdhead+2) = (uint32_t)(54 + 3 * w * h);
  *(uint16_t*)(stdhead+6) = (uint16_t)0;
  *(uint16_t*)(stdhead+8) = (uint16_t)0;
  *(uint32_t*)(stdhead+10) = (uint32_t)54;
  fwrite(stdhead, 1, 14, fp);
  
  //BMP old-style extended header: 40 bytes
  uint8_t exthead[40];
  *(uint32_t*)exthead = (uint32_t)40;
  *(uint32_t*)(exthead+4) = (uint32_t)w;
  *(uint32_t*)(exthead+8) = (uint32_t)h;
  *(uint16_t*)(exthead+12) = (uint16_t)1;
  *(uint16_t*)(exthead+14) = (uint16_t)24;
  *(uint32_t*)(exthead+16) = (uint32_t)0;
  *(uint32_t*)(exthead+20) = (uint32_t)(3 * w * h);
  *(uint32_t*)(exthead+24) = (uint32_t)w;
  *(uint32_t*)(exthead+28) = (uint32_t)h;
  *(uint32_t*)(exthead+32) = (uint32_t)0;
  *(uint32_t*)(exthead+36) = (uint32_t)0;
  fwrite(exthead, 1, 40, fp);

  //Color pixel data, bottom to top, and padded on one side
  Vect4 color;
  int pad_bytes = (w * 3) % 4;
  if (pad_bytes) pad_bytes = 4 - pad_bytes;
  for (int r = h - 1; r >= 0; r--) {
    for (int c = 0; c < w; c++) {
      color = getColor(r, c);
      fprintf(fp, "%c%c%c", toByte(color[2]), toByte(color[1]), toByte(color[0]));
    }
    for (int i = 0; i < pad_bytes; i++) fprintf(fp, "%c", 0);
  }

  fclose(fp);
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

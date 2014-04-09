#ifndef _BPJ_REITREI_RENDERQUEUE_H
#define _BPJ_REITREI_RENDERQUEUE_H

#include "scene.h"
#include "texture.h"
#include <SDL2/SDL.h>
#include <queue>

class RenderQueue {
private:
  RenderQueue(const RenderQueue&) { }
  RenderQueue& operator=(const RenderQueue&) {return *this;}

public:
  class Point {
  public:
    int r, c;
    Point() {r = c = 0;}
    Point(int r, int c) {this->r = r; this->c = c;}
  };

  Scene* scene;
  Texture* screen;
  SDL_mutex* mutex;
  
  std::queue<Point> queue;

  RenderQueue(Scene* scene, Texture* screen);
  ~RenderQueue();
  void push(int r, int c);
  void pushRow(int r); 
  bool empty();
  bool checkPop(Point& p);
};

#endif

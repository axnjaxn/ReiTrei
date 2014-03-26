#ifndef _BPJ_REITREI_RENDERQUEUE_H
#define _BPJ_REITREI_RENDERQUEUE_H

#include "ray5scene.h"
#include "ray5screen.h"
#include <SDL2/SDL.h>
#include <queue>

class RenderQueue {
public:
  class Point {
  public:
    int r, c;
    Point() {r = c = 0;}
    Point(int r, int c) {this->r = r; this->c = c;}
  };

  Ray5Scene* scene;
  Ray5Screen* screen;
  SDL_mutex* mutex;
  
  std::queue<Point> queue;

  RenderQueue(Ray5Scene* scene = NULL, Ray5Screen* screen = NULL) {
    this->scene = scene;
    this->screen = screen;
    mutex = SDL_CreateMutex();
  }
  ~RenderQueue() {
    SDL_DestroyMutex(mutex);
  }

  void push(int r, int c) {
    SDL_LockMutex(mutex);
    queue.push(Point(r, c));
    SDL_UnlockMutex(mutex);
  }

  void pushRow(int r) {
    SDL_LockMutex(mutex);
    for (int c = 0; c < screen->width(); c++)
      queue.push(Point(r, c));
    SDL_UnlockMutex(mutex);
  }

  Point pop() {
    Point p = queue.front();
    queue.pop();
    return p;
  }
  
  bool empty() const {return queue.empty();}
};

#endif

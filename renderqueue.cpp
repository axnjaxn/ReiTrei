#include "renderqueue.h"

RenderQueue::RenderQueue(Ray5Scene* scene, Texture* screen) {
  this->scene = scene;
  this->screen = screen;
  mutex = SDL_CreateMutex();
}

RenderQueue::~RenderQueue() {
  SDL_DestroyMutex(mutex);
}

void RenderQueue::push(int r, int c) {
  SDL_LockMutex(mutex);
  queue.push(Point(r, c));
  SDL_UnlockMutex(mutex);
}

void RenderQueue::pushRow(int r) {
  SDL_LockMutex(mutex);
  for (int c = 0; c < screen->width(); c++)
    queue.push(Point(r, c));
  SDL_UnlockMutex(mutex);
}

bool RenderQueue::empty() {
  SDL_LockMutex(mutex);
  bool b = queue.empty();
  SDL_UnlockMutex(mutex);
  return b;
}

bool RenderQueue::checkPop(Point& p) {
  SDL_LockMutex(mutex);
  if (empty()) {
    SDL_UnlockMutex(mutex);
    return 0;
  }
  else {
    p = queue.front();
    queue.pop();
    SDL_UnlockMutex(mutex);
    return 1;
  }
}

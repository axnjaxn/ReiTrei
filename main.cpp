#define MAX_RECURSE 10
#define TITLE "ReiTrei5"
//#define NO_GRID

#include <ctime>
#include "circlelist.h"
#include "ray5screen.h"
#include "ray5parser.h"
#include "ray5shapes.h"
#include <vector>

Vect4 traceAmbientRay(const Ray5Scene& scene, const Vect4& O, const Vect4& D, int nrecurse = 0) {
  Vect4 color;

  if (nrecurse >= MAX_RECURSE) return color;
 
  Ray5Intersection nearest = scene.intersect(O, D);

  if (nearest.t <= 0) return scene.bgcolor;

  //Ambient lighting
  color += nearest.obj->material.ambient;
  
  //Reflective lighting
  if (nonzero(nearest.obj->material.reflective)) {
    Vect4 _D = D + (2 * nearest.N * -dot(nearest.N, D));
    Vect4 _O = nearest.P + EPS * _D;
    color += nearest.obj->material.reflective.multComp(traceAmbientRay(scene, _O, _D, nrecurse + 1));
  }

  return color;
}

//uf need to change this function
Vect4 traceDiffuseRay(const Ray5Scene& scene, const Vect4& O, const Vect4& D, int nrecurse = 0) {
  Vect4 color;
  
  if (nrecurse >= MAX_RECURSE) return color;
 
  Ray5Intersection nearest = scene.intersect(O, D);

  if (nearest.t <= 0) return color;
  
  //Reflective lighting
  if (nonzero(nearest.obj->material.reflective)) {
    Vect4 _D = D + (2 * nearest.N * -dot(nearest.N, D));
    Vect4 _O = nearest.P + EPS * _D;
    color += nearest.obj->material.reflective.multComp(traceDiffuseRay(scene, _O, _D, nrecurse + 1));
  }

  //uf Where's the nonreflective lighting?
  //uf missing an equation here

  return color;
}

#include <csignal>
inline void segFault(int param) {
  printf("Segmentation fault.");
  exit(0);
}


void traceAt(Ray5Scene& scene, Ray5Screen& screen, int r, int c) {
  Vect4 O = scene.camera.getOrigin();
  Vect4 D = scene.camera.getDirection(r, c);
      
  screen.setAmbient(r, c, traceAmbientRay(scene, O, D));
  screen.setDiffuse(r, c, traceDiffuseRay(scene, O, D));
}

int stripExtension(char* str) {
  for (int i = strlen(str) - 1; i >= 0; i--)
    if (str[i] == '.') {
      str[i] = 0;
      return i;
    }
  return -1;
}

int render(Ray5Scene& scene, Ray5Screen& screen) {
  char titlebuf[200];
  sprintf(titlebuf, "%s",TITLE);
  SDL_WM_SetCaption(titlebuf, NULL);

  Uint32 started = SDL_GetTicks();

  bool exitflag = 0;
  for (int r = 0; r < screen.height(); r++) {
    SDL_Event event;
      while (SDL_PollEvent(&event))
	if (event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)) exit(0);
	else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_RETURN) exitflag = 1;
      if (exitflag) break;

      for (int c = 0; c < screen.width(); c++)
	traceAt(scene, screen, r, c);

      sprintf(titlebuf, "%s [%d / %d]",TITLE, r + 1, screen.height());
      SDL_WM_SetCaption(titlebuf, NULL);

      screen.drawScanline(r);
      SDL_Flip(SDL_GetVideoSurface());
  }

  printf("Tracing complete:\n");
  printf("\tTotal time elapsed: %.3fs\n", 0.001 * (SDL_GetTicks() - started));
  fflush(0);

  SDL_SaveBMP(SDL_GetVideoSurface(), "out.bmp");

  return 0;
}

void printUsage() {
  printf("ReiTrei4 by Brian Jackson\n");
  printf("Usage: ReiTrei4 [options] [scene.ray]\n");
}

int main(int argc, char* argv[]) {
  signal(SIGSEGV, segFault);

  if (argc < 2) {
    printUsage();
    return 0;
  }

#ifdef DEPRECATED
  printf("*Deprecation warning*\n");
  fflush(0); 
#endif
  
  srand(time(NULL));

  Ray5Scene& scene = *Ray5Scene::getInstance();
  Ray5Screen r_screen;
  for (int i = 1; i < argc - 1; i++) {
    /* else */ {
      printf("Unrecognized token \"%s\".\n", argv[i]);
      exit(0);
    }
  }

  parseScene(argv[argc - 1], &scene);
  scene.init();

  /*
   * Set up scene
   */
  scene.camera.setScreen(300, 300, 1, 1, PI / 2);

  if (SDL_Init(SDL_INIT_VIDEO) < 0) exit(1);
  atexit(SDL_Quit);
  SDL_Surface* screen = SDL_SetVideoMode(scene.camera.pxw, scene.camera.pxh, 32, SDL_HWSURFACE|SDL_DOUBLEBUF);

  r_screen.setTargetSurface(screen);
  r_screen.setDimensions(screen->w, screen->h);

  render(scene, r_screen);

  SDL_Event event;
  bool exitflag = 0;
  char mapname[200];
  int j;
  while (!exitflag) {
    while (SDL_PollEvent(&event))
      if (event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)) exitflag = 1;
    SDL_Delay(100);
  }

  return 0;
}

#define MAX_RECURSE 10
#define TITLE "ReiTrei5"
//#define NO_GRID

#include <ctime>
#include "ray5screen.h"
#include "ray5parser.h"
#include "ray5shapes.h"
#include <vector>

float traceDepthRay(const Ray5Scene& scene, const Vect4& O, const Vect4& D) {
  Ray5Intersection nearest = scene.intersect(O, D);

  if (nearest.t <= 0) return 1e99;

  return nearest.t;
}

Vect4 traceColorRay(const Ray5Scene& scene, const Vect4& O, const Vect4& D, int nrecurse = 0) {
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
    color += nearest.obj->material.reflective.multComp(traceColorRay(scene, _O, _D, nrecurse + 1));
  }

  //Lighting-dependent color
  for (int l = 0; l < scene.countLights(); l++) {
    if (!nearest.obj->material.shadowless) {
      Vect4 shadow_ray = scene.getLight(l)->position - nearest.P;
      Vect4 shadow_direction = shadow_ray.unit();
      Ray5Intersection shadow = scene.intersect(nearest.P + EPS * shadow_ray, shadow_direction);
      if (shadow.t > 0 && shadow.t < shadow_ray.length()) continue;

      //Specular
      if (nearest.obj->material.specular > 0 && nearest.obj->material.shininess > 0) {
	Vect4 R = D + (2 * nearest.N * -dot(nearest.N, D));
	Real coef = dot(shadow_direction, R);
	if (coef > 0.0)
	  color += nearest.obj->material.diffuse.multComp(nearest.obj->material.specular * pow(coef, nearest.obj->material.shininess) * scene.getLight(l)->color);
      }
      
      //Diffuse
      Real diffuse_coefficient = dot(shadow_direction, nearest.N);
      if (diffuse_coefficient < 0) {
	if (nearest.obj->material.twosided) diffuse_coefficient = -diffuse_coefficient;
	else diffuse_coefficient = 0;
      }
      color += nearest.obj->material.diffuse.multComp(diffuse_coefficient * scene.getLight(l)->color);
    }    
  }
  
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
      
  screen.setColor(r, c, traceColorRay(scene, O, D));
  screen.setDepth(r, c, traceDepthRay(scene, O, D));
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
  while (!exitflag) {
    while (SDL_PollEvent(&event))
      if (event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)) exitflag = 1;
    SDL_Delay(100);
  }

  return 0;
}

#define MAX_RECURSE 10
#define TITLE "ReiTrei5"
//#define NO_GRID
//#define SHOW_AA

#include "ray5screen.h"
#include "ray5parser.h"
#include "ray5shapes.h"
#include "randomizer.h"
#include <SDL/SDL.h>
#include <vector>

class RenderSettings {
public:
  int nsamples;
  int nrenders;
  int nshadows;
  bool coherence;
  float dof_range;
  bool aa_enabled;
  float aa_threshold;

  RenderSettings() {
    nsamples = nrenders = nshadows = 1;
    coherence = 0;
    dof_range = 0.0;
    aa_enabled = 1;
    aa_threshold = 0.25;
  }
} settings;

Vect4 traceRay(const Ray5Scene& scene, const Vect4& O, const Vect4& D, int nrecurse = 0) {
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
    color += nearest.obj->material.reflective.multComp(traceRay(scene, _O, _D, nrecurse + 1));
  }

  //Lighting-dependent color
  for (int l = 0; l < scene.countLights(); l++) {
    Vect4 lv, shadow_ray;
    Real power = 0.0;
    for (int s = 0; s < settings.nshadows; s++) {
      lv = scene.getLight(l)->position + randomizer.randomSpherical(scene.getLight(l)->radius) - nearest.P;
      shadow_ray = lv.unit();
      Ray5Intersection shadow = scene.intersect(nearest.P + EPS * shadow_ray, shadow_ray, TRACE_SHADOW);
      if (shadow.t < 0 || shadow.t >= lv.length()) power++;
    }
    power /= settings.nshadows;
    if (power == 0.0) continue;

    lv = scene.getLight(l)->position - nearest.P;
    shadow_ray = lv.unit();
    
    //Specular
    if (nearest.obj->material.specular > 0 && nearest.obj->material.shininess > 0) {
      Vect4 R = D + (2 * nearest.N * -dot(nearest.N, D));
      Real coef = dot(shadow_ray, R);
      if (coef > 0.0)
	color += nearest.obj->material.diffuse.multComp(nearest.obj->material.specular * pow(coef, nearest.obj->material.shininess) * scene.getLight(l)->getColor() * power);
    }
    
    //Diffuse
    Real diffuse_coefficient = dot(shadow_ray, nearest.N);
    if (diffuse_coefficient < 0) {
      if (nearest.obj->material.twosided) diffuse_coefficient = -diffuse_coefficient;
      else diffuse_coefficient = 0;
    }
    color += nearest.obj->material.diffuse.multComp(diffuse_coefficient * scene.getLight(l)->getColor() * power);
  }
  
  return color;
}

#include <csignal>
inline void segFault(int param) {
  printf("Segmentation fault.\n");
  exit(0);
}

void traceAt(Ray5Scene& scene, Ray5Screen& screen, int r, int c) {
  if (settings.coherence) randomizer.reseed();

  Vect4 O, D, color;
  float rmag, rth, rx, ry;
  for (int i = 0; i < settings.nsamples; i++) {    
    if (settings.dof_range > 0.0) {
      rmag = randomizer.uniform() * settings.dof_range; 
      rth = randomizer.uniform() * 2 * PI;
      rx = rmag * cos(rth);
      ry = rmag * sin(rth);
    }
    else {rx = ry = 0.0;}

    scene.camera.xrotate(rx);
    scene.camera.yrotate(ry);    
    O = scene.camera.getOrigin();
    D = scene.camera.getDirection(r, c);
    color += traceRay(scene, O, D);
    scene.camera.yrotate(-ry);
    scene.camera.xrotate(-rx);
  }
      
  screen.setColor(r, c, color / settings.nsamples);
}

void traceAt_AA(Ray5Scene& scene, Ray5Screen& screen, int r, int c) {
  if (settings.coherence) randomizer.reseed();

  Vect4 O, D, color;
  float rmag, rth, rx, ry;
  for (int i = 0; i < settings.nsamples; i++) {    
    if (settings.dof_range > 0.0) {
      rmag = randomizer.uniform() * settings.dof_range; 
      rth = randomizer.uniform() * 2 * PI;
      rx = rmag * cos(rth);
      ry = rmag * sin(rth);
    }
    else {rx = ry = 0.0;}

    scene.camera.xrotate(rx);
    scene.camera.yrotate(ry);
    
    O = scene.camera.getOrigin();
    for (Real r1 = -0.5; r1 <= 0.5; r1 += 0.5)
      for (Real c1 = -0.5; c1 <= 0.5; c1 += 0.5) {
	D = scene.camera.getDirection(r + r1, c + c1);
	color += traceRay(scene, O, D) / 9.0;
      }

    scene.camera.yrotate(-ry);
    scene.camera.xrotate(-rx);
  }
      
  screen.setColor(r, c, color / settings.nsamples);
}

int stripExtension(char* str) {
  for (int i = strlen(str) - 1; i >= 0; i--)
    if (str[i] == '.') {
      str[i] = 0;
      return i;
    }
  return -1;
}

inline Uint8 toByte(Real r) {return (r > 1.0)? 255 : (Uint8)(255 * r);}

void redraw(const Ray5Screen& r_screen) {
  SDL_Surface* screen = SDL_GetVideoSurface();
  if (SDL_MUSTLOCK(screen)) SDL_LockSurface(screen);

  Uint32* bufp = (Uint32*)screen->pixels;
  Vect4 color;
  for (int r = 0; r < r_screen.height(); r++) {
    for (int c = 0; c < r_screen.width(); c++) {
      color = r_screen.getColor(r, c);
      bufp[r * screen->w + c] = SDL_MapRGB(screen->format, toByte(color[0]), toByte(color[1]), toByte(color[2]));
    }
  }
  
  if (SDL_MUSTLOCK(screen)) SDL_UnlockSurface(screen);
  SDL_Flip(screen);
}

void render(Ray5Scene& scene, Ray5Screen& r_screen, int renderno = 0, int outof = 1) {
  char titlebuf[200];
  sprintf(titlebuf, "%s",TITLE);
  SDL_WM_SetCaption(titlebuf, NULL);

  SDL_Surface* screen = SDL_GetVideoSurface();

  Uint32* bufp = (Uint32*)screen->pixels;
  Vect4 color;
  bool exitflag = 0;
  for (int r = 0; r < r_screen.height(); r++) {
    SDL_Event event;
    while (SDL_PollEvent(&event))
      if (event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)) exit(0);
      else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_RETURN) exitflag = 1;
    if (exitflag) break;


    if (SDL_MUSTLOCK(screen)) SDL_LockSurface(screen);
    for (int c = 0; c < r_screen.width(); c++) {
      traceAt(scene, r_screen, r, c);
      color = r_screen.getColor(r, c);
      bufp[r * screen->w + c] = SDL_MapRGB(screen->format, toByte(color[0]), toByte(color[1]), toByte(color[2]));
    }

    if (outof > 1)
      sprintf(titlebuf, "%s [%d / %d, %d of %d]",TITLE, r + 1, r_screen.height(), renderno, outof);
    else
      sprintf(titlebuf, "%s [%d / %d]",TITLE, r + 1, r_screen.height());
    SDL_WM_SetCaption(titlebuf, NULL);

    if (SDL_MUSTLOCK(screen)) SDL_UnlockSurface(screen);
    SDL_Flip(screen);
  }

  if (!settings.aa_enabled) return;

  Ray5Screen dmap = r_screen.differenceMap();
  float d;
  for (int r = 0; r < r_screen.height(); r++) {
    SDL_Event event;
    while (SDL_PollEvent(&event))
      if (event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)) exit(0);
      else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_RETURN) exitflag = 1;
    if (exitflag) break;

    if (SDL_MUSTLOCK(screen)) SDL_LockSurface(screen);
    for (int c = 0; c < r_screen.width(); c++) {
      d = dot(dmap.getColor(r, c), Vect4(1, 1, 1, 0));
      if (d > settings.aa_threshold) {
#ifdef SHOW_AA
	color = Vect4(0.0, 1.0, 0.0);
#else
	traceAt_AA(scene, r_screen, r, c);
	color = r_screen.getColor(r, c);
#endif
	bufp[r * screen->w + c] = SDL_MapRGB(screen->format, toByte(color[0]), toByte(color[1]), toByte(color[2]));
      }
    }

    if (outof > 1)
      sprintf(titlebuf, "%s [AA: %d / %d, %d of %d]",TITLE, r + 1, r_screen.height(), renderno, outof);
    else
      sprintf(titlebuf, "%s [AA: %d / %d]",TITLE, r + 1, r_screen.height());
    SDL_WM_SetCaption(titlebuf, NULL);

    if (SDL_MUSTLOCK(screen)) SDL_UnlockSurface(screen);
    SDL_Flip(screen);
  }

}

void printUsage() {
  printf("ReiTrei5 by Brian Jackson\n");
  printf("Usage: ReiTrei5 [options] scene-file\n");
  printf("Options:\n");
  printf("\t--size width height : Give the size of the desired output image\n");
  printf("\t--renders : Turn on multirendering for statistical effects\n");
  printf("\t--samples : Turn on multisampling for statistical effects\n");
  printf("\t--shadows : Set soft-shadow sampling rate\n");
  printf("\t--dof_degrees degrees: Allow depth of field to rotate around the focal point\n");
  printf("\t--coherence: Turn on coherent rendering mode\n");
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
  
  randomizer.timeSeed();

  Ray5Scene& scene = *Ray5Scene::getInstance();
  Ray5Screen r_screen;
  int w = 300, h = 300;
  for (int i = 1; i < argc - 1; i++) {
    if (!strcmp(argv[i], "--size")) {
      sscanf(argv[++i], "%d", &w);
      sscanf(argv[++i], "%d", &h);
    }
    else if (!strcmp(argv[i], "--renders")) {
      sscanf(argv[++i], "%d", &settings.nrenders);
    }
    else if (!strcmp(argv[i], "--samples")) {
      sscanf(argv[++i], "%d", &settings.nsamples);
    }
    else if (!strcmp(argv[i], "--shadows")) {
      sscanf(argv[++i], "%d", &settings.nshadows);
    }
    else if (!strcmp(argv[i], "--coherent")) {
      settings.coherence = 1;
    }
    else if (!strcmp(argv[i], "--dof_degrees")) {
      sscanf(argv[++i], "%f", &settings.dof_range);
      settings.dof_range *= PI / 180.0;
    }
    else {
      printf("Unrecognized token \"%s\".\n", argv[i]);
      exit(0);
    }
  }

  parseScene(argv[argc - 1], &scene);
  scene.init();

  /*
   * Set up scene
   */
  scene.camera.setScreen(w, h, (float)w / h, 1, PI / 2);

  if (SDL_Init(SDL_INIT_VIDEO) < 0) exit(1);
  atexit(SDL_Quit);
  SDL_Surface* screen = SDL_SetVideoMode(scene.camera.pxw, scene.camera.pxh, 32, SDL_HWSURFACE|SDL_DOUBLEBUF);

  r_screen.setDimensions(screen->w, screen->h);

  Uint32 started = SDL_GetTicks();
  
  std::vector<Ray5Screen> screens;

  float rmag, rth, rx, ry;
  for (int i = 0; i < settings.nrenders; i++) {
    render(scene, r_screen, i + 1, settings.nrenders); 
    screens.push_back(r_screen);
    randomizer.advanceSeed();
  }

  r_screen = Ray5Screen(screens);
#ifndef SHOW_AA
  redraw(r_screen);
#endif
  
  printf("Tracing complete:\n");
  printf("\tTotal time elapsed: %.3fs\n", 0.001 * (SDL_GetTicks() - started));
  fflush(0);

  SDL_SaveBMP(SDL_GetVideoSurface(), "out.bmp");

  SDL_Event event;
  bool exitflag = 0;
  while (!exitflag) {
    while (SDL_PollEvent(&event))
      if (event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)) exitflag = 1;
    SDL_Delay(100);
  }

  return 0;
}

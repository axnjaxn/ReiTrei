#define MAX_RECURSE 10
#define TITLE "ReiTrei"
//#define NO_GRID

#include "renderqueue.h"
#include "ray5parser.h"
#include "ray5shapes.h"
#include "randomizer.h"
#include <string>
#include <vector> 

class RenderSettings {
public:
  int nworkers;
  int nsamples;
  int nrenders;
  int nshadows;
  bool coherence;
  float dof_range;
  bool aa_enabled;
  float aa_threshold;

  RenderSettings() {
    nworkers = nsamples = nrenders = nshadows = 1;
    coherence = 0;
    dof_range = 0.0;
    aa_enabled = 1;
    aa_threshold = 0.2;
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
    Real diffuse_power = 0.0, specular_power = 0.0, coef;
    for (int s = 0; s < settings.nshadows; s++) {
      lv = scene.getLight(l)->position + randomizer.randomSpherical(scene.getLight(l)->radius) - nearest.P;
      shadow_ray = lv.unit();

      Ray5Intersection shadow = scene.intersect(nearest.P + EPS * shadow_ray, shadow_ray, TRACE_SHADOW);
      if (shadow.t >= 0 && shadow.t < lv.length()) continue;

      //Specular
      if (nearest.obj->material.specular > 0 && nearest.obj->material.shininess > 0) {
	Vect4 R = D + (2 * nearest.N * -dot(nearest.N, D));
	coef = dot(shadow_ray, R);
	if (scene.getLight(l)->falloff) coef /= lv.sqLength();
	if (coef < 0.0 && nearest.obj->material.twosided) coef = -coef;
	if (coef > 0.0) specular_power += pow(coef, nearest.obj->material.shininess);
      }
    
      //Diffuse
      coef = dot(shadow_ray, nearest.N);
      if (scene.getLight(l)->falloff) coef /= lv.sqLength();
      if (coef < 0.0 && nearest.obj->material.twosided) coef = -coef;
      if (coef > 0.0) diffuse_power += coef;
    }

    diffuse_power /= settings.nshadows;
    specular_power /= settings.nshadows;

    color += nearest.obj->material.diffuse.multComp(scene.getLight(l)->getColor() * diffuse_power);
    color += nearest.obj->material.diffuse.multComp(nearest.obj->material.specular * scene.getLight(l)->getColor() * specular_power);
  }
  
  return color;
}

#include <csignal>
inline void segFault(int param) {
  printf("Segmentation fault.\n");
  exit(0);
}

void traceAt(const Ray5Scene& scene, Ray5Screen& screen, int r, int c) {
  if (settings.coherence) randomizer.reseed();

  Vect4 O, D, color;
  float rmag, rth, rx, ry;
  Ray5Camera camera;
  for (int i = 0; i < settings.nsamples; i++) {    
    if (settings.dof_range > 0.0) {
      rmag = randomizer.uniform() * settings.dof_range; 
      rth = randomizer.uniform() * 2 * PI;
      rx = rmag * cos(rth);
      ry = rmag * sin(rth);
    }
    else {rx = ry = 0.0;}

    camera = scene.camera;
    camera.xrotate(rx);
    camera.yrotate(ry);    
    O = camera.getOrigin();
    D = camera.getDirection(r, c);
    color += traceRay(scene, O, D);
  }
      
  screen.setColor(r, c, color / settings.nsamples);
}

void traceAt_AA(const Ray5Scene& scene, Ray5Screen& screen, int r, int c) {
  if (settings.coherence) randomizer.reseed();

  Vect4 O, D, color;
  float rmag, rth, rx, ry;
  Ray5Camera camera;
  for (int i = 0; i < settings.nsamples; i++) {    
    if (settings.dof_range > 0.0) {
      rmag = randomizer.uniform() * settings.dof_range; 
      rth = randomizer.uniform() * 2 * PI;
      rx = rmag * cos(rth);
      ry = rmag * sin(rth);
    }
    else {rx = ry = 0.0;}

    camera = scene.camera;
    camera.xrotate(rx);
    camera.yrotate(ry);
    
    O = scene.camera.getOrigin();
    for (Real r1 = -0.5; r1 <= 0.5; r1 += 0.5)
      for (Real c1 = -0.5; c1 <= 0.5; c1 += 0.5) {
	D = scene.camera.getDirection(r + r1, c + c1);
	color += traceRay(scene, O, D) / 9.0;
      }
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

#include "pixelrenderer.h"

SDL_Window* window = NULL;
PixelRenderer* px = NULL;

void redraw(const Ray5Screen& screen) {
  Vect4 color;
  for (int r = 0; r < screen.height(); r++) {
    for (int c = 0; c < screen.width(); c++) {
      color = screen.getColor(r, c);
      px->set(r, c, toByte(color[0]), toByte(color[1]), toByte(color[2]));
    }
  }

  px->redraw();
  SDL_RenderPresent(px->getRenderer());
}

int renderThread(void* v) {
  RenderQueue* rq = (RenderQueue*)v;
  RenderQueue::Point p;
  while (rq->checkPop(p)) {
    traceAt(*rq->scene, *rq->screen, p.r, p.c);
  }
  return 0;
}

int renderThread_AA(void* v) {
  RenderQueue* rq = (RenderQueue*)v;
  RenderQueue::Point p;
  while (rq->checkPop(p)) {
    traceAt_AA(*rq->scene, *rq->screen, p.r, p.c);
  }
  return 0;
}

void drawRow(Ray5Screen& screen, int r) {
  Vect4 color;
  for (int c = 0; c < screen.width(); c++) {
    color = screen.getColor(r, c);
    px->set(r, c, toByte(color[0]), toByte(color[1]), toByte(color[2]));
  }
}

void render(Ray5Scene& scene, Ray5Screen& screen, int renderno = 0, int outof = 1) {
  char titlebuf[200];
  sprintf(titlebuf, "%s",TITLE);
  SDL_SetWindowTitle(window, titlebuf);

  Vect4 color;
  bool exitflag = 0;

  int v; //Return value from threads
  RenderQueue rq(&scene, &screen);
  SDL_Thread** threads = new SDL_Thread* [settings.nworkers];

  rq.pushRow(0);
  for (int i = 0; i < settings.nworkers; i++) threads[i] = SDL_CreateThread(&renderThread, NULL, &rq);
  for (int i = 0; i < settings.nworkers; i++) SDL_WaitThread(threads[i], &v);

  for (int r = 1; r < screen.height(); r++) {
    SDL_Event event;
    while (SDL_PollEvent(&event))
      if (event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)) exit(0);
      else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_RETURN) exitflag = 1;
    if (exitflag) break;

    rq.pushRow(r);
    
    for (int i = 0; i < settings.nworkers; i++) threads[i] = SDL_CreateThread(&renderThread, NULL, &rq);

    drawRow(screen, r - 1);
    px->redraw();
    SDL_RenderPresent(px->getRenderer()); 

    for (int i = 0; i < settings.nworkers; i++) SDL_WaitThread(threads[i], &v);

    if (outof > 1) sprintf(titlebuf, "%s [%d / %d, %d of %d]",TITLE, r + 1, screen.height(), renderno, outof);
    else sprintf(titlebuf, "%s [%d / %d]",TITLE, r + 1, screen.height());
    SDL_SetWindowTitle(window, titlebuf);
  }

  drawRow(screen, screen.height() - 1);
  px->redraw();
  SDL_RenderPresent(px->getRenderer()); 
  
  if (!settings.aa_enabled) {
    delete [] threads;
    return;
  }

  Ray5Screen dmap = screen.differenceMap();
  float d;
  for (int r = 1; r < screen.height() - 1; r++) {
    SDL_Event event;
    while (SDL_PollEvent(&event))
      if (event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)) exit(0);
      else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_RETURN) exitflag = 1;
    if (exitflag) break;
    
    for (int c = 1; c < screen.width() - 1; c++) {
      d = dot(dmap.getColor(r, c), Vect4(1, 1, 1, 0));
      if (d > settings.aa_threshold) rq.push(r, c);
    }

    for (int i = 0; i < settings.nworkers; i++) threads[i] = SDL_CreateThread(&renderThread_AA, NULL, &rq);

    drawRow(screen, r - 1);
    px->redraw();
    SDL_RenderPresent(px->getRenderer());

    for (int i = 0; i < settings.nworkers; i++) SDL_WaitThread(threads[i], &v);

    if (outof > 1)
      sprintf(titlebuf, "%s [AA: %d / %d, %d of %d]", TITLE, r + 1, screen.height(), renderno, outof);
    else
      sprintf(titlebuf, "%s [AA: %d / %d]",TITLE, r + 1, screen.height());
    SDL_SetWindowTitle(window, titlebuf);
  }

  delete [] threads;

  drawRow(screen, screen.height() - 2);
  px->redraw();
  SDL_RenderPresent(px->getRenderer());
  SDL_SetWindowTitle(window, TITLE);
}

void printUsage() {
  printf("ReiTrei by Brian Jackson\n");
  printf("Usage: ReiTrei [options] scene-file\n");
  printf("Options:\n");
  printf("\t--size width height : Give the size of the desired output image\n");
  printf("\t--renders : Turn on multirendering for statistical effects\n");
  printf("\t--samples : Turn on multisampling for statistical effects\n");
  printf("\t--shadows : Set soft-shadow sampling rate\n");
  printf("\t--dof_degrees degrees: Allow depth of field to rotate around the focal point\n");
  printf("\t--coherence: Turn on coherent rendering mode\n");
  printf("\t--no-aa: Turn off anti-aliasing\n");
  printf("\t--aa-threshold: Set threshold (1-norm) for anti-aliasing\n");
  printf("\t--output filename: Set filename of rendered bitmap\n");
}

void drawPattern(Ray5Screen& screen) {
  Vect4 black(0.0, 0.0, 0.0), gray(0.125, 0.125, 0.125);
  for (int r = 0; r < screen.height(); r++)
    for (int c = 0; c < screen.width(); c++) {
      if (((r + c) / 5) % 2) screen.setColor(r, c, gray);
      else screen.setColor(r, c, black);
    }  
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

  if (SDL_Init(SDL_INIT_VIDEO) < 0) exit(1);
  atexit(SDL_Quit);

  settings.nworkers = SDL_GetCPUCount();
  if (settings.nworkers > 1) settings.nworkers--; //Use n threads (counting this one) on n-core machines, but 2 threads for single-core machines

  Ray5Scene& scene = *Ray5Scene::getInstance();
  Ray5Screen screen;
  int w = 300, h = 300;
  std::string filename, output = "out.bmp";
  for (int i = 1; i < argc; i++) {
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
    else if (!strcmp(argv[i], "--no-aa")) {
      settings.aa_enabled = 0;
    }
    else if (!strcmp(argv[i], "--aa-threshold")) {
      sscanf(argv[++i], "%f", &settings.aa_threshold);
    }
    else if (!strcmp(argv[i], "--threads")) {
      sscanf(argv[++i], "%d", &settings.nworkers);
    }
    else if (!strcmp(argv[i], "--output") || !strcmp(argv[i], "-o")) {
      output = argv[++i];
    }
    else {
      if (filename.empty()) {
	filename = argv[i];
      }
      else {
	printf("Unrecognized token \"%s\".\n", argv[i]);
	exit(0);
      }
    }
  }

  parseScene(filename.c_str(), &scene);
  scene.init();
  
  /*
   * Set up scene
   */
  scene.camera.setScreen(w, h, (float)w / h, 1, PI / 2);

  window = SDL_CreateWindow(TITLE,
			    SDL_WINDOWPOS_CENTERED,
			    SDL_WINDOWPOS_CENTERED,
			    scene.camera.pxw, scene.camera.pxh,
			    0);
  px = new PixelRenderer(SDL_CreateRenderer(window, -1, 0), scene.camera.pxw, scene.camera.pxh);

  screen.setDimensions(scene.camera.pxw, scene.camera.pxh);

  drawPattern(screen);
  redraw(screen);

  Uint32 started = SDL_GetTicks();
  
  std::vector<Ray5Screen> screens;

  for (int i = 0; i < settings.nrenders; i++) {
    render(scene, screen, i + 1, settings.nrenders); 
    screens.push_back(screen);
    randomizer.advanceSeed();
  }

  screen = Ray5Screen(screens);
#ifndef SHOW_AA
  redraw(screen);
#endif
  
  printf("Tracing complete:\n");
  printf("\tTotal time elapsed: %.3fs\n", 0.001 * (SDL_GetTicks() - started));
  fflush(0);

  SDL_Surface* surf = px->getSurface();
  SDL_SaveBMP(surf, output.c_str());
  SDL_FreeSurface(surf);

  SDL_Event event;
  bool exitflag = 0;
  while (!exitflag) {
    while (SDL_PollEvent(&event))
      if (event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)) exitflag = 1;
    SDL_Delay(100);
  }

  SDL_DestroyRenderer(px->getRenderer());
  delete px;
  SDL_DestroyWindow(window);

  return 0;
}

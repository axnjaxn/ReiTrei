#define TITLE "ReiTrei"
//#define NO_GRID

#include "trace.h"
#include "renderqueue.h"
#include "parser.h"
#include "randomizer.h"

std::string toString(const Vect4& v) {
  static char buf[256];
  sprintf(buf, "<%6.3lf, %6.3f, %6.3lf>", v[0], v[1], v[2]);
  return std::string(buf);
}

#include <csignal>
inline void segFault(int param) {
  printf("Segmentation fault.\n");
  exit(0);
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

void redraw(const Texture& screen) {
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

void drawRow(Texture& screen, int r) {
  Vect4 color;
  for (int c = 0; c < screen.width(); c++) {
    color = screen.getColor(r, c);
    px->set(r, c, toByte(color[0]), toByte(color[1]), toByte(color[2]));
  }
}

void render(Scene& scene, Texture& screen, int renderno = 0, int outof = 1) {
  char titlebuf[200];
  if (settings.show_preview) {
    sprintf(titlebuf, "%s", TITLE);
    SDL_SetWindowTitle(window, titlebuf);
  }

  Vect4 color;
  bool exitflag = 0;

  int v; //Return value from threads
  RenderQueue rq(&scene, &screen);
  SDL_Thread** threads = new SDL_Thread* [settings.nworkers];

  rq.pushRow(0);
  for (int i = 0; i < settings.nworkers; i++) threads[i] = SDL_CreateThread(&renderThread, NULL, &rq);
  for (int i = 0; i < settings.nworkers; i++) SDL_WaitThread(threads[i], &v);

  for (int r = 1; r < screen.height(); r++) {
    if (settings.show_preview) {
      SDL_Event event;
      while (SDL_PollEvent(&event))
	if (event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)) exit(0);
	else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_RETURN) exitflag = 1;
      if (exitflag) break;
    }

    rq.pushRow(r);
    
    for (int i = 0; i < settings.nworkers; i++) threads[i] = SDL_CreateThread(&renderThread, NULL, &rq);

    if (settings.show_preview) {
      drawRow(screen, r - 1);
      px->redraw();
      SDL_RenderPresent(px->getRenderer()); 
    }

    for (int i = 0; i < settings.nworkers; i++) SDL_WaitThread(threads[i], &v);

    if (outof > 1) sprintf(titlebuf, "%s [%d / %d, %d of %d]",TITLE, r + 1, screen.height(), renderno, outof);
    else sprintf(titlebuf, "%s [%d / %d]", TITLE, r + 1, screen.height());

    if (settings.show_preview)
      SDL_SetWindowTitle(window, titlebuf);
    else {
      printf("\r%s", titlebuf);
      fflush(0);
    }
  }

  if (settings.show_preview) {
    drawRow(screen, screen.height() - 1);
    px->redraw();
    SDL_RenderPresent(px->getRenderer()); 
  }
  else printf("\n");
  
  if (!settings.aa_enabled) {
    delete [] threads;
    return;
  }

  Texture dmap = screen.differenceMap();
  float d;
  for (int r = 1; r < screen.height() - 1; r++) {
    if (settings.show_preview) {
      SDL_Event event;
      while (SDL_PollEvent(&event))
	if (event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)) exit(0);
	else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_RETURN) exitflag = 1;
      if (exitflag) break;
    }
    
    for (int c = 1; c < screen.width() - 1; c++) {
      d = dot(dmap.getColor(r, c), Vect4(1, 1, 1, 0));
      if (d > settings.aa_threshold) rq.push(r, c);
    }

    for (int i = 0; i < settings.nworkers; i++) threads[i] = SDL_CreateThread(&renderThread_AA, NULL, &rq);

    if (settings.show_preview) {
      drawRow(screen, r - 1);
      px->redraw();
      SDL_RenderPresent(px->getRenderer());
    }

    for (int i = 0; i < settings.nworkers; i++) SDL_WaitThread(threads[i], &v);

    if (outof > 1) sprintf(titlebuf, "%s [AA: %d / %d, %d of %d]", TITLE, r + 2, screen.height(), renderno, outof);
    else sprintf(titlebuf, "%s [AA: %d / %d]",TITLE, r + 2, screen.height());

    if (settings.show_preview)
      SDL_SetWindowTitle(window, titlebuf);
    else {
      printf("\r%s", titlebuf);
      fflush(0);
    }
  }

  delete [] threads;

  if (settings.show_preview) {
    drawRow(screen, screen.height() - 2);
    px->redraw();
    SDL_RenderPresent(px->getRenderer());
    SDL_SetWindowTitle(window, TITLE);
  }
  else printf("\n");
}

void printUsage() {
  printf("ReiTrei by Brian Jackson\n");
  printf("Usage: ReiTrei [options] scene-file\n");
  printf("Options:\n");
  printf("\t--no-preview : Disable the preview window\n");
  printf("\t--size width height : Give the size of the desired output image\n");
  printf("\t--renders : Turn on multirendering for statistical effects\n");
  printf("\t--samples : Turn on multisampling for statistical effects\n");
  printf("\t--shadows : Set soft-shadow sampling rate\n");
  printf("\t--recursion-depth: Set the maximum depth for reflection / refraction\n");
  printf("\t--dof-degrees degrees: Allow DOF by rotating around the focal point\n");
  printf("\t--coherence: Turn on coherent rendering mode\n");
  printf("\t--point-lights: Force all light sources to behave as point lights\n");
  printf("\t--no-aa: Turn off anti-aliasing\n");
  printf("\t--aa-threshold: Set threshold (1-norm) for anti-aliasing\n");
  printf("\t--output filename: Set filename of rendered bitmap\n");
  printf("\t--no-output: Suppress file output\n");
}

void drawPattern(Texture& screen) {
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

  Scene& scene = *Scene::getInstance();
  Texture screen;
  int w = 300, h = 300;
  bool threads_changed = 0;
  std::string filename, output = "out.bmp";
  for (int i = 1; i < argc; i++) {
    if (!strcmp(argv[i], "--no-preview")) {
      settings.show_preview = 0;
    }
    else if (!strcmp(argv[i], "--size")) {
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
    else if (!strcmp(argv[i], "--recursion-depth")) {
      sscanf(argv[++i], "%d", &settings.recurse_depth);
    }
    else if (!strcmp(argv[i], "--coherent")) {
      settings.coherence = 1;
    }
    else if (!strcmp(argv[i], "--point-lights")) {
      settings.point_lights = 1;
    }
    else if (!strcmp(argv[i], "--dof-degrees")) {
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
      threads_changed = 1;
      sscanf(argv[++i], "%d", &settings.nworkers);
    }
    else if (!strcmp(argv[i], "--output") || !strcmp(argv[i], "-o")) {
      output = argv[++i];
    }
    else if (!strcmp(argv[i], "--no-output")) {
      output = "";
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

  if (settings.show_preview) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) return -1;
  }
  else {
    if (SDL_Init(0) < 0) return -1;
  }
  atexit(SDL_Quit);

  if (!threads_changed) {
    settings.nworkers = SDL_GetCPUCount();
    if (settings.nworkers > 1) settings.nworkers--; //Use n threads (counting this one) on n-core machines, but 2 threads for single-core machines
  }

  try {
    Parser parser;
    parser.parseInto(filename.c_str(), &scene);
  }
  catch (std::exception& e) {
    fprintf(stderr, "%s\n", e.what());
    return 1;
  }
  scene.init();
  
  /*
   * Set up scene
   */
  scene.camera.setScreen(w, h, (float)w / h, 1, PI / 2);
  screen.setDimensions(scene.camera.pxw, scene.camera.pxh);
  drawPattern(screen);

  if (settings.show_preview) {
    window = SDL_CreateWindow(TITLE,
			      SDL_WINDOWPOS_CENTERED,
			      SDL_WINDOWPOS_CENTERED,
			      scene.camera.pxw, scene.camera.pxh,
			      0);
    px = new PixelRenderer(SDL_CreateRenderer(window, -1, 0), scene.camera.pxw, scene.camera.pxh);
    redraw(screen);
  }


  Uint32 started = SDL_GetTicks();
  
  std::vector<Texture> screens;

  for (int i = 0; i < settings.nrenders; i++) {
    render(scene, screen, i + 1, settings.nrenders); 
    screens.push_back(screen);
    randomizer.advanceSeed();
  }

  screen = Texture(screens);
#ifndef SHOW_AA
  if (settings.show_preview)
    redraw(screen);
#endif
  
  printf("Tracing complete:\n");
  printf("\tTotal time elapsed: %.3fs\n", 0.001 * (SDL_GetTicks() - started));
  fflush(0);

  if (output != "") 
    screen.save_filename(output);

  if (settings.show_preview) {
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
  }

  return 0;
}

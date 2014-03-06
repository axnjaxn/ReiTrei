#ifndef _BPJ_REITREI4_SCENE_H
#define _BPJ_REITREI4_SCENE_H

#include <cstdio>
#include <vector>
#include "ray5camera.h"
#include "ray4light.h"
#include "ray4grid.h"

//uf Make this deep copyable or a singleton
//Do not copy this item! Only use pointers, please.
class Ray4Scene {
 protected:
  Ray4ObjectSet objects;
  std::vector<Ray4Light*> lights;
  Ray4Grid grid;

public:
  Ray5Camera camera;
  Vect4 bgcolor;

  Ray4Scene() { }
  ~Ray4Scene() {
    objects.release();
    for (int i = 0; i < lights.size(); i++) delete lights[i];
  }

  inline void addObject(Ray4Object* obj) {objects.add(obj);}
  inline int countObjects() const {return objects.count();}
  inline Ray4Object* operator[](int i) const {return objects[i];}

  inline void addLight(Ray4Light* light) {lights.push_back(light);}
  inline Ray4Light* getLight(int i) const {return lights[i];}
  inline int countLights() const {return lights.size();}

  inline void init() {grid.setup(objects);}
  
  inline Ray4Intersection intersect(const Vect4& O, const Vect4& D) const {
#ifdef NO_GRID
    return objects.intersect(O, D);
#else
    return grid.intersect(O, D);
#endif
  }
};

#endif

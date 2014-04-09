#ifndef _BPJ_REITREI_SCENE_H
#define _BPJ_REITREI_SCENE_H

#include <cstdio>
#include <vector>
#include "camera.h"
#include "light.h"
#include "grid.h"

//uf Why have I made this a singleton class?

class Scene {
private:
  Scene() { }
  Scene(const Scene&) { }
  ~Scene() {
    objects.release();
    for (int i = 0; i < lights.size(); i++) delete lights[i];
  }
  Scene& operator=(const Scene&) {return *this;}

protected:
  ObjectSet objects;
  std::vector<Light*> lights;
  Grid grid;

public:
  Camera camera;
  Vect4 bgcolor;
  
  static Scene* getInstance() {
    static Scene scene;
    return &scene;
  }

  inline void addObject(Object* obj) {objects.add(obj);}
  inline int countObjects() const {return objects.count();}
  inline Object* operator[](int i) const {return objects[i];}

  inline void addLight(Light* light) {lights.push_back(light);}
  inline Light* getLight(int i) const {return lights[i];}
  inline int countLights() const {return lights.size();}

  inline void init() {grid = Grid(objects);}
  
  inline Intersection intersect(const Vect4& O, const Vect4& D, TraceMode mode = TRACE_NORMAL) const {
#ifdef NO_GRID
    return objects.intersect(O, D, mode);
#else
    return grid.intersect(O, D, mode);
#endif
  }
};

#endif

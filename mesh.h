#ifndef _BPJ_REITREI_MESH_H
#define _BPJ_REITREI_MESH_H

#include "scene.h"
#include <string>

ObjectSet readOBJ(const std::string& filename);
int readOBJ(const std::string& filename, Scene* scene);

#endif

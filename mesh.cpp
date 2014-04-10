#include "mesh.h"
#include "triangle.h"
#include <cstdio>
#include <cstring>

void skipLine(FILE* fp) {
  while (fgetc(fp) != '\n');
}

ObjectSet readOBJ(const std::string& filename) {
  FILE* fp = fopen(filename.c_str(), "r");
  //uf throw exception

  std::vector<Vect4> vertices;
  std::vector<Vect4> normals;
  ObjectSet set;

  while (!feof(fp)) {
    char buf[256];
    fscanf(fp, "%s", buf);
    if (!strcmp(buf, "#")) skipLine(fp);
    else if (!strcmp(buf, "g")) {
      //Group ID
      skipLine(fp);
    }
    else if (!strcmp(buf, "v")) {
      Vect4 v;
      fscanf(fp, "%lf%lf%lf", &v[0], &v[1], &v[2]);
      vertices.push_back(v);
    }      
    else if (!strcmp(buf, "vn")) {
      Vect4 vn;
      fscanf(fp, "%lf%lf%lf", &vn[0], &vn[1], &vn[2]);
      normals.push_back(vn.unit());
    }
    else if (!strcmp(buf, "f")) {
      int a, b, c;
      fscanf(fp, "%d%d%d", &a, &b, &c);
      a--; b--; c--;
      InterpTriangle* tri = new InterpTriangle(vertices[a], vertices[b], vertices[c],
					       normals[a], normals[b], normals[c]);
      tri->material.diffuse = Vect4(1.0, 1.0, 1.0);
      tri->material.twosided = 1;
      set.add(tri);
    }
    else printf("Don't know what's happening here: %s\n", buf);//uf exception
  }
  
  fclose(fp);
  return set;
}

int readOBJ(const std::string& filename, Scene* scene) {
  ObjectSet set = readOBJ(filename);
  for (int i = 0; i < set.count(); i++) scene->addObject(set[i]);
  return 0;
}

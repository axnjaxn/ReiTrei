#include "mesh.h"
#include "triangle.h"
#include <cstdio>
#include <cstring>

void skipLine(FILE* fp) {
  while (fgetc(fp) != '\n');
}

int readOBJ(const char* filename, Ray5Scene* scene) {
  FILE* fp = fopen(filename, "r");
  if (!fp) return -1;

  std::vector<Vect4> vertices;
  std::vector<Vect4> normals;

  while (!feof(fp)) {
    char buf[256];
    fscanf(fp, "%s", buf);
    if (!strcmp(buf, "v")) {
      Vect4 v;
      fscanf(fp, "%lf%lf%lf", &v[0], &v[1], &v[2]);
      vertices.push_back(v);
    }      
    else if (!strcmp(buf, "vn")) {
      Vect4 vn;
      fscanf(fp, "%lf%lf%lf", &vn[0], &vn[1], &vn[2]);
      normals.push_back(vn);
    }
    else if (!strcmp(buf, "f")) {
      int a, b, c;
      fscanf(fp, "%d%d%d", &a, &b, &c);
      Triangle* tri = new Triangle(vertices[a], vertices[b], vertices[c]);
      tri->material.diffuse = Vect4(0.0, 0.0, 1.0);
      //tri->setNormal(normals[c]);
      tri->material.twosided = 1;
      scene->addObject(tri);
    }
  }
  
  fclose(fp);
  return 0;
}

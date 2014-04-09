#ifndef _BPJ_REITREI_PARSER_H
#define _BPJ_REITREI_PARSER_H

#include "tokens.h"
#include "ray5scene.h"
#include "ray5shapes.h"
#include "triangle.h"

class Parser {
protected:
  TokenStream ts;

  Real parseNumber();
  Real parseTerm();
  Real parseReal();

  Real parseAngle();

  Vect4 parseTriplet();
  Vect4 parseVectorTerm();
  Vect4 parseVector();

  Ray5Material parseMaterial();
  void parseModifiers(Ray5Object* obj);

  Ray5Box* parseBox();
  Ray5Sphere* parseSphere();
  Ray5Plane* parsePlane();
  Triangle* parseTriangle();
  Ray5Object* parseShape();
  Light* parseLight();

  bool parsedMacro();
  bool parsedShape(Ray5Scene* scene);
  bool parsedMesh(Ray5Scene* scene);
  bool parsedLight(Ray5Scene* scene);
  bool parsedBG(Ray5Scene* scene);
  bool parsedCamera(Ray5Scene* scene);

  bool parsedSceneItem(Ray5Scene* scene);

public:  
  void parseInto(const char* filename, Ray5Scene* scene);
};

#endif

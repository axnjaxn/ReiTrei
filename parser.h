#ifndef _BPJ_REITREI_PARSER_H
#define _BPJ_REITREI_PARSER_H

#include "tokens.h"
#include "scene.h"
#include "shapes.h"
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

  Material parseMaterial();
  void parseModifiers(Object* obj);

  Box* parseBox();
  Sphere* parseSphere();
  Plane* parsePlane();
  Triangle* parseTriangle();
  Object* parseShape();
  Light* parseLight();

  bool parsedMacro();
  bool parsedShape(Scene* scene);
  bool parsedMesh(Scene* scene);
  bool parsedLight(Scene* scene);
  bool parsedBG(Scene* scene);
  bool parsedCamera(Scene* scene);

  bool parsedSceneItem(Scene* scene);

public:  
  void parseInto(const char* filename, Scene* scene);
};

#endif

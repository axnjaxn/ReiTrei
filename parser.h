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

  Vect3 parseTriplet();
  Vect3 parseVectorTerm();
  Vect3 parseVector();

  void parseModifiers(Object* obj);

  Box* parseBox();
  Sphere* parseSphere();
  Cone* parseCone();
  Plane* parsePlane();
  Triangle* parseTriangle();
  Object* parseShape();
  Light* parseLight();

  bool parsedMacro();
  bool parsedMaterialProperty(Material* mat);
  bool parsedMaterial(Material* mat);
  bool parsedModifier(Modifier* mod);
  bool parsedShape(Scene* scene);

  bool parsedSetModifiers(ObjectSet* set);
  bool parsedMesh(Scene* scene);
  bool parsedLight(Scene* scene);
  bool parsedBG(Scene* scene);
  bool parsedCamera(Scene* scene);

  bool parsedSceneItem(Scene* scene);

public:  
  void setConstant(const std::string& name, const std::string& value);
  void parseInto(const char* filename, Scene* scene);
};

#endif

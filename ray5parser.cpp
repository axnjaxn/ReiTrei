#include "ray5parser.h"
#include "tokens.h"
#include "ray5shapes.h"
#include "triangle.h"

/*
 * Parser
 */

Real parseReal(TokenStream*);

Real parseNumber(TokenStream* ts) {
  Token token = ts->getToken();

  if (token == "-") return -parseNumber(ts);
  else if (token == "+") return parseNumber(ts);
  else if (token == "(") {
    Real r = parseReal(ts);
    ts->expectToken(")");
    return r;
  }
  
  //Validate token
  bool haspt = 0;
  for (int i = 0; token[i]; i++)
    if (token[i] == '.') {
      if (!haspt) haspt = 1;
      else throw ParseError("_Real_", token, ts->lineNumber());
    }
    else if (!isdigit(token[i])) throw ParseError("_Real_", token, ts->lineNumber());

  Real r;
  sscanf(token.c_str(), "%lf", &r);
  return r;
}

Real parseTerm(TokenStream* ts) {
  Real r = parseNumber(ts);
  for (;;) {
    Token token = ts->getToken();
    if (token == "*" && ts->peekToken() != "<") r = r * parseNumber(ts);
    else if (token == "/") r = r / parseNumber(ts);
    else {
      ts->ungetToken(token);
      break;
    }
  }
  return r;
}

Real parseReal(TokenStream* ts) {
  Real r = parseTerm(ts);
  for (;;) {
    Token token = ts->getToken();
    if (token == "+") r = r + parseTerm(ts);
    else if (token == "-") r = r - parseTerm(ts);
    else {
      ts->ungetToken(token);
      break;
    }
  }
  return r;
}

Real parseAngle(TokenStream* ts) {
  return PI * parseReal(ts) / 180.0;
}

Vect4 parseTriplet(TokenStream* ts) {
  ts->expectToken("<");
  Vect4 v;
  v[0] = parseReal(ts);
  ts->expectToken(",");
  v[1] = parseReal(ts);
  ts->expectToken(",");
  v[2] = parseReal(ts);
  ts->expectToken(">");
  return v;
}

Vect4 parseVectorTerm(TokenStream* ts) {
  Real coef = 1.0;
  Vect4 v;
  if (ts->peekToken() != "<") {
    coef = parseReal(ts);
    ts->expectToken("*");
  }
  v = parseTriplet(ts);
  for (;;) {
    Token token = ts->peekToken();
    if (token == "*") {
      ts->getToken();
      coef = coef * parseNumber(ts);
    }
    else if (token == "/") {
      ts->getToken();
      coef = coef / parseNumber(ts);
    }
    else break;
  }
  return coef * v;
}

Vect4 parseVector(TokenStream* ts) {
  Vect4 v = parseVectorTerm(ts);
  for (;;) {
    Token token = ts->getToken();
    if (token == "+") v = v + parseVectorTerm(ts);
    else if (token == "-") v = v - parseVectorTerm(ts);
    else {
      ts->ungetToken(token);
      break;
    }
  }
  return v;
}

Ray5Material parseMaterial(TokenStream* ts) {
  Ray5Material material;
  ts->expectToken("{");
  for (;;) {
    Token token = ts->getToken();
    if (token == "shadowless") material.shadowless = 1;
    else if (token == "twosided") material.twosided = 1;
    else if (token == "ambient") material.ambient = parseVector(ts);
    else if (token == "diffuse") material.diffuse = parseVector(ts);
    else if (token == "reflective") material.reflective = parseVector(ts);
    else if (token == "refractive") {
      material.refractive = parseVector(ts);
      material.refractive_index = parseReal(ts);
    }
    else if (token == "specular") material.specular = parseReal(ts);
    else if (token == "shininess") material.shininess = parseReal(ts);
    else {
      ts->ungetToken(token);
      break;
    }
  }
  ts->expectToken("}");
  return material;
}

void parseModifiers(TokenStream* ts, Ray5Object* obj) {
  for (;;) {
    Token token = ts->getToken();
    if (token == "translate") obj->translate(parseVector(ts));
    else if (token == "scale") obj->scale(parseVector(ts));
    else if (token == "rotate") {
      token = ts->getToken();
      if (token == "x") obj->xrotate(-parseAngle(ts));
      else if (token == "y") obj->yrotate(-parseAngle(ts));
      else if (token == "z") obj->zrotate(-parseAngle(ts));
      else throw ParseError("_Axis_", token, ts->lineNumber());
    }
    else if (token == "material") obj->material = parseMaterial(ts);
    else {
      ts->ungetToken(token);
      break;
    }
  }
}

Ray5Box* parseBox(TokenStream* ts) {
  ts->expectToken("{");
  Vect4 upper = parseVector(ts);
  Vect4 lower = parseVector(ts);
  Ray5Box* box = new Ray5Box();
  box->scale((upper - lower) / 2);
  box->translate((upper + lower) / 2);
  parseModifiers(ts, box);
  ts->expectToken("}");
  return box;
}

Ray5Sphere* parseSphere(TokenStream* ts) {
  ts->expectToken("{");
  Vect4 center = parseVector(ts);
  Real radius = parseReal(ts);
  Ray5Sphere* sphere = new Ray5Sphere();
  sphere->scale(Vect4(radius, radius, radius));
  sphere->translate(center);
  parseModifiers(ts, sphere);
  ts->expectToken("}");
  return sphere;
}

Ray5Cone* parseCone(TokenStream* ts) {
  //uf
  return NULL;
}

Ray5Plane* parsePlane(TokenStream* ts) {
  ts->expectToken("{");
  Ray5Plane* plane = new Ray5Plane();
  plane->A = parseVector(ts);
  plane->N = parseVector(ts);
  parseModifiers(ts, plane);
  ts->expectToken("}");
  return plane;
}

Triangle* parseTriangle(TokenStream* ts) {
  ts->expectToken("{");
  Vect4 a = parseVector(ts);
  Vect4 b = parseVector(ts);
  Vect4 c = parseVector(ts);
  Triangle* tri = new Triangle(a, b, c);
  parseModifiers(ts, tri);
  ts->expectToken("}");
  return tri;
}

Light* parseLight(TokenStream* ts) {
  ts->expectToken("{");
  Light* light = new Light();
  light->position = parseVector(ts);
  light->color = parseVector(ts);
  while (ts->peekToken() != "}") {
    Token token = ts->getToken();
    if (token == "intensity") light->intensity = parseReal(ts);
    else if (token == "radius") light->radius = parseReal(ts);    
    else if (token == "falloff") light->falloff = 1;
    else throw ParseError("_LightProperty_", token, ts->lineNumber());
  }
  ts->expectToken("}");
  return light;
}

Ray5Camera parseCamera(TokenStream* ts) {
  ts->expectToken("{");
  Ray5Camera camera;
  parseModifiers(ts, &camera); 
  ts->expectToken("}");
  return camera;
}

void parseDefine(TokenStream* ts) {
  Macro macro(ts->getToken()); 
  Token value = ts->getToken();
  if (value == "{") {
    int braces_count = 0;
    for (;;) {
      value = ts->getToken();
      if (value == "{") braces_count++;
      if (value == "}") {
	if (braces_count == 0) break;
	else braces_count--;
      }
      else if (ts->eof()) throw ParseError("}", "_EOF_", ts->lineNumber());
      macro.addToken(value);
    }
  }
  ts->addMacro(macro);
}

#include "mesh.h"
#include <string>
void parseSceneItem(TokenStream* ts, Ray5Scene* scene) {
  Token token = ts->getToken();

  if (ts->eof()) return;

  //Defines
  if (token == "Define") parseDefine(ts);

  //Cameras
  else if (token == "Camera") scene->camera = parseCamera(ts);
  
  //Lights
  else if (token == "Light") scene->addLight(parseLight(ts));

  //Shapes
  else if (token == "Box") scene->addObject(parseBox(ts));
  else if (token == "Sphere") scene->addObject(parseSphere(ts));
  //uf cone?
  else if (token == "Plane") scene->addObject(parsePlane(ts));

  else if (token == "Triangle") scene->addObject(parseTriangle(ts));

  //Miscellaneous
  else if (token == "BGColor") scene->bgcolor = parseVector(ts);

  else if (token == "Obj") {
    std::string str = ts->getToken();
    printf("Reading OBJ: %s\n", str.c_str());
    readOBJ(str.c_str(), scene);
  }

  else throw ParseError("_SceneItem_", token, ts->lineNumber());
}

void parseScene(TokenStream* ts, Ray5Scene* scene) {
  while (!ts->eof() && !(ts->peekToken() == "EOF")) parseSceneItem(ts, scene);
}

void parseScene(const char* fn, Ray5Scene* scene) {
  TokenStream ts;
  if (ts.open(fn)) {
    throw ParseError("Couldn't open scene file.");
  }
  else {
    parseScene(&ts, scene);
    ts.close();
  }
}

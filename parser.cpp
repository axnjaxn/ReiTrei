#include "parser.h"
#include "ray5shapes.h"
#include "triangle.h"
#include "mesh.h"

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

Ray5Box* Parser::parseBox() {
  if (ts.peekToken() != "Box") return NULL;
  else ts.getToken();

  ts.expectToken("{");
  Vect4 upper = parseVector(&ts);
  Vect4 lower = parseVector(&ts);
  Ray5Box* box = new Ray5Box();
  box->scale((upper - lower) / 2);
  box->translate((upper + lower) / 2);
  parseModifiers(&ts, box);
  ts.expectToken("}");
  return box;
}

Ray5Sphere* Parser::parseSphere() {
  if (ts.peekToken() != "Sphere") return NULL;
  else ts.getToken();

  ts.expectToken("{");
  Vect4 center = parseVector(&ts);
  Real radius = parseReal(&ts);
  Ray5Sphere* sphere = new Ray5Sphere();
  sphere->scale(Vect4(radius, radius, radius));
  sphere->translate(center);
  parseModifiers(&ts, sphere);
  ts.expectToken("}");
  return sphere;
}

Ray5Plane* Parser::parsePlane() {
  if (ts.peekToken() != "Plane") return NULL;
  else ts.getToken();

  ts.expectToken("{");
  Ray5Plane* plane = new Ray5Plane();
  plane->A = parseVector(&ts);
  plane->N = parseVector(&ts);
  parseModifiers(&ts, plane);
  ts.expectToken("}");
  return plane;
}

Triangle* Parser::parseTriangle() {
  if (ts.peekToken() != "Triangle") return NULL;
  else ts.getToken();

  ts.expectToken("{");
  Vect4 a = parseVector(&ts);
  Vect4 b = parseVector(&ts);
  Vect4 c = parseVector(&ts);
  Triangle* tri = new Triangle(a, b, c);
  parseModifiers(&ts, tri);
  ts.expectToken("}");
  return tri;
}

Ray5Object* Parser::parseShape() {
  Ray5Object* obj;
  if (obj = parseBox()) return obj;
  else if (obj = parseSphere()) return obj;
  else if (obj = parsePlane()) return obj;
  else if (obj = parseTriangle()) return obj;
  else return NULL;
}

Light* Parser::parseLight() {
  if (ts.peekToken() != "Light") return NULL;
  else ts.getToken(); 

  ts.expectToken("{");
  Light* light = new Light();
  light->position = parseVector(&ts);
  light->color = parseVector(&ts);
  while (ts.peekToken() != "}") {
    Token token = ts.getToken();
    if (token == "intensity") light->intensity = parseReal(&ts);
    else if (token == "radius") light->radius = parseReal(&ts);    
    else if (token == "falloff") light->falloff = 1;
    else throw ParseError("_LightProperty_", token, ts.lineNumber());
  }
  ts.expectToken("}");
  return light;
}

Ray5Camera parseCamera(TokenStream* ts) {
  ts->expectToken("{");
  Ray5Camera camera;
  parseModifiers(ts, &camera); 
  ts->expectToken("}");
  return camera;
}

bool Parser::parsedMacro() {
  if (ts.peekToken() != "Define") return 0;
  else ts.getToken();

  Macro macro(ts.getToken()); 
  ts.expectToken("{");
  
  int braces_count = 1;
  for (Token value = ts.getToken(); value != "}"; value = ts.getToken()) {
    if (value == "{") braces_count++;
    else if (value == "}") {if (--braces_count == 0) break;}
    else if (ts.eof()) throw ParseError("}", "_EOF_", ts.lineNumber());

    macro.addToken(value);
  }

  ts.addMacro(macro);
  return 1;
}

bool Parser::parsedShape(Ray5Scene* scene) {
  Ray5Object* obj = parseShape();
  if (obj) scene->addObject(obj);
  return obj;
}

bool Parser::parsedMesh(Ray5Scene* scene) {
  if (ts.peekToken() == "Obj") {
    ts.getToken();
    ts.ungetToken(Token("teapot.obj"));//uf we have issues loading this
    printf("Attempting to load %s\n", ts.peekToken().c_str());
    readOBJ(ts.getToken().c_str(), scene);
    ts.getToken();
    return 1;
  }
  else return 0;
}

bool Parser::parsedLight(Ray5Scene* scene) {
  Light* light = parseLight();
  if (light) scene->addLight(light);
  return light;
}

bool Parser::parsedBG(Ray5Scene* scene) {
  if (ts.peekToken() != "BGColor") return 0;
  
  ts.getToken();
  scene->bgcolor = parseVector(&ts);
  return 1;
}

bool Parser::parsedCamera(Ray5Scene* scene) {
  if (ts.peekToken() != "Camera") return 0;

  ts.getToken();
  scene->camera = parseCamera(&ts);
  return 1;
}

bool Parser::parsedSceneItem(Ray5Scene* scene) {
  return (parsedMacro()
	  || parsedShape(scene) 
	  || parsedMesh(scene)
	  || parsedLight(scene)
	  || parsedBG(scene)
	  || parsedCamera(scene));
}

void Parser::parseInto(const char* filename, Ray5Scene* scene) {
  if (ts.open(filename)) {
    throw ParseError("Couldn't open scene file.");
  }
  else {
    while (!ts.eof() && (ts.peekToken() != "EOF")) 
      if (!parsedSceneItem(scene)) {
	printf("%d\n", ts.lineNumber());
	throw ParseError("_SceneItem_", ts.getToken(), ts.lineNumber());
      }
    ts.close();
  }
}

#include "parser.h"
#include "ray5shapes.h"
#include "triangle.h"
#include "mesh.h"

/*
 * Parser
 */

Real Parser::parseNumber() {
  Token token = ts.getToken();

  if (token == "-") return -parseNumber();
  else if (token == "+") return parseNumber();
  else if (token == "(") {
    Real r = parseReal();
    ts.expectToken(")");
    return r;
  }
  
  //Validate token
  bool haspt = 0;
  for (int i = 0; token[i]; i++)
    if (token[i] == '.') {
      if (!haspt) haspt = 1;
      else throw ParseError("_Real_", token, ts.lineNumber());
    }
    else if (!isdigit(token[i])) throw ParseError("_Real_", token, ts.lineNumber());

  Real r;
  sscanf(token.c_str(), "%lf", &r);
  return r;
}

Real Parser::parseTerm() {
  Real r = parseNumber();
  for (;;) {
    Token token = ts.getToken();
    if (token == "*" && ts.peekToken() != "<") r = r * parseNumber();
    else if (token == "/") r = r / parseNumber();
    else {
      ts.ungetToken(token);
      break;
    }
  }
  return r;
}

Real Parser::parseReal() {
  Real r = parseTerm();
  for (;;) {
    Token token = ts.getToken();
    if (token == "+") r = r + parseTerm();
    else if (token == "-") r = r - parseTerm();
    else {
      ts.ungetToken(token);
      break;
    }
  }
  return r;
}

Real Parser::parseAngle() {
  return PI * parseReal() / 180.0;
}

Vect4 Parser::parseTriplet() {
  ts.expectToken("<");
  Vect4 v;
  v[0] = parseReal();
  ts.expectToken(",");
  v[1] = parseReal();
  ts.expectToken(",");
  v[2] = parseReal();
  ts.expectToken(">");
  return v;
}

Vect4 Parser::parseVectorTerm() {
  Real coef = 1.0;
  Vect4 v;
  if (ts.peekToken() != "<") {
    coef = parseReal();
    ts.expectToken("*");
  }
  v = parseTriplet();
  for (;;) {
    Token token = ts.peekToken();
    if (token == "*") {
      ts.getToken();
      coef = coef * parseNumber();
    }
    else if (token == "/") {
      ts.getToken();
      coef = coef / parseNumber();
    }
    else break;
  }
  return coef * v;
}

Vect4 Parser::parseVector() {
  Vect4 v = parseVectorTerm();
  for (;;) {
    Token token = ts.getToken();
    if (token == "+") v = v + parseVectorTerm();
    else if (token == "-") v = v - parseVectorTerm();
    else {
      ts.ungetToken(token);
      break;
    }
  }
  return v;
}

Material Parser::parseMaterial() {
  Material material;
  ts.expectToken("{");
  for (;;) {
    Token token = ts.getToken();
    if (token == "shadowless") material.shadowless = 1;
    else if (token == "twosided") material.twosided = 1;
    else if (token == "ambient") material.ambient = parseVector();
    else if (token == "diffuse") material.diffuse = parseVector();
    else if (token == "reflective") material.reflective = parseVector();
    else if (token == "refractive") {
      material.refractive = parseVector();
      material.refractive_index = parseReal();
    }
    else if (token == "specular") material.specular = parseReal();
    else if (token == "shininess") material.shininess = parseReal();
    else {
      ts.ungetToken(token);
      break;
    }
  }
  ts.expectToken("}");
  return material;
}

void Parser::parseModifiers(Object* obj) {
  for (;;) {
    Token token = ts.getToken();
    if (token == "translate") obj->translate(parseVector());
    else if (token == "scale") obj->scale(parseVector());
    else if (token == "rotate") {
      token = ts.getToken();
      if (token == "x") obj->xrotate(-parseAngle());
      else if (token == "y") obj->yrotate(-parseAngle());
      else if (token == "z") obj->zrotate(-parseAngle());
      else throw ParseError("_Axis_", token, ts.lineNumber());
    }
    else if (token == "material") obj->material = parseMaterial();
    else {
      ts.ungetToken(token);
      break;
    }
  }
}

Ray5Box* Parser::parseBox() {
  if (ts.peekToken() != "Box") return NULL;
  else ts.getToken();

  ts.expectToken("{");
  Vect4 upper = parseVector();
  Vect4 lower = parseVector();
  Ray5Box* box = new Ray5Box();
  box->scale((upper - lower) / 2);
  box->translate((upper + lower) / 2);
  parseModifiers(box);
  ts.expectToken("}");
  return box;
}

Ray5Sphere* Parser::parseSphere() {
  if (ts.peekToken() != "Sphere") return NULL;
  else ts.getToken();

  ts.expectToken("{");
  Vect4 center = parseVector();
  Real radius = parseReal();
  Ray5Sphere* sphere = new Ray5Sphere();
  sphere->scale(Vect4(radius, radius, radius));
  sphere->translate(center);
  parseModifiers(sphere);
  ts.expectToken("}");
  return sphere;
}

Ray5Plane* Parser::parsePlane() {
  if (ts.peekToken() != "Plane") return NULL;
  else ts.getToken();

  ts.expectToken("{");
  Ray5Plane* plane = new Ray5Plane();
  plane->A = parseVector();
  plane->N = parseVector();
  parseModifiers(plane);
  ts.expectToken("}");
  return plane;
}

Triangle* Parser::parseTriangle() {
  if (ts.peekToken() != "Triangle") return NULL;
  else ts.getToken();

  ts.expectToken("{");
  Vect4 a = parseVector();
  Vect4 b = parseVector();
  Vect4 c = parseVector();

  Triangle* tri;
  if (ts.peekToken() == "<") {
    //This triangle has normal vectors too!
    Vect4 n0 = parseVector();
    Vect4 n1 = parseVector();
    Vect4 n2 = parseVector();    
    tri = new InterpTriangle(a, b, c, n0, n1, n2);//uf test this
  }
  else tri = new Triangle(a, b, c);

  parseModifiers(tri);
  ts.expectToken("}");
  return tri;
}

Object* Parser::parseShape() {
  Object* obj;
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
  light->position = parseVector();
  light->color = parseVector();
  while (ts.peekToken() != "}") {
    Token token = ts.getToken();
    if (token == "intensity") light->intensity = parseReal();
    else if (token == "radius") light->radius = parseReal();    
    else if (token == "falloff") light->falloff = 1;
    else throw ParseError("_LightProperty_", token, ts.lineNumber());
  }
  ts.expectToken("}");
  return light;
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
  Object* obj = parseShape();
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
  scene->bgcolor = parseVector();
  return 1;
}

bool Parser::parsedCamera(Ray5Scene* scene) {
  if (ts.peekToken() != "Camera") return 0;
  else ts.getToken();

  ts.expectToken("{");
  parseModifiers(&scene->camera); 
  ts.expectToken("}");
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

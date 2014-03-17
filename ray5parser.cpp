#include "ray5parser.h"
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cctype>
#include "ray5shapes.h"

/*
 * Token
 */

class Token {
public:
  char* str;

  Token() {str = NULL;}
  Token(const char* str) {
    this->str = new char [strlen(str) + 1];
    sprintf(this->str, "%s", str);
  }
  Token(const Token& t) {
    str = NULL;
    *this = t;
  }
  ~Token() {
    delete [] str;
  }
  
  Token& operator=(const Token& t) {
    if (str) delete [] str;
    str = new char [strlen(t.str) + 1];
    sprintf(str, "%s", t.str);
    return *this;
  }
  inline char& operator[](int i) {return str[i];}
  char operator[](int i) const {return str[i];}
};
inline bool operator==(const Token& t, const char* str) {return !strcmp(t.str, str);}
inline bool operator==(const char* str, const Token& t) {return (t == str);}
inline bool operator!=(const Token& t, const char* str) {return !(t == str);}
inline bool operator!=(const char* str, const Token& t) {return !(t == str);}
inline bool operator==(const Token& t1, const Token& t2) {return (t1 == t2.str);}
inline bool operator!=(const Token& t1, const Token& t2) {return !(t1 == t2);}

/*
 * DefinedToken
 */

class Define {
public:
  Token name;
  std::vector<Token> values;

  Define(Token name) {this->name = name;}
  void addToken(Token token) {values.push_back(token);}
};

/*
 * TokenStream
 */

char fpeek(FILE* fp) {
  char c = fgetc(fp);
  ungetc(c, fp);
  return c;
}

bool isenclosure(char c) {return (c == '{' || c == '}' || c == '<' || c == '>' || c == '(' || c == ')');}
bool isoperator(char c) {return (c == '+' || c == '-' || c == '*' || c == '/');}
bool isseparator(char c) {return (c == ',');}

class TokenStream {
protected:
  FILE* fp;
  std::vector<Token> tokens;
  std::vector<Define> defines;

  void readToken() {
    char tokenbuf[1024];
    memset(tokenbuf, 0, 1024);

    char next;
    
    //Remove whitespace and peek at leading character
    do {
      next = fgetc(fp);
      if (next == '%') {
	while (fgetc(fp) != '\n');
	next = ' ';
      }
    } while (isspace(next));
    ungetc(next, fp);
    
    //Enclosure tokens
    if (isenclosure(next) || isoperator(next) || isseparator(next)) {
      fgetc(fp);
      sprintf(tokenbuf, "%c", next);
      tokens.push_back(Token(tokenbuf));
      return;
    } 

    //General tokens
    for (int i = 0; i < 1024; i++) {
      next = fgetc(fp);
      if (isspace(next) || isenclosure(next) || isseparator(next)) {
	ungetc(next, fp);
	break;
      }
      tokenbuf[i] = next;
    }
    tokens.push_back(Token(tokenbuf));
  }
public:
  int open(const char* fn) {
    fp = fopen(fn, "r");
    return !fp;
  }
  void close() {
    fclose(fp);
  }

  Token getToken() {
    Token token = peekToken();
    tokens.pop_back();
    return token;
  }
  void ungetToken(Token token) {
    tokens.push_back(token);
  }
  Token peekToken() {
    if (tokens.empty()) readToken();

    //Load a defined symbol
    for (int i = 0; i < defines.size(); i++)
      if (tokens[tokens.size() - 1] == defines[i].name) {
	tokens.pop_back();
	for (int j = defines[i].values.size() - 1; j >= 0; j--)
	  tokens.push_back(defines[i].values[j]);
	return peekToken();
      }

    return tokens[tokens.size() - 1];
  }
  void expectToken(const char* expect) {
    Token token = getToken();
    if (token != expect) parseError(expect, token);
  }
  
  int lineNumber() {
    int L = 1;
    int pos = ftell(fp);
    rewind(fp);
    for (int i = 0; i < pos; i++)
      if (fgetc(fp) == '\n') L++;
    return L;
  }
  bool eof() {
    return feof(fp);
  }
  void parseError(const char* expected, const char* got) {
    printf("Syntax error on line %d: expected %s, got \"%s\" instead.\n", lineNumber(), expected, got);
    exit(0);
  }
  void parseError(const char* expected, const Token& got) {
    parseError(expected, got.str);
  }
  void addDefine(Define define) {
    defines.push_back(define);
  }
};

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
      else ts->parseError("_Real_", token);
    }
    else if (!isdigit(token[i])) ts->parseError("_Real_", token);

  Real r;
  sscanf(token.str, "%lf", &r);
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
      if (token == "x") obj->xrotate(parseReal(ts) * -2 * PI);
      else if (token == "y") obj->yrotate(parseReal(ts) * -2 * PI);
      else if (token == "z") obj->zrotate(parseReal(ts) * -2 * PI);
      else ts->parseError("_Axis_", token);
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

Ray5Light* parseLight(TokenStream* ts) {
  ts->expectToken("{");
  Ray5Light* light = new Ray5Light();
  light->position = parseVector(ts);
  light->color = parseVector(ts);
  while (ts->peekToken() != "}") {
    Token token = ts->getToken();
    if (token == "radius") light->radius = parseReal(ts);
    else if (token == "intensity") light->intensity = parseReal(ts);
    else ts->parseError("_LightProperty_", token);
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
  Define define(ts->getToken()); 
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
      else if (ts->eof()) ts->parseError("}", "_EOF_");
      define.addToken(value);
    }
  }
  ts->addDefine(define);
}

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

  //Miscellaneous
  else if (token == "BGColor") scene->bgcolor = parseVector(ts);

  else ts->parseError("_SceneItem_", token);
}

void parseScene(TokenStream* ts, Ray5Scene* scene) {
  while (!ts->eof() && !(ts->peekToken() == "EOF")) parseSceneItem(ts, scene);
}

void parseScene(const char* fn, Ray5Scene* scene) {
  TokenStream ts;
  if (ts.open(fn))
    printf("Couldn't open scene file.\n");
  else {
    parseScene(&ts, scene);
    ts.close();
  }
}

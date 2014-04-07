#include "tokens.h"
#include <cstdio>
#include <cstring>
#include <cstdlib>

char fpeek(FILE* fp) {
  char c = fgetc(fp);
  ungetc(c, fp);
  return c;
}

bool isenclosure(char c) {return (c == '{' || c == '}' || c == '<' || c == '>' || c == '(' || c == ')');}
bool isoperator(char c) {return (c == '+' || c == '-' || c == '*' || c == '/');}
bool isseparator(char c) {return (c == ',');}

void TokenStream::readToken() {
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

int TokenStream::open(const char* fn) {
  fp = fopen(fn, "r");
  return !fp;
}

void TokenStream::close() {
  fclose(fp);
}

Token TokenStream::getToken() {
  Token token = peekToken();
  tokens.pop_back();
  return token;
}

void TokenStream::ungetToken(Token token) {
  tokens.push_back(token);
}

Token TokenStream::peekToken() {
  if (tokens.empty()) readToken();

  //Load a defined symbol
  for (int i = 0; i < macros.size(); i++)
    if (tokens[tokens.size() - 1] == macros[i].name) {
      tokens.pop_back();
      for (int j = macros[i].values.size() - 1; j >= 0; j--)
	tokens.push_back(macros[i].values[j]);
      return peekToken();
    }

  return tokens[tokens.size() - 1];
}

void TokenStream::expectToken(const char* expect) {
  Token token = getToken();
  if (token != expect) parseError(expect, token);
}
  
int TokenStream::lineNumber() {
  int L = 1;
  int pos = ftell(fp);
  rewind(fp);
  for (int i = 0; i < pos; i++)
    if (fgetc(fp) == '\n') L++;
  return L;
}

bool TokenStream::eof() {return feof(fp);}

void TokenStream::parseError(const char* expected, const char* got) {
  printf("Syntax error on line %d: expected %s, got \"%s\" instead.\n", lineNumber(), expected, got);
  exit(0);
}

void TokenStream::parseError(const char* expected, const Token& got) {
  parseError(expected, got.c_str());
}

void TokenStream::addMacro(Macro macro) {
  macros.push_back(macro);
}

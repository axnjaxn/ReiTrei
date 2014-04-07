#ifndef _BPJ_REITREI_TOKENS_H
#define _BPJ_REITREI_TOKENS_H

#include <string>
#include <vector>

typedef std::string Token;

class Macro {
public:
  Token name;
  std::vector<Token> values;
  
  Macro(const Token& name) {this->name = name;}
  inline void addToken(const Token& token) {values.push_back(token);}
};

class TokenStream {
protected:
  FILE* fp;
  std::vector<Token> tokens;
  std::vector<Macro> macros;

  void readToken();

public:
  int open(const char* fn);
  void close();

  Token getToken();
  void ungetToken(Token token);
  Token peekToken();
  void expectToken(const char* expect);
  int lineNumber();
  bool eof();

  void parseError(const char* expected, const char* got);
  void parseError(const char* expected, const Token& got);
  void addMacro(Macro macro);
};

#endif

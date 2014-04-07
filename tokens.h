#ifndef _BPJ_REITREI_TOKENS_H
#define _BPJ_REITREI_TOKENS_H

#include <string>
#include <vector>
#include <exception>

typedef std::string Token;

class Macro {
public:
  Token name;
  std::vector<Token> values;
  
  Macro(const Token& name) {this->name = name;}
  inline void addToken(const Token& token) {values.push_back(token);}
};

class ParseError : public std::exception {
protected:
  std::string message;
  int lineno;

public:
  ParseError(const std::string& message, int lineno = 0);
  ParseError(const std::string& expected, const std::string& got, int lineno);
  virtual ~ParseError() throw() { }
  virtual const char* what() const throw();
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

  void addMacro(Macro macro);
};

#endif

#ifndef PARSING_TOKEN_H
#define PARSING_TOKEN_H

#include <string>
#include <unordered_map>

namespace pl0 {

#define IGNORE_TOKEN(name, string)

// This is for better presentation
#define TOKEN_LIST(T, K)     \
  T(UNUSED, "unused")           \
  /* End of source indicator */ \
  T(EOS, "end of source")       \
  T(NUMBER, "number")           \
  T(IDENTIFIER, "identifier")   \
  /* Unary operator */          \
  T(ODD, "odd")                 \
  /* Binary operators */        \
  T(ADD, "+")                   \
  T(SUB, "-")                   \
  T(MUL, "*")                   \
  T(DIV, "/")                   \
  T(ASSIGN, ":=")               \
  /* Compare operators */       \
  T(EQ, "=")                    \
  T(NEQ, "#")                   \
  T(LE, "<")                    \
  T(LEQ, "<=")                  \
  T(GE, ">")                    \
  T(GEQ, ">=")                  \
  /* Punctuators */             \
  T(LPAREN, "(")                \
  T(RPAREN, ")")                \
  T(SEMICOLON, ";")             \
  T(PERIOD, ".")                \
  T(COMMA, ",")                 \
  /* Keywords */                \
  K(BEGIN, "begin")             \
  K(CALL, "call")               \
  K(CONST, "const")             \
  K(DO, "do")                   \
  K(ELSE, "else")               \
  K(END, "end")                 \
  K(IF, "if")                   \
  K(PROCEDURE, "procedure")     \
  K(READ, "read")               \
  K(RETURN, "return")           \
  K(THEN, "then")               \
  K(VAR, "var")                 \
  K(WHILE, "while")             \
  K(WRITE, "write")             \
  /* Error */                   \
  T(ILLEGAL, "illegal token")

// Gets the left side of the above macro as the enum name
#define T(name, string) name,
enum class Token : int { TOKEN_LIST(T, T) };
#undef T

// Gets the right-hand build string array of the macros mentioned above
#define T(name, string) string,
const char* const token_string[] = {TOKEN_LIST(T, T)};
#undef T

#define K(name, string) {string, Token::name},
const std::unordered_map<std::string, Token> keyword_map{
    TOKEN_LIST(IGNORE_TOKEN, K)};
#undef K

inline const char* operator*(Token tkty) {
  return token_string[static_cast<int>(tkty)];
}

inline bool is_compare_operator(Token tk) {
  return static_cast<int>(Token::EQ) <= static_cast<int>(tk)
         && static_cast<int>(tk) <= static_cast<int>(Token::GEQ);
}

} // namespace pl0

#endif // PARSING_TOKEN_H
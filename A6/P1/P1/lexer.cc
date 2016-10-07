#include "lexer.h"
#include "kind.h"
using std::string;
using std::vector;

// Use the annonymous namespace to prevent external linking
namespace {
  // An array represents the Token kind that each state represents
  // The ERR Kind represents an invalid token
  ASM::Kind stateKinds[] = {
    ASM::ERR,            // ST_ERR
    ASM::ERR,            // ST_START
    ASM::ID ,            // ST_ID
    ASM::NUM,            // ST_NUM
    ASM::LPAREN,       // ST_LPAREN
    ASM::RPAREN,            // ST_RPAREN
    ASM::LBRACE,             // ST_LBRACE
    ASM::RBRACE,          // ST_RBRACE
    ASM::ID,          // ST_xR
    ASM::ID,         // ST_xRE
    ASM::ID,         // ST_xRET
    ASM::ID,            // ST_xRETU
    ASM::ID,            // ST_xRETUR
    ASM::RETURN,         // ST_xRETURN
    ASM::ID,     // ST_xI
    ASM::IF,            // ST_xIF
    ASM::ID,            // ST_xE
    ASM::ID,            // ST_xEL
    ASM::ID,            // ST_xELS
    ASM::ELSE,            // ST_xELSE
    ASM::ID,            // ST_xW
    ASM::ID,            // ST_xWH
    ASM::ID,            // ST_xWHI
    ASM::ID,            // ST_xWHIL
    ASM::WHILE,            // ST_xWHILE
    ASM::ID,            // ST_xP
    ASM::ID,            // ST_xPR
    ASM::ID,            // ST_xPRI
    ASM::ID,            // ST_xPRIN
    ASM::ID,            // ST_xPRINT
    ASM::ID,            // ST_xPRINTL
    ASM::PRINTLN,            // ST_xPRTINLN
    ASM::ID,            // ST_xWA
    ASM::ID,            // ST_xWAI
    ASM::WAIN,            // ST_xWAIN
    ASM::BECOMES,            // ST_BECOMES
    ASM::ID,            // ST_xIN
    ASM::INT,            // ST_xINT
    ASM::EQ,            // ST_xEQ
    ASM::ERR,            // ST_nE
    ASM::NE,            // ST_NE
    ASM::LT,            // ST_LT
    ASM::GT,            // ST_GT
    ASM::LE,            // ST_LE
    ASM::GE,            // ST_GE
    ASM::PLUS,            // ST_PLUS
    ASM::MINUS,            // ST_MINUS
    ASM::STAR,            // ST_STAR
    ASM::SLASH,            // ST_SLASH
    ASM::PCT,            // ST_PCT
    ASM::COMMA,            // ST_COMMA
    ASM::SEMI,            // ST_SEMI
    ASM::ID,            // ST_xN
    ASM::ID,            // ST_xNE
    ASM::NEW,            // ST_xNEW
    ASM::ID,            // ST_xD
    ASM::ID,            // ST_xDE
    ASM::ID,            // ST_xDEL
    ASM::ID,            // ST_xDELE
    ASM::ID,            // ST_xDELET
    ASM::DELETE,            // ST_xDELETE
    ASM::LBRACK,            // ST_LBRACK
    ASM::RBRACK,            // ST_RBRACK
    ASM::AMP,            // ST_AMP
    ASM::ID,            // ST_xNU
    ASM::ID,            // ST_xNUL
    ASM::NUL,            // ST_xNULL
    ASM::WHITESPACE,     // ST_COMMENT
    ASM::WHITESPACE      // ST_WHITESPACE
  };
  const string whitespace = "\t\n\r ";
  const string letters = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
  const string digits = "0123456789";
  const string hexDigits = "0123456789ABCDEFabcdef";
  const string oneToNine =  "123456789";

}

ASM::Lexer::Lexer(){
  // Set default transitions to the Error state
  for(int i=0; i < maxStates; ++i){
    for(int j=0; j < maxTrans; ++j){
      delta[i][j] = ST_ERR;
    }
  }
  // Change transitions as appropriate for the MIPS recognizer
  setTrans( ST_START,      whitespace,     ST_WHITESPACE );
  setTrans( ST_WHITESPACE, whitespace,     ST_WHITESPACE );
  setTrans( ST_START,      letters,        ST_ID         );
  setTrans( ST_ID,         letters+digits, ST_ID         );
  setTrans( ST_START,      digits,         ST_NUM        );
  setTrans( ST_NUM,        digits,         ST_NUM        );
  setTrans( ST_START,      "(",            ST_LPAREN     );
  setTrans( ST_START,      ")",            ST_RPAREN     );
  setTrans( ST_START,      "{",            ST_LBRACE     );
  setTrans( ST_START,      "}",            ST_RBRACE     );
  setTrans( ST_WHITESPACE,      "r",            ST_xR         );
  setTrans( ST_xR,         "e",            ST_xRE        );
  setTrans( ST_xRE,        "t",            ST_xRET       );
  setTrans( ST_xRET,       "u",            ST_xRETU      );
  setTrans( ST_xRETU,      "r",            ST_xRETUR     );
  setTrans( ST_xRETUR,     "n",            ST_xRETURN    );
  setTrans( ST_WHITESPACE,      "i",            ST_xI         );
  setTrans( ST_xI,         "f",            ST_xIF        );
  setTrans( ST_WHITESPACE,      "e",            ST_xE         );
  setTrans( ST_xE,         "l",            ST_xEL        );
  setTrans( ST_xEL,        "s",            ST_xELS       );
  setTrans( ST_xELS,       "e",            ST_xELSE      );
  setTrans( ST_WHITESPACE,      "w",            ST_xW         );
  setTrans( ST_xW,         "h",            ST_xWH        );
  setTrans( ST_xWH,        "i",            ST_xWHI       );
  setTrans( ST_xWHI,       "l",            ST_xWHIL      );
  setTrans( ST_xWHIL,      "e",            ST_xWHILE     );
  setTrans( ST_WHITESPACE,      "p",            ST_xP         );
  setTrans( ST_xP,         "r",            ST_xPR        );
  setTrans( ST_xPR,        "i",            ST_xPRI       );
  setTrans( ST_xPRI,       "n",            ST_xPRIN      );
  setTrans( ST_xPRIN,      "t",            ST_xPRINT     );
  setTrans( ST_xPRINT,     "l",            ST_xPRINTL    );
  setTrans( ST_xPRINTL,    "n",            ST_xPRINTLN   );
  setTrans( ST_xW,         "a",            ST_xWA        );
  setTrans( ST_xWA,        "i",            ST_xWAI       );
  setTrans( ST_xWAI,       "n",            ST_xWAIN      );
  setTrans( ST_START,      "=",            ST_BECOMES    );
  setTrans( ST_xI,         "n",            ST_xIN        );
  setTrans( ST_xIN,        "t",            ST_xINT       );
  setTrans( ST_BECOMES,    "=",            ST_EQ         );
  setTrans( ST_START,      "!",            ST_nE         );
  setTrans( ST_nE,         "=",            ST_NE         );
  setTrans( ST_START,      "<",            ST_LT         );
  setTrans( ST_START,      ">",            ST_GT         );
  setTrans( ST_LT,         "=",            ST_LE         );
  setTrans( ST_GT,         "=",            ST_GE         );
  setTrans( ST_START,      "+",            ST_PLUS       );
  setTrans( ST_START,      "-",            ST_MINUS      );
  setTrans( ST_START,      "*",            ST_STAR       );
  setTrans( ST_START,      "/",            ST_SLASH      );
  setTrans( ST_START,      "%",            ST_PCT        );
  setTrans( ST_START,      ",",            ST_COMMA      );
  setTrans( ST_START,      ";",            ST_SEMI       );
  setTrans( ST_WHITESPACE,      "n",            ST_xN         );
  setTrans( ST_xN,         "e",            ST_xNE        );
  setTrans( ST_xNE,        "w",            ST_xNEW       );
  setTrans( ST_WHITESPACE,      "d",            ST_xD         );
  setTrans( ST_xD,         "e",            ST_xDE        );
  setTrans( ST_xDE,        "l",            ST_xDEL       );
  setTrans( ST_xDEL,       "e",            ST_xDELE      );
  setTrans( ST_xDELE,      "t",            ST_xDELET     );
  setTrans( ST_xDELET,     "e",            ST_xDELETE    );
  setTrans( ST_START,      "[",            ST_LBRACK     );
  setTrans( ST_START,      "]",            ST_RBRACK     );
  setTrans( ST_START,      "&",            ST_AMP        );
  setTrans( ST_WHITESPACE,      "N",            ST_xN         );
  setTrans( ST_xN,         "U",            ST_xNU        );
  setTrans( ST_xNU,        "L",            ST_xNUL       );
  setTrans( ST_xNUL,       "L",            ST_xNULL      );
  setTrans( ST_SLASH,     "/",            ST_COMMENT    );
  
  // A comment can only ever lead to the comment state
  for(int j=0; j < maxTrans; ++j) delta[ST_COMMENT][j] = ST_COMMENT;
}

// Set the transitions from one state to another state based upon characters in the
// given string
void ASM::Lexer::setTrans(ASM::State from, const string& chars, ASM::State to){
  for(string::const_iterator it = chars.begin(); it != chars.end(); ++it)
    delta[from][static_cast<unsigned int>(*it)] = to;
}

// Scan a line of input (as a string) and return a vector
// of Tokens representing the MIPS instruction in that line
vector<ASM::Token*> ASM::Lexer::scan(const string& line){
  // Return vector
  vector<Token*> ret;
  if(line.size() == 0) return ret;
  // Always begin at the start state
  State currState = ST_START;
  // startIter represents the beginning of the next Token
  // that is to be recognized. Initially, this is the beginning
  // of the line.
  // Use a const_iterator since we cannot change the input line
  string::const_iterator startIter = line.begin();
  // Loop over the the line
  for(string::const_iterator it = line.begin();;){
    // Assume the next state is the error state
    State nextState = ST_ERR;
    // If we aren't done then get the transition from the current
    // state to the next state based upon the current character of
    //input
    if(it != line.end())
      nextState = delta[currState][static_cast<unsigned int>(*it)];
    // If there is no valid transition then we have reach then end of a
    // Token and can add a new Token to the return vector
    if(ST_ERR == nextState){
      // Get the kind corresponding to the current state
      Kind currKind = stateKinds[currState];
      // If we are in an Error state then we have reached an invalid
      // Token - so we throw and error and delete the Tokens parsed
      // thus far
      if(ERR == currKind){
        vector<Token*>::iterator vit;
        for(vit = ret.begin(); vit != ret.end(); ++vit)
          delete *vit;
        throw "ERROR in lexing after reading " + string(line.begin(),it);
      }
      // If we are not in Whitespace then we push back a new token
      // based upon the kind of the State we end in
      // Whitespace is ignored for practical purposes
      if(WHITESPACE != currKind)
        ret.push_back(Token::makeToken(currKind,string(startIter,it)));
      // Start of next Token begins here
      startIter = it;
      // Revert to start state to begin recognizing next token
      currState = ST_START;
      if(it == line.end()) break;
    } else {
      // Otherwise we proceed to the next state and increment the iterator
      currState = nextState;
      ++it;
    }
  }
  return ret;
}

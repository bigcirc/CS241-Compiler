#include "kind.h"
#include "lexer.h"
#include <stdlib.h>
#include <vector>
#include <utility>
#include <string>
#include <iostream>
// Use only the neeeded aspects of each namespace
using std::string;
using std::vector;
using std::endl;
using std::cerr;
using std::cin;
using std::cout;
using std::getline;
using ASM::Token;
using ASM::Lexer;
using std::pair;


//print int as binary string
void printNum(int i) {
  char c = i >> 24;
  cout << c;
  c = i >> 16;
  cout << c;
  c = i >> 8;
  cout << c; 
  c = i;
  cout << c;
}

//check if label is unique
bool checkLabel( string label, vector< pair<string, int> > symbolTable ) {
    vector< pair<string, int> >::iterator it;
    for(it = symbolTable.begin(); it != symbolTable.end(); ++it){
      if( label == it->first ) { return true; }
    }
    return false;
}

int findLabelValue( string label, vector< pair<string, int> > symbolTable ) {
   vector< pair<string, int> >::iterator it;
    for(it = symbolTable.begin(); it != symbolTable.end(); ++it){
      if( label == it->first ) { return it->second; }
    }
}

int main(int argc, char* argv[]){
  // Nested vector representing lines of Tokens
  // Needs to be used here to cleanup in the case
  // of an exception
  vector< vector<Token*> > tokLines;

  bool error = false;
  int addressCount = 0;
  vector< pair<string, int> > symbolTable;

  try{
    // Create a MIPS recognizer to tokenize
    // the input lines
    Lexer lexer;
    // Tokenize each line of the input
    string line;
    while(getline(cin,line)){
      tokLines.push_back(lexer.scan(line));
    }
    
    // PASS 1 Fill symbol table
    vector<vector<Token*> >::iterator it;
    vector<Token*>::iterator it2;
    for(it = tokLines.begin(); it != tokLines.end(); ++it){
      for( it2 = it->begin(); it2 != it->end(); ++it2){
	
	//LABEL
	if( (*(*it2)).getKind() == ASM::LABEL ){
	  
	  if( it2 != it->begin()){ //label is not at start of line, check for preceding instruction
	    if( (*(*(it2-1))).getKind() != ASM::LABEL ){
	      cerr << "ERROR: label definition after instruction" << endl;
	      error = true;
	      break;
	    } 
	  } 

          if( !checkLabel( (*(*(it2))).getLexeme().substr(0, (*(*(it2))).getLexeme().size() - 1), symbolTable )) {
	    symbolTable.push_back( make_pair( (*(*(it2))).getLexeme().substr(0, (*(*(it2))).getLexeme().size() - 1), addressCount) ); //will remove trailing ":"
          } else {
            cerr << "ERROR: duplicate label" << endl;
	    error = true;
	    break;
	  }
	} else if( (*(*it2)).getKind() == ASM::DOTWORD) {
	  addressCount += 4;
	}
	
      }
    }
    
    if(!error){
    // PASS 2 Check for valid input and print
    for(it = tokLines.begin(); it != tokLines.end(); ++it){
      for( it2 = it->begin(); it2 != it->end(); ++it2){
	
	//LABEL
	if( (*(*it2)).getKind() == ASM::LABEL ){
	
	//DOTWORD
	} else if( (*(*it2)).getKind() == ASM::DOTWORD ){
	  
	  if( it2+1 == it->end() ) {
	    cerr << "ERROR: no word after .word" << endl;
	    error = true;
	    break;
	  } else if( it2+2 == it->end() ){ //nothing after .word x
	   
	    if( (*(*(it2+1))).getKind() == ASM::INT || (*(*(it2+1))).getKind() == ASM::HEXINT ){
	     
	      printNum( (*(*(it2+1))).toInt() );
	    
	    } else { //not and integer, check for matching label
	     
	      if( !checkLabel( (*(*(it2+1))).getLexeme(), symbolTable )) {
		cerr << "ERROR: no matching label" << endl;
		error = true;
		break;
	      } else { 
		printNum( findLabelValue((*(*(it2+1))).getLexeme(), symbolTable) ); //prints label's matching value
	      }
	    }
	  
	  } else {
	     cerr << "ERROR: more stuff after word" << endl;
	     error = true;
	     break;
	  }

	  ++it2; //skip integer that follows .word	  

	} else {
	  cerr << "ERROR: incorrect input (not label or .word)" << endl;
	  error = true;
	  break;
	}
	
      }
    }
    }
 
  } catch(const string& msg){
    // If an exception occurs print the message and end the program
    cerr << msg << endl;
  }
  
  // Iterate and print symbol table
  if( !error ) {
    vector< pair<string, int> >::iterator it3;
    for(it3 = symbolTable.begin(); it3 != symbolTable.end(); ++it3){
      cerr << it3->first  << " " << it3->second << endl;
    }
  }

  // Delete the Tokens that have been made
  vector<vector<Token*> >::iterator it;
  for(it = tokLines.begin(); it != tokLines.end(); ++it){
    vector<Token*>::iterator it2;
    for(it2 = it->begin(); it2 != it->end(); ++it2){
      delete *it2;
    }
  }
}

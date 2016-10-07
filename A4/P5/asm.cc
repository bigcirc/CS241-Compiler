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

  int PC = 0;
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
	} else if( (*(*it2)).getKind() == ASM::DOTWORD || 
		   ((*(*it2)).getKind() == ASM::ID && 
		    ((*(*it2)).getLexeme() == "jr" ||
		     (*(*it2)).getLexeme() == "jalr" ||
		     (*(*it2)).getLexeme() == "add" ||
		     (*(*it2)).getLexeme() == "sub" ||
		     (*(*it2)).getLexeme() == "slt" ||
		     (*(*it2)).getLexeme() == "sltu" ||
		     (*(*it2)).getLexeme() == "beq" ||
		     (*(*it2)).getLexeme() == "bne" ))
		   ){
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
	  PC+=4;

	// OPERATOR
	} else if( (*(*it2)).getKind() == ASM::ID ){

	  string op = (*(*it2)).getLexeme(); //We will be checking the operator a lot 
 
	  if( it2+1 == it->end() ) {
	    cerr << "ERROR: nothing after ID" << endl;
	    error = true;
	    break;

	  // 1 input operator
	  } else if( op == "jr" || op == "jalr" || op == "lis" || op == "mflo" || op == "mfhi" ){
	  
	    if( it2+2 == it->end() ){ //nothing after ID $XX
	      if( (*(*(it2+1))).getKind() == ASM::REGISTER ){ //Valid register follows either jr or jalr
		
		unsigned int reg = (*(*(it2+1))).toInt();
		unsigned int instr;
		
		if( op == "jr" ){ instr = (0 << 26) | (reg << 21) | 0x00000008; }
		else if( op == "jalr" ){ instr = (0 << 26) | (reg << 21) | 0x00000009; } 
		else if( op == "mfhi" ){ instr = (0 << 16) | (reg << 11) | 0x00000010; } 
		else if( op == "mflo" ){ instr = (0 << 16) | (reg << 11) | 0x00000012; } 
		else if( op == "lis" ){ instr = (0 << 16) | (reg << 11) | 0x00000014; } 
		printNum( instr );
	    
	      } else { //Not a register
		cerr << "ERROR: not valid input, expected register" << endl;
		error = true;
		break;
	      }

	      ++it2; //skip the register token after ID
	    } else {
	     cerr << "ERROR: more stuff after ID, expected 1 entry" << endl;
	     error = true;
	     break;
	    }

	  // 3 input operator                 
	  } else if( op == "add" || op == "sub" || op == "slt" || op == "sltu" || op == "beq" || op == "bne" ){
	    if( it2+1 == it->end() || it2+2 == it->end() || it2+3 == it->end() || it2+4 == it->end() || it2+5 == it->end() ){
	      cerr << "ERROR: not enough entries, expected 5 with commas" << endl;
	      error = true;
	      break;
	    
	    } else if( it2+6 == it->end() ){ // No extra garbage
	     
	      if( (*(*(it2+1))).getKind() == ASM::REGISTER && 
		  (*(*(it2+2))).getKind() == ASM::COMMA && 
		  (*(*(it2+3))).getKind() == ASM::REGISTER && 
		  (*(*(it2+4))).getKind() == ASM::COMMA && 
		  (*(*(it2+5))).getKind() == ASM::REGISTER  ){
	      
		unsigned int reg1 = (*(*(it2+1))).toInt();
		unsigned int reg2 = (*(*(it2+3))).toInt();
		unsigned int reg3 = (*(*(it2+5))).toInt();
		unsigned int instr;

		if( op == "add" ){ instr = (0 << 26) | (reg2 << 21) | (reg3 << 16) | (reg1 << 11) | 0x00000020; }
		else if( op == "sub" ) { instr = (0 << 26) | (reg2 << 21) | (reg3 << 16) | (reg1 << 11) | 0x00000022; }
		else if( op == "slt" ) { instr = (0 << 26) | (reg2 << 21) | (reg3 << 16) | (reg1 << 11) | 0x0000002a; }
		else if( op == "sltu" ) { instr = (0 << 26) | (reg2 << 21) | (reg3 << 16) | (reg1 << 11) | 0x0000002b; }
		else { 
		  cerr << "ERROR: invalid input, expected REG, REG, INT" << endl;
		  error = true;
		  break; 
		}
		printNum( instr );
	    
	      } else if( (*(*(it2+1))).getKind() == ASM::REGISTER && 
			 (*(*(it2+2))).getKind() == ASM::COMMA && 
			 (*(*(it2+3))).getKind() == ASM::REGISTER && 
			 (*(*(it2+4))).getKind() == ASM::COMMA && 
			 ((*(*(it2+5))).getKind() == ASM::INT || (*(*(it2+5))).getKind() == ASM::HEXINT || (*(*(it2+5))).getKind() == ASM::ID)){

		unsigned int reg1 = (*(*(it2+1))).toInt();
		unsigned int reg2 = (*(*(it2+3))).toInt();
		int offset;
		unsigned int instr;
		
		//Check for INT
		if( (*(*(it2+5))).getKind() == ASM::INT || (*(*(it2+5))).getKind() == ASM::HEXINT ){  
		  offset = (*(*(it2+5))).toInt();
		//Check got LABEL
		} else if( checkLabel( (*(*(it2+5))).getLexeme(), symbolTable) ) { //Not an int so check for matching label
		  offset = (findLabelValue((*(*(it2+5))).getLexeme(), symbolTable)-PC-4) / 4;
		} else { 
		  cerr << "ERROR: no matching label" << endl;
		  error = true;
		  break;
		}
	      
	      	if( (((*(*(it2+5))).getKind() == ASM::INT ||
		      (*(*(it2+5))).getKind() == ASM::ID) && 
		       offset > -32769 && 
		       offset < 32768)
		      ||
		      ((*(*(it2+5))).getKind() == ASM::HEXINT &&  
		       offset < 0x10000) ){
		    
		    if( op == "beq" ){ instr = (4 << 26) | (reg1 << 21) | (reg2 << 16) | offset & 0xffff; }
		    else if( op == "bne" ) { instr = (5 << 26) | (reg1 << 21) | (reg2 << 16) | offset & 0xffff; }
		    else { 
		      cerr << "ERROR: invalid input, expected REG, REG, REG" << endl;
		      error = true;
		      break; 
		    }
		    printNum( instr ); 
		    
		} else {
		  cerr << "ERROR: int out of range, must be in the range of -32768 to 32767" << endl;
		  error = true;
		  break;
		}
			
		
	      } else {
		cerr << "ERROR: invalid input, expected REG, REG, REG" << endl;
		error = true;
		break;
	      }
	      
	      it2+=5; //skip the register tokens after ID
	      PC+=4;
	      
	    } else {
	      cerr << "ERROR: expected newline but there's more stuff" << endl;
	      error = true;
	      break;
	    }
	    
	  }
	  
	} else {
	  cerr << "ERROR: Incorrect input. Not label or .word or operator(ID)" << endl;
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
    cerr << "PC " << PC << endl;
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

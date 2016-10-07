#include <iostream>
#include <sstream>
#include <utility>
#include <map>
#include <vector>
using namespace std;

class Tree {
public:
  string rules;
  vector< string > tokens;
  vector< Tree > children;
};


typedef pair<string, int> tLoc;
typedef map<string, tLoc> sT;
typedef pair<vector<string>, sT> sigST;
typedef map<string, sigST > tST;
tST topSymbolTable; 
string proc = "none";
string sigProc = "none";
bool error = false;
int sigCounter = 0;
int offset = 0;
int loopCounter = 0;
int testCounter = 0;

bool isTerminal( string word ) {
  string terminals[] = {"BOF", "BECOMES", "COMMA", "ELSE", "EOF", "EQ", "GE", "GT", "ID", "IF", "INT", "LBRACE", "LE", "LPAREN", "LT", "MINUS", "NE", "NUM", "PCT", "PLUS", "PRINTLN", "RBRACE", "RETURN", "RPAREN", "SEMI", "SLASH", "STAR", "WAIN", "WHILE", "AMP", "LBRACK", "RBRACK", "NEW", "DELETE", "NULL"};
  for(int i = 0; i < 35; i++){
    if( word == terminals[i] ) return true;
  }
  return false;
}


Tree BuildNode( string rules ) {

  Tree parseNode;
  parseNode.rules = rules;
  
  istringstream iss(rules);
  string word;

  while( iss >> word ) {
    parseNode.tokens.push_back( word );
  }
  
  if(isTerminal(parseNode.tokens[0])==false) {
    
    for( int i = 1; i < parseNode.tokens.size(); i++ ) {
      string line;
      getline(cin, line);
      parseNode.children.push_back( BuildNode( line ));
    }

  }
   
  return parseNode;
}
 
void PrintTree( Tree parseTree ) {
  for( vector<Tree>::const_iterator it=parseTree.children.begin(); it!=parseTree.children.end(); ++it ) {
    cout << (*it).rules << endl;
    PrintTree(*it);
  }
}  


void BuildSymbolTable(Tree parseTree){
  for( vector<Tree>::const_iterator it=parseTree.children.begin(); it!=parseTree.children.end(); ++it ) {
    if(it->rules == "procedure INT ID LPAREN params RPAREN LBRACE dcls statements RETURN expr SEMI RBRACE"){  //new function
      
      if( topSymbolTable.find( it->children[1].tokens[1] ) == topSymbolTable.end() ){ //new function
	  proc = it->children[1].tokens[1];
	  topSymbolTable.insert( make_pair( proc, sigST() ));
      } else {
	cerr << "ERROR: Function already declared" << endl;
	error = true;
      }

    } else  if(it->rules == "main INT WAIN LPAREN dcl COMMA dcl RPAREN LBRACE dcls statements RETURN expr SEMI RBRACE"){    //wain
      
      if( topSymbolTable.find( "wain" ) == topSymbolTable.end() ){
	proc = "wain";
	topSymbolTable.insert( make_pair( proc, sigST() ));
	
	if(it->children[3].children[0].rules == "type INT STAR"){
	  topSymbolTable[proc].first.push_back( it->children[3].children[0].children[0].tokens[1] + it->children[3].children[0].children[1].tokens[1] );
	} else if(it->children[3].children[0].rules == "type INT"){
	  topSymbolTable[proc].first.push_back( it->children[3].children[0].children[0].tokens[1] );
	}

	cout << "sw $1, -4($30)" << endl;
	cout << "sub $30, $30, $4" << endl;
	
	if(it->children[5].children[0].rules == "type INT STAR"){
	  topSymbolTable[proc].first.push_back( it->children[5].children[0].children[0].tokens[1] + it->children[5].children[0].children[1].tokens[1] );
	} else if(it->children[5].children[0].rules == "type INT"){
	  topSymbolTable[proc].first.push_back( it->children[5].children[0].children[0].tokens[1] );
	}
       
	cout << "sw $2, -4($30)" << endl;
	cout << "sub $30, $30, $4" << endl;

      } else {
	cerr << "ERROR: WAIN already declared" << endl;
	error = true;
      }

    } else if(it->rules == "paramlist dcl COMMA paramlist" || it->rules == "paramlist dcl"){ //retrieve signatures

      if(it->children[0].children[0].rules == "type INT STAR"){
	topSymbolTable[proc].first.push_back( it->children[0].children[0].children[0].tokens[1] + it->children[0].children[0].children[1].tokens[1] );
      } else if(it->children[0].children[0].rules == "type INT"){
	topSymbolTable[proc].first.push_back( it->children[0].children[0].children[0].tokens[1] );
      }

    } else if(it->rules == "factor ID LPAREN arglist RPAREN" || it->rules == "factor ID LPAREN RPAREN"){ //function called
      
      if( topSymbolTable.find( it->children[0].tokens[1] ) == topSymbolTable.end() ){ //function not declared
	cerr << "ERROR: function not declared" << endl;
	error = true;
      }

    } else if(it->rules == "dcl type ID"){       
      
      if( topSymbolTable[proc].second.find(it->children[1].tokens[1]) == topSymbolTable[proc].second.end()){
	if( it->children[0].rules == "type INT STAR" ){
	  topSymbolTable[proc].second.insert( make_pair( it->children[1].tokens[1], make_pair( it->children[0].children[0].tokens[1] + it->children[0].children[1].tokens[1], offset )));
	} else if( it->children[0].rules == "type INT" ){
	  topSymbolTable[proc].second.insert( make_pair( it->children[1].tokens[1], make_pair( it->children[0].children[0].tokens[1], offset )));
	}
	
	offset -= 4;

      } else {
	cerr << "ERROR: Multiple declerations" << endl;
	error = true;
      }

    } else if(it->rules == "dcls dcls dcl BECOMES NUM SEMI"){
      cout << "lis $3" << endl << ".word " << it->children[3].tokens[1] << endl; 
      cout << "sw $3, -4($30)" << endl;
      cout << "sub $30, $30, $4" << endl;
    } 

    BuildSymbolTable(*it);
  }
}

void CheckForUndeclared(Tree parseTree){
  for( vector<Tree>::const_iterator it=parseTree.children.begin(); it!=parseTree.children.end(); ++it ) {    
    if(it->rules == "procedure INT ID LPAREN params RPAREN LBRACE dcls statements RETURN expr SEMI RBRACE"){
      proc = it->children[1].tokens[1];
    } else  if(it->rules == "main INT WAIN LPAREN dcl COMMA dcl RPAREN LBRACE dcls statements RETURN expr SEMI RBRACE"){
      proc = "wain";
    } else if(it->rules == "factor ID"){    //iterate through symbol table and check if term is declared
      for(map< string, sigST >::const_iterator it1=topSymbolTable.begin(); it1!=topSymbolTable.end(); ++it1){
	if( topSymbolTable[proc].second.find(it->children[0].tokens[1]) == topSymbolTable[proc].second.end() ){
	  cerr << "ERROR: variable undeclared" << endl;
	  error = true;
	} 
      }
    }
    CheckForUndeclared(*it);
  }
}

/*
string typeOf(Tree parseTree){
  if(parseTree.tokens[0] == "ID"){
    
    return topSymbolTable[proc].second[parseTree.tokens[1]];

  } else if(parseTree.tokens[0] == "NUM") {

    return "int";

  } else if(parseTree.rules == "expr term" || parseTree.rules == "term factor" || parseTree.rules == "factor ID" || parseTree.rules == "factor NUM" || parseTree.rules == "lvalue ID"){
    
    return typeOf(parseTree.children[0]);

  } else if(parseTree.rules == "lvalue LPAREN lvalue RPAREN" || parseTree.rules == "factor LPAREN expr RPAREN"){
    
    return typeOf(parseTree.children[1]);

  } else if(parseTree.rules == "lvalue STAR factor" || parseTree.rules == "factor STAR factor"){

    if(typeOf(parseTree.children[1]) != "int*"){
      return "ERROR: not a pointer";
    } else {
      return "int";
    }

  } else if(parseTree.rules == "factor AMP lvalue"){

    if(typeOf(parseTree.children[1]) != "int"){
      return "ERROR: ampersand error";
    } else {
      return "int*";
    }

  } else if(parseTree.rules == "procedure INT ID LPAREN params RPAREN LBRACE dcls statements RETURN expr SEMI RBRACE"){
    
    if(typeOf(parseTree.children[9]) != "int"){
      return "ERROR: function needs int";
    } else {
      return typeOf(parseTree.children[9]);
    }

  } else  if(parseTree.rules == "main INT WAIN LPAREN dcl COMMA dcl RPAREN LBRACE dcls statements RETURN expr SEMI RBRACE"){
    
    if(typeOf(parseTree.children[11]) != "int"){
      return "ERROR: wain needs int";
    } else {
      return typeOf(parseTree.children[11]);
    }

  } else if(parseTree.rules == "dcls dcls dcl BECOMES NULL SEMI" ){

    if(typeOf(parseTree.children[1].children[1]) != "int*"){
      return "ERROR: cannot initiate int to NULL";
    } else {
      return typeOf(parseTree.children[1]);
    }
    
  } else if(parseTree.rules == "test expr EQ expr" || parseTree.rules == "test expr EQ expr"){

    if(typeOf(parseTree.children[0]) != typeOf(parseTree.children[2])){
      return "ERROR: test type mismatch";
    } else {
      return typeOf(parseTree.children[0]);
    }

  } else if(parseTree.rules == "test expr LT expr" || parseTree.rules == "test expr GT expr" || parseTree.rules == "test expr LE expr" || parseTree.rules == "test expr GE expr"){

    if(typeOf(parseTree.children[0]) != typeOf(parseTree.children[2])){
      return "ERROR: test type mismatch";
    } else {
      if(typeOf(parseTree.children[0]) == "*int"){
	return "array comparison";
      }
    }
    
  } else if(parseTree.rules == "PRINTLN LPAREN expr RPAREN SEMI"){
    
    if(typeOf(parseTree.children[1]) != "int"){
      return "ERROR: println requires int";
    }

  } else if(parseTree.rules == "DELETE LBRACK RBRACK expr SEMI"){
    
    if(typeOf(parseTree.children[2]) != "int*"){
      return "ERROR: delete required pointer";
    }
  
  }else if(parseTree.rules == "factor NEW INT LBRACK expr RBRACK"){
    
    if(typeOf(parseTree.children[3]) != "int"){
      return "ERROR: argument of NEW INT [] must be an int";
    } else {
      return "int*";
    }
    
  } else if(parseTree.rules == "statement lvalue BECOMES expr SEMI"){
    
    if(typeOf(parseTree.children[0]) != typeOf(parseTree.children[2])){
      return "ERROR: type mismatch";
    } else {
      return typeOf(parseTree.children[0]);
    }
      

    //CHECKS FOR SIGNATURES
  }else if(parseTree.rules == "factor ID LPAREN arglist RPAREN"){ //new function called, reset sigCounter
    
    sigCounter = 0;
    sigProc = parseTree.children[0].tokens[1];
    return typeOf(parseTree.children[2]);

  }else if(parseTree.rules == "arglist expr COMMA arglist") {
    
    if(sigCounter < topSymbolTable[sigProc].first.size()){
      if(topSymbolTable[sigProc].first[sigCounter] != typeOf(parseTree.children[0])){
	return "ERROR: signatures do not match " + topSymbolTable[sigProc].first[sigCounter] + " and " + typeOf(parseTree.children[0]);
      } else {
	sigCounter++;
	return typeOf(parseTree.children[2]);
      }
    } else {
      return "ERROR: not enough signatures";
    }

  }else if(parseTree.rules == "arglist expr"){

    if(sigCounter != topSymbolTable[sigProc].first.size() - 1){
      return "ERROR: not enough parameters";
    }else if(sigCounter < topSymbolTable[sigProc].first.size()){
      if(topSymbolTable[sigProc].first[sigCounter] != typeOf(parseTree.children[0])){
	return "ERROR: signatures do not match " + topSymbolTable[sigProc].first[sigCounter] + " and " + typeOf(parseTree.children[0]);
      } else {
	return typeOf(parseTree.children[0]);
      }
    } else {
      return "ERROR: not enough signatures";
    }

  
  } else if(parseTree.rules == "expr expr PLUS term"){

    if(typeOf(parseTree.children[0]) == "int" && typeOf(parseTree.children[2]) == "int"){
      return "int";
    } else if(typeOf(parseTree.children[0]) == "int*" && typeOf(parseTree.children[2]) == "int*"){
      return "ERROR: int* + int*";
    } else {
      return "int*";
    }

  } else if(parseTree.rules == "expr expr MINUS term"){
    
    if(typeOf(parseTree.children[0]) == "int" && typeOf(parseTree.children[2]) == "int"){
      return "int";
    } else if(typeOf(parseTree.children[0]) == "int*" && typeOf(parseTree.children[2]) == "int*"){
      return "int";
    } else if(typeOf(parseTree.children[0]) == "int*" && typeOf(parseTree.children[2]) == "int"){
      return "int*";
    } else {
      return "ERROR: int - int*";
    }

  } else if(parseTree.rules == "term term STAR factor" || parseTree.rules == "term term SLASH factor" || parseTree.rules == "term term PCT factor"){

    if(typeOf(parseTree.children[0]) == "int" && typeOf(parseTree.children[2]) == "int"){
      return "int";
    } else {
      return "ERROR: multiplication error";
    }
  } else {
    return "no type";
  }
}


bool CheckTypes(Tree parseTree){
  for( vector<Tree>::const_iterator it=parseTree.children.begin(); it!=parseTree.children.end(); ++it ) {    
    if(it->rules == "procedure INT ID LPAREN params RPAREN LBRACE dcls statements RETURN expr SEMI RBRACE"){
      proc = it->children[1].tokens[1];
    } else  if(it->rules == "main INT WAIN LPAREN dcl COMMA dcl RPAREN LBRACE dcls statements RETURN expr SEMI RBRACE"){
      proc = "wain";
    } else { 
      if( typeOf(*it).find("ERROR") != string::npos ){
	error = true;
	cerr << typeOf(*it) << endl;
	return false;
      }
    }
    CheckTypes(*it);
  }
  return true;
}
*/

void push(int reg){
  cout << "sw $" << reg << ", -4($30)" << endl;
  cout << "sub $30, $30, $4" << endl;
}
void pop(int reg){
  cout << "add $30, $30, $4" << endl;
  cout << "lw $" << reg << ", -4($30)" << endl;
}

int value(string var){
  return topSymbolTable[proc].second[var].second;
}

void code(string s){
  cout << "lw $3, " << value(s) << "($29)" << endl;
}


void GenCode(Tree t){    
  if(t.rules == "start BOF procedures EOF" || t.rules == "factor LPAREN expr RPAREN"){
   
    GenCode(t.children[1]);

  } else if(t.rules == "statements statements statement"){
  
    GenCode(t.children[0]);
    GenCode(t.children[1]);

  } else if(t.rules == "procedures procedure procedures"){
  
    GenCode(t.children[0]);
    GenCode(t.children[1]);

  } else if(t.rules == "procedures main" || t.rules == "expr term" || t.rules == "term factor" || t.rules == "factor ID" || t.rules == "factor NUM"){ 
    
    GenCode(t.children[0]);

  } else if(t.tokens[0] == "NUM"){
    
    cout << "lis $3" << endl << ".word " << t.tokens[1] << endl;
    
  } else if(t.tokens[0] == "ID"){
    
    code(t.tokens[1]);
   
  } else if(t.rules == "main INT WAIN LPAREN dcl COMMA dcl RPAREN LBRACE dcls statements RETURN expr SEMI RBRACE"){
    
    proc = "wain";
    GenCode(t.children[8]);
    GenCode(t.children[9]);
    GenCode(t.children[11]);

  } else if(t.rules == "procedure INT ID LPAREN params RPAREN LBRACE dcls statements RETURN expr SEMI RBRACE"){
    
    proc = t.children[1].tokens[1];
    cout << "F" << t.children[1].tokens[1] << ":" << endl;
    cout << "sub $29, $30, $4" << endl;
    GenCode(t.children[6]);
    GenCode(t.children[7]);
    GenCode(t.children[9]);
    cout << "jr $31" << endl;
    
  } else if(t.rules == "factor ID LPAREN RPAREN"){
    
    push(29);
    push(31);
    cout << "lis $31" << endl;
    cout << ".word F" << t.children[0].tokens[1] << endl;
    cout << "jalr $31" << endl;
    pop(31);
    pop(29);

  } else if(t.rules == "dcls dcls dcl BECOMES NUM SEMI"){
    
    GenCode(t.children[0]);
    cout << "lis $3" << endl << ".word " << t.children[3].tokens[1] << endl; 
    cout << "sw $3, " << value( t.children[1].children[1].tokens[1] ) << "($29)" << "         ;variable: " << t.children[1].children[1].tokens[1] << endl;
    
  } else if(t.rules == "statement lvalue BECOMES expr SEMI"){
    
    GenCode(t.children[2]); 
    if(t.children[0].rules == "lvalue STAR factor"){
      GenCode(t.children[0]);
      cout << "add $6, $3, $0" << endl;
      GenCode(t.children[2]);
      cout << "sw $3, 0($6)" << endl;
      
      //cout << "sw $3, " << value( t.children[0].children[1].children[0].tokens[1] ) << "($29)" << "         ;variable: " << t.children[0].children[1].children[0].tokens[1] << endl;
    } else {
      cout << "sw $3, " << value( t.children[0].children[0].tokens[1] ) << "($29)" << "         ;variable: " << t.children[0].children[0].tokens[1] << endl;
    }
     
  } else if(t.rules == "expr expr PLUS term" || t.rules == "expr expr MINUS term" || t.rules == "term term STAR factor" || t.rules == "term term SLASH factor" || t.rules == "term term PCT factor" ){

    GenCode(t.children[0]);
    push(3);
    GenCode(t.children[2]);
    pop(5);
    
    if(t.rules == "expr expr PLUS term"){
      cout << "add $3, $5, $3                  ;ADD" << endl;
    } else if(t.rules == "expr expr MINUS term"){
      cout << "sub $3, $5, $3                  ;SUB" << endl;  
    } else if(t.rules == "term term STAR factor"){
      cout << "mult $5, $3                     ;MULT" << endl;
      cout << "mflo $3" << endl;
    } else if(t.rules == "term term SLASH factor"){
      cout << "div $5, $3                      ;DIV" << endl;
      cout << "mflo $3" << endl;
    } else if(t.rules == "term term PCT factor"){
      cout << "div $5, $3                      ;PCT" << endl;
      cout << "mfhi $3" << endl;
    }

  } else if(t.rules == "statement PRINTLN LPAREN expr RPAREN SEMI"){
    
    GenCode(t.children[2]);
    cout << "add $1, $3, $0" << endl;
    push(31);
    cout << "lis $5" << endl << ".word print" << endl << "jalr $5" << endl;
    pop(31);
 
  } else if(t.rules == "statement WHILE LPAREN test RPAREN LBRACE statements RBRACE"){
    cout << "toploop" << loopCounter << ":"<< endl;
    GenCode(t.children[2]);
    cout << "beq $3, $0, endloop" << loopCounter << endl;
    loopCounter++;
    GenCode(t.children[5]);
    loopCounter--;
    cout << "beq $0, $0, toploop" << loopCounter << endl;
    cout << "endloop" << loopCounter << ":"<< endl;
  
  } else if(t.rules == "statement IF LPAREN test RPAREN LBRACE statements RBRACE ELSE LBRACE statements RBRACE"){
  
    GenCode(t.children[2]);
    cout << "beq $3, $0, else" << testCounter << endl;
    testCounter++;
    GenCode(t.children[5]);
    testCounter--;
    cout << "beq $0, $0, endif" << testCounter << endl;
    cout << "else" << testCounter << ":" << endl;
    testCounter++;
    GenCode(t.children[9]);
    testCounter--;
    cout << "endif" << testCounter << ":" << endl;

  } else if(t.tokens[0] == "test"){
    
    GenCode(t.children[0]);
    push(3);
    GenCode(t.children[2]);
    pop(5);
   
    if(t.rules == "test expr LT expr"){
      cout << "slt $6, $5, $3" << endl;
      cout << "add $3, $0, $6" << endl;
    } else if(t.rules == "test expr GT expr"){
      cout << "slt $6, $3, $5" << endl;
      cout << "add $3, $0, $6" << endl;
    } else if(t.rules == "test expr EQ expr"){
      cout << "slt $6, $5, $3" << endl;
      cout << "slt $7, $3, $5" << endl;
      cout << "add $3, $6, $7" << endl;
      cout << "sub $3, $11, $3" << endl;
    } else if(t.rules == "test expr NE expr"){
      cout << "slt $6, $5, $3" << endl;
      cout << "slt $7, $3, $5" << endl;
      cout << "add $3, $6, $7" << endl;
    } else if(t.rules == "test expr LE expr"){
      cout << "slt $6, $5, $3" << endl;
      cout << "slt $7, $3, $5" << endl;
      cout << "add $3, $6, $7" << endl;
      cout << "sub $3, $11, $3" << endl;
      cout << "add $3, $3, $6" << endl;
    } else if(t.rules == "test expr GE expr"){
      cout << "slt $6, $5, $3" << endl;
      cout << "slt $7, $3, $5" << endl;
      cout << "add $3, $6, $7" << endl;
      cout << "sub $3, $11, $3" << endl;
      cout << "add $3, $3, $7" << endl;
    }
    
  } else if(t.rules == "dcls dcls dcl BECOMES NULL SEMI"){
 
    GenCode(t.children[0]);
    cout << "add $3, $11, $0" << endl; 
    cout << "sw $3, " << value( t.children[1].children[1].tokens[1] ) << "($29)" << "         ;variable pointer: " << t.children[1].children[1].tokens[1] << endl;
    

  } else if(t.rules == "factor NULL"){
    
    cout << "add $3, $11, $0" << endl;

  } else if(t.rules == "factor STAR factor"){
    
    GenCode(t.children[1]);
    cout << "lw $3, 0($3)" << endl;    

  } else if(t.rules == "lvalue STAR factor"){

    GenCode(t.children[1]);

  } else if(t.rules == "factor AMP lvalue"){
    
    GenCode(t.children[1]);
  
  } else if(t.rules == "lvalue ID"){
    
    cout << "lis $3" << endl;
    cout << ".word " << value( t.children[0].tokens[1] ) << endl;
    cout << "add $3, $3, $29" << endl;

  } else if(t.rules == "factor NEW INT LBRACK expr RBRACK"){
    
    GenCode(t.children[3]);
    cout << "add $1, $3, $0" << endl;
    push(31);
    cout << "lis $5" << endl << ".word new" << endl << "jalr $5" << endl;
    pop(31);
    cout << "bne $3, $0, 1" << endl;
    cout << "add $3, $11, $0              ;SET POINTER TO NULL" << endl;
    
  } else if(t.rules == "statement DELETE LBRACK RBRACK expr SEMI"){
    
    GenCode(t.children[3]);
    cout << "beq $3, $11, skipDelete" << endl;
    cout << "add $1, $3, $0" << endl;
    push(31);
    cout << "lis $5" << endl << ".word delete" << endl << "jalr $5" << endl;
    pop(31);
    cout << "skipDelete:" << endl;
    
  }
  
}

int main() {
  Tree parseTree;
  
  string line;
  getline(cin, line);
  parseTree = BuildNode(line);


  //Prologue
  cout << ";Prologue" << endl;
 
  cout << ".import print" << endl;
  cout << ".import init" << endl;
  cout << ".import new" << endl;
  cout << ".import delete" << endl;
 
  cout << "lis $4" << endl << ".word 4" << endl;
  cout << "lis $11" << endl << ".word 1" << endl << "sub $29, $30, $4" << endl;
  
  BuildSymbolTable( parseTree );
  //CheckForUndeclared( parseTree );
  //CheckTypes( parseTree );
  
  cout << ";Prologue init" << endl;
  if( topSymbolTable["wain"].first.front() == "int" ){
    cout << "add $2, $0, $0                ;INIT $2 TO 0" << endl;
  }
  push(31);
  cout << "lis $5" << endl << ".word init" << endl << "jalr $5" << endl;
  pop(31);
  

  cout << ";GenCode" << endl;
  GenCode( parseTree );
 
  //Epilogue
  cout << ";Epilogue" << endl;
  cout << "add $30, $29, $4" << endl << "jr $31" << endl;
 

  /*
  cerr << "Symbol Table:" << endl;
  if(error == false){
    for( map< string, sigST >::const_iterator it=topSymbolTable.begin(); it!=topSymbolTable.end(); ++it){
      
      cerr << it->first;
      for( vector<string>::const_iterator it2=it->second.first.begin(); it2!=it->second.first.end(); ++it2){
	cerr << " " << *it2;
      }
      cerr << endl;
      
      if( it->first == "wain"){
	for( sT::const_iterator it1=it->second.second.begin(); it1!=it->second.second.end(); ++it1){
	  cerr << it1->first << " " << it1->second.first << " " << it1->second.second << endl;
	}
      }
      
      cerr << endl;
    }
  }
  */
}

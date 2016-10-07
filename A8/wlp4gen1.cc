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

typedef map<string, string> sT;
typedef pair<vector<string>, sT> sigST;
typedef map<string, sigST > tST;
tST topSymbolTable; 
string proc = "none";
bool error = false;

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
    if(it->rules == "procedure INT ID LPAREN params RPAREN LBRACE dcls statements RETURN expr SEMI RBRACE"){

      proc = it->children[1].tokens[1];
      topSymbolTable.insert( make_pair( proc, sigST() ));

    } else  if(it->rules == "main INT WAIN LPAREN dcl COMMA dcl RPAREN LBRACE dcls statements RETURN expr SEMI RBRACE"){

      proc = "wain";
      topSymbolTable.insert( make_pair( proc, sigST() ));
      
      if(it->children[3].children[0].rules == "type INT STAR"){
	topSymbolTable[proc].first.push_back( it->children[3].children[0].children[0].tokens[1] + it->children[3].children[0].children[1].tokens[1] );
      } else if(it->children[3].children[0].rules == "type INT"){
	topSymbolTable[proc].first.push_back( it->children[3].children[0].children[0].tokens[1] );
      }

      if(it->children[5].children[0].rules == "type INT STAR"){
	topSymbolTable[proc].first.push_back( it->children[5].children[0].children[0].tokens[1] + it->children[5].children[0].children[1].tokens[1] );
      } else if(it->children[5].children[0].rules == "type INT"){
	topSymbolTable[proc].first.push_back( it->children[5].children[0].children[0].tokens[1] );
      }


    } else if(it->rules == "paramlist dcl COMMA paramlist" || it->rules == "paramlist dcl"){

      if(it->children[0].children[0].rules == "type INT STAR"){
	topSymbolTable[proc].first.push_back( it->children[0].children[0].children[0].tokens[1] + it->children[0].children[0].children[1].tokens[1] );
      } else if(it->children[0].children[0].rules == "type INT"){
	topSymbolTable[proc].first.push_back( it->children[0].children[0].children[0].tokens[1] );
      }

    } else if(it->rules == "dcl type ID"){
      
      if( topSymbolTable[proc].second.find(it->children[1].tokens[1]) == topSymbolTable[proc].second.end()){
	if( it->children[0].rules == "type INT STAR" ){
	  topSymbolTable[proc].second.insert( make_pair( it->children[1].tokens[1], it->children[0].children[0].tokens[1] + it->children[0].children[1].tokens[1] ));
	} else if( it->children[0].rules == "type INT" ){
	  topSymbolTable[proc].second.insert( make_pair( it->children[1].tokens[1], it->children[0].children[0].tokens[1] ));
	}
      } else {
	cerr << "ERROR: Multiple declerations" << endl;
	error = true;
      }

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

int main() {
  Tree parseTree;
  
  string line;
  getline(cin, line);
  parseTree = BuildNode(line);
  
  BuildSymbolTable( parseTree );
  CheckForUndeclared( parseTree );
  
  if(error == false){
    for( map< string, sigST >::const_iterator it=topSymbolTable.begin(); it!=topSymbolTable.end(); ++it){
      //if( it->first == "wain"){
      cerr << it->first;
      for( vector<string>::const_iterator it2=it->second.first.begin(); it2!=it->second.first.end(); ++it2){
	cerr << " " << *it2;
      }
      cerr << endl;
      for( sT::const_iterator it1=it->second.second.begin(); it1!=it->second.second.end(); ++it1){
	cerr << it1->first << " " << it1->second << endl;
      }
      //}
    }
  }
  
}

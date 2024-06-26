#include <stdlib.h>
#include <iostream>
#include "lexer.h"
#include "parser.h"
#include "parse_tree_nodes.h"
 
using namespace std;

int nextToken = 0;            // hold nextToken returned by lex
bool printParse = true;

// Which tree level are we currently in?  
static int level = 0;

void error() {
  cout << endl << "===========================" << endl;
  cout << "ERROR near: " << yytext;
  cout << endl << "===========================" << endl;
  if (yyin)
    fclose(yyin);
  exit(EXIT_FAILURE);
}

// main() to print out the symbol table after a successful parse
//*****************************************************************************
// Symbol table
symbolTableT symbolTable;
// Determine if a symbol is in the symbol table

//*****************************************************************************
// Indent to reveal tree structure
string psp(void) { // Stands for p-space, but I want the name short
  string str("");
  for(int i = 0; i < level; i++)
    str += "|  ";
  return str;
}
//*****************************************************************************
// Report what we found
void output(string what) {
  cout << psp() << "found |" << yytext << "| " << what << endl;
}

//*****************************************************************************
// Print each level with appropriate indentation
void indent() {
  for (int i = 0; i<level; i++)
    cout << ("  ");
}
//*****************************************************************************
// Announce what the lexical analyzer has found
void output1() {
  indent();
  cout << "---> FOUND " << yytext << endl;
}
//*****************************************************************************
//*****************************************************************************
// Forward declarations of FIRST_OF functions.  These check whether the current 
// token is in the FIRST set of a production rule.
bool first_of_program();
bool first_of_block(void);
bool first_of_statement(void);
bool first_of_assignment(void);
bool first_of_factor(void);
bool in_there(string temp);
//*****************************************************************************
// Parses strings in the language generated by the rule:
// <program> → TOK_PROGRAM TOK_IDENT TOK_SEMICOLON <block>
ProgramNode* program()
{
  
  if (!first_of_program()) // Check for PROGRAM
    throw "3: 'PROGRAM' expected";
  
  output("PROGRAM");
  ProgramNode* newProgramNode = nullptr;
  nextToken = yylex();

    // TODO: finish this parsing function...

  if (printParse) cout << psp() << "enter <program>" << endl;
  ++level;

  if(nextToken == TOK_IDENT)
  {
    output("IDENTIFIER");
    nextToken = yylex();
  }
  else
    throw "2: identifier expected";

  if(nextToken == TOK_SEMICOLON)
  {
    output("SEMICOLON");
    nextToken = yylex();
  }
  else {
    throw "14: ';' expected";
  }
  newProgramNode = new ProgramNode(level, block()); 

  --level;
  if (printParse) cout << psp() << "exit <program>" << endl;
  return newProgramNode;
}

BlockNode* block()
{
  
  if(!first_of_block())
    throw "18: error in declaration part OR 17: 'BEGIN' expected";
  
  output("BLOCK");

  if (printParse) cout << psp() << "enter <block>" << endl;
  ++level;

  BlockNode* newBlockNode = nullptr;

if(nextToken == TOK_VAR)
{
  nextToken = yylex();
    
  string idName;
  string idType;

  while(nextToken != TOK_BEGIN)
  {
    
    if(nextToken == TOK_IDENT)
    {
      output("IDENTIFIER");
      idName = yytext;
      nextToken = yylex();
    }
    else
      throw "2: identifier expected";

    if(nextToken == TOK_COLON)
    {
      output("COLON");
      nextToken = yylex();
    }
    else
      throw "5: ':' expected";

    if(nextToken == TOK_REAL)
    {
      output("TYPE");
      idType = yytext;
      nextToken = yylex();
    }
  
    else if(nextToken == TOK_INTEGER)
    {
      output("TYPE");
      idType = yytext;
      nextToken = yylex();
    }
    else 
      throw "10: error in type";

    if(nextToken == TOK_SEMICOLON)
    {
      output("SEMICOLON");
    }
    else 
      throw "14: ';' expected";

    if (printParse) cout << psp() << "-- idName: |" << idName << "| idType: |" << idType << "| --" << endl;
    
    if(in_there(idName))
    { 
      throw
        "101: identifier declared twice";
    } 
    else
      symbolTable.insert(pair<string, float>(idName, 0.0));
      
    nextToken = yylex(); 
    
  }
  
}
  
  newBlockNode = new BlockNode(level, compound());

  --level;
  if (printParse) cout << psp() << "exit <block>" << endl;
  return newBlockNode;
 
}

StatementNode* statement()
{

  output("STATEMENT");

  StatementNode* newStatementNode = nullptr;

  switch( nextToken )
 {
    case TOK_IDENT:
      newStatementNode = assignment();
      break;

    case TOK_BEGIN:
      newStatementNode = compound();
      break;

    case TOK_IF:
      newStatementNode = If();
      break;

    case TOK_WHILE:
      newStatementNode = While();
      break;

    case TOK_READ:
      newStatementNode = Read();
      break;

    case TOK_WRITE:
      newStatementNode = Write();
      break;
  }

  return newStatementNode;
}

AssignmentNode* assignment()
{
  
  if (printParse) cout << psp() << "enter <assignment>" << endl;
  ++level;

  AssignmentNode* newAssignmentNode = new AssignmentNode(level); 

  if(nextToken == TOK_IDENT){
    output("IDENTIFIER");
    if (printParse) cout << psp() << yytext << endl;
    newAssignmentNode->ident = yytext;
    nextToken = yylex();
  }
  else
    throw "2: identifier expected";

  if(nextToken == TOK_ASSIGN){
    output("ASSIGN");
    nextToken = yylex();
  }
  else
    throw "51: ':=' expected";
  
  output("EXPRESSION");
  newAssignmentNode->expression = expression();

  --level;
  if (printParse) cout << psp() << "exit <assignment>" << endl;
  return newAssignmentNode;
}

CompoundNode* compound()
{

  output("BEGIN");

  if (printParse) cout << psp() << "enter <compound_stmt>" << endl;
  ++level;

  CompoundNode* newCompound = new CompoundNode(level);

  if(nextToken == TOK_BEGIN)
  {
     nextToken = yylex();
  }
  else
    throw "17: 'BEGIN' expected";

  newCompound->Statements.push_back(statement());

  while(nextToken == TOK_SEMICOLON)
  {
    output("SEMICOLON");
    nextToken = yylex();
    newCompound->Statements.push_back(statement());
  }
  
  if(nextToken != TOK_END)
  {
    throw "13: 'END' expected";
  }

  --level;
  output("END");
  nextToken = yylex();

  if(printParse) cout << psp() << "exit <compound_stmt>" << endl;
  return newCompound;
}

IfNode* If()
{

 if (printParse) cout << psp() << "enter <if>" << endl;
  ++level;

  IfNode* newIf = new IfNode(level);

  if(nextToken == TOK_IF)
  {
    nextToken = yylex();
  }  

  output("EXPRESSION");
  newIf->expression = expression();

  if(nextToken == TOK_THEN)
  {
    output("THEN");
    nextToken = yylex();
    newIf->then_stmt = statement();
  }
  else
    throw "52: 'THEN' expected";

  if(nextToken == TOK_ELSE){
    --level;
    output("ELSE");
    if(printParse) cout << psp() << "enter <else>" << endl;
    ++level;
    nextToken = yylex();
    newIf->else_stmt = statement();
  }

  --level;
  if (printParse) cout << psp() << "exit <if>" << endl;
  return newIf;

}

WhileNode* While()
{

  if (printParse) cout << psp() << "enter <while>" << endl;
  ++level;

  WhileNode* newWhileNode = new WhileNode(level);

  if(nextToken == TOK_WHILE){
    nextToken = yylex();
    output("EXPRESSION");
  }

  newWhileNode->expression = expression();
  newWhileNode->statement = statement();

  --level;
  if (printParse) cout << psp() << "exit <while>" << endl;
  return newWhileNode;
}

ReadNode* Read()
{

  if (printParse) cout << psp() << "enter <read>" << endl;
  ++level;

  ReadNode* newRead = new ReadNode(level);
  
  if(nextToken == TOK_READ){
    nextToken = yylex();
  }
  
  if(nextToken == TOK_OPENPAREN){
    output("OPENPAREN");
    nextToken = yylex();
  }
  else 
    throw "4: ')' expected";

  if(nextToken == TOK_IDENT){
    output("IDENTIFIER");
    if (printParse) cout << psp() << yytext << endl;
    newRead->READ = yytext;
    nextToken = yylex();
  }
  else
    throw "2: identifier expected";

  if(nextToken == TOK_CLOSEPAREN){
    output("CLOSEPAREN");
    nextToken = yylex();
  }
  else 
    throw "9: '(' expected";
 
  --level;
  if (printParse) cout << psp() << "exit <read>" << endl;
  return newRead;
}

WriteNode* Write()
{
  
  if (printParse) cout << psp() << "enter <write>" << endl;
  ++level;

  WriteNode* newWrite = new WriteNode(level);
  
  nextToken = yylex();

  if(nextToken == TOK_OPENPAREN){
    output("OPENPAREN");
    nextToken = yylex();
  }
  else {
    throw "9: '(' expected";
  }

  if(nextToken == TOK_IDENT){
    output("WRITE");
    if (printParse) cout << psp() << yytext << endl;
    newWrite->type = nextToken;
    newWrite->content = yytext;
    nextToken = yylex();
  }
  
  if(nextToken == TOK_STRINGLIT){
    output("WRITE");
    if (printParse) cout << psp() << yytext << endl;
    newWrite->type = nextToken;
    newWrite->content = yytext;
    nextToken = yylex();
  }

  if(nextToken == TOK_CLOSEPAREN){
    output("CLOSEPAREN");
    nextToken = yylex();
  }
  else {
    throw "4: ')' expected";
  }
  --level;
  if (printParse) cout << psp() << "exit <write>" << endl;
  return newWrite;

}

ExpressionNode* expression(){
  
  if (printParse) cout << psp() << "enter <expression>" << endl;
  ++level;
  

  output("SIMPLE_EXP");
  ExpressionNode* newExprNode = nullptr;
  Simple_expressionNode* firstSimpleExpr = simple_expression();
  newExprNode = new ExpressionNode(level, firstSimpleExpr);
  
  
  if(nextToken == TOK_EQUALTO){
    output("EQUALTO");
    if (printParse) cout << psp() << yytext << endl;
    output("SIMPLE_EXP");
    
    newExprNode->simpleExprOps = nextToken;
    nextToken = yylex();
    newExprNode->secondSimpleExpr = simple_expression();
   }
  
  else if(nextToken == TOK_LESSTHAN){
    output("LESSTHAN");
    if (printParse) cout << psp() << yytext << endl;
    output("SIMPLE_EXP");
    
    newExprNode->simpleExprOps = nextToken;
    nextToken = yylex();
    newExprNode->secondSimpleExpr = simple_expression();
    
  }  
  
  else if(nextToken == TOK_GREATERTHAN){
    output("GREATERTHAN");
    if (printParse) cout << psp() << yytext << endl;
    output("SIMPLE_EXP");
    
    newExprNode->simpleExprOps = nextToken;
    nextToken = yylex();
    newExprNode->secondSimpleExpr = simple_expression();
  }
  
  else if(nextToken == TOK_NOTEQUALTO){
    output("NOTEQUALTO");
    if (printParse) cout << psp() << yytext << endl;
    output("SIMPLE_EXP");
    
    newExprNode->simpleExprOps = nextToken;
    nextToken = yylex(); 
    newExprNode->secondSimpleExpr = simple_expression();
   
  }

  --level;
  if (printParse) cout << psp() << "exit <expression>" << endl;
  return newExprNode;
}

Simple_expressionNode* simple_expression(){

  if (printParse) cout << psp() << "enter <simple_exp>" << endl;
  ++level;

  output("TERM");
  Simple_expressionNode* newSimpExprNode = nullptr;
  TermNode* firstTerm = term();
  newSimpExprNode = new Simple_expressionNode(level, firstTerm);

   
    while(nextToken == TOK_PLUS || nextToken == TOK_MINUS || nextToken == TOK_OR)
    {
     if(nextToken == TOK_PLUS){
        output("PLUS");
        if (printParse) cout << psp() << yytext << endl;
        output("TERM");
        newSimpExprNode->restTermOps.push_back(nextToken);
        nextToken = yylex();
        newSimpExprNode->restTerms.push_back(term());
      }

      else if(nextToken == TOK_MINUS){
        output("MINUS");
        if (printParse) cout << psp() << yytext << endl;
        output("TERM");
        newSimpExprNode->restTermOps.push_back(nextToken);
        nextToken = yylex();
        
        newSimpExprNode->restTerms.push_back(term());
      }

      else if(nextToken == TOK_OR){
        output("OR");
        if (printParse) cout << psp() << yytext << endl;
        output("TERM");
        newSimpExprNode->restTermOps.push_back(nextToken);
        nextToken = yylex();
        newSimpExprNode->restTerms.push_back(term());
      }
    }
  
  --level;
  if (printParse) cout << psp() << "exit <simple_exp>" << endl;
  return newSimpExprNode;
}

TermNode* term()
{

  if (printParse) cout << psp() << "enter <term> " << endl;
  ++level;

  TermNode* newTermNode = nullptr;
  FactorNode* firstFactor = factor();
  newTermNode = new TermNode(level, firstFactor);
   
  while(nextToken == TOK_MULTIPLY || nextToken == TOK_DIVIDE || nextToken == TOK_AND) 
  {
    if(nextToken == TOK_MULTIPLY)
    {
      output("MULTIPLY");
      if (printParse) cout << psp() << yytext << endl;
      newTermNode->restFactorOps.push_back(nextToken);
      
      nextToken = yylex();
      newTermNode->restFactors.push_back(factor());
    }

    if(nextToken == TOK_DIVIDE)
    {
      output("DIVIDE");
      if (printParse) cout << psp() << yytext << endl;
      newTermNode->restFactorOps.push_back(nextToken);
      
      nextToken = yylex();
      newTermNode->restFactors.push_back(factor());
    }

    if(nextToken == TOK_AND)
    {
      output("AND");
      if (printParse) cout << psp() << yytext << endl;
      newTermNode->restFactorOps.push_back(nextToken);
      nextToken = yylex();
      newTermNode->restFactors.push_back(factor());
    }
  }

  --level;
  if (printParse) cout << psp() << "exit <term>" << endl;
  return newTermNode;
}

FactorNode* factor(){ 

  if(first_of_factor)
  {
    output("FACTOR");
  } else { 
      throw "903: illegal type of factor";
  }

  if (printParse) cout << psp() << "enter <factor>" << endl;
  ++level;

    FactorNode* newFactorNode = nullptr;
   

    if(nextToken == TOK_INTLIT)
    {
      output("INTLIT");
      if (printParse) cout << psp() << yytext << endl;
      newFactorNode = new IntLitNode(level, atoi(yytext));
      nextToken = yylex();
    }  
    
    else if(nextToken == TOK_FLOATLIT)
    {
      output("FLOATLIT");
      if (printParse) cout << psp() << yytext << endl;
      newFactorNode = new FloatLitNode(level, atof(yytext));
      nextToken = yylex();
    }
  
    else if(nextToken == TOK_IDENT)
    {
      output("IDENTIFIER");
      if (printParse) cout << psp() << yytext << endl;
      if(!in_there(yytext))
        {
          throw "104: identifier not declared";
        }
        newFactorNode = new IdNode(level, string(yytext));
        nextToken = yylex();

    }

    else if(nextToken == TOK_OPENPAREN)
    {
      output("OPENPAREN");
      if (printParse) cout << psp() << yytext << endl;
      nextToken = yylex();
      output("EXPRESSION");
      newFactorNode = new NestedExprNode(level, expression());
   
        if(nextToken == TOK_CLOSEPAREN)
        {
          output("CLOSEPAREN");
          nextToken = yylex();
        }
    }

    else if(nextToken == TOK_NOT)
    {
      output("NOT");
      if (printParse) cout << psp() << yytext << endl;
      nextToken = yylex();
      newFactorNode = new NotNode(level, factor());
    }
    
    else if(nextToken == TOK_MINUS)
    {
      output("MINUS");
      if (printParse) cout << psp() << yytext << endl;
      nextToken = yylex();
      newFactorNode = new MinusNode(level, factor());
    } 
   
  --level;
  if (printParse) cout << psp() << "exit <factor>" << endl;
  return newFactorNode;
}

bool first_of_factor(void){
    return nextToken = TOK_INTLIT || nextToken == TOK_FLOATLIT || nextToken == TOK_IDENT;
    }

bool first_of_program(void) {
    return nextToken == TOK_PROGRAM;
    }

bool first_of_block(void) {
    return nextToken == TOK_VAR || nextToken == TOK_BEGIN;
    }

bool first_of_statement(void){
    return  nextToken == TOK_IDENT || nextToken == TOK_BEGIN || nextToken == TOK_IF || nextToken == TOK_WHILE || nextToken == TOK_READ || nextToken == TOK_WRITE;
    }

bool first_of_assignment(void){
    return nextToken == TOK_IDENT;
    }

bool in_there(string temp){
  symbolTableT::iterator i;
  i=symbolTable.find(temp);
  
  if(i==symbolTable.end())
    return false;
  else 
    return true; 
}

#include "CommandParser.h"

CommandParser::CommandParser(){
  
}

CommandParser::~CommandParser(){
  Command* current = firstCommand;
  Command* next;
  while(current != NULL){
    next = current->next;
    delete current;
    current = next;
  }
}

void CommandParser::parseLines(String str){
  String tmpStr = "";
  
  char curChar = ' ';
  char prevChar = ' ';
  int strLen = str.length();

  if(strLen == 0) return;
  
  for(int i=0;i<strLen;i++){
    prevChar = curChar;
    curChar = str.charAt(i);

    if(prevChar == ';' && curChar == ';'){
      parse(tmpStr.substring(0,tmpStr.length()-1));
      tmpStr = "";
    } else if((curChar == '\n' || curChar == '\r') && tmpStr.length() > 0){
      parse(tmpStr);
      tmpStr = "";
    } else{
      tmpStr += curChar;
    }
  }

  parse(tmpStr);
}

void CommandParser::parse(String str){
  Argument* argList = NULL;
  String cmdName = "";
  int argNum = 0;
  int strLen = str.length();

  bool escaped = false;
  bool inQuotes = false;
  
  char tmpChar;
  String tmpStr;
  Argument* tmpArg = NULL;

  if(strLen == 0) return;
  
  for(int i=0;i<=strLen;i++){
    if(i == strLen) tmpChar = ' ';
    else tmpChar = str.charAt(i);

    // escape character BACKSLASH
    if(tmpChar == '\\'){
      if(escaped){
        escaped = false;
      } else {
        escaped = true;
        continue;
      }
    }

    // Quotes
    else if(!escaped && tmpChar == '"'){
      inQuotes = !inQuotes;
      continue;
    }
    
    
    if(tmpStr.length() > 0 && !escaped && !inQuotes && (tmpChar == ' ' || tmpChar == '\r' || tmpChar == '\n')){

      // add command name empty
      if(cmdName.length() == 0){
        cmdName = tmpStr;
      } 

      else {
        // add argument to list
        if(tmpStr.charAt(0) == '-'){
          tmpArg = new Argument(tmpStr.substring(1),"","",false);
          tmpArg->next = argList;
          argList = tmpArg;
          argNum++;
        }
        // add value to argument
        else{
          if(tmpArg == NULL){
            if(onParseError) onParseError(tmpStr);
            return;
          }
          tmpArg->setValue(tmpStr);
          tmpArg = NULL; // prevent overwrite of arg value
        }
      }
      tmpStr = "";
    } 

    // add chars to temp-string
    else {
      tmpStr += tmpChar;
      escaped = false;
    }
    
  }
  
  Command* h = firstCommand;
  bool found = false;
  while(h != NULL && !found){
    if(h->getName() == cmdName){
      found = true;
      if(h->equals(cmdName, argNum, argList))
        h->exec(h);
      else 
        h->error();
    }
    h = h->next;
  }

  if(!found && onNotFound)
    onNotFound(cmdName);
}

void CommandParser::addCommand(Command* newCommand){
  newCommand->next = firstCommand;
  firstCommand = newCommand;
}


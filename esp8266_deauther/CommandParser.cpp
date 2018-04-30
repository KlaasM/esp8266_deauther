#include "CommandParser.h"

CommandParser::CommandParser(){
  
}

CommandParser::~CommandParser(){
  if(firstCommand) delete firstCommand;
  
  Command* currentCmd = firstCommand;
  Command* nextCmd;
  while(currentCmd != NULL){
    nextCmd = currentCmd->next;
    delete currentCmd;
    currentCmd = nextCmd;
  }
  
}

void CommandParser::parseLines(char* str){
  int h=1;
  int i=0;
  int strLen = strlen(str);

  if(strLen == 0) return;
  
  char prevChar;
  char curChar = str[i];
  
  while(++i < strLen){
    prevChar = curChar;
    curChar = str[i];
    
    if(prevChar == ';' && curChar == ';'){
      parse(&str[i-h-1], h);
      h = 0;
    } else if((curChar == '\n' || curChar == '\r') && h > 0){
      parse(&str[i-h], h);
      h = 0;
    } else{
      h++;
    }
  }
  if(h>0) parse(&str[i-h], h);
}

void CommandParser::parse(char* str, int length){
  Argument* argList = NULL;
  int argNum = 0;

  bool escaped = false;
  bool inQuotes = false;
  int h = 0;
  
  char tmpChar;
  Argument* tmpArg = NULL;

  char* cmdName = NULL;
  
  for(int i=0;i<=length;i++){
    if(i < length)
      tmpChar = str[i];
    else
      tmpChar = '\0';
      

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
        
    if(!escaped && !inQuotes && (tmpChar == ' ' || tmpChar == '\r' || tmpChar == '\n' || tmpChar == '\0')){
      if(h == 0)
        continue;
        
      // save command name
      else if(!cmdName){
        char* tmpStr = new char[h+1];
        memcpy(&tmpStr[0], &str[i-h], h);
        tmpStr[h] = '\0';
        cmdName = tmpStr;
      } 

      // add argument
      else {
        
        // add argument to list
        if(str[i-h] == '-'){
          h--;
          char* tmpStr = new char[h+1];
          memcpy(&tmpStr[0], &str[i-h], h);
          tmpStr[h] = '\0';
      
          tmpArg = new Argument(tmpStr,"","",false);
          tmpArg->next = argList;
          argList = tmpArg;
          argNum++;

          delete tmpStr;
        }
        
        // add value to argument
        else{
          char* tmpStr = new char[h+1];
          memcpy(&tmpStr[0], &str[i-h], h);
          tmpStr[h] = '\0';
          
          if(tmpArg == NULL){
            if(onParseError){
              onParseError(tmpStr);
            }
          }else{
            tmpArg->setValue(tmpStr);
            tmpArg = NULL; // prevent overwrite of arg value
          }
          
          delete tmpStr;
        }
        
      }
      
      h = 0;
    } 

    // add chars to temp-string
    else {
      h++;
      escaped = false;
    }
    
  }
  
  // go through list to find command
  Command* cmd = firstCommand;
  bool found = false;
  
  while(cmd != NULL && !found){
    if(strcmp(cmd->getName(), cmdName) == 0){
      found = true;
      if(cmd->equals(cmdName, argNum, argList))
        cmd->exec(cmd);
      else 
        cmd->error();
    }
    cmd = cmd->next;
  }
  
  if(!found && onNotFound)
    onNotFound(cmdName);
  
  delete cmdName;
  
  while(argList != NULL){
    tmpArg = argList->next;
    delete argList;
    argList = tmpArg;
  }
}

void CommandParser::addCommand(Command* newCommand){
  newCommand->next = firstCommand;
  firstCommand = newCommand;
}


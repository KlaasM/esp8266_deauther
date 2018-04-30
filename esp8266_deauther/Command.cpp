#include "Command.h"

Command::Command(String keyword, std::function<void(Command* command)> runFnct, std::function<void()> onError){
  Command::keyword = keyword;
  Command::runFnct = runFnct;
  Command::onError = onError;
}

Command::~Command(){
  Argument* current = firstArg;
  Argument* next;
  while(current != NULL){
    next = current->next;
    delete current;
    current = next;
  }
}

void Command::exec(Command* command){
  if(runFnct){
    runFnct(command);
  }
}

void Command::error(){
  if(onError)
    onError();
}

String Command::getName(){
  return keyword;
}

bool Command::equals(String keyword, int argNum, Argument* firstArg){
  if(Command::keyword != keyword) return false;
  if(argNum > Command::argNum) return false;
  
  resetArguments();

  // check and set argument values
  Argument* h = firstArg;
  Argument* tmp;
  while(h != NULL){
    tmp = getArg(h->getArgName());
    
    if(tmp){
      if(!tmp->wasGiven())
        tmp->setValue(h->getValue());
      else 
        return false; // argument twice in the list
    } else { 
      return false; // argument not found
    }
    
    h = h->next;
  }

  // check for all required arguments
  h = Command::firstArg;
  while(h != NULL){
    if(h->isRequired() && !h->wasGiven())
      return false;
    h = h->next;
  }
  
  return true;
}

bool Command::hasArg(String argName){
  return getArg(argName) != NULL;
}

Argument* Command::getArg(String argName){
  Argument* h = firstArg;
  while(h != NULL){
    if(h->getArgName() == argName || h->getAltName() == argName)
      return h;
    h = h->next;
  }

  return NULL;
}

bool Command::has(String argName){
  Argument* arg = getArg(argName);
  if(arg) 
    return arg->wasGiven();
  else 
    return false;
}

String Command::value(String argName){
  Argument* arg = getArg(argName);
  if(arg) 
    return arg->getValue();
  else 
    return "";
}

void Command::resetArguments(){
  Argument* h = firstArg;
  while(h != NULL){
    h->reset();
    h = h->next;
  }
}

void Command::addOptArg(String argName, String altName, String defaultValue){
  addArg(argName, altName, defaultValue, false);
}

void Command::addReqArg(String argName, String altName, String defaultValue){
  addArg(argName, altName, defaultValue, true);
}

void Command::addArg(String argName, String altName, String defaultValue, bool required){
  Argument* newArg = new Argument(argName, altName, defaultValue, required);
  addArg(newArg);
}

void Command::addArg(Argument* newArg){ 
  if(!hasArg(newArg->getArgName())){
    newArg->next = firstArg;
    firstArg = newArg;
    argNum++;
  }
}

#include "Command.h"

Command::Command(const char* name, std::function<void(Command* command)> runFnct, std::function<void()> onError){
  Command::name = name;
  Command::runFnct = runFnct;
  Command::onError = onError;
}

Command::~Command(){
  if(name) delete name;
  if(firstArg) delete firstArg;
  
  Argument* currentArg = firstArg;
  Argument* nextArg;
  while(currentArg != NULL){
    nextArg = currentArg->next;
    delete currentArg;
    currentArg = nextArg;
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

const char* Command::getName(){
  return name;
}

bool Command::equals(char* name, int argNum, Argument* firstArg){
  if(strcmp(Command::name, name) != 0) return false;
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

bool Command::hasArg(const char* argName){
  return getArg(argName) != NULL;
}

Argument* Command::getArg(const char* argName){
  Argument* h = firstArg;
  while(h != NULL){
    if(strcmp(h->getArgName(), argName) == 0 || strcmp(h->getAltName(), argName) == 0)
      return h;
    h = h->next;
  }

  return NULL;
}

bool Command::has(const char* argName){
  Argument* arg = getArg(argName);
  if(arg) 
    return arg->wasGiven();
  else 
    return false;
}

char* Command::value(const char* argName){
  Argument* arg = getArg(argName);
  if(arg) 
    return arg->getValue();
  else 
    return NULL;
}

void Command::resetArguments(){
  Argument* h = firstArg;
  while(h != NULL){
    h->reset();
    h = h->next;
  }
}

void Command::addOptArg(const char* argName, const char* altName, const char* defaultValue){
  addArg(argName, altName, defaultValue, false);
}

void Command::addReqArg(const char* argName, const char* altName, const char* defaultValue){
  addArg(argName, altName, defaultValue, true);
}

void Command::addArg(const char* argName, const char* altName, const char* defaultValue, bool required){
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

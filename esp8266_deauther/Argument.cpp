#include "Argument.h"

Argument::Argument(const char* argName, const char* altName, const char* defaultValue, bool required){
  char* newArgName = new char[strlen(argName)+1];
  strcpy(newArgName, argName);
  Argument::argName = newArgName;

  char* newAltName = new char[strlen(altName)+1];
  strcpy(newAltName, altName);
  Argument::altName = newAltName;

  char* newDefaultValue = new char[strlen(defaultValue)+1];
  strcpy(newDefaultValue, defaultValue);
  Argument::defaultValue = newDefaultValue;

  Argument::required = required;
  reset();
}

Argument::~Argument(){
  if(argName) delete argName;
  if(altName) delete altName;
  if(defaultValue) delete defaultValue;
  if(value) delete value;
}

const char* Argument::getArgName(){
  return argName;
}

const char* Argument::getAltName(){
  return altName;
}

char* Argument::getValue(){
  return value;
}

bool Argument::wasGiven(){
  return given;
}

bool Argument::isRequired(){
  return required;
}

void Argument::setValue(char* value){
  char* newValue = new char[strlen(value)+1];
  strcpy(newValue, value);
  if(Argument::value) delete Argument::value;
  Argument::value = newValue;
  given = true;
}

void Argument::reset(){
  setValue((char*)defaultValue);
  given = false;
}


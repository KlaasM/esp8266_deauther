#include "Argument.h"

Argument::Argument(String argName, String altName, String defaultValue, bool required){
  Argument::argName = argName;
  Argument::altName = altName;
  Argument::defaultValue = defaultValue;
  Argument::required = required;
  reset();
}

Argument::~Argument(){
  
}

String Argument::getArgName(){
  return argName;
}

String Argument::getAltName(){
  return altName;
}

String Argument::getValue(){
  return value;
}

bool Argument::wasGiven(){
  return given;
}

bool Argument::isRequired(){
  return required;
}

void Argument::setValue(String value){
  Argument::value = value;
  given = true;
}

void Argument::reset(){
  value = defaultValue;
  given = false;
}


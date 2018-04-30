#ifndef Argument_h
#define Argument_h

#include <stddef.h>
#include "Arduino.h"

class Argument {
  public:
    Argument* next = NULL;
    
    Argument(const char* argName, const char* altName, const char* defaultValue, bool required);
    ~Argument();

    const char* getArgName();
    const char* getAltName();
    char* getValue();
    bool wasGiven();
    bool isRequired();
    
    void setValue(char* value);
    
    void reset(); 
  private:
    const char* argName = NULL;
    const char* altName = NULL;
    const char* defaultValue = NULL;
    char* value = NULL;
    bool given = false;
    bool required = false;
};

#endif

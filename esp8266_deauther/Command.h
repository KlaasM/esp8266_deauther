#ifndef Command_h
#define Command_h

#include <stddef.h>
#include <functional>
#include "Arduino.h"
#include "Argument.h"

class Command {
  public:
    Command* next = NULL;
    
    Command(const char* name, std::function<void(Command* command)> runFnct, std::function<void()> onError);
    ~Command();

    const char* getName();
    
    void exec(Command* command);
    bool equals(char* name, int argNum, Argument* firstArg);
    
    void addOptArg(const char* argName, const char* altName, const char* defaultValue);
    void addReqArg(const char* argName, const char* altName, const char* defaultValue);
    void addArg(const char* argName, const char* altName, const char* defaultValue, bool required);
    void addArg(Argument* newArg);
    
    bool hasArg(const char* argName);
    Argument* getArg(const char* argName);
    
    bool has(const char* argName);
    char* value(const char* argName);
    
    void resetArguments();
    void error();
  private:
    const char* name = NULL;
    int argNum = 0;
    Argument* firstArg = NULL;
    std::function<void(Command* command)> runFnct = NULL;
    std::function<void()> onError = NULL;
};

#endif

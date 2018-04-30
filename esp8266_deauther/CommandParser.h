#ifndef CommandParser_h
#define CommandParser_h

#include <stddef.h>
#include "Arduino.h"
#include "Command.h"

class CommandParser {
  public:
    CommandParser();
    ~CommandParser();
    
    void parseLines(char* str);
    void parse(char* str, int length);
    
    void addCommand(Command* newCommand);

    std::function<void(char* cmdName)> onNotFound = NULL;
    std::function<void(char* invalidArgument)> onParseError = NULL;
  private:
    Command* firstCommand = NULL;
};

#endif

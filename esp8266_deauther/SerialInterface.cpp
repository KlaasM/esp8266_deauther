#include "SerialInterface.h"

/*
   A short introduction for the reader:
   This class is a huuuuuuuge mess of if statements!!!!!1!eleven
   But it works and I tried to make it as compact as possible.
   If someone was able to make an Arduino framework for parsing an input string in different commands with certain rules,
   required- and optional parameters of different datatypes to run different callback functions... shoot me a quick message on GitHub or Twitter: @spacehuhn!
   PS: the framework has to be good in both CPU performance and RAM usage, otherwise it's useless. Good luck!
   I already wasted way too much time trying to come up with the best way of programming this, so I will just keep it that way for now.
   Also: NEVER CHANGE A RUNNING SYSTEM!
*/

SerialInterface::SerialInterface() {
  list = new SimpleList<String>;

  cli = new CommandParser();
  
  cli->onNotFound = [](String cmdName){
   prntln("Command \""+cmdName+"\" not found :(");
  };
  
  cli->onParseError = [](String invalidArgument){
    prntln("Error parsing command at \""+invalidArgument+"\"");
  };

  // ===== HELP ===== //
  cli->addCommand(new Command("help", [this](Command* cmd){
    prntln(CLI_HELP_HEADER);

    prntln(CLI_HELP_HELP);
    prntln(CLI_HELP_SCAN);
    prntln(CLI_HELP_SHOW);
    prntln(CLI_HELP_SELECT);
    prntln(CLI_HELP_DESELECT);
    prntln(CLI_HELP_SSID_A);
    prntln(CLI_HELP_SSID_B);
    prntln(CLI_HELP_SSID_C);
    prntln(CLI_HELP_NAME_A);
    prntln(CLI_HELP_NAME_B);
    prntln(CLI_HELP_NAME_C);
    prntln(CLI_HELP_SET_NAME);
    prntln(CLI_HELP_ENABLE_RANDOM);
    prntln(CLI_HELP_DISABLE_RANDOM);
    prntln(CLI_HELP_LOAD);
    prntln(CLI_HELP_SAVE);
    prntln(CLI_HELP_REMOVE_A);
    prntln(CLI_HELP_REMOVE_B);
    prntln(CLI_HELP_ATTACK);
    prntln(CLI_HELP_ATTACK_STATUS);
    prntln(CLI_HELP_STOP);
    prntln(CLI_HELP_SYSINFO);
    prntln(CLI_HELP_CLEAR);
    prntln(CLI_HELP_FORMAT);
    prntln(CLI_HELP_PRINT);
    prntln(CLI_HELP_DELETE);
    prntln(CLI_HELP_REPLACE);
    prntln(CLI_HELP_COPY);
    prntln(CLI_HELP_RENAME);
    prntln(CLI_HELP_RUN);
    prntln(CLI_HELP_WRITE);
    prntln(CLI_HELP_GET);
    prntln(CLI_HELP_SET);
    prntln(CLI_HELP_RESET);
    prntln(CLI_HELP_CHICKEN);
    prntln(CLI_HELP_REBOOT);
    prntln(CLI_HELP_INFO);
    prntln(CLI_HELP_COMMENT);
    prntln(CLI_HELP_SEND_DEAUTH);
    prntln(CLI_HELP_SEND_BEACON);
    prntln(CLI_HELP_SEND_PROBE);
    prntln(CLI_HELP_LED_A);
    prntln(CLI_HELP_LED_B);
    prntln(CLI_HELP_LED_ENABLE);
    prntln(CLI_HELP_DRAW);
    prntln(CLI_HELP_SCREEN_ON);
    prntln(CLI_HELP_SCREEN_MODE);

    prntln(CLI_HELP_FOOTER);
  },NULL));

  // ===== SYSTEM ===== //
  // sysinfo
  cli->addCommand(new Command("sysinfo", [this](Command* cmd){
    prntln(CLI_SYSTEM_INFO);
    char s[150];
    sprintf(s,str(CLI_SYSTEM_OUTPUT).c_str(), 81920 - system_get_free_heap_size(), 100 - system_get_free_heap_size() / (81920 / 100), system_get_free_heap_size(), system_get_free_heap_size() / (81920 / 100), 81920);
    prntln(String(s));
    
    prnt(CLI_SYSTEM_CHANNEL);
    prntln(settings.getChannel());

    uint8_t mac[6];
    
    prnt(CLI_SYSTEM_AP_MAC);
    wifi_get_macaddr(SOFTAP_IF, mac);
    prntln(macToStr(mac));

    prnt(CLI_SYSTEM_ST_MAC);
    wifi_get_macaddr(STATION_IF, mac);
    prntln(macToStr(mac));

    FSInfo fs_info;
    SPIFFS.info(fs_info);
    sprintf(s,str(CLI_SYSTEM_RAM_OUT).c_str(), fs_info.usedBytes, fs_info.usedBytes / (fs_info.totalBytes / 100), fs_info.totalBytes - fs_info.usedBytes, (fs_info.totalBytes - fs_info.usedBytes) / (fs_info.totalBytes / 100), fs_info.totalBytes);
    prnt(String(s));
    sprintf(s,str(CLI_SYSTEM_SPIFFS_OUT).c_str(), fs_info.blockSize, fs_info.pageSize);
    prnt(String(s));
    prntln(CLI_FILES);
    Dir dir = SPIFFS.openDir(String(SLASH));
    while (dir.next()) {
      prnt(String(SPACE) + String(SPACE) + dir.fileName() + String(SPACE));
      File f = dir.openFile("r");
      prnt(int(f.size()));
      prntln(str(CLI_BYTES));
    }
    printWifiStatus();
    prntln(CLI_SYSTEM_FOOTER);
  }, NULL));

  // ===== REBOOT ===== //
  // reboot
  cli->addCommand(new Command("reboot",[](Command* cmd){
      ESP.reset();
  },NULL));

  // ===== CLEAR ===== //
  // clear
  cli->addCommand(new Command("clear",[this](Command* cmd){
    for (int i = 0; i < 100; i++) prnt(HASHSIGN);
    for (int i = 0; i < 60; i++) prntln();
  },NULL));
  
  // ===== FORMAT ==== //
  // format
  cli->addCommand(new Command("format",[](Command* cmd){
    prnt(CLI_FORMATTING_SPIFFS);
    SPIFFS.format();
    prntln(SETUP_OK);
  },NULL));

  // ===== RESET ===== //
  // reset
  cli->addCommand(new Command("reset",[](Command* cmd){
    settings.reset();
  },NULL));

  // ====== CHICKEN ===== //
  // chicken
  cli->addCommand(new Command("chicken",[](Command* cmd){
    prntln(CLI_CHICKEN_OUTPUT);
  },NULL));
    
  // ===== INFO ===== //
  // info
  cli->addCommand(new Command("info",[](Command* cmd){
    prntln(CLI_INFO_HEADER);
    prnt(CLI_INFO_SOFTWARE);
    prntln(settings.getVersion());
    prntln(CLI_INFO_COPYRIGHT);
    prntln(CLI_INFO_LICENSE);
    prntln(CLI_INFO_ADDON);
    prntln(CLI_INFO_HEADER);
  },NULL));

  // ===== DRAW ===== //
  // draw [-w <width>] [-h <height>]
  Command* drawCmd = new Command("draw", [this](Command* drawCmd){
    int width = toInt(drawCmd->value("width"));
    int height = toInt(drawCmd->value("height"));
    double scale = scan.getScaleFactor(height);
    
    prnt(String(DASH) + String(DASH) + String(DASH) + String(DASH) + String(VERTICALBAR)); // ----|
    for (int j = 0; j < SCAN_PACKET_LIST_SIZE; j++) {
      for (int k = 0; k < width; k++) prnt(EQUALS);
    }
    prntln(VERTICALBAR);
    
    for (int i = height; i >= 0; i--) {
      char s[200];
      if (i == height) sprintf(s,str(CLI_DRAW_OUTPUT).c_str(), scan.getMaxPacket() > (uint32_t)height ? scan.getMaxPacket() : (uint32_t)height);
      else if (i == height / 2) sprintf(s,str(CLI_DRAW_OUTPUT).c_str(), scan.getMaxPacket() > (uint32_t)height ? scan.getMaxPacket()/2 : (uint32_t)height/2);
      else if (i == 0) sprintf(s,str(CLI_DRAW_OUTPUT).c_str(), 0);
      else{
        s[0] = SPACE;
        s[1] = SPACE;
        s[2] = SPACE;
        s[3] = SPACE;
        s[4] = ENDOFLINE;
      }
      prnt(String(s));
      
      prnt(VERTICALBAR);
      for (int j = 0; j < SCAN_PACKET_LIST_SIZE; j++) {
        if (scan.getPackets(j)*scale > i) {
          for (int k = 0; k < width; k++) prnt(HASHSIGN);
        } else {
          for (int k = 0; k < width; k++) prnt(SPACE);
        }
      }
      prntln(VERTICALBAR);
    }

    prnt(String(DASH) + String(DASH) + String(DASH) + String(DASH) + String(VERTICALBAR)); // ----|
    
    for (int j = 0; j < SCAN_PACKET_LIST_SIZE; j++) {
      for (int k = 0; k < width; k++) prnt(EQUALS);
    }
    prntln(VERTICALBAR);
    
    prnt(String(SPACE) + String(SPACE) + String(SPACE) + String(SPACE) + String(VERTICALBAR));
    for (int j = 0; j < SCAN_PACKET_LIST_SIZE; j++) {
      char s[6];
      String helper = String(PERCENT)+DASH+(String)width+D;
      if(j==0) sprintf(s,helper.c_str(),SCAN_PACKET_LIST_SIZE-1);
      else if(j==SCAN_PACKET_LIST_SIZE/2) sprintf(s,helper.c_str(),SCAN_PACKET_LIST_SIZE/2);
      else if(j==SCAN_PACKET_LIST_SIZE-1) sprintf(s,helper.c_str(),0);
      else{
        int k;
        for (k = 0; k < width; k++) s[k] = SPACE;
        s[k] = ENDOFLINE;
      }
      prnt(s);
    } 
    prntln(VERTICALBAR);
  }, [](){
    prntln(String(F("Invalid format. Did you mean \" draw [-w <width>] [-h <height>]\"?")));
  });
  drawCmd->addOptArg("w","width","2");
  drawCmd->addOptArg("h","height","25");
  cli->addCommand(drawCmd);
  
  // ===== STOP ===== //
  // stop [<mode: -all, -scan, -attack, -script>]
  Command* stopCmd = new Command("stop", [this](Command* stopCmd){
    led.setMode(LED_MODE_IDLE, true);
    
    bool hasScan = stopCmd->has("scan");
    bool hasAttack = stopCmd->has("attack");
    bool hasScript = stopCmd->has("script");
    
    if(stopCmd->has("all") || (!hasScan && !hasAttack && !hasScript)){
      scan.stop();
      attack.stop();
      stopScript();
    }else{
      if(hasScan) scan.stop();
      if(hasAttack) attack.stop();
      if(hasScript) stopScript();
    }
  }, [](){
    prntln("Invalid format. Did you mean \"stop [<mode: -all, -scan, -attack, -script>]\"?");
  });
  stopCmd->addOptArg("all","","");
  stopCmd->addOptArg("scan","","");
  stopCmd->addOptArg("attack","","");
  stopCmd->addOptArg("script","","");
  cli->addCommand(stopCmd);
  
  // ===== SCAN ===== //
  // scan [<mode: -ap, -st, -all>] [-t <time>] [-c <continue-time>] [-ch <channel>]
  Command* scanCmd = new Command("scan", [this](Command* scanCmd){
    uint8_t scanMode = SCAN_MODE_ALL;
    if(scanCmd->has("stations")) scanMode = SCAN_MODE_STATIONS;
    else if(scanCmd->has("accesspoints")) scanMode = SCAN_MODE_APS;

    uint32_t time = getTime(scanCmd->value("time"));

    bool channelHop = !scanCmd->has("channel");
    uint8_t channel = channelHop ? wifi_channel : toInt(scanCmd->value("channel"));
    
    uint8_t nextmode = scanCmd->has("continue") ? scanMode : SCAN_MODE_OFF;
    uint32_t continueTime = getTime(scanCmd->value("continue"));
    
    scan.start(scanMode, time, nextmode, continueTime, channelHop, channel);
  },[](){
    prntln(String(F("Invalid format. Did you mean \"scan [<mode: -ap, -st, -all>] [-t <time>] [-c <continue-time>] [-ch <channel>]\"?")));
  });
  
  scanCmd->addOptArg("a","all","");
  scanCmd->addOptArg("st","stations","");
  scanCmd->addOptArg("ap","accesspoints","");
  scanCmd->addOptArg("t","time","15000");
  scanCmd->addOptArg("c","continue","10000");
  scanCmd->addOptArg("ch","channel","1");
  cli->addCommand(scanCmd);

  // ===== SHOW ===== //
  // show [-s] [<mode: -a, -ap, -st, -n, -ss>]
  Command* showCmd = new Command("show",[this](Command* showCmd){
    bool hasAP = showCmd->has("ap");
    bool hasSt = showCmd->has("station");
    bool hasName = showCmd->has("name");
    bool hasSSID = showCmd->has("ssid");
    bool selected = showCmd->has("selected");
    
    if(showCmd->has("all") || (!hasAP && !hasSt && !hasName && !hasSSID)){
      selected ? scan.printSelected(): scan.printAll();
      if(!selected) ssids.printAll();
    }else{
      if(hasAP) selected ? accesspoints.printSelected() : accesspoints.printAll();
      if(hasSt) selected ? stations.printSelected() : stations.printAll();
      if(hasName) selected ? names.printSelected() : names.printAll();
      if(hasSSID && !selected) ssids.printAll();
    }
  },[](){
    prntln(String(F("Invalid format. Did you mean \"scan show [-s] [<mode: -a, -ap, -st, -n, -ss>]\"?")));
  });
  showCmd->addOptArg("s", "selected", "");
  showCmd->addOptArg("a", "all", "");
  showCmd->addOptArg("ap", "accesspoint", "");
  showCmd->addOptArg("st", "station", "");
  showCmd->addOptArg("n", "name", "");
  showCmd->addOptArg("ss", "ssid", "");
  cli->addCommand(showCmd);

  // ===== SELECT ===== //
  // select [<type: -a, -ap, -st, -n>] [-i <id>]
  Command* selectCmd = new Command("select", [this](Command* selectCmd){
    bool hasAP = selectCmd->has("accesspoint");
    bool hasSt = selectCmd->has("station");
    bool hasName = selectCmd->has("name");
    bool hasId = selectCmd->has("id");
    int idValue = toInt(selectCmd->value("id"));
    
    if(selectCmd->has("all") || (!hasAP && !hasSt && !hasName)){
      scan.selectAll();
    }else{
      if(hasAP) hasId ? accesspoints.select(idValue) : accesspoints.selectAll();
      if(hasSt) hasId ? stations.select(idValue) : stations.selectAll();
      if(hasName) hasId ? names.select(idValue) : names.selectAll();
    }
  }, [](){
    prntln(String(F("Invalid format. Did you mean \"select [<type: -a, -ap, -st, -n>] [-i <id>]\"?")));
  });
  selectCmd->addOptArg("a","all","");
  selectCmd->addOptArg("ap","accesspoint","");
  selectCmd->addOptArg("st","station","");
  selectCmd->addOptArg("n","name","");
  selectCmd->addOptArg("i","id","0");
  cli->addCommand(selectCmd);

  // ===== DESELECT ===== //
  // deselect [<type: -a, -ap, -st, -n>] [<id>]
  Command* deselectCmd = new Command("deselect", [this](Command* deselectCmd){
    bool hasAP = deselectCmd->has("accesspoint");
    bool hasSt = deselectCmd->has("station");
    bool hasName = deselectCmd->has("name");
    bool hasId = deselectCmd->has("id");
    int idValue = toInt(deselectCmd->value("id"));
    
    if(deselectCmd->has("all") || (!hasAP && !hasSt && !hasName)){
      scan.deselectAll();
    }else{
      if(hasAP) hasId ? accesspoints.deselect(idValue) : accesspoints.deselectAll();
      if(hasSt) hasId ? stations.deselect(idValue) : stations.deselectAll();
      if(hasName) hasId ? names.deselect(idValue) : names.deselectAll();
    }
  }, [](){
    prntln(String(F("Invalid format. Did you mean \"deselect [<type: -a, -ap, -st, -n>] [<id>]\"?")));
  });
  deselectCmd->addOptArg("a","all","");
  deselectCmd->addOptArg("ap","accesspoint","");
  deselectCmd->addOptArg("st","station","");
  deselectCmd->addOptArg("n","name","");
  deselectCmd->addOptArg("i","id","0");
  cli->addCommand(deselectCmd);

  // ===== ADD ===== //
  // addssid -name <ssid> [-wpa2] [-cl <clones>] [-f]
  Command* addssidCmd = new Command("addssid", [this](Command* addssidCmd){
    
  },[](){
    
  });
  addssidCmd->addReqArg("n","name","");
  cli->addCommand(addssidCmd);
  
  // clonessid -s [-f]
  
  // clonessid -i <id> [-num <clones>] [-f]

  // addname -name <name> <type: -ap, -st> <id> [-s] [-f]

  // addname -name <name> [-m <mac>] [-ch <channel>] [-b <bssid>] [-s] [-f]

  
  /*
  // ===== ADD ===== //
  else if (list->size() >= 3 && eqlsCMD(0, CLI_ADD) && eqlsCMD(1, CLI_SSID)) {
    
    // add ssid -s [-f]
    if(eqlsCMD(2, CLI_SELECT)){
      bool force = eqlsCMD(3, CLI_FORCE);
      ssids.cloneSelected(force);
    }
    
    // add ssid <ssid> [-wpa2] [-cl <clones>] [-f]
    // add ssid -ap <id> [-cl <clones>] [-f]
    else{
      String ssid = list->get(2);
      bool wpa2 = false;
      bool force = false;
      int clones = 1;
      int i = 3;
  
      if (eqlsCMD(2, CLI_AP)) {
        ssid = accesspoints.getSSID(list->get(3).toInt());
        wpa2 = accesspoints.getEncStr(list->get(3).toInt()) != " - ";
        i = 4;
      }
  
      while (i < list->size()) {
        if (eqlsCMD(i, CLI_WPA2)) wpa2 = true;
        else if (eqlsCMD(i, CLI_FORCE)) force = true;
        else if (eqlsCMD(i, CLI_CLONES)) {
          clones = list->get(i + 1).toInt();
          i++;
        } else parameterError(list->get(i));
        i++;
      }
  
      ssids.add(ssid, wpa2, clones, force);
    }
  }

  // add name <name> [-ap <id>] [-s] [-f]
  // add name <name> [-st <id>] [-s] [-f]
  // add name <name> [-m <mac>] [-ch <channel>] [-b <bssid>] [-s] [-f]
  else if (list->size() >= 3 && eqlsCMD(0, CLI_ADD) && eqlsCMD(1, CLI_NAME)) {
    String name = list->get(2);
    String mac;
    uint8_t channel = wifi_channel;
    String bssid;
    bool selected = false;
    bool force = false;

    for (int i = 3; i < list->size(); i++) {
      if (eqlsCMD(i, CLI_MAC)) mac = list->get(i + 1);
      else if (eqlsCMD(i, CLI_AP)) mac = accesspoints.getMacStr(list->get(i + 1).toInt());
      else if (eqlsCMD(i, CLI_STATION)) {
        mac = stations.getMacStr(list->get(i + 1).toInt());
        bssid = stations.getAPMacStr(list->get(i + 1).toInt());
      }
      else if (eqlsCMD(i, CLI_CHANNEL)) channel = (uint8_t)list->get(i + 1).toInt();
      else if (eqlsCMD(i, CLI_BSSID)) bssid = list->get(i + 1);
      else if (eqlsCMD(i, CLI_SELECT)){
        selected = true;
        i--;
      } else if (eqlsCMD(i, CLI_FORCE)){
        force = true;
        i--;
      } else {
        parameterError(list->get(i));
        i--;
      }
      i++;
    }

    if (name.length() == 0) prntln(CLI_ERROR_NAME_LEN);
    else if (mac.length() == 0) prntln(CLI_ERROR_MAC_LEN);
    else names.add(mac, name, bssid, channel, selected, force);
  }
  */
}

void SerialInterface::load() {
  checkFile(execPath, String());
  executing = true;
}

void SerialInterface::load(String filepath) {
  execPath = filepath;
  load();
}

void SerialInterface::enable() {
  enabled = true;
  prntln(CLI_SERIAL_ENABLED);
}

void SerialInterface::disable() {
  enabled = true;
  prntln(CLI_SERIAL_DISABLED);
}

void SerialInterface::error(String message) {
  prnt(CLI_ERROR);
  prntln(message);
}

void SerialInterface::parameterError(String parameter) {
  prnt(CLI_ERROR_PARAMETER);
  prnt(parameter);
  prntln(DOUBLEQUOTES);
}

bool SerialInterface::isInt(String str) {
  if (eqls(str,STR_TRUE) || eqls(str,STR_FALSE))
    return true;
  for (uint32_t i = 0; i < str.length(); i++)
    if (!isDigit(str.charAt(i))) return false;
  return true;
}

int SerialInterface::toInt(String str) {
  if (eqls(str,STR_TRUE))
    return 1;
  else if (eqls(str,STR_FALSE))
    return 0;
  else
    return str.toInt();
}

uint32_t SerialInterface::getTime(String time) {
  int value = time.toInt();
  if (value < 0) value = -value;
  if (time.substring(time.length() - 1).equalsIgnoreCase(String(S)))
    value *= 1000;
  else if (time.substring(time.length() - 3).equalsIgnoreCase(str(STR_MIN)) || time.charAt(time.length() - 1) == M)
    value *= 60000;
  return value;
}

bool SerialInterface::eqlsCMD(int i, const char* keyword) {
  return eqls(list->get(i).c_str(), keyword);
}

void SerialInterface::stopScript() {
  if (continuously) {
    continuously = false;
    prnt(CLI_STOPPED_SCRIPT);
    prnt(execPath);
    prntln(DOUBLEQUOTES);
    prnt(SPACE);
    prnt(CLI_CONTINUOUSLY);
  }
}

void SerialInterface::update() {
  if (executing) {
    if (execPath.charAt(0) != SLASH) 
      execPath = SLASH + execPath;
    prnt(CLI_EXECUTING);
    prntln(execPath);
    File f = SPIFFS.open(execPath, "r");
    if (f.size() > 0) {
      String line;
      char tmp;
      while (f.available()) {
        tmp = f.read();
        if (tmp == NEWLINE) {
          runCommands(line);
          line = String();
        } else {
          line += tmp;
        }
      }
      runCommand(line);
    }

    f.close();
    executing = false;
    
    if(continuously) prntln(CLI_SCRIPT_DONE_CONTINUE);
    else prntln(CLI_SCRIPT_DONE);
    
    loopTime = currentTime;
  } else {
    if (Serial.available()){
      String input = Serial.readStringUntil('\n');
      if(settings.getSerialEcho()){
        prnt(HASHSIGN);
        prnt(SPACE);
        prntln(input);
      }
      cli->parseLines(input);
    }
    /*
    if (enabled && Serial.available() > 0)
      runCommands(Serial.readStringUntil(NEWLINE));*/
    if (continuously) {
      if (currentTime - loopTime > continueTime)
        executing = true;
    }
  }
}

void SerialInterface::runCommands(String input) {
  String tmp;

  for (uint32_t i = 0; i < input.length(); i++) {
    // when 2 semicolons in a row without a backslash escaping the first
    if (input.charAt(i) == SEMICOLON && input.charAt(i + 1) == SEMICOLON && input.charAt(i - 1) != BACKSLASH) {
      runCommand(tmp);
      tmp = String();
      i++;
    } else {
      tmp += input.charAt(i);
    }
  }

  tmp.replace(BACKSLASH + SEMICOLON + SEMICOLON, SEMICOLON + SEMICOLON);
  if (tmp.length() > 0) runCommand(tmp);
}

void SerialInterface::runCommand(String input) {
  input.replace(String(NEWLINE), String());
  input.replace(String(CARRIAGERETURN), String());
  
  list->clear();

  // parse/split input in list
  String tmp;
  bool withinQuotes = false;
  bool escaped = false;
  char c;
  for (uint32_t i = 0; i < input.length() && i < 512; i++) {
    c = input.charAt(i);

    // when char is an unescaped 
    if(!escaped && c == BACKSLASH){
      escaped = true;
    }
    
    // (when char is a unescaped space AND it's not within quotes) OR char is \r or \n
    else if ((c == SPACE && !escaped && !withinQuotes) || c == CARRIAGERETURN || c == NEWLINE) {
      // when tmp string isn't empty, add it to the list
      if (tmp.length() > 0){
        list->add(tmp);
         tmp = String(); // reset tmp string
      }
    } 
    
    // when char is an unescaped "
    else if(c == DOUBLEQUOTES && !escaped){
      // update wheter or not the following chars are within quotes or not
      withinQuotes = !withinQuotes;
      if(tmp.length() == 0 && !withinQuotes) tmp += SPACE; // when exiting quotes and tmp string is empty, add a space
    }
    
    // add character to tmp string
    else {
      tmp += c;
      escaped = false;
    }
  }

  // add string if something is left from the loop above
  if (tmp.length() > 0) list->add(tmp);

  // stop when input is empty/invalid
  if (list->size() == 0) return;

  // print comments
  if (list->get(0) == str(CLI_COMMENT)) {
    prntln(input);
    return;
  }

  if(settings.getSerialEcho()){
    // print command
    prnt(CLI_INPUT_PREFIX);
    prntln(input);
  }
  
  if (list->size() == 0) return;

  // ===== SET NAME ==== //
  // set name <id> <newname>
  else if (list->size() == 4 && eqlsCMD(0, CLI_SET) && eqlsCMD(1, CLI_NAME)) {
    names.setName(list->get(2).toInt(), list->get(3));
  }

  // ===== REPLACE ===== //
  // replace name <id> [-n <name>} [-m <mac>] [-ch <channel>] [-b <bssid>] [-s]
  else if (list->size() >= 4 && eqlsCMD(0, CLI_REPLACE) && eqlsCMD(1, CLI_NAME)) {
    int id = list->get(2).toInt();
    String name = names.getName(id);
    String mac = names.getMacStr(id);
    uint8_t channel = names.getCh(id);
    String bssid = names.getBssidStr(id);
    bool selected = names.getSelected(id);

    for (int i = 3; i < list->size(); i++) {
      if (eqlsCMD(i, CLI_NAME)) name = list->get(i + 1);
      else if (eqlsCMD(i, CLI_MAC)) mac = list->get(i + 1);
      else if (eqlsCMD(i, CLI_CHANNEL)) channel = (uint8_t)list->get(i + 1).toInt();
      else if (eqlsCMD(i, CLI_BSSID)) bssid = list->get(i + 1);
      else if (eqlsCMD(i, CLI_SELECT)){
        selected = true;
        i--;
      } else {
        parameterError(list->get(i));
        i--;
      }
      i++;
    }

    names.replace(id,mac, name, bssid, channel, selected);
  }
  
  // replace ssid <id> [-n <name>} [-wpa2]
  else if (list->size() >= 3 && eqlsCMD(0, CLI_REPLACE) && eqlsCMD(1, CLI_SSID)) {
    int id = list->get(2).toInt();
    String name = ssids.getName(id);
    bool wpa2 = false;

    for (int i = 3; i < list->size(); i++) {
      if (eqlsCMD(i, CLI_NAME)){
        name = list->get(i + 1);
        i++;
      }else if(eqlsCMD(i, CLI_WPA2)){
        wpa2 = true;
      }
    }

    ssids.replace(id,name, wpa2);
  }

  // ===== REMOVE ===== //
  // remove <type> [-a]
  // remove <type> <id>
  else if (list->size() >= 2 && eqlsCMD(0, CLI_REMOVE)) {
    if (list->size() == 2 || (eqlsCMD(2, CLI_ALL))) {
      if (eqlsCMD(1, CLI_SSID)) ssids.removeAll();
      else if (eqlsCMD(1, CLI_NAME)) names.removeAll();
      else if (eqlsCMD(1, CLI_AP)) accesspoints.removeAll();
      else if (eqlsCMD(1, CLI_STATION)) stations.removeAll();
      else parameterError(list->get(1));
    } else {
      if (eqlsCMD(1, CLI_SSID)) ssids.remove(list->get(2).toInt());
      else if (eqlsCMD(1, CLI_NAME)) names.remove(list->get(2).toInt());
      else if (eqlsCMD(1, CLI_AP)) accesspoints.remove(list->get(2).toInt());
      else if (eqlsCMD(1, CLI_STATION)) stations.remove(list->get(2).toInt());
      else parameterError(list->get(1));
    }
  }

  // ===== RANDOM ===== //
  // enable random <interval>
  else if (eqlsCMD(0, CLI_ENABLE) && eqlsCMD(1, CLI_RANDOM) && list->size() == 3) {
    ssids.enableRandom(getTime(list->get(2)));
  }

  // disable random
  else if (eqlsCMD(0, CLI_DISABLE) && eqlsCMD(1, CLI_RANDOM)) {
    ssids.disableRandom();
  }

  // ====== RICE ===== //
  // => have you tried putting it into a bowl rice?
  // funny command to mess with people, please don't share the info thanks <3
  else if (eqlsCMD(0, CLI_RICE)) {
    prntln(CLI_RICE_START);
    uint32_t i = 0;
    uint8_t multi = 1;
    uint8_t end = random(80, 99);
    if (list->get(1).toInt() > 1) multi = list->get(1).toInt();
    while (true) {
      if (i % 10 == 0 && i > 0) {
        char s[100];
        sprintf(s,str(CLI_RICE_OUTPUT).c_str(), i / 10);
        prnt(String(s));
        if ((i / 10) == end) {
          prnt(CLI_RICE_ERROR);
          prnt(String(random(16, 255), HEX));
          prnt(String(random(16, 255), HEX));
          prnt(String(random(16, 255), HEX));
          prntln(String(random(16, 255), HEX));
          for (int i = 1; i <= 32; i++) {
            for (int i = 0; i < 2; i++) {
              for (int i = 1; i <= 8; i++) {
                prnt(String(random(16, 255), HEX));
                prnt(SPACE);
              }
              prnt(SPACE);
            }
            prntln();
          }
          ESP.reset();
        } else if ((i / 10) % 10 == 0) {
          prnt(CLI_RICE_MEM);
          prnt(String(random(16, 255), HEX));
          prnt(String(random(16, 255), HEX));
          prnt(String(random(16, 255), HEX));
          prntln(String(random(16, 255), HEX));
        }
      }
      prnt(POINT);
      delay(100 * multi * multi);
      i++;
    }
  }

  // ===== LOAD/SAVE ===== //
  // save [<type>] [<file>]
  // load [<type>] [<file>]
  else if ((eqlsCMD(0, CLI_LOAD) || eqlsCMD(0, CLI_SAVE)) && list->size() >= 1 && list->size() <= 3) {
    bool load = eqlsCMD(0, CLI_LOAD);

    if (list->size() == 1 || eqlsCMD(1, CLI_ALL)) {
      load ? ssids.load() : ssids.save(false);
      load ? names.load() : names.save(false);
      load ? settings.load() : settings.save(false);
      if(!load) scan.save(false);
      return;
    }

    if (list->size() == 3) { // Todo: check if -f or filename
      if (eqlsCMD(1, CLI_SSID)) load ? ssids.load(list->get(2)) : ssids.save(true, list->get(2));
      else if (eqlsCMD(1, CLI_NAME)) load ? names.load(list->get(2)) : names.save(true, list->get(2));
      else if (eqlsCMD(1, CLI_SETTING)) load ? settings.load(list->get(2)) : settings.save(true, list->get(2));
      else parameterError(list->get(1));
    } else {
      if (eqlsCMD(1, CLI_SSID)) load ? ssids.load() : ssids.save(false);
      else if (eqlsCMD(1, CLI_NAME)) load ? names.load() : names.save(false);
      else if (eqlsCMD(1, CLI_SETTING)) load ? settings.load() : settings.save(false);
      else if ((eqlsCMD(1, CLI_SCAN) || eqlsCMD(1, CLI_AP) || eqlsCMD(1, CLI_STATION)) && !load) scan.save(false);
      else parameterError(list->get(1));
    }
  }

  // ===== ATTACK ===== //
  // attack [-b] [-d] [-da] [p] [-t <timeout>]
  // attack status [<on/off>]
  else if (eqlsCMD(0, CLI_ATTACK)) {
    if (eqlsCMD(1, CLI_STATUS)) {
      if (list->size() == 2) {
        attack.status();
      } else {
        if (eqlsCMD(2, CLI_ON)) attack.enableOutput();
        else if (eqlsCMD(2, CLI_OFF)) attack.disableOutput();
        else parameterError(list->get(2));
      }
      return;
    }

    bool beacon = false;
    bool deauth = false;
    bool deauthAll = false;
    bool probe = false;
    bool output = true;
    uint32_t timeout = settings.getAttackTimeout() * 1000;

    for (int i = 1; i < list->size(); i++) {
      if (eqlsCMD(i, CLI_BEACON)) beacon = true;
      else if (eqlsCMD(i, CLI_DEAUTH)) deauth = true;
      else if (eqlsCMD(i, CLI_DEAUTHALL)) deauthAll = true;
      else if (eqlsCMD(i, CLI_PROBE)) probe = true;
      else if (eqlsCMD(i, CLI_NOOUTPUT)) output = false;
      else if (eqlsCMD(i, CLI_TIMEOUT)) {
        timeout = getTime(list->get(i + 1));
        i++;
      }
      else parameterError(list->get(i));
    }

    attack.start(beacon, deauth, deauthAll, probe, output, timeout);
  }

  // ===== GET/SET ===== //
  // get <setting>
  else if (eqlsCMD(0, CLI_GET) && list->size() == 2) {
    prntln(settings.get(list->get(1).c_str()));
  }

  // set <setting> <value>
  else if (eqlsCMD(0, CLI_SET) && list->size() == 3) {
    settings.set(list->get(1).c_str(), list->get(2));
  }
  
  // ===== DELETE ==== //
  // delete <file> [<lineFrom>] [<lineTo>]
  else if (list->size() >= 2 && eqlsCMD(0, CLI_DELETE)) {
    if (list->size() == 2) {
      // remove whole file
      if (removeFile(list->get(1))) {
        prnt(CLI_REMOVED);
        prntln(list->get(1));
      } else {
        prnt(CLI_ERROR_REMOVING);
        prntln(list->get(1));
      }
    } else {
      // remove certain lines
      int beginLine = list->get(2).toInt();
      int endLine = list->size() == 4 ? list->get(3).toInt() : beginLine;
      if (removeLines(list->get(1), beginLine, endLine)) {
        prnt(CLI_REMOVING_LINES);
        prnt(beginLine);
        prnt(String(SPACE) + String(DASH) + String(SPACE));
        prnt(endLine);
        prntln(String(SPACE) + list->get(1));
      } else {
        prnt(CLI_ERROR_REMOVING);
        prntln(list->get(1));
      }
    }
  }

  // ===== COPY ==== //
  // delete <file> <newfile>
  else if (list->size() == 3 && eqlsCMD(0, CLI_COPY)) {
    if(copyFile(list->get(1), list->get(2))){
      prntln(CLI_COPIED_FILES);
    }else{
      prntln(CLI_ERROR_COPYING);
    }
  }

  // ===== RENAME ==== //
  // delete <file> <newfile>
  else if (list->size() == 3 && eqlsCMD(0, CLI_RENAME)) {
    if(renameFile(list->get(1), list->get(2))){
      prntln(CLI_RENAMED_FILE);
    }else{
      prntln(CLI_ERROR_RENAMING_FILE);
    }
  }

  // ===== WRITE ==== //
  // write <file> <commands>
  else if (list->size() >= 3 && eqlsCMD(0, CLI_WRITE)) {
    String path = list->get(1);
    String buf = String();
    
    int listSize = list->size();
    for (int i = 2; i < listSize; i++) {
      buf += list->get(i);
      if (i < listSize - 1) buf += SPACE;
    }
    
    prnt(CLI_WRITTEN);
    prnt(buf);
    prnt(CLI_TO);
    prntln(list->get(1));
    
    buf += NEWLINE;
    appendFile(path, buf);
  }

  // ===== REPLACE ==== //
  // replace <file> <line> <new-content>
  else if (list->size() >= 4 && eqlsCMD(0, CLI_REPLACE)) {
    int line = list->get(2).toInt();
    String tmp = String();
    for (int i = 3; i < list->size(); i++) {
      tmp += list->get(i);
      if (i < list->size() - 1) tmp += SPACE;
    }
    if (replaceLine(list->get(1), line, tmp)) {
      prnt(CLI_REPLACED_LINE);
      prnt(line);
      prnt(CLI_WITH);
      prntln(list->get(1));
    } else {
      prnt(CLI_ERROR_REPLACING_LINE);
      prntln(list->get(1));
    }
  }

  // ===== RUN ==== //
  // run <file> [continue <num>]
  else if (!executing && list->size() >= 2 && eqlsCMD(0, CLI_RUN)) {
    executing = true;
    execPath = list->get(1);
    if (eqlsCMD(2, CLI_CONTINUE)) {
      continuously = true;
      continueTime = 10000;
      if(list->size() == 3){
        continueTime = getTime(list->get(3));
        if(continueTime < 1000) continueTime = 1000;
      }
    }
  }
  
  // ===== PRINT ==== //
  // print <file> [<lines>]
  else if (list->size() >= 2 && eqlsCMD(0, CLI_PRINT)) {
    readFileToSerial(list->get(1), eqlsCMD(2, CLI_LINE));
    prntln();
  }

  // ===== SEND ===== //
  // send deauth <apMac> <stMac> <rason> <channel>
  else if (eqlsCMD(0, CLI_SEND) && list->size() == 6 && eqlsCMD(1, CLI_DEAUTH)) {
    uint8_t apMac[6];
    uint8_t stMac[6];
    strToMac(list->get(2), apMac);
    strToMac(list->get(3), stMac);
    uint8_t reason = list->get(4).toInt();
    uint8_t channel = list->get(5).toInt();
    prnt(CLI_DEAUTHING);
    prnt(macToStr(apMac));
    prnt(CLI_ARROW);
    prntln(macToStr(stMac));
    attack.deauthDevice(apMac, stMac, reason, channel);
  }

  // send beacon <mac> <ssid> <ch> [wpa2]
  else if (eqlsCMD(0, CLI_SEND) && list->size() >= 5 && eqlsCMD(1, CLI_BEACON)) {
    uint8_t mac[6];
    strToMac(list->get(2), mac);
    uint8_t channel = list->get(4).toInt();
    String ssid = list->get(3);
    for (int i = ssid.length(); i < 32; i++)
      ssid += SPACE;
    prnt(CLI_SENDING_BEACON);
    prnt(list->get(3));
    prntln(DOUBLEQUOTES);
    attack.sendBeacon(mac, ssid.c_str(), channel, eqlsCMD(5, CLI_WPA2));
  }

  // send probe <mac> <ssid> <ch>
  else if (eqlsCMD(0, CLI_SEND) && list->size() == 5 && eqlsCMD(1, CLI_PROBE)) {
    uint8_t mac[6];
    strToMac(list->get(2), mac);
    uint8_t channel = list->get(4).toInt();
    String ssid = list->get(3);
    for (int i = ssid.length(); i < 32; i++)
      ssid += SPACE;
    prnt(CLI_SENDING_PROBE);
    prnt(list->get(3));
    prntln(DOUBLEQUOTES);
    attack.sendProbe(mac, ssid.c_str(), channel);
  }

  // send custom <packet>
  else if (eqlsCMD(0, CLI_SEND) && eqlsCMD(1, CLI_CUSTOM)) {
    String packetStr = list->get(2);
    packetStr.replace(String(DOUBLEQUOTES), String());
    uint16_t counter = 0;
    uint16_t packetSize = packetStr.length() / 2;
    uint8_t packet[packetSize];

    for (int i = 0; i < packetSize; i++)
      packet[i] = strtoul((packetStr.substring(i * 2, i * 2 + 2)).c_str(), NULL, 16);

    if (attack.sendPacket(packet, packetSize, wifi_channel, 10)){
      prntln(CLI_CUSTOM_SENT);
      counter++;
    } else{
      prntln(CLI_CUSTOM_FAILED);
    }
  }

  // ===== LED ===== //
  // led <r> <g> <b> [<brightness>]
  else if (list->size() >= 4 && list->size() <= 5 && eqlsCMD(0, CLI_LED)) {
    if (list->size() == 4)
      led.setColor(list->get(1).toInt(), list->get(2).toInt(), list->get(3).toInt());
    else
      led.setColor(list->get(1).toInt(), list->get(2).toInt(), list->get(3).toInt(), list->get(4).toInt());
  }

  // led <#rrggbb> [<brightness>]
  else if (list->size() >= 2 && list->size() <= 3 && eqlsCMD(0, CLI_LED) && list->get(1).charAt(0) == HASHSIGN) {
    uint8_t c[3];
    strToColor(list->get(1), c);
    if (list->size() == 2)
      led.setColor(c[0], c[1], c[2]);
    else
      led.setColor(c[0], c[1], c[2], list->get(2).toInt());
  }

  // led <enable/disable>
  else if (list->size() == 2 && eqlsCMD(0, CLI_LED)) {
    if (eqlsCMD(1, CLI_ENABLE))
      led.tempEnable();
    else if (eqlsCMD(1, CLI_DISABLE))
      led.tempDisable();
    else
      parameterError(list->get(1));

  }

  // ===== DELAY ===== //
  else if (list->size() == 2 && eqlsCMD(0, CLI_DELAY)) {
    uint32_t endTime = currentTime + getTime(list->get(1));
    while (currentTime < endTime) {
      // ------- loop function ----- //
      currentTime = millis();

      wifiUpdate(); // manage access point
      scan.update(); // run scan
      attack.update(); // run attacks
      ssids.update(); // run random mode, if enabled
      led.update(); // update LED color

      // auto-save
      if (settings.getAutosave() && currentTime - autosaveTime > settings.getAutosaveTime()) {
        autosaveTime = currentTime;
        names.save(false);
        ssids.save(false);
        settings.save(false);
      }
      // ------- loop function end ----- //
      yield();
    }
  }

  // ===== START/STOP AP ===== //
  // startap [-p <path][-s <ssid>] [-pswd <password>] [-ch <channel>] [-h] [-cp]
  else if (eqlsCMD(0, CLI_STARTAP)) {
    String path = String(F("/web"));
    String ssid = settings.getSSID();
    String password = settings.getPassword();
    int ch = wifi_channel;
    bool hidden = settings.getHidden();
    bool captivePortal = settings.getCaptivePortal();;

    for (int i = 1; i < list->size(); i++) {
      if (eqlsCMD(i, CLI_PATH)) {
        i++;
        path = list->get(i);
      } else if (eqlsCMD(i, CLI_SSID)) {
        i++;
        ssid = list->get(i);
      } else if (eqlsCMD(i, CLI_PASSWORD)) {
        i++;
        password = list->get(i);
      } else if (eqlsCMD(i, CLI_CHANNEL)) {
        i++;
        ch = list->get(i).toInt();
      } else if (eqlsCMD(i, CLI_HIDDEN)) {
        hidden = true;
      } else if (eqlsCMD(i, CLI_CAPTIVEPORTAL)) {
        captivePortal = true;
      } else {
        parameterError(list->get(1));
      }
    }

    startAP(path, ssid, password, ch, hidden, captivePortal);
  }

  //stopap
  else if (eqlsCMD(0, CLI_STOPAP)) {
    stopAP();
  }

  // ===== SCREEN ===== //
  // screen mode <menu/packetmonitor/buttontest/loading>
  else if(eqlsCMD(0, CLI_SCREEN) && eqlsCMD(1,CLI_MODE)){
    if(eqlsCMD(2,CLI_MODE_BUTTONTEST)) displayUI.mode = SCREEN_MODE_BUTTON_TEST;
    else if(eqlsCMD(2,CLI_MODE_PACKETMONITOR)) displayUI.mode = SCREEN_MODE_PACKETMONITOR;
    else if(eqlsCMD(2,CLI_MODE_LOADINGSCREEN)) displayUI.mode = SCREEN_MODE_LOADSCAN;
    else if(eqlsCMD(2,CLI_MODE_MENU)) displayUI.mode = SCREEN_MODE_MENU;
    else parameterError(list->get(2));
    prntln(CLI_CHANGED_SCREEN);
  }

  // screen <on/off>
  else if(eqlsCMD(0, CLI_SCREEN) && (eqlsCMD(1,CLI_ON) || eqlsCMD(1,CLI_OFF))){
    if(eqlsCMD(1,CLI_ON)){
      displayUI.on();
    } else if(eqlsCMD(1,CLI_OFF)){
      displayUI.off();
    }
  }
}



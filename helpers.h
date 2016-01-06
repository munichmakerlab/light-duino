#ifndef helpers_h
#define helpers_h

/*
 *  light-duino v2
 *  MQTT <-> DMX controller with hw switches, based on ESP8266 
 *  See attached Readme.md for details
 *  
 *  This is based on the work of Jorgen (aka Juergen Skrotzky, JorgenVikingGod@gmail.com), buy him a beer. ;-) 
 *  Rest if not noted otherwise by Peter Froehlich, tarwin@tarwin.de - Munich Maker Lab e.V. (January 2016)
 *  
 *  Published under Creative Commons Attribution-ShareAlike 4.0 International (CC BY-SA 4.0)
 *  You'll find a copy of the licence text in this repo. 
 */

template <typename Generic>
void DEBUG_PRINT(Generic text) {
  if(_debug) {
    Serial.print(text);    
  }
}

template <typename Generic>
void DEBUG_PRINTLN(Generic text) {
  if(_debug) {
    Serial.println(text);    
  }
}


void byteToHexString(String &dataString, byte *uidBuffer, byte bufferSize, String strSeperator=":") {
  dataString = "";
  for (byte i = 0; i < bufferSize; i++) {
    if (i>0) {
      dataString += strSeperator;
      if (uidBuffer[i] < 0x10)
        dataString += String("0");
    }
    dataString += String(uidBuffer[i], HEX);
  }
  dataString.toUpperCase();
}


// rest unused

void assign(char *dest, String str, int len)
{
  char chStr[len+1];
  str.toCharArray(chStr, len+1);
  strcpy(dest, chStr);
}

/*
 * https://github.com/BenTommyE/Arduino_getStringPartByNr
 */
String getStringPartByNr(String data, char separator, int index)
{
  // spliting a string and return the part nr index
  // split by separator
  int stringData = 0;        //variable to count data part nr 
  String dataPart = "";      //variable to hole the return text
  for(int i = 0; i<data.length(); i++) {    //Walk through the text one letter at a time
    if(data[i]==separator) {
      //Count the number of times separator character appears in the text
      stringData++;
    }else if(stringData==index) {
      //get the text when separator is the rignt one
      dataPart.concat(data[i]);
    }else if(stringData>index) {
      //return text and stop if the next separator appears - to save CPU-time
      return dataPart;
      break;
    }
  }
  //return text if this is the last part
  return dataPart;
}

/*
 * https://coderwall.com/p/zfmwsg/arduino-string-to-char
 */
char* string2char(String command){
  if(command.length()!=0){
    char *p = const_cast<char*>(command.c_str());
    return p;
  }
}

int countCharacters(const String &str, char character) {
  int count = 0;
  for(int i = 0; i<str.length()-1; i++) {    //Walk through the text one letter at a time
    if(str[i] == character)
      count++;
  }
  return count;
}


#endif //helpers_h

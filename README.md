# light-duino
Our DMX light controller w. API 

### Ext. libraries

* DmxSimple

  When you get the error 'error: wiring.h: No such file or directory' you'll need to patch the file DmxSimple.cpp: 
  * replace #include "wiring.h" with #include "Arduino.h"

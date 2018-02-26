#ifndef DEBUGUTILS_H
#define DEBUGUTILS_H

#ifdef SERIAL_DEBUG
  #define DEBUGPRINT(...) Serial.print(__VA_ARGS__)
  #define DEBUGPRINTLN(...) Serial.println(__VA_ARGS__)
#elif defined SERIAL_DEBUG_V2
  #define DEBUGPRINT(...)                  \
        Serial.print(millis());             \
        Serial.print(": ");                 \
        Serial.print(__PRETTY_FUNCTION__);  \
        Serial.print(' ');                  \
        Serial.print(__LINE__);             \
        Serial.print(' ');                  \
        Serial.print(__VA_ARGS__)
  #define DEBUGPRINTLN(...)                \
        Serial.print(millis());             \
        Serial.print(": ");                 \
        Serial.print(__PRETTY_FUNCTION__);  \
        Serial.print(' ');                  \
        Serial.print(__LINE__);             \
        Serial.print(' ');                  \
        Serial.println(__VA_ARGS__)
#else
  #define DEBUGPRINT(...)
  #define DEBUGPRINTLN(...)
#endif

#endif

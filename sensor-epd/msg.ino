
const int msgHeaderLen = 14;

void msg(const String message) {  
    if (!configLogSerial) return;
    
    Serial.print(message);
}

void msg(const Printable& message) {  
    if (!configLogSerial) return;
    
    Serial.print(message);
}

void msg(const char * header, const String message) {    
    if (!configLogSerial) return;

    Serial.printf("[%s] ", header);
    for (int i = 0; i < msgHeaderLen-strlen(header); i++) {
        Serial.print(" ");
    }
    Serial.print(message);
}

void msgln(const String message) {    
    if (!configLogSerial) return;

    msg(message);
    Serial.println();
}

void msgln(const Printable& message) {  
    if (!configLogSerial) return;
    
    Serial.println(message);
}

void msgln(const char * header, const String message) {    
    if (!configLogSerial) return;

    msg(header, message);
    Serial.println();
}

void msgSetup() {
    Serial.begin(115200);

    while (!Serial);
    delay(500);
    Serial.println("                             "); // print some spaces to let the Serial Monitor catch up  
}

void msgLoop() {
    
}

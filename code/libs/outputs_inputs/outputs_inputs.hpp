

void writeOUTPUT(unsigned char pin, char port, bool value); // writeOUTPUT(uint8_t pin, char port, bool value)
void toggleOUTPUT(unsigned char pin, char port); // toggleOUTPUT(uint8_t pin, char port)
void writePWM(unsigned char pin, char port, unsigned char vrednost);
void pwmOFF(unsigned char pin, char port);
unsigned short readANALOG(unsigned char channel);
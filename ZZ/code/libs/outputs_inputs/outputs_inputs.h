

void writeOUTPUT(unsigned char pin, char port, bool value); // writeOUTPUT(uint8_t pin, char port, bool value)
void toggleOUTPUT(unsigned char pin, char port); // toggleOUTPUT(uint8_t pin, char port)
void writePWM(uint8_t pin, char port, uint8_t vrednost);
void pwmOFF(uint8_t pin, char port);
unsigned short readANALOG(uint8_t channel);
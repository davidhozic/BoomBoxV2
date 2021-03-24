



struct EEPROM_t
{
	void pisi(uint8_t podatek, uint16_t naslov);
	uint8_t beri (uint16_t naslov);
};

extern EEPROM_t EEPROM;


#pragma once


class EEPROM_t
{
public:
	void pisi(uint8_t podatek, uint16_t naslov);
	uint8_t beri (uint16_t naslov);
};

extern EEPROM_t EEPROM;
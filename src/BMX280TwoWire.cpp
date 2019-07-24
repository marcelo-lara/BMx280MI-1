//Multi interface Bosch Sensortec BMP280 / BME280 pressure sensor library 
// Copyright (c) 2018-2019 Gregor Christandl <christandlg@yahoo.com>
// home: https://bitbucket.org/christandlg/bmp280mi
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.

// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

#include "BMX280TwoWire.h"

BMX280TwoWire::BMX280TwoWire(Wire *wire, uint8_t i2c_address) :
	wire_(wire),
	address_(i2c_address)
{
	//nothing to do here...
}

BMX280TwoWire::~BMX280TwoWire()
{
	wire_ = nullptr;
}

bool BMX280TwoWire::beginInterface()
{
	return true;
}

uint8_t BMX280TwoWire::readRegister(uint8_t reg)
{
	if (!wire_)
		return 0;

#if defined(ARDUINO_SAM_DUE)
	//workaround for Arduino Due. The Due seems not to send a repeated start with the code above, so this 
	//undocumented feature of Wire::requestFrom() is used. can be used on other Arduinos too (tested on Mega2560)
	//see this thread for more info: https://forum.arduino.cc/index.php?topic=385377.0
	Wire.requestFrom(address_, 1, reg, 1, true);
#else
	Wire.beginTransmission(address_);
	Wire.write(reg);
	Wire.endTransmission(false);
	Wire.requestFrom(address_, static_cast<uint8_t>(1));
#endif

	return Wire.read();
}

uint32_t BMX280TwoWire::readRegisterBurst(uint8_t reg, uint8_t length)
{
	if (!wire_)
		return 0;

	if (length > 4)
		return 0L;

	uint32_t data = 0L;

#if defined(ARDUINO_SAM_DUE)
	//workaround for Arduino Due. The Due seems not to send a repeated start with the code below, so this 
	//undocumented feature of Wire::requestFrom() is used. can be used on other Arduinos too (tested on Mega2560)
	//see this thread for more info: https://forum.arduino.cc/index.php?topic=385377.0
	Wire.requestFrom(address_, length, data, length, true);
#else
	Wire.beginTransmission(address_);
	Wire.write(reg);
	Wire.endTransmission(false);
	Wire.requestFrom(address_, static_cast<uint8_t>(length));

	for (uint8_t i = 0; i < length; i++)
	{
		data <<= 8;
		data |= Wire.read();
	}
#endif

	return data;
}

void BMX280TwoWire::writeRegister(uint8_t reg, uint8_t value)
{
	if (!wire_)
		return;

	Wire.beginTransmission(address_);
	Wire.write(reg);
	Wire.write(value);
	Wire.endTransmission();
}
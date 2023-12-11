/////////////////////////////////////////////////////////////////////////////////////////
// USVBusAddresses.h
//
// Created: 08.12.2023 16:14
//  Author: Franke
//
//  rev. 1.0
//
//  - Header for documentation "\USV\00 doc\00 Bussystem\Bussystem.pdf" rev. 16
//
//  - initialization version
//
/////////////////////////////////////////////////////////////////////////////////////////

#ifndef USVBUSADDRESSES_H_
#define USVBUSADDRESSES_H_

/////////////////////////////////////////////////////////////////////////////////////////
//
// Adressen Enumerator
//
/////////////////////////////////////////////////////////////////////////////////////////
enum USVBusAddresses_e
{
	SB1 = 0x000,
	SB2 = 0x001,
	SB3 = 0x005,
	SB4 = 0x009,
	SB5 = 0x00A,
	SB6 = 0x00C,
	SB7 = 0x00E,
	SB8 = 0x012,
	SB9 = 0x014,
	AF1 = 0x100,
	AF2 = 0x108,
	AF3 = 0x110,
	AF4 = 0x112,
	AS1 = 0x120,
	AS2 = 0x122,
	ER1 = 0x200,
	ER2 = 0x201,
	ER3 = 0x202,
	ER4 = 0x203,
	ER5 = 0x205,
	ES1 = 0x220,
}

enum USVBusAddressesLength_e
{
	SB1_len = 001,
	SB2_len = 004,
	SB3_len = 004,
	SB4_len = 001,
	SB5_len = 002,
	SB6_len = 002,
	SB7_len = 003,
	SB8_len = 002,
	SB9_len = 001,
	AF1_len = 008,
	AF2_len = 008,
	AF3_len = 002,
	AF4_len = 002,
	AS1_len = 002,
	AS2_len = 002,
	ER1_len = 001,
	ER2_len = 001,
	ER3_len = 001,
	ER4_len = 002,
	ER5_len = 001,
	ES1_len = 362,
}

#endif // USVBUSADDRESSES_H_ //

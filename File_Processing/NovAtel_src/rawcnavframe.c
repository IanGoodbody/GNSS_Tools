#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "parseNovAtel.h"

#define RCNFRAME_PRN_O 4
#define RCNFRAME_PRN_B 4

#define RCNFRAME_FRAMEID_O 8
#define RCNFRAME_FRAMEID_B 4

#define PREAMB_S 0
#define PREAMB_L 8
#define PRN_S 8
#define PRN_L 6
#define MSGID_S 14
#define MSGID_L 6
#define TOW_S 20
#define TOW_L 17
#define ALERT_S 37
#define ALERT_L 1
#define ALERT_S 276
#define ALERT_L 24

#define M10_WN_S 38
#define M10_WN_L 13
#define M10_Toe_S 70
#define M10_Toe_L 11
#define M10_A_S 81
#define M10_A_L 26
#define M10_ADot_S 107
#define M10_ADot_L 25
#define M10_dN_S 132
#define M10_dN_L 23
#define M10_dNDot_S 149
#define M10_dNDot_L 23
#define M10_M0_S 172
#define M10_M0_L 33
#define M10_e_S 205
#define M10_e_L 33
#define M10_omega_S 238
#define M10_omega_L 33

#define M11_Toe_S 38
#define M11_Toe_L 11
#define M11_Omega_S 49
#define M11_Omega_L 33
#define M11_i_S 82
#define M11_i_L 33
#define M11_OmegaDot_S 115
#define M11_OmegaDot_L 17
#define M11_iDot_S 132
#define M11_iDot_L 15
#define M11_Cis_S 147
#define M11_Cis_L 16
#define M11_Cic_S 163
#define M11_Cic_L 16
#define M11_Crs_S 179
#define M11_Crs_L 24
#define M11_Crc_S 203
#define M11_Crc_L 24
#define M11_Cus_S 227
#define M11_Cus_L 21
#define M11_Cuc_S 248
#define M11_Cuc_L 16

#define M30_Toc_S 60
#define M30_Toc_L 11
#define M30_a0_S 71
#define M30_a0_L 26
#define M30_a1_S 97
#define M30_a1_L 20
#define M30_a2_S 117
#define M30_a2_L 10
#define M30_Tgd_S 127
#define M30_Tgd_L 13
#define M30_iscL1CA_S 140
#define M30_iscL1CA_L 13
#define M30_iscL2C_S 153
#define M30_iscL2C_L 13
#define M30_iscL5I_S 166
#define M30_iscL5I_L 13
#define M30_iscL5Q_S 179
#define M30_iscL5Q_L 13

int parseRange(FILE* binLog, rawcnavframeDataSt* dataStruct, long int bodyStart)
{
	double m = nan;
}
int64_t loadCnavField(uint8_t* cnavFrame, int fieldStart, int fieldLength, 
 int tcFlag);
{
	int64_t field = 0x00;
	int size = fieldLength;
	while(1){
		startBit = fieldStart % 8;
		// The end of the field is containd in the current byte
		if(startBit + fieldLength <= 8){ 
			field |= *(cnavFrame + fieldStart/8) >> (8 - startBit - fieldLengh) & 
			 (0x01 << fieldLength)-1;
			break;
		}
		// The data field extends beyond the current byte
		else{
			field |= *(cnavFrame + fieldStart/8) << (fieldLengh - 8 + startBit) &
			 ((0x01 << fieldLength)-1) &~ ((0x01 << fieldLength - 8 + startBit)-1);
			fieldStart += 8-startBit;
			fieldLength -= 8-startBit;
		}
	}

	if( (tcFlag) && (field & 0x01<<size) )
		field |= (~0x00) & ~((0x01<<size)-1); // all 1's with bits cleared
	
	return field;
}

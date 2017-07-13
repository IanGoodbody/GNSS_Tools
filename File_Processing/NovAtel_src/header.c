#include <stdio.h>
#include "parseNovAtel.h"

#define HEAD_BUFF_SIZE 4

#define HEAD_HLEN_B 1 // field size in bytes
#define HEAD_HLEN_O 3 // field offset in bytes

#define HEAD_ID_B 2
#define HEAD_ID_O 4

#define HEAD_MTYPE_B 1
#define HEAD_MTYPE_O 6

#define HEAD_MLEN_B 2
#define HEAD_MLEN_O 8

#define HEAD_WEEK_B 2
#define HEAD_WEEK_O 14

#define HEAD_GPST_B 4
#define HEAD_GPST_O 14

#define HEAD_TSTAT_B 1
#define HEAD_TSTAT_O 13

#define HEAD_RSTAT_B 4
#define HEAD_RSTAT_O 20

int parseHeader(FILE* binLog, headerDataSt* headerData, long int logStart)
{
	unsigned char preamble[] = {0xAA, 0x44, 0x12};

	int i; // Check the preamble
	for(i = 0; i < 3; i++){
		if(fgetc(binLog) != preamble[i]){
			fseek(binLog, logStart, SEEK_SET);
			if(fgetc(binLog) == '#')
				return HEAD_ASCII_SYNC;
			else
				return HEAD_BAD_SYNC;
		}
	}

	// Header length
	fseek(binLog, logStart+HEAD_HLEN_O, SEEK_SET);
	fread(&headerData->headLen, HEAD_HLEN_B, 1, binLog);
	// Message ID
	fseek(binLog, logStart+HEAD_ID_O, SEEK_SET);
	fread(&headerData->msgID, HEAD_ID_B, 1, binLog);
	// Message Type
	fseek(binLog, logStart+HEAD_MTYPE_O, SEEK_SET);
	fread(&headerData->msgType, HEAD_MTYPE_B, 1, binLog);
	// Message Length
	fseek(binLog, logStart+HEAD_MLEN_O, SEEK_SET);
	fread(&headerData->msgLen, HEAD_MLEN_B, 1, binLog);
	// Week Number
	fseek(binLog, logStart+HEAD_WEEK_O, SEEK_SET);
	fread(&headerData->weekNum, HEAD_WEEK_B, 1, binLog);
	// GPST
	fseek(binLog, logStart+HEAD_GPST_O, SEEK_SET);
	fread(&headerData->gpst, HEAD_GPST_B, 1, binLog);
	// Time Status
	fseek(binLog, logStart+HEAD_TSTAT_O, SEEK_SET);
	fread(&headerData->timeStat, HEAD_TSTAT_B, 1, binLog);
	// Receiver Status
	fseek(binLog, logStart+HEAD_RSTAT_O, SEEK_SET);
	fread(&headerData->rcvrStat, HEAD_RSTAT_B, 1, binLog);

	return HEAD_GOOD;
}

void indicateRcvrStatus(headerDataSt* headerData)
{
	printf("Receiver status decodeing not yet implemented.\n");
	printf("Reference Table 140 in the firmware reference manual.\n");
}

void inidcateTimeStatus(headerDataSt* headerData)
{
	switch(headerData->timeStat){
		case 20: // Unknown
			printf("20:  Time validity is unknown\n");
			break;
		case 60: // Approximate
			printf("60:  Time is set approximately\n");
			break;
		case 80: // Coarse Adjusting
			printf("80:  Time is approaching coase precision\n");
			break;
		case 100: // Coarse
			printf("100: Time is valid to coarse precision\n");
			break;
		case 120: // Coarse Steering
			printf("120: Time is coase set and being steered\n");
			break;
		case 130: // Freewheeling
			printf("130: Positionis lost and the range bias cannot be calculated\n");
			break;
		case 140: // Fine Adjusting
			printf("140: Time is adjsting to fine precision\n");
			break;
		case 160: // Fine
			printf("160: Time has fine precision\n");
			break;
		case 170: // Fine Backup Steering
			printf("170: Time is fine set and is being steered by the backup system\n");
			break;
		case 180: // Fine Steering
			printf("180: Time is fine set and being steered\n");
			break;
		case 200: // Sat Time
			printf("200: Time from satellite (only in ephemeris or almanac logs)\n");
			break;
		default:
			printf("%i: Invalid time status code\n", (int)headerData->timeStat);
			break;
	}
}


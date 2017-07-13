#include <stdio.h>
#include <inttypes.h>
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

#define HEAD_IDLE_B 1
#define HEAD_IDLE_O 12

#define HEAD_WEEK_B 2
#define HEAD_WEEK_O 14

#define HEAD_GPST_B 4
#define HEAD_GPST_O 16

#define HEAD_TSTAT_B 1
#define HEAD_TSTAT_O 13

#define HEAD_RSTAT_B 4
#define HEAD_RSTAT_O 20

int parseHeader(FILE* binLog, headerDataSt* headerData, long int logStart)
{
	uint8_t preamble[] = {0xAA, 0x44, 0x12};

	fseek(binLog, logStart, SEEK_SET);
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
	// Idle time
	fseek(binLog, logStart+HEAD_IDLE_O, SEEK_SET);
	fread(&headerData->idle, HEAD_IDLE_B, 1, binLog);
	// Week Number
	fseek(binLog, logStart+HEAD_WEEK_O, SEEK_SET);
	fread(&headerData->weekNum, HEAD_WEEK_B, 1, binLog);
	// GPST (ms)
	fseek(binLog, logStart+HEAD_GPST_O, SEEK_SET);
	fread(&headerData->gpst, HEAD_GPST_B, 1, binLog);
	// Time Status
	fseek(binLog, logStart+HEAD_TSTAT_O, SEEK_SET);
	fread(&headerData->timeStat, HEAD_TSTAT_B, 1, binLog);
	// Receiver Status
	fseek(binLog, logStart+HEAD_RSTAT_O, SEEK_SET);
	fread(&headerData->rcvrStat, HEAD_RSTAT_B, 1, binLog);

	#if PARSE_VERBOSE // PRINT the header output
	fprintf(stdout, "Header Fields:\n");
	fprintf(stdout, "  Header Length: %hhu 0x%04hhX\n", headerData->headLen, 
	 headerData->headLen);
	fprintf(stdout, "  Message ID: %hu\n", headerData->msgID);
	fprintf(stdout, "  Message Type: 0x%hhX\n", headerData->msgType);
	fprintf(stdout, "  Message Length: %hu 0x%04hX\n", headerData->msgLen, 
	 headerData->msgLen);
	fprintf(stdout, "  Processor Idle: %hu%%\n", (headerData->idle)/2);
	fprintf(stdout, "  Week Number: %hu\n", headerData->weekNum);
	fprintf(stdout, "  GPST: %u\n", headerData->gpst);
	fprintf(stdout, "  Time Status: %hhu; %s\n", headerData->timeStat,
	 decodeTimeStatus(headerData->timeStat));
	fprintf(stdout, "  Receiver Status: 0x%X\n", headerData->rcvrStat);
	#endif

	return HEAD_GOOD;
}

void indicateRcvrStatus(headerDataSt* headerData)
{
	printf("Receiver status decodeing not yet implemented.\n");
	printf("Reference Table 140 in the firmware reference manual.\n");
}

const char* decodeTimeStatus(uint8_t timeStat)
{
	switch(timeStat){
		case 20: // Unknown
			return "Time validity is unknown";
		case 60: // Approximate
			return "Time is set approximately";
		case 80: // Coarse Adjusting
			return "Time is approaching coase precision";
		case 100: // Coarse
			return "Time is valid to coarse precision";
		case 120: // Coarse Steering
			return "Time is coase set and being steered";
		case 130: // Freewheeling
			return "Positionis lost and the range bias cannot be calculated";
		case 140: // Fine Adjusting
			return "Time is adjsting to fine precision";
		case 160: // Fine
			return "Time has fine precision";
		case 170: // Fine Backup Steering
			return "Time is fine set and is being steered by the backup system\n";
		case 180: // Fine Steering
			return "Time is fine set and being steered";
		case 200: // Sat Time
			return "Time from satellite (only in ephemeris or almanac logs)";
		default:
			return "Invalid time status code";
	}
}


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "parseNovAtel.h"

#define BUFFER_SIZE 512
#define DEBUG 1

#define CRC_B 4
#define CRC_POLY 0xEDB88320

int main(int argc, char** argv)
{
	// Check all the data type sizes and make sure you didn't screw yourself
	int sizeMatch = 1; 
	sizeMatch = sizeMatch && (sizeof( unsigned char) ==  1);
	sizeMatch = sizeMatch && (sizeof(unsigned short) ==  2);
	sizeMatch = sizeMatch && (sizeof(  unsigned int) ==  4);
	sizeMatch = sizeMatch && (sizeof( unsigned long) ==  8);
	sizeMatch = sizeMatch && (sizeof(         float) ==  4);
	sizeMatch = sizeMatch && (sizeof(        double) ==  8);
	sizeMatch = sizeMatch && (sizeof(   long double) == 16);
	if(!sizeMatch){
		fprintf(stderr, "WARNING: Native floating point data types sizes do not match assumed sizes!\n");
		fprintf(stderr, "  Integer types are specified with <inttypes.h> but may causes warnings for\n  printf outputs\n");
		fprintf(stderr, "  +===============================+\n");
		fprintf(stderr, "  |Data Type   | Assumed | Native |\n");
		fprintf(stderr, "  |------------+---------+--------|\n");
		fprintf(stderr, "  |char        |       1 |     %2i |\n", (int)sizeof(unsigned char));
		fprintf(stderr, "  |short int   |       2 |     %2i |\n", (int)sizeof(unsigned short));
		fprintf(stderr, "  |int         |       4 |     %2i |\n", (int)sizeof(unsigned int));
		fprintf(stderr, "  |long int    |       8 |     %2i |\n", (int)sizeof(unsigned long));
		fprintf(stderr, "  |------------+---------+--------|\n");
		fprintf(stderr, "  |float       |       4 |     %2i |\n", (int)sizeof(float));
		fprintf(stderr, "  |double      |       8 |     %2i |\n", (int)sizeof(double));
		fprintf(stderr, "  |long double |      16 |     %2i |\n", (int)sizeof(long double));
		fprintf(stderr, "  +===============================+\n");
		//return 7;
	}

	FILE *binLogFile, *bestposFile, *rangeFile;
	char binLogFileName[BUFFER_SIZE];
	char bestposFileName[BUFFER_SIZE];
	char rangeFileName[BUFFER_SIZE];

	long int logStart;
	int headerStatus;

	int logCount = 0;
	int bestposCount = 0;
	int rangeCount = 0;
	int rawephemCount = 0;
	int rawcnavframeCount = 0;

	headerDataSt headerData;
	bestposDataSt bestposData;
	rangeDataSt rangeData;
	 rangeData.numObs = 0;
	 rangeData.rangeObsBlock = NULL;
	
	if(argc != 2 && argc != 3){
		fprintf(stderr, "Error: invalid number of arguments: %i\n", argc);
		return 1;
	}

	strncpy(binLogFileName, *(argv + 1), BUFFER_SIZE);
	if(argc == 2){ // Only log file name specified, append log type names
		strncpy(bestposFileName, *(argv + 1), BUFFER_SIZE);
		strcpy(bestposFileName + strlen(bestposFileName) - 4, "_bestpos.dat");
		strncpy(rangeFileName, *(argv + 1), BUFFER_SIZE);
		strcpy(rangeFileName + strlen(rangeFileName) - 4, "_range.dat");
	}
	else{
		strncpy(bestposFileName, *(argv + 2), BUFFER_SIZE);
		strcpy(bestposFileName + strlen(bestposFileName),  "_bestpos.dat");
		strncpy(rangeFileName, *(argv + 2), BUFFER_SIZE);
		strcpy(rangeFileName + strlen(rangeFileName),  "_range.dat");
	}
 
	binLogFile = fopen(binLogFileName, "rb");
	bestposFile = fopen(bestposFileName, "w");
	rangeFile = fopen(rangeFileName, "w");

	logStart = ftell(binLogFile);
	while(1){ // loop breaks when the header fails
		#if PARSE_VERBOSE 
		fprintf(stdout, "\nLog %i at 0x%08lX\n", logCount, logStart);
		#endif
		// Read the header
		headerStatus = parseHeader(binLogFile, &headerData, logStart);
		if( headerStatus != HEAD_GOOD )
			break; // Failed Header indicates end of file, hopefully no seg-fault
		logCount++;

		// Check the CRC
		if( checkCrc(binLogFile, logStart, 
		 headerData.msgLen + headerData.headLen) ){ // CRC test passes
			// Process the log body
			switch(headerData.msgID){
				case BESTPOS_ID:
					bestposCount++;
					parseBestpos(binLogFile, &bestposData, logStart + headerData.headLen);
					break;

				case RANGE_ID:
					rangeCount++;
					parseRange(binLogFile, &rangeData, logStart + headerData.headLen);
					clearRangeData(&rangeData);
					break;

				case RAWCNAVFRAME_ID:
					rawcnavframeCount++;
					break;

				case RAWEPHEM_ID:
					rawephemCount++;
					break;
			}
			// write the fields to the appropriate file
		}
		else{ // CRC test fails, report to user
			fprintf(stderr, "CRC failed for log %i, starting at 0x%08lx\n", 
			 logCount, logStart);
		}

		//#if DEBUG
		if(bestposCount >= 1)
			break;
		//#endif

		// Set the log start pointer to the next log
		logStart += headerData.headLen + headerData.msgLen + CRC_B;
	}
 	
	fprintf(stdout, "%s parsed with %i entries:\n", binLogFileName, logCount);
	fprintf(stdout, "%5i BESTPOS logs\n", bestposCount);
	if( headerStatus == HEAD_ASCII_SYNC )
		fprintf(stdout, "Tracking terminated by an ASCII style header, check log type\n");

	fclose(binLogFile);
	fclose(bestposFile);
	return 0;
}

int checkCrc(FILE* binLog, long int logStart, int blockSize)
{
	uint32_t crc = 0;
	uint32_t refCrc = 0;
	int i;

	// Iterate through each byte of the current log
	fseek(binLog, logStart, SEEK_SET);
	while(blockSize-- > 0){
		crc ^= fgetc(binLog);
		// Apply the CRC division to the byte just read in (LSB first)
		for(i = 8; i > 0; i--){
			if(crc & 0x01) 
				crc = (crc >> 1) ^ CRC_POLY;
			else
				crc >>= 1;
		}
	}
	fread(&refCrc, CRC_B, 1, binLog); // CRC stored as the last 4 bytes

	return crc == refCrc;
}

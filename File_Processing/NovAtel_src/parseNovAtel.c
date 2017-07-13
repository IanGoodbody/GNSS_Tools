#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "parseNovAtel.h"

#define BUFFER_SIZE 1024

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
		printf("Native data types sizes do not match assumed sizes!\n");
		printf("  +===============================+\n");
		printf("  |Data Type   | Assumed | Native |\n");
		printf("  |------------+---------+--------|\n");
		printf("  |char        |       1 |     %2i |\n", (int)sizeof(unsigned char));
		printf("  |short int   |       2 |     %2i |\n", (int)sizeof(unsigned short));
		printf("  |int         |       4 |     %2i |\n", (int)sizeof(unsigned int));
		printf("  |long int    |       8 |     %2i |\n", (int)sizeof(unsigned long));
		printf("  |float       |       4 |     %2i |\n", (int)sizeof(float));
		printf("  |double      |       8 |     %2i |\n", (int)sizeof(double));
		printf("  |long double |      16 |     %2i |\n", (int)sizeof(long double));
		printf("  +===============================+\n");
		return 7;
	}

	FILE *binLogFile, *bestposFile;
	char binLogFileName[BUFFER_SIZE];
	char bestposFileName[BUFFER_SIZE];

	long int logStart;
	int headerStatus;

	int logCount = 0;
	int bestposCount = 0;
	int rangeCount = 0;
	int rawephemCount = 0;
	int rawcnavframeCount = 0;

	headerDataSt headerData;
	bestposDataSt bestposData;
	
	if(argc != 2 && argc != 3){
		fprintf(stderr, "Error, invalid number of arguments: %i\n", argc);
		return 1;
	}

	strcpy(binLogFileName, *(argv + 1));
	if(argc == 2){ // Only log file name specified, append log type names
		strcpy(bestposFileName, *(argv + 1));
		strcpy(bestposFileName + strlen(bestposFileName) - 4, "_bestpos.csv");
	}
	else{
		strcpy(bestposFileName, *(argv + 2));
		strcpy(bestposFileName + strlen(bestposFileName),  "_bestpos.csv");
	}

	binLogFile = fopen(binLogFileName, "rb");
	bestposFile = fopen(bestposFileName, "w");

	logStart = ftell(binLogFile);
	while(1){ // loop breaks when the header fails
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
					parseBestpos(binLogFile, &bestposData, logStart);

					break;
				case RANGE_ID:
					rangeCount++;
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

		/* BEGIN DEBUG */
		if(bestposCount >= 1)
			break;
		/* END DEBUG */

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
	unsigned int crc = 0;
	unsigned int refCrc = 0;
	int i;

	// Iterate through each byte of the current log
	while(blockSize-- > 0){
	fseek(binLog, logStart, SEEK_SET);
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

	fprintf(stdout, "Reference CRC:  0x%08x\n", refCrc);
	fprintf(stdout, "Calculated CRC: 0x%08x\n", crc);
	return crc == refCrc;
}

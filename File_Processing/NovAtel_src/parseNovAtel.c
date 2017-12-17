#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "parseNovAtel.h"
#include "decoderLibrary.h"

#define BUFFER_SIZE 512

#define CRC_B 4
#define CRC_POLY 0xEDB88320

int main(int argc, char** argv)
{
	FILE *binLogFile, *bestposFile, *rangeFile, *trackstatFile;
	char binLogFileName[BUFFER_SIZE];
	char bestposFileName[BUFFER_SIZE];
	char rangeFileName[BUFFER_SIZE];
	char trackstatFileName[BUFFER_SIZE];

	long int logStart;
	int logCount = 0;
	int headerStatus;
	headerDataSt headerData;

	int bestposCount = 0;
	bestposDataSt bestposData;
	void (*bestposWriter)(FILE*, headerDataSt*, bestposDataSt*);

	int rangeCount = 0;
	rangeDataSt rangeData;
	 rangeData.numObs = 0;
	 rangeData.rangeObsBlock = NULL;
	void (*rangeWriter)(FILE*, headerDataSt*, rangeDataSt*);

	int trackstatCount = 0;
	trackstatDataSt trackstatData;
	 trackstatData.numObs = 0;
	 trackstatData.trackstatObsBlock = NULL;
	void (*trackstatWriter)(FILE*, headerDataSt*, trackstatDataSt*);

	int rawephemCount = 0;
	int rawcnavframeCount = 0;

	if(argc != 3 && argc != 4){
		fprintf(stderr, "Error: invalid number of arguments: %i\n", argc);
		return 1;
	}

	// Set the output type
	if( strcmp(*(argv+1), "csv") == 0 ){
		fprintf(stdout, "Writing .dat files in csv format\n");
		bestposWriter = &write_bestpos_GPS_essential_csv;
		rangeWriter = &write_range_GPS_essential_csv;
		trackstatWriter = &write_trackstat_GPS_csv;
	}
	else{
		fprintf(stdout, "Writing .dat files in column format\n");
		bestposWriter = &write_bestpos_GPS_essential_col;
		rangeWriter = &write_range_GPS_essential_col;
		trackstatWriter = &write_trackstat_GPS_col;
	}

	strncpy( binLogFileName, *(argv + 2), BUFFER_SIZE );
	if(argc == 3){ // Only log file name specified, append log type names
		strncpy( bestposFileName, *(argv + 2), BUFFER_SIZE );
		strcpy( bestposFileName + strlen(bestposFileName) - 4, "_bestpos.dat" );
		strncpy( rangeFileName, *(argv + 2), BUFFER_SIZE);
		strcpy( rangeFileName + strlen(rangeFileName) - 4, "_range.dat" );
		strncpy( trackstatFileName, *(argv + 2), BUFFER_SIZE );
		strcpy( trackstatFileName + strlen(trackstatFileName) - 4, "_trackstat.dat" );
	}
	else{
		strncpy( bestposFileName, *(argv + 3), BUFFER_SIZE );
		strcpy( bestposFileName + strlen(bestposFileName),  "_bestpos.dat" );
		strncpy( rangeFileName, *(argv + 3), BUFFER_SIZE );
		strcpy( rangeFileName + strlen(rangeFileName),  "_range.dat" );
		strncpy( trackstatFileName, *(argv + 3), BUFFER_SIZE );
		strcpy( trackstatFileName + strlen(trackstatFileName),  "_trackstat.dat" );
	}
 
	binLogFile = fopen(binLogFileName, "rb");
	bestposFile = fopen(bestposFileName, "w");
	rangeFile = fopen(rangeFileName, "w");
	trackstatFile = fopen(trackstatFileName, "w");

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
					parseBestpos( binLogFile, &bestposData, 
					 logStart + headerData.headLen );
					(*bestposWriter)( bestposFile, &headerData, &bestposData );
					break;

				case RANGE_ID:
					rangeCount++;
					parseRange( binLogFile, &rangeData, logStart + headerData.headLen );
					(*rangeWriter)( rangeFile, &headerData, &rangeData );
					clearRangeData( &rangeData );
					break;

				case TRACKSTAT_ID:
					trackstatCount++;
					parseTrackstat( binLogFile, &trackstatData, 
					 logStart + headerData.headLen );
					(*trackstatWriter)( trackstatFile, &headerData, &trackstatData );
					clearTrackstatData( &trackstatData );
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

		#if 0
		if(bestposCount >= 3)
			break;
		#endif

		// Set the log start pointer to the next log
		logStart += headerData.headLen + headerData.msgLen + CRC_B;
	}
 	
	fprintf(stdout, "%s parsed with %i entries:\n", binLogFileName, logCount);
	fprintf(stdout, "%5i BESTPOS logs\n", bestposCount);
	fprintf(stdout, "%5i RANGE logs\n", rangeCount);
	fprintf(stdout, "%5i TRACKSTAT logs\n", trackstatCount);
	if( headerStatus == HEAD_ASCII_SYNC )
		fprintf(stdout, "Tracking terminated by an ASCII style header, check log type\n");

	fclose(binLogFile);
	fclose(bestposFile);
	fclose(rangeFile);
	fclose(trackstatFile);
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

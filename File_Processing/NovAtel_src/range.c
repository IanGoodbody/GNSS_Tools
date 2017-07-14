#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "parseNovAtel.h"

/* The function records all the fields in the range measurements. Because
 * reads can cascade though the log, it is faster not to seek to the 
 * appropriate offset each time. Sorry if you want less data.
 */
#define RANGE_NUMOBS_B 4
#define RANGE_PRN_B 2 
#define RANGE_GLOFREQ_B 2
#define RANGE_PSR_B 8
#define RANGE_PSRSTD_B 4
#define RANGE_CARRIER_B 8
#define RANGE_CARRIERSTD_B 4
#define RANGE_DOPP_B 4
#define RANGE_CN0_B 4
#define RANGE_LOCKTIME_B 4
#define RANGE_CHANSTAT_B 4

int parseRange(FILE* binLog, rangeDataSt* dataStruct, long int bodyStart)
{
	// Determine the number of observations
	uint32_t numObs;
	fseek(binLog, bodyStart, SEEK_SET); // # obs at the start of the body
	fread(&numObs, RANGE_NUMOBS_B, 1, binLog);

	if(numObs == 0){
		dataStruct->numObs = 0;
		dataStruct->rangeObsBlock = NULL;
		return 0; // Best not try an allocate zero memory
	}

	// Allocate memory for the obersvation structs
	rangeDataObsSt* rangeObsBlock = 
	 (rangeDataObsSt*) malloc( sizeof(rangeDataObsSt)*numObs );
	if( !rangeObsBlock ) // Error allocating memory
		return RANGE_MALLOC_FAIL;

	uint32_t i; 
	for(i = 0; i < numObs; i++){
		// PRN
		fread(&(rangeObsBlock+i)->prn, RANGE_PRN_B, 1, binLog);
		// GLONASS Frequency Number
		fread(&(rangeObsBlock+i)->gloFreq, RANGE_GLOFREQ_B, 1, binLog);
		// Pseudorange (m)
		fread(&(rangeObsBlock+i)->psr, RANGE_PSR_B, 1, binLog);
		// Pseudorange standard deviation (m)
		fread(&(rangeObsBlock+i)->psrStd, RANGE_PSRSTD_B, 1, binLog);
		// Carrier Phase (cycles)
		fread(&(rangeObsBlock+i)->carrier, RANGE_CARRIER_B, 1, binLog);
		// Carrier standard deviation (cycles)
		fread(&(rangeObsBlock+i)->carrierStd, RANGE_CARRIERSTD_B, 1, binLog);
		// Doppler (Hz)
		fread(&(rangeObsBlock+i)->dopp, RANGE_DOPP_B, 1, binLog);
		// Carrier to noise density ratio (db-Hz)
		fread(&(rangeObsBlock+i)->cn0, RANGE_CN0_B, 1, binLog);
		// Lock time (s)
		fread(&(rangeObsBlock+i)->lockTime, RANGE_LOCKTIME_B, 1, binLog);
		// Channel Status
		fread(&(rangeObsBlock+i)->chanStat, RANGE_CHANSTAT_B, 1, binLog);

		#if PARSE_VERBOSE // PRINT the header output
		fprintf(stdout, "\nRange Fields:");
		fprintf(stdout, " observatrion %u of %u\n", i+1, numObs);
		fprintf(stdout, "  Signal Type: %s\n", 
		 decodeSignalStr( (rangeObsBlock+i)->chanStat) );
		fprintf(stdout, "  PRN/slot: %hu\n", (rangeObsBlock+i)->prn);
		fprintf(stdout, "  GLONASS Freq: %hu\n", (rangeObsBlock+i)->gloFreq);
		fprintf(stdout, "  Pseudorange: %f\n", (rangeObsBlock+i)->psr);
		fprintf(stdout, "  Psr Stdev: %f\n", (rangeObsBlock+i)->psrStd);
		fprintf(stdout, "  Carrier: %f\n", (rangeObsBlock+i)->carrier);
		fprintf(stdout, "  Carrier Stdev: %f\n", (rangeObsBlock+i)->carrierStd);
		fprintf(stdout, "  Doppler: %f\n", (rangeObsBlock+i)->dopp);
		fprintf(stdout, "  CN0: %f\n", (rangeObsBlock+i)->cn0);
		fprintf(stdout, "  Lock Time: %f\n", (rangeObsBlock+i)->lockTime);
		#endif
	}

	dataStruct->numObs = numObs;
	dataStruct->rangeObsBlock = rangeObsBlock;

	return (int)numObs;
}

int clearRangeData(rangeDataSt* dataStruct){
	uint32_t numObs = dataStruct->numObs;
	if(dataStruct->rangeObsBlock != NULL){
		free(dataStruct->rangeObsBlock);
		dataStruct->rangeObsBlock = NULL;
		dataStruct->numObs = 0;
	}
	return numObs;
}

void write_RANGE_GPS_essential_col(FILE* rangeFile, 
 headerDataSt* headerData, rangeDataSt* rangeData)
{
	unsigned int obs;
	for(obs = 0; obs < rangeData->numObs; obs++){
		if( decodeSystem((rangeData->rangeObsBlock + obs)->chanStat) == 
		 RANGE_GPS_ID ){ // Verify it is a GPS log
			fprintf( rangeFile, "%5u %9u %2u %.15E % .15E % .6E %2.4f \
%2u\n",
			 headerData->weekNum, headerData->gpst, 
			 (rangeData->rangeObsBlock + obs)->prn,
			 (rangeData->rangeObsBlock + obs)->psr,
			 (rangeData->rangeObsBlock + obs)->carrier,
			 (rangeData->rangeObsBlock + obs)->dopp,
			 (rangeData->rangeObsBlock + obs)->cn0,
			 decodeSignal((rangeData->rangeObsBlock + obs)->chanStat) );
		}
	} 
}

void write_RANGE_GPS_essential_csv(FILE* rangeFile, 
 headerDataSt* headerData, rangeDataSt* rangeData)
{
	unsigned int obs;
	for(obs = 0; obs < rangeData->numObs; obs++){
		if( decodeSystem((rangeData->rangeObsBlock + obs)->chanStat) == 
		 RANGE_GPS_ID ){ // Verify it is a GPS log
			fprintf( rangeFile, "%u,%u,%u,%.15E,%.15E,%.6E,%g,%u\n",
			 headerData->weekNum, headerData->gpst, 
			 (rangeData->rangeObsBlock + obs)->prn,
			 (rangeData->rangeObsBlock + obs)->psr,
			 (rangeData->rangeObsBlock + obs)->carrier,
			 (rangeData->rangeObsBlock + obs)->dopp,
			 (rangeData->rangeObsBlock + obs)->cn0,
			 decodeSignal((rangeData->rangeObsBlock + obs)->chanStat) );
		}
	} 
}

uint32_t decodeSystem(uint32_t chanStat)
{
	return (chanStat & 0x00070000) >> 16;
}

uint32_t decodeSignal(uint32_t chanStat)
{
	return (chanStat & 0x03E00000) >> 21;
}

const char* decodeSignalStr(uint32_t chanStat)
{
	uint32_t sigType = (chanStat & 0x03E00000) >> 21;
	switch((chanStat & 0x00070000) >> 16){
		case RANGE_GPS_ID:
			switch(sigType){
				case 0:
					return "GPS L1C/A";
				case 5:
					return "GPS L2P";
				case 9:
					return "GPS L2P Codeless";
				case 14:
					return "GPS L5Q";
				case 17:
					return "GPS L2C";
				default:
					return "Undocumented GPS Signal";
				}
		case RANGE_GLONASS_ID:
			switch(sigType){
				case 0:
					return "GLONASS L1C/A";
				case 1:
					return "GLONASS L2C/A";
				case 5:
					return "GLONASS L2P";
				default:
					return "Undocumented GLONASS Signal";
			}
		case RANGE_SBAS_ID:
			switch(sigType){
				case 0:
					return "SBAS L1C/A";
				case 6:
					return "SBAS L5I";
				default:
					return "Undocumentd SBAS Signal";
			}
		case RANGE_GALILEO_ID:
			switch(sigType){
				case 2:
					return "Galileo E1C";
				case 12:
					return "Galileo E5a Q";
				case 17:
					return "Galileo E5b Q";
				case 20:
					return "AltBOC Q";
				default:
					return "Undocumented Galileo Signal";
			}
		case RANGE_BEIDOU_ID:
			switch(sigType){
				case 0:
					return "BeiDou B1 with D1 data";
				case 1:
					return "BeiDou B2 with D1 data";
				case 4:
					return "BeiDou B1 with D2 data";
				case 5:
					return "BeiDou B2 with D2 data";
				default:
					return "Undocumented BeiDou Signal";
			}
		case RANGE_QZSS_ID:
			switch(sigType){
				case 0:
					return "QZSS L1C/A";
				case 14:
					return "QZSS L5Q";
				case 17:
					return "QZSS L2C";
				default:
					return "Undocumented QZSS Signal";
			}
		default:
			return "Unknown satellite system";
	}
}

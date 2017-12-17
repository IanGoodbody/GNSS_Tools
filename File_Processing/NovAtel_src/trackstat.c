#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "parseNovAtel.h"
#include "decoderLibrary.h"

/* The function records all the fields in the trackstat measurements. Because
 * reads can cascade though the log, it is faster not to seek to the 
 * appropriate offset each time. Sorry if you want less data.
 */

#define TRACK_SOL_STATUS_B 4
#define TRACK_POS_TYPE_B 4
#define TRACK_CUTOFF_B 4
#define TRACK_NUMOBS_B 4

#define TRACK_PRN_B 2 
#define TRACK_GLOFREQ_B 2
#define TRACK_CHANSTAT_B 4
#define TRACK_PSR_B 8
#define TRACK_DOPP_B 4
#define TRACK_CN0_B 4
#define TRACK_LOCKTIME_B 4
#define TRACK_PSR_RES_B 4
#define TRACK_REJECT_B 4
#define TRACK_PSR_WGHT_B 4

int parseTrackstat( FILE* binLog, trackstatDataSt* dataStruct, 
 long int bodyStart )
{
	// Determine the number of observations
	uint32_t solStatus;
	uint32_t posType;
	uint32_t numObs;
	float cutoff;

	fseek( binLog, bodyStart, SEEK_SET ); // # obs at the start of the body
	fread( &solStatus, TRACK_SOL_STATUS_B, 1, binLog );
	fread( &posType, TRACK_POS_TYPE_B, 1, binLog );
	fread( &cutoff, TRACK_CUTOFF_B, 1, binLog );

	fread( &numObs, TRACK_NUMOBS_B, 1, binLog );

	if( numObs == 0 ){
		dataStruct->numObs = 0;
		dataStruct->trackstatObsBlock = NULL;
		return 0; // Best not try an allocate zero memory
	}

	// Allocate memory for the obersvation structs
	trackstatDataObsSt* trackstatObsBlock = 
	 (trackstatDataObsSt*) malloc( sizeof(trackstatDataObsSt)*numObs );
	if( !trackstatObsBlock ) // Error allocating memory
		return TRACKSTAT_MALLOC_FAIL;

	uint32_t i; 
	for(i = 0; i < numObs; i++){
		// PRN
		fread( &(trackstatObsBlock+i)->prn, TRACK_PRN_B, 1, binLog );
		// GLONASS frequency slot
		fread( &(trackstatObsBlock+i)->gloFreq, TRACK_GLOFREQ_B, 1, binLog );
		// Channel tracking status
		fread( &(trackstatObsBlock+i)->chanStat, TRACK_CHANSTAT_B, 1, binLog );
		// Pseudorange
		fread( &(trackstatObsBlock+i)->psr, TRACK_PSR_B, 1, binLog );
		// Doppler
		fread( &(trackstatObsBlock+i)->dopp, TRACK_DOPP_B, 1, binLog );
		// Carrier to noise density ratio
		fread( &(trackstatObsBlock+i)->cn0, TRACK_CN0_B, 1, binLog );
		// Lock time
		fread( &(trackstatObsBlock+i)->lockTime, TRACK_LOCKTIME_B, 1, binLog );
		// Pseudorange Residuals
		fread( &(trackstatObsBlock+i)->psrResidual, TRACK_PSR_RES_B, 1, binLog );
		// Reject code
		fread( &(trackstatObsBlock+i)->reject, TRACK_REJECT_B, 1, binLog );
		// Pseudorange filter weight
		fread( &(trackstatObsBlock+i)->psrWeight, TRACK_PSR_WGHT_B, 1, binLog );

		#if PARSE_VERBOSE // PRINT the header output
		fprintf(stdout, "\nTrackStat Fields:");
		fprintf(stdout, " observatrion %u of %u\n", i+1, numObs);
		fprintf(stdout, "  Signal Type: %s\n", 
		 decodeChanSignalStr( (trackstatObsBlock+i)->chanStat) );
		fprintf(stdout, "  PRN/slot: %hu\n", (trackstatObsBlock+i)->prn);
		fprintf(stdout, "  GLONASS Freq: %hu\n", (trackstatObsBlock+i)->gloFreq);
		fprintf(stdout, "  Pseudorange: %f\n", (trackstatObsBlock+i)->psr);
		fprintf(stdout, "  Doppler: %f\n", (trackstatObsBlock+i)->dopp);
		fprintf(stdout, "  CN0: %f\n", (trackstatObsBlock+i)->cn0);
		fprintf(stdout, "  Lock Time: %f\n", (trackstatObsBlock+i)->lockTime);
		fprintf(stdout, "  Residual: %f\n", (trackstatObsBlock+i)->psrResidual);
		fprintf(stdout, "  Rejection code: %s\n", 
		 decodeReject( (trackstatObsBlock+i)->psrResidual ) );
		fprintf(stdout, "  Weight: %f\n", (trackstatObsBlock+i)->psrWeight);
		fprintf(stdout, "  Solution Status: %s\n", decodeSolnStatus( solStatus ));
		fprintf(stdout, "  Position Type: %s\n", decodePosType( posType ));
		#endif
	}

	dataStruct->solStatus = solStatus;
	dataStruct->posType = posType;
	dataStruct->cutoff = cutoff;
	dataStruct->numObs = numObs;

	dataStruct->trackstatObsBlock = trackstatObsBlock;

	return (int)numObs;
}

int clearTrackstatData( trackstatDataSt* dataStruct ){
	uint32_t numObs = dataStruct->numObs;
	if( dataStruct->trackstatObsBlock != NULL ){
		free( dataStruct->trackstatObsBlock );
		dataStruct->trackstatObsBlock = NULL;

		dataStruct->solStatus = 0x00;
		dataStruct->posType = 0x00;
		dataStruct->cutoff = 0.0;
		dataStruct->numObs = 0;
	}
	return numObs;
}

void write_trackstat_GPS_col(FILE* trackstatFile, 
 headerDataSt* headerData, trackstatDataSt* trackstatData)
{
	unsigned int obs;
	for(obs = 0; obs < trackstatData->numObs; obs++){
		if( decodeChanSystem((trackstatData->trackstatObsBlock + obs)->chanStat) ==
		 CHAN_STAT_GPS_ID ){ // Verify it is a GPS log
			fprintf( trackstatFile, "%5u %9u%2u %2u % .15E % .15E % .6E %2.4f \
%2u %3u %3u %3u\n",
			 headerData->weekNum, headerData->gpst, 
			 (trackstatData->trackstatObsBlock + obs)->prn,
			 (trackstatData->trackstatObsBlock + obs)->psr,
			 (trackstatData->trackstatObsBlock + obs)->dopp,
			 (trackstatData->trackstatObsBlock + obs)->cn0,
			 (trackstatData->trackstatObsBlock + obs)->lockTime,
			 (trackstatData->trackstatObsBlock + obs)->reject,
			 decodeChanSignal((trackstatData->trackstatObsBlock + obs)->chanStat),
			 trackstatData->solStatus,
			 trackstatData->posType );
		}
	} 
}

void write_trackstat_GPS_csv(FILE* trackstatFile, 
 headerDataSt* headerData, trackstatDataSt* trackstatData)
{
	unsigned int obs;
	for(obs = 0; obs < trackstatData->numObs; obs++){
		if( decodeChanSystem((trackstatData->trackstatObsBlock + obs)->chanStat) == 
		 CHAN_STAT_GPS_ID ){ // Verify it is a GPS log
			fprintf( trackstatFile, "%5u,%9u,%2u,%.15E,%.15E,%.6E,%2.4f,%2u,\
%3u,%3u,%3u\n",
			 headerData->weekNum, headerData->gpst, 
			 (trackstatData->trackstatObsBlock + obs)->prn,
			 (trackstatData->trackstatObsBlock + obs)->psr,
			 (trackstatData->trackstatObsBlock + obs)->dopp,
			 (trackstatData->trackstatObsBlock + obs)->cn0,
			 (trackstatData->trackstatObsBlock + obs)->lockTime,
			 (trackstatData->trackstatObsBlock + obs)->reject,
			 decodeChanSignal((trackstatData->trackstatObsBlock + obs)->chanStat),
			 trackstatData->solStatus,
			 trackstatData->posType );
		}
	} 
}


#include <stdio.h>
#include <inttypes.h>
#include "parseNovAtel.h"
#include "decoderLibrary.h"

#define BESTPOS_SOLSTAT_B 4
#define BESTPOS_SOLSTAT_O 0

#define BESTPOS_POSTYPE_B 4
#define BESTPOS_POSTYPE_O 4

#define BESTPOS_LAT_B 8
#define BESTPOS_LAT_O 8

#define BESTPOS_LON_B 8
#define BESTPOS_LON_O 16

#define BESTPOS_HEIGHT_B 8
#define BESTPOS_HEIGHT_O 24

#define BESTPOS_LATSTD_B 4
#define BESTPOS_LATSTD_O 40

#define BESTPOS_LONSTD_B 4
#define BESTPOS_LONSTD_O 44

#define BESTPOS_HEIGHTSTD_B 4
#define BESTPOS_HEIGHTSTD_O 48

#define BESTPOS_TRACKSV_B 1
#define BESTPOS_TRACKSV_O 64

#define BESTPOS_SOLNSV_B 1
#define BESTPOS_SOLNSV_O 65

#define BESTPOS_SOLNSVL1_B 1
#define BESTPOS_SOLNSVL1_O 66

int parseBestpos(FILE* binLog, bestposDataSt* dataStruct, long int bodyStart)
{
	// Solution Status
	fseek(binLog, bodyStart+BESTPOS_SOLSTAT_O, SEEK_SET);
	fread(&dataStruct->solStat, BESTPOS_SOLSTAT_B, 1, binLog);
	// Position Type
	fseek(binLog, bodyStart+BESTPOS_POSTYPE_O, SEEK_SET);
	fread(&dataStruct->posType, BESTPOS_POSTYPE_B, 1, binLog);

	// Latitude 
	fseek(binLog, bodyStart+BESTPOS_LAT_O, SEEK_SET);
	fread(&dataStruct->lat, BESTPOS_LAT_B, 1, binLog);
	// Longitude 
	fseek(binLog, bodyStart+BESTPOS_LON_O, SEEK_SET);
	fread(&dataStruct->lon, BESTPOS_LON_B, 1, binLog);
	// Height 
	fseek(binLog, bodyStart+BESTPOS_HEIGHT_O, SEEK_SET);
	fread(&dataStruct->height, BESTPOS_HEIGHT_B, 1, binLog);

	// Latitude StDev
	fseek(binLog, bodyStart+BESTPOS_LATSTD_O, SEEK_SET);
	fread(&dataStruct->latStd, BESTPOS_LATSTD_B, 1, binLog);
	// Longitude StDev
	fseek(binLog, bodyStart+BESTPOS_LONSTD_O, SEEK_SET);
	fread(&dataStruct->lonStd, BESTPOS_LONSTD_B, 1, binLog);
	// Height  StDev
	fseek(binLog, bodyStart+BESTPOS_HEIGHTSTD_O, SEEK_SET);
	fread(&dataStruct->heightStd, BESTPOS_HEIGHTSTD_B, 1, binLog);

	// Tracked Satellites
	fseek(binLog, bodyStart+BESTPOS_TRACKSV_O, SEEK_SET);
	fread(&dataStruct->trackSv, BESTPOS_TRACKSV_B, 1, binLog);
	// Satellites used in solution
	fseek(binLog, bodyStart+BESTPOS_SOLNSV_O, SEEK_SET);
	fread(&dataStruct->solnSv, BESTPOS_SOLNSVL1_B, 1, binLog);
	// Satellites used in solution with L1 signals
	fseek(binLog, bodyStart+BESTPOS_SOLNSV_O, SEEK_SET);
	fread(&dataStruct->solnSvL1, BESTPOS_SOLNSVL1_B, 1, binLog);
	// Satellites used in solution with L1 signals
	fseek(binLog, bodyStart+BESTPOS_SOLNSV_O, SEEK_SET);
	fread(&dataStruct->solnSvL1, BESTPOS_SOLNSVL1_B, 1, binLog);

	#if PARSE_VERBOSE
	fprintf(stdout, "\nBESTPOS Fields:\n");
	fprintf(stdout, "  Solution Status: %u; %s\n", dataStruct->solStat,
	 decodeSolnStatus(dataStruct->solStat));
	fprintf(stdout, "  Position Type: %u; %s\n", dataStruct->posType,
	 decodePosType(dataStruct->posType));
	fprintf(stdout, "  Latitude: %f\n", dataStruct->lat);
	fprintf(stdout, "  Longitude: %f\n", dataStruct->lon);
	fprintf(stdout, "  Height: %f\n", dataStruct->height);
	fprintf(stdout, "  Latitude Std: %f\n", dataStruct->latStd);
	fprintf(stdout, "  Longitude Std: %f\n", dataStruct->lonStd);
	fprintf(stdout, "  Height Std: %f\n", dataStruct->heightStd);
	fprintf(stdout, "  Tracked Sv: %hhu\n", dataStruct->trackSv);
	fprintf(stdout, "  Soln Sv: %hhu\n", dataStruct->solnSv);
	fprintf(stdout, "  Soln L1 Sv: %hhu\n", dataStruct->solnSvL1);
	#endif
}


void write_bestpos_GPS_essential_col(FILE* bestposFile, 
 headerDataSt* headerData, bestposDataSt* bestposData)
{
	fprintf( bestposFile, "%5u %10u % .15E % .15E % .15E %2hhu\n",
	 headerData->weekNum, headerData->gpst,
	 bestposData->lat,
	 bestposData->lon,
	 bestposData->height,
	 bestposData->solnSv );
}

void write_bestpos_GPS_essential_csv(FILE* bestposFile, 
 headerDataSt* headerData, bestposDataSt* bestposData)
{
	fprintf( bestposFile, "%u,%u,%.15E,%.15E,%.15E,%hhu\n",
	 headerData->weekNum, headerData->gpst,
	 bestposData->lat,
	 bestposData->lon,
	 bestposData->height,
	 bestposData->solnSv );
}

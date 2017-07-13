#include <stdio.h>
#include "parseNovAtel.h"

#define BESTPOS_LENGTH 76

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
	fseek(binLog, bodyStart+BESTPOS_SOLSTAT_B, SEEK_SET);
	fread(&dataStruct->solStat, BESTPOS_SOLSTAT_O, 1, binLog);
	// Position Type
	fseek(binLog, bodyStart+BESTPOS_POSTYPE_B, SEEK_SET);
	fread(&dataStruct->posType, BESTPOS_POSTYPE_O, 1, binLog);

	// Latitude 
	fseek(binLog, bodyStart+BESTPOS_LAT_B, SEEK_SET);
	fread(&dataStruct->lat, BESTPOS_LAT_O, 1, binLog);
	// Longitude 
	fseek(binLog, bodyStart+BESTPOS_LON_B, SEEK_SET);
	fread(&dataStruct->lon, BESTPOS_LON_O, 1, binLog);
	// Height 
	fseek(binLog, bodyStart+BESTPOS_HEIGHT_B, SEEK_SET);
	fread(&dataStruct->height, BESTPOS_HEIGHT_O, 1, binLog);

	// Latitude StDev
	fseek(binLog, bodyStart+BESTPOS_LATSTD_B, SEEK_SET);
	fread(&dataStruct->latStd, BESTPOS_LATSTD_O, 1, binLog);
	// Longitude StDev
	fseek(binLog, bodyStart+BESTPOS_LONSTD_B, SEEK_SET);
	fread(&dataStruct->lonStd, BESTPOS_LONSTD_O, 1, binLog);
	// Height  StDev
	fseek(binLog, bodyStart+BESTPOS_HEIGHTSTD_B, SEEK_SET);
	fread(&dataStruct->heightStd, BESTPOS_HEIGHTSTD_O, 1, binLog);

	// Tracked Satellites
	fseek(binLog, bodyStart+BESTPOS_TRACKSV_B, SEEK_SET);
	fread(&dataStruct->trackSv, BESTPOS_TRACKSV_O, 1, binLog);
	// Satellites used in solution
	fseek(binLog, bodyStart+BESTPOS_SOLNSV_B, SEEK_SET);
	fread(&dataStruct->solnSv, BESTPOS_SOLNSVL1_O, 1, binLog);
	// Satellites used in solution with L1 signals
	fseek(binLog, bodyStart+BESTPOS_SOLNSV_B, SEEK_SET);
	fread(&dataStruct->solnSvL1, BESTPOS_SOLNSVL1_O, 1, binLog);
	// Satellites used in solution with L1 signals
	fseek(binLog, bodyStart+BESTPOS_SOLNSV_B, SEEK_SET);
	fread(&dataStruct->solnSvL1, BESTPOS_SOLNSVL1_O, 1, binLog);
}

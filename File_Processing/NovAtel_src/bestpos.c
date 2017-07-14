#include <stdio.h>
#include <inttypes.h>
#include "parseNovAtel.h"

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


void write_BESTPOS_GPS_essential_col(FILE* bestposFile, 
 headerDataSt* headerData, bestposDataSt* bestposData)
{
	fprintf( bestposFile, "%5u %10u % .15E % .15E % .15E %2hhu\n",
	 headerData->weekNum, headerData->gpst,
	 bestposData->lat,
	 bestposData->lon,
	 bestposData->height,
	 bestposData->solnSv );
}

void write_BESTPOS_GPS_essential_csv(FILE* bestposFile, 
 headerDataSt* headerData, bestposDataSt* bestposData)
{
	fprintf( bestposFile, "%u,%u,%.15E,%.15E,%.15E,%hhu\n",
	 headerData->weekNum, headerData->gpst,
	 bestposData->lat,
	 bestposData->lon,
	 bestposData->height,
	 bestposData->solnSv );
}

const char* decodeSolnStatus(uint32_t solStat)
{
	switch(solStat){
		case 0: // SOL_COMPUTED
			return "Solution Computed";
		case 1: // INSUFFICIENT_OBS
			return "Insufficient Observations";
		case 2: // NO_CONVERGENCE
			return "No convergence";
		case 3: // SINGULARITY
			return "Singularity at parameters matrix";
		case 4: // COV_TRACE
			return "Covariance trace exceeds maximum (trace > 1000 m)";
		case 5: // TEST_DIST
			return "Test distance exceeds (maximum of 3 rejections if distance > 10 km)";
		case 6: // COLD_START
			return "Not yet converged from cold start";
		case 7: // V_H_LIMIT
			return "Height or velocity limits exceeded (what on God's green Earth are you doing with this receiver?)";
		case 8: // VARIANCE
			return "Variance exceeds limits";
		case 9: // RESIDUALS
			return "Residuals are too large";
		case 13: // INTEGRITY_WARNING
			return "Large residuals make position unreliable";
		case 18: // PENDING
			return "When a \"FIX POSITION\" command is entered, the receiver computes its own position and determines if the fixed position is valid";
		case 19: // INVALID_FIX
			return "The fixed position, entered using the \"FIX POSITION\" command, is not valid";
		case 20: // UNAUTHORIZED
			return "Position type is unauthorized - HP or XP on a receiver not authroized for it";
		default:
			return "Invalid solution status code";
	}
}

const char* decodePosType(uint32_t posType)
{
	switch(posType){
		case 0: // NONE
			return "No Solution";
		case 1: // FIXEDPOS
			return "Position has been fixed by the \"FIX POSITION\" command";
		case 2: // FIXEDHEIGHT
			return "Position has been fixed by the \"FIX HEIGHT/AUTO\" command";
		case 8:	// DOPPLER_VELOCITY
			return "Velocity is computed using instantaneous Dopppler";
		case 16: // SINGLE
			return "Single point position";
		case 17: // PSRDIFF
			return "Pseudorange differential solution";
		case 18: // WAAS
			return "Solution calculated using corrections from a WAAS";
		case 19: // PROPAGATED 
			return "Propagated by a Kalman filter without new observations";
		case 20: // OMNISTAR
			return "OmniSTAR VBS position";
		case 32: // L1_FLOAT
			return "Floating L1 ambiguity solution";
		case 33: // IONOFREE_FLOAT
			return "Floating ionosphereic-free ambiguity solution";
		case 34: // NARROW_FLOAT
			return "Floating narrow-lane ambiguity solution";
		case 48: // L1_INT
			return "Integer L1 ambiguity solution";
		case 50: // NARROW_INT
			return "Integer narrow-lane ambiguity solution";
		case 64: // OMNISTAR_HP
			return "OmniSTAR HP position";
		case 65: // OMNISTAR_XP
			return "OmniSTAR XP position";
		case 68: // PPP_CONVERGIN
			return "Converging TerraStar-C solution";
		case 69: // PPP
			return "TerraStar-C solution";
		case 70: // OPERATIONAL
			return "Solution accuracy is within UAL operational limit";
		case 71: // WARNING
			return "Solution accuracy is outside UAL operational limit but within warning limit";
		case 72: // OUT_OF_BOUNDS
			return "Solution accuracy is outside UAL operational limits";
		case 77: // PPT_BASIC_CONVERGING
			return "Converging TerraStar-L solution";
		case 78: // PPT_BASIC
			return "Converged TerraStar-L solution";
		default:
			return "Invalid position type code";
	}
}

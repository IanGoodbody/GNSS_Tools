#ifndef PARSE_NOVATEL_H
#define PARSE_NOVATEL_H

#include <inttypes.h>

#define PARSE_VERBOSE 0 // All parsed data is written to stdout

int checkCrc(FILE* binFile, long int logStart, int blockSize);

// Header fields

#define HEAD_GOOD 0
#define HEAD_BAD_SYNC 1 
#define HEAD_ASCII_SYNC 2

typedef struct{
	uint8_t  headLen;
	uint16_t msgID;
	uint8_t  msgType;
	uint16_t msgLen;
	uint8_t  idle;
	uint16_t weekNum;
	uint32_t gpst; // ms
	uint8_t  timeStat;
	uint32_t rcvrStat;
} headerDataSt;

int parseHeader(FILE* binFile, headerDataSt* headerData, long int logStart);
void indicateRcvrStatus(headerDataSt* headerData);
const char* decodeTimeStatus(uint8_t timeStat);

/* 
 * BESTPOS: Best LLH Position 
 */
#define BESTPOS_ID 42

typedef struct{
	uint32_t solStat;
	uint32_t posType;
	double lat; // deg
	double lon; // deg
	double height; // m
	float latStd; // deg
	float lonStd; // deg
	float heightStd; // m
	uint8_t trackSv;
	uint8_t solnSv;
	uint8_t solnSvL1;
}	bestposDataSt;

int parseBestpos(FILE* binFile, bestposDataSt* dataStruct, long int bodyStart);
const char* decodePosType(uint32_t posType);
const char* decodeSolnStatus(uint32_t posType);

void write_BESTPOS_GPS_essential_col(FILE*, headerDataSt*, bestposDataSt*);
void write_BESTPOS_GPS_essential_csv(FILE*, headerDataSt*, bestposDataSt*);

/* 
 * RANGE: "raw" gnss measurements (code, carrier, CN0)
 */
#define RANGE_ID 43
#define RANGE_MALLOC_FAIL -1

// Each range log has a separate elment for each observation
typedef struct{
	uint16_t prn;
	uint16_t gloFreq;
	double psr; // m
	float psrStd; // m
	double carrier; // cycles
	float carrierStd; // cycles
	float dopp; // Hz
	float cn0; // dB-Hz
	float lockTime; // s
	uint32_t chanStat;
} rangeDataObsSt;
// The data structure holds the number of obersvations and a pointer to the 
//  block of memory storing all the observations, malloc away!
typedef struct{ 
	uint32_t numObs;
	rangeDataObsSt* rangeObsBlock;
} rangeDataSt;

int parseRange(FILE* binLog, rangeDataSt* dataStruct, long int bodyStart);
int clearRangeData(rangeDataSt* dataStruct);
#define decodeRangeSystem(chanStat) (chanStat & 0x00070000) >> 16
#define decodeRangeSignal(chanStat) (chanStat & 0x03E00000) >> 21
const char* decodeRangeSignalStr(uint32_t chanStat);

void write_RANGE_GPS_essential_col(FILE*, headerDataSt*, rangeDataSt*);
void write_RANGE_GPS_essential_csv(FILE*, headerDataSt*, rangeDataSt*);

/* 
 * RAWEPHEM: GPS ephemeris data
 */
#define RAWEPHEM_ID 41

/* 
 * RAWCNAVFRAME: raw CNAV Frame data
 */
#define RAWCNAVFRAME_ID 1066

typedef struct{
	uint16_t prn;
	uint16_t frameID;
	uint16_t weekNum;
	uint8_t health;
	double Toe
	double A;
	double ADot;
	double dN;
	double dNDot;
	double M0;
	double e;
	double omega;
	double Omega0;
	double OmegaDot;
	double i;
	double iDot;
	double Cis;
	double Cic;
	double Crs;
	double Crc;
	double Cus;
	double Cuc;
	double Tgd;
	double iscL1CA;
	double iscL2C;
	double iscL5I;
	double iscL5Q;
	double Toc;
	double A0;
	double A1;
	double A2;
} rawcnavframeDataSt;

#endif

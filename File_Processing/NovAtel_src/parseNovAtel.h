#ifndef PARSE_NOVATEL_H
#define PARSE_NOVATEL_H

int checkCrc(FILE* binFile, long int logStart, int blockSize);

// Header fields

#define HEAD_GOOD 0
#define HEAD_BAD_SYNC 1 
#define HEAD_ASCII_SYNC 2

typedef struct{
	unsigned char headLen;
	unsigned short msgID;
	unsigned char msgType;
	unsigned short msgLen;
	unsigned short weekNum;
	unsigned int gpst;
	unsigned char timeStat;
	unsigned int rcvrStat;
} headerDataSt;

int parseHeader(FILE* binFile, headerDataSt* headerData, long int logStart);
void indicateRcvrStatus(headerDataSt* headerData);
void inidcateTimeStatus(headerDataSt* headerData);

// BESTPOS: Best LLH Position
#define BESTPOS_ID 42

typedef struct{
	unsigned int solStat;
	unsigned int posType;
	double lat;
	double lon;
	double height;
	float latStd;
	float lonStd;
	float heightStd;
	unsigned char trackSv;
	unsigned char solnSv;
	unsigned char solnSvL1;
}	bestposDataSt;

int parseBestpos(FILE* binFile, bestposDataSt* dataStruct, long int bodyStart);

// RANGE: "raw" gnss measurements (code, carrier, CN0)
#define RANGE_ID 43

// RAWEPHEM: GPS ephemeris data
#define RAWEPHEM_ID 41

// RAWCNAVFRAME: raw CNAV Frame data
#define RAWCNAVFRAME_ID 1066
#endif

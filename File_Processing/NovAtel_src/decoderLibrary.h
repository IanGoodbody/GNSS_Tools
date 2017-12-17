#ifndef DECODER_LIBRARY_H
#define DECODER_LIBRARY_H

#include <inttypes.h>

#define CHAN_STAT_GPS_ID     0
#define CHAN_STAT_GLONASS_ID 1
#define CHAN_STAT_SBAS_ID    2
#define CHAN_STAT_GALILEO_ID 3
#define CHAN_STAT_BEIDOU_ID  4
#define CHAN_STAT_QZSS_ID    5
#define CHAN_STAT_OTHER_ID   7

#define decodeChanSystem(chanStat) (chanStat & 0x00070000) >> 16
#define decodeChanSignal(chanStat) (chanStat & 0x03E00000) >> 21

const char* decodeChanSignalStr( uint32_t chanStat );
const char* decodePosType( uint32_t posType );
const char* decodeSolnStatus( uint32_t solStat );
const char* decodeReject( uint32_t  reject );

#endif

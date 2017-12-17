#include <inttypes.h>
#include "decoderLibrary.h"

const char* decodeChanSignalStr( uint32_t chanStat )
{
	switch( decodeChanSystem( chanStat ) ){
		case CHAN_STAT_GPS_ID:
			switch( decodeChanSignal( chanStat ) ){
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
		case CHAN_STAT_GLONASS_ID:
			switch( decodeChanSignal( chanStat ) ){
				case 0:
					return "GLONASS L1C/A";
				case 1:
					return "GLONASS L2C/A";
				case 5:
					return "GLONASS L2P";
				default:
					return "Undocumented GLONASS Signal";
			}
		case CHAN_STAT_SBAS_ID:
			switch( decodeChanSignal( chanStat ) ){
				case 0:
					return "SBAS L1C/A";
				case 6:
					return "SBAS L5I";
				default:
					return "Undocumentd SBAS Signal";
			}
		case CHAN_STAT_GALILEO_ID:
			switch( decodeChanSignal( chanStat ) ){
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
		case CHAN_STAT_BEIDOU_ID:
			switch( decodeChanSignal( chanStat ) ){
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
		case CHAN_STAT_QZSS_ID:
			switch( decodeChanSignal( chanStat ) ){
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

const char* decodePosType( uint32_t posType )
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

const char* decodeSolnStatus( uint32_t solStat )
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

const char* decodeReject( uint32_t  reject )
{
	switch(reject){
		case 0: // GOOD
			return "Observation good";
		case 1: // BADHEALTH
			return "Eph or almanac bad health";
		case 2: // OLDEPHEMERIS
			return "Old ephemeris";
		case 6: // ELEVATIONERROR
			return "SV below cutoff";
		case 7: // MISCLOSURE
			return "Observation too far from predicted";
		case 8: // NODIFFCORR
			return "No differential correction";
		case 9: // NOEPHEMERIS
			return "No ephemeris available";
		case 10: // INVALIDIODE
			return "IODE is invalid";
		case 11: // LOCKEDOUT
			return "Satellit is locked out";
		case 12: // LOWPOWER
			return "Low signal power";
		case 13: // OBSL2
			return "L2 observation not used";
		case 15: // Unknown
			return "Unknown observation";
		case 16: // NOIONOCORR
			return "No ionospheric correction availabe";
		case 17: // NOTUSED
			return "Not used in solution";
		case 18: // OBSL1
			return "L1 observation unused";
		case 19: // OBSE1
			return "E1 observation unused";
		case 20: // OBSL5
			return "L5 observation unused";
		case 21: // OBSE5
			return "E5 observation unused";
		case 22: // OBSB2
			return "B2 observation unused";
		case 23: // OBSB1
			return "B1 observation unused";
		case 25: // NOSIGNALMATCH
			return "Signal type does not match";
		case 26: // SUPPLEMENTARY
			return "Only used as suplemental information";
		case 99: // NA
			return "No observation available";
		case 100: // BAD_INTEGRITY
			return "Unused outlier observation";
		case 101: // LOSSOFLOCK
			return "Signal lock broken";
		case 102: // NOAMBIGUITY
			return "RTK ambuguity unresolved";
		default: // Unknown
			return "Invalid reject code";
	}
}

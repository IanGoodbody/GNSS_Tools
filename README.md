# GNSS Tools -- ALPHA
### A collection of useful MATLAB tools for GNSS data processing
Note: this software is still in alpha development stages; I am rappidly
devloping the features of this project and reserve the right to make 
substantial changes to the code base without seeking input or notifying 
anybody.



## Directory Structure
##### File\_Processing
Matlab tools for extracting data from rinex and (eventually) binary NovAtel
logs. 

- **getNewEphemeris.m** loads the most recent ephemeris data for all GPS 
satellites from the IGS repository at 
[ftp://cddis.gsfc.nasa.gov/gnss/data/hourly/](ftp://cddis.gsfc.nasa.gov/gnss/data/hourly/)
- **loadEphemeris.m** loads ephemeris data from a specified RINEX nav file
- **RinexObsToCSV.py** standalone python script that parses a RINEX 
observation file into a CSV with user-specified fields
- **NovAtel\_Src** This directory holds C files that can parse NovAtel binary
.gps files to extract BESTPOS and RANGE logs.  Eventually these can be built
as .mex files that can be called from matlab, but the C version is now
functional and can parse the data into colum or csv formats.  I haven't written
the documentation comments for it yet, come find me if you need to use this.

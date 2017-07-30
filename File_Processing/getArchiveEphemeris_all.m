function [gpsTags, gpsData, galTags, galData, gloTags, gloData, utcOffset] = ...
 getArchiveEphemeris_ALL(logDateUTC)
% GETATCHIVEPHEMERIS_ALL retrieves the broadcast Galileo ephemeris for 
% all SVs on the provided date from the IGS repository at cddis.gfsc.nasa.gov.
% The function will currently parse GALILEO, GLONASS,  and GPS ephemerides from
% a rinex 3 formatted text file
% The broadcast ephemeris has one day of latency.
%  [ephTags, ephData, utcOffset] = GETARCHIVEEPHEMERIS_ALL(logDateUTC)
%
% Parameters
%   logDateUTC : a matlab datetime() object specifying the UTC date for the 
%                 ephemeris record
% Returns
%   ???Tags   : Structure containing the column indeices for the output 
%               ephemeris patameters for the respective system
%   gpsData   : Nx32 array containing all the GPS ephemeris data in the file
%   galData   : Nx32 array containing all the GALILEO ephemeris data in the file
%   gloData   : Nx32 array containing all the GLONASS ephemeris data in the file
%   utcOffset : UTC time offset (s) UTC = GALT + utcOffset
	% Define indicies for ephemeris paramters

% GPS Tags
	gpsTags.system   =  1; % Enumerated type
	gpsTags.PRN      =  2; % Unitless
	gpsTags.Toe      =  3; % Seconds since the start of the week
	gpsTags.week     =  4; % Week number
	gpsTags.sqrtA    =  5; % m^{1/2}
	gpsTags.e        =  6; % Unitless
	gpsTags.i0       =  7; % rad
	gpsTags.Omega0   =  8; % rad
	gpsTags.omega    =  9; % rad
	gpsTags.M0       = 10; % rad
	gpsTags.DeltaN   = 11; % rad/s
	gpsTags.iDot     = 12; % rad/s
	gpsTags.OmegaDot = 13; % rad/s
	gpsTags.Cuc      = 14; % rad
	gpsTags.Cus      = 15; % rad
	gpsTags.Crc      = 16; % m
	gpsTags.Crs      = 17; % m
	gpsTags.Cic      = 18; % rad
	gpsTags.Cis      = 19; % rad
	gpsTags.Toc      = 20; % s
	gpsTags.A0       = 21; % s
	gpsTags.A1       = 22; % s/s
	gpsTags.A2       = 23; % s/s^2
	gpsTags.Tgd      = 24; % s
	gpsTags.fit      = 25; % hr; Ephemeris fit intereval, 0 if undknown
	gpsTags.GPST_y   = 26; % year
	gpsTags.GPST_m   = 27; % month
	gpsTags.GPST_d   = 28; % day
	gpsTags.GPST_h   = 29; % hr
	gpsTags.GPST_mi  = 30; % minute
	gpsTags.GPST_s   = 31; % s
	gpsTags.valid    = 32; % s

% GLONASS Tags
	gloTags.system    =  1; % Enum 2 for Glonass
	gloTags.slot      =  2;
	gloTags.freq      =  3; % Slot number
	gloTags.Toc       =  4; % Slot number
	gloTags.clkBias   =  5; % s
	gloTags.freqBias  =  6; % 
	gloTags.frameTime =  7; % s
	gloTags.x         =  8; % km
	gloTags.dx        =  9; % km/s
	gloTags.ddx       = 10; % km/s^2s
	gloTags.y         = 11; % km
	gloTags.dy        = 12; % km/s
	gloTags.ddy       = 13; % km/s^2s
	gloTags.z         = 14; % km
	gloTags.dz        = 15; % km/s
	gloTags.ddz       = 16; % km/s^2s
	gloTags.GLOT_y    = 17; % year
	gloTags.GLOT_m    = 18; % month
	gloTags.GLOT_d    = 19; % day
	gloTags.GLOT_h    = 20; % hr
	gloTags.GLOT_mi   = 21; % minute
	gloTags.GLOT_s    = 22; % s
	gloTags.valid     = 23;

% Galileo Tags
	galTags.system   =  1; % Enumerated type 3 for Galileo
	galTags.SVID     =  2; % Unitless
	galTags.Toe      =  3; % Seconds since the start of the week
	galTags.week     =  4; % Week number
	galTags.sqrtA    =  5; % m^{1/2}
	galTags.e        =  6; % Unitless
	galTags.i0       =  7; % rad
	galTags.Omega0   =  8; % rad
	galTags.omega    =  9; % rad
	galTags.M0       = 10; % rad
	galTags.DeltaN   = 11; % rad/s
	galTags.iDot     = 12; % rad/s
	galTags.OmegaDot = 13; % rad/s
	galTags.Cuc      = 14; % rad
	galTags.Cus      = 15; % rad
	galTags.Crc      = 16; % m
	galTags.Crs      = 17; % m
	galTags.Cic      = 18; % rad
	galTags.Cis      = 19; % rad
	galTags.Toc      = 20; % s
	galTags.A0       = 21; % s
	galTags.A1       = 22; % s/s
	galTags.A2       = 23; % s/s^2
	galTags.bgdA     = 24; % s
	galTags.bgdB     = 25; % hr; Ephemeris fit intereval, 0 if undknown
	galTags.GALT_y   = 26; % year
	galTags.GALT_m   = 27; % month
	galTags.GALT_d   = 28; % day
	galTags.GALT_h   = 29; % hr
	galTags.GALT_mi  = 30; % minute
	galTags.GALT_s   = 31; % s
	galTags.valid    = 32; % s

	year2D = logDateUTC.Year-2000;
	if year2D < 0 % Two digit year used for file name lookup
		year2D = year2D + 100;
	end % 2 digit year if

	yearDays = floor(days(logDateUTC - datetime(logDateUTC.Year, 1, 1))) + 1;
	ephFile = sprintf('brdm%03i0.%02ip', yearDays, year2D);
	if exist( strcat(ephFile, '.mat'), 'file')
		load(strcat(ephFile, '.mat'));
	else
		fprintf('Updating %s\n', strcat(ephFile, '.mat'));
	
		% Download data from NASA's IGS MGEX repository
		ftpFolder = sprintf('/gnss/data/campaign/mgex/daily/rinex3/%04i/brdm/', ...
		 logDateUTC.Year);
		ftpID = ftp('cddis.gsfc.nasa.gov', 'anonymous', '');
		cd(ftpID, ftpFolder);
		mget(ftpID, strcat(ephFile, '.Z'));
		close(ftpID);
		fprintf('%s downloaded...\n', strcat(ephFile, '.Z'));
	
		system(sprintf('gunzip %s.Z', ephFile)); % Assums unix system
		fprintf('%s unpacked...\n', ephFile);
		
		% Open the RINEX file for parsing
		fileID = fopen(ephFile, 'r');
		% Find start of data
		line = fgetl(fileID);
		utcOffset = 0;
		while ~strncmp(line(61:end), 'END OF HEADER', 13)
			if strncmp(line(61:end), 'LEAP SECONDS', 12)
				utcOffset = str2num(line(1:6));
			end
			line = fgetl(fileID);
		end
		dataStart = ftell(fileID);
		% Scan though and count the lines to determine the number of records
		gpsRecords = 0;
		galRecords = 0;
		gloRecords = 0;
		line = fgetl(fileID);
		while line ~= -1
			line = fgetl(fileID);
			if line(1) == 'G'
				gpsRecords = gpsRecords + 1;
			elseif line(1) == 'R'
				gloRecords = gloRecords + 1;
			else if line(1) == 'E'
				galRecords = galRecords + 1;
			end
		end
	
		% Begin populating the fields of ephData
		gpsData = zeros(gpsRecords, length(fieldnames(gpsTags)));
		gloData = zeros(gloRecords, length(fieldnames(gloTags)));
		galData = zeros(galRecords, length(fieldnames(galTags)));

		gpsInd = 1;
		gloInd = 1;
		galInd = 1;

		fseek(fileID, dataStart, -1);
		line = fgetl(fileID);
		while line ~= -1
			switch line(1)
				case 'G'
					gpsData(gpsInd, gpsTags.system) = 1; % 1 for GPS
					% Line 1: PRN, Toc, clock polynomial terms
					gpsData(gpsInd, gpsTags.GPST_y)  = str2num(line( 5: 8));
					gpsData(gpsInd, gpsTags.GPST_m)  = str2num(line(10:11));
					gpsData(gpsInd, gpsTags.GPST_d)  = str2num(line(13:14));
					gpsData(gpsInd, gpsTags.GPST_h)  = str2num(line(16:17));
					gpsData(gpsInd, gpsTags.GPST_mi) = str2num(line(19:20));
					gpsData(gpsInd, gpsTags.GPST_s)  = str2num(line(22:23));

					tocDate = datetime(str2num(line(5:8)), str2num(line(10:11)), ...
					 str2num(line(13:14)), str2num(line(16:17)), str2num(line(19:20)), ...
					 str2num(line(22:23)));

					gpsData(gpsInd, gpsTags.Toc) = (weekday(tocDate)-1)*86400 + ...
					 tocDate.Hour*3600 + tocDate.Minute*60 + tocDate.Second;
	
					gpsData(gpsInd, gpsTags.PRN) = str2num(line( 2: 3));
					gpsData(gpsInd, gpsTags.A0)  = str2num(line(24:42));
					gpsData(gpsInd, gpsTags.A1)  = str2num(line(43:61));
					gpsData(gpsInd, gpsTags.A2)  = str2num(line(62:80));
					% Line 2: parameters
					line = fgetl(fileID);
					gpsData(gpsInd, gpsTags.Crs)    = str2num(line(24:42));
					gpsData(gpsInd, gpsTags.DeltaN) = str2num(line(43:61));
					gpsData(gpsInd, gpsTags.M0)     = str2num(line(62:80));
					% Line 3: parameters
					line = fgetl(fileID);
					gpsData(gpsInd, gpsTags.Cuc)   = str2num(line( 5:23));
					gpsData(gpsInd, gpsTags.e)     = str2num(line(24:42));
					gpsData(gpsInd, gpsTags.Cus)   = str2num(line(43:61));
					gpsData(gpsInd, gpsTags.sqrtA) = str2num(line(62:80));
					% Line 4: parameters
					line = fgetl(fileID);
					gpsData(gpsInd, gpsTags.Toe)    = str2num(line( 5:23));
					gpsData(gpsInd, gpsTags.Cic)    = str2num(line(24:42));
					gpsData(gpsInd, gpsTags.Omega0) = str2num(line(43:61));
					gpsData(gpsInd, gpsTags.Cis)    = str2num(line(62:80));
					% Line 5: parameters
					line = fgetl(fileID);
					gpsData(gpsInd, gpsTags.i0)       = str2num(line( 5:23));
					gpsData(gpsInd, gpsTags.Crc)      = str2num(line(24:42));
					gpsData(gpsInd, gpsTags.omega)    = str2num(line(43:61));
					gpsData(gpsInd, gpsTags.OmegaDot) = str2num(line(62:80));
					% Line 6: parameters
					line = fgetl(fileID);
					gpsData(gpsInd, gpsTags.iDot) = str2num(line( 5:23));
					gpsData(gpsInd, gpsTags.week) = str2num(line(43:61));
					% Line 7: parameters
					line = fgetl(fileID);
					gpsData(gpsInd, gpsTags.valid) = str2num(line(24:42)) == 0;
					gpsData(gpsInd, gpsTags.Tgd)   = str2num(line(43:61));
					% Line 8: parameters
					line = fgetl(fileID);
					gpsData(gpsInd, gpsTags.fit) = str2num(line(24:42));

					gpsInd = gpsInd + 1;

				case 'R'
					gloData(gloInd, gloTags.system) = 2; % 2 for GLONASS
					% Line 1: Clock data
					gloData(gloInd, gloTags.GLOT_y)  = str2num(line( 5: 8));
					gloData(gloInd, gloTags.GLOT_m)  = str2num(line(10:11));
					gloData(gloInd, gloTags.GLOT_d)  = str2num(line(13:14));
					gloData(gloInd, gloTags.GLOT_h)  = str2num(line(16:17));
					gloData(gloInd, gloTags.GLOT_mi) = str2num(line(19:20));
					gloData(gloInd, gloTags.GLOT_s)  = str2num(line(22:23));

					tocDate = datetime(str2num(line(5:8)), str2num(line(10:11)), ...
					 str2num(line(13:14)), str2num(line(16:17)), str2num(line(19:20)), ...
					 str2num(line(22:23)));
					gloData(gloInd, gloTags.Toc) = (weekday(tocDate)-1)*86400 + ...
					 tocDate.Hour*3600 + tocDate.Minute*60 + tocDate.Second;

					gloData(gloInd, gloTags.slot)      = str2num(line( 2: 3));
					gloData(gloInd, gloTags.clkBias)   = str2num(line(24:42));
					gloData(gloInd, gloTags.freqBias)  = str2num(line(43:61));
					gloData(gloInd, gloTags.frameTime) = str2num(line(62:80));
					% Line 2: X paramters
					line = fgetl(fileID);
					gloData(gloInd, gloTags.x)     = str2num(line( 5:23));
					gloData(gloInd, gloTags.dx)    = str2num(line(24:42));
					gloData(gloInd, gloTags.ddx)   = str2num(line(43:61));
					gloData(gloInd, gloTags.valid) = str2num(line(62:80)) == 0;
					% Line 3: Y parameters
					line = fgetl(fileID);
					gloData(gloInd, gloTags.y)     = str2num(line( 5:23));
					gloData(gloInd, gloTags.dy)    = str2num(line(24:42));
					gloData(gloInd, gloTags.ddy)   = str2num(line(43:61));
					gloData(gloInd, gloTags.freq)  = str2num(line(62:80));
					% Line 4: Z parameters
					line = fgetl(fileID);
					gloData(gloInd, gloTags.z)     = str2num(line( 5:23));
					gloData(gloInd, gloTags.dz)    = str2num(line(24:42));
					gloData(gloInd, gloTags.ddz)   = str2num(line(43:61));

					gloInd = gloInd + 1;

				case 'E'
					galData(galInd, galTags.system) = 3; % 3 for GALILEO
					% Line 1: PRN, Toc, clock polynomial terms
					galData(galInd, galTags.GALT_y)  = str2num(line( 5: 8));
					galData(galInd, galTags.GALT_m)  = str2num(line(10:11));
					galData(galInd, galTags.GALT_d)  = str2num(line(13:14));
					galData(galInd, galTags.GALT_h)  = str2num(line(16:17));
					galData(galInd, galTags.GALT_mi) = str2num(line(19:20));
					galData(galInd, galTags.GALT_s)  = str2num(line(22:23));

					tocDate = datetime(str2num(line(5:8)), str2num(line(10:11)), ...
					 str2num(line(13:14)), str2num(line(16:17)), str2num(line(19:20)), ...
					 str2num(line(22:23)));
					galData(galInd, galTags.Toc) = (weekday(tocDate)-1)*86400 + ...
					 tocDate.Hour*3600 + tocDate.Minute*60 + tocDate.Second;
	
					galData(galInd, galTags.SVID) = str2num(line( 2: 3));
					galData(galInd, galTags.A0)   = str2num(line(24:42));
					galData(galInd, galTags.A1)   = str2num(line(43:61));
					galData(galInd, galTags.A2)   = str2num(line(62:80));
					% Line 2: parameters
					line = fgetl(fileID);
					galData(galInd, galTags.Crs)    = str2num(line(24:42));
					galData(galInd, galTags.DeltaN) = str2num(line(43:61));
					galData(galInd, galTags.M0)     = str2num(line(62:80));
					% Line 3: parameters
					line = fgetl(fileID);
					galData(galInd, galTags.Cuc)   = str2num(line( 5:23));
					galData(galInd, galTags.e)     = str2num(line(24:42));
					galData(galInd, galTags.Cus)   = str2num(line(43:61));
					galData(galInd, galTags.sqrtA) = str2num(line(62:80));
					% Line 4: parameters
					line = fgetl(fileID);
					galData(galInd, galTags.Toe)    = str2num(line( 5:23));
					galData(galInd, galTags.Cic)    = str2num(line(24:42));
					galData(galInd, galTags.Omega0) = str2num(line(43:61));
					galData(galInd, galTags.Cis)    = str2num(line(62:80));
					% Line 5: parameters
					line = fgetl(fileID);
					galData(galInd, galTags.i0)       = str2num(line( 5:23));
					galData(galInd, galTags.Crc)      = str2num(line(24:42));
					galData(galInd, galTags.omega)    = str2num(line(43:61));
					galData(galInd, galTags.OmegaDot) = str2num(line(62:80));
					% Line 6: parameters
					line = fgetl(fileID);
					galData(galInd, galTags.iDot) = str2num(line( 5:23));
					galData(galInd, galTags.week) = str2num(line(43:61));
					% Line 7: parameters
					line = fgetl(fileID);
					healthWord = str2num(line(24:42));
					galData(galInd, galTags.valid) = ...
					 (mod(healthWord, 2) < 1) * (mod(healthWord, 8) < 4) * ...
					 (mod(healthWord, 128) < 64); % data valid for each signal
					galData(galInd, galTags.bgdA)  = str2num(line(43:61));
					galData(galInd, galTags.bgdB)  = str2num(line(62:80));
					% Line 8: parameters
					line = fgetl(fileID);

					galInd = galInd + 1;

				case 'S' % SBAS not implemented
					for dummy = 1:3
						line = fgetl(fileID);
					end

				otherwise
					fprintf('Line not recognized:\n%s\n', line);
					break;
				end

			line = fgetl(fileID);
		end % Ephemeris reading while loop
		fclose(fileID);
		delete(ephFile);
		save(strcat(ephFile, '.mat'), 'gpsTags', 'gpsData', 'gloTags', ...
		 'gloData', 'galTags', 'galData', 'utcOffset');
		fprintf('%s.mat parsed and saved.\n', ephFile);

	end % File exists switch
end % function

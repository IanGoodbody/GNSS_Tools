function [ephTags, ephData, utcOffset] = getArchiveEphemeris(logDateUTC)
% GETATCHIVEPHEMERIS retrieves the most up-to-date broadcast ephemeris for all 
% SVs on the provided date from the IGS repository at cddis.gfsc.nasa.gov.
% The broadcast ephemeris for the current day is updated hourly.
%  [ephTags, ephData, utcOffset] = GETARCHIVEEPHEMERIS(logDateUTC)
%
% Parameters
%   logDateUTC : a matlab datetime() object specifying the UTC date for the 
%                 ephemeris record
% Returns
%   ephTags   : Structure containing the column indeices for the output 
%               ephemeris patameters
%   ephData   : Nx30 array containing all the ephemeris data in the file
%   utcOffset : UTC time offset (s) UTC = GPST + utcOffset
	% Define indicies for ephemeris paramters
	ephTags.PRN      =  1; % Unitless
	ephTags.Toe      =  2; % Seconds since the start of the week
	ephTags.week     =  3; % Week number
	ephTags.sqrtA    =  4; % m^{1/2}
	ephTags.e        =  5; % Unitless
	ephTags.i0       =  6; % rad
	ephTags.Omega0   =  7; % rad
	ephTags.omega    =  8; % rad
	ephTags.M0       =  9; % rad
	ephTags.DeltaN   = 10; % rad/s
	ephTags.iDot     = 11; % rad/s
	ephTags.OmegaDot = 12; % rad/s
	ephTags.Cuc      = 13; % rad
	ephTags.Cus      = 14; % rad
	ephTags.Crc      = 15; % m
	ephTags.Crs      = 16; % m
	ephTags.Cic      = 17; % rad
	ephTags.Cis      = 18; % rad
	ephTags.Toc      = 19; % s
	ephTags.A0       = 20; % s
	ephTags.A1       = 21; % s/s
	ephTags.A2       = 22; % s/s^2
	ephTags.Tgd      = 23; % s
	ephTags.fit      = 24; % hr; Ephemeris fit intereval, 0 if undknown
	ephTags.GPST_y   = 25; % year
	ephTags.GPST_m   = 26; % month
	ephTags.GPST_d   = 27; % day
	ephTags.GPST_h   = 28; % hr
	ephTags.GPST_mi  = 29; % minute
	ephTags.GPST_s   = 30; % s
	ephTags.valid    = 31; % s

	year2D = logDateUTC.Year-2000;
	if year2D < 0
		year2D = year2D + 100;
	end % 2 digit year if

	yearDays = floor(days(logDateUTC - datetime(logDateUTC.Year, 1, 1))) + 1;
	ephFile = sprintf('brdc%03i0.%02in', yearDays, year2D);
	if exist( strcat(ephFile, '.mat'), 'file')
		load(strcat(ephFile, '.mat'));
	else
		fprintf('Updating %s\n', strcat(ephFile, '.mat'));
	
		% Download data from NASA's hourly ephemeris server
		ftpFolder = sprintf('/gnss/data/daily/%i/brdc/', ...
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
		lineCount = 0;
		line = fgetl(fileID);
		while line ~= -1
			lineCount = lineCount + 1;
			line = fgetl(fileID);
		end
		records = lineCount/8;
	
		% Begin populating the fields of ephData
		ephData = zeros(records, length(fieldnames(ephTags)));
		fseek(fileID, dataStart, -1);
		for record = 1:records
			% Line 1: PRN, Toc, clock polynomial terms
			line = fgetl(fileID);
			ephData(record, ephTags.GPST_y)  = str2num(line( 4: 5));
			ephData(record, ephTags.GPST_m)  = str2num(line( 7: 8));
			ephData(record, ephTags.GPST_d)  = str2num(line(10:11));
			ephData(record, ephTags.GPST_h)  = str2num(line(13:14));
			ephData(record, ephTags.GPST_mi) = str2num(line(16:17));
			ephData(record, ephTags.GPST_s)  = str2num(line(18:22));
			if ephData(record, ephTags.GPST_y) < 80
				tocDate = datetime(2000 + str2num(line(4:5)), str2num(line(7:8)), ...
				 str2num(line(10:11)), str2num(line(13:14)), str2num(line(16:17)), ...
				 str2num(line(18:22)));
			else
				tocDate = datetime(1900 + str2num(line(4:5)), str2num(line(7:8)), ...
				 str2num(line(10:11)), str2num(line(13:14)), str2num(line(16:17)), ...
				 str2num(line(18:22)));
			end
			ephData(record, ephTags.Toc) = (weekday(tocDate)-1)*86400 + ...
			 tocDate.Hour*3600 + tocDate.Minute*60 + tocDate.Second;
	
			ephData(record, ephTags.PRN) = str2num(line( 1: 2));
			ephData(record, ephTags.A0)  = str2num(line(23:41));
			ephData(record, ephTags.A1)  = str2num(line(42:60));
			ephData(record, ephTags.A2)  = str2num(line(61:79));
			% Line 2: parameters
			line = fgetl(fileID);
			ephData(record, ephTags.Crs)    = str2num(line(23:41));
			ephData(record, ephTags.DeltaN) = str2num(line(42:60));
			ephData(record, ephTags.M0)     = str2num(line(61:79));
			% Line 3: parameters
			line = fgetl(fileID);
			ephData(record, ephTags.Cuc)   = str2num(line( 4:22));
			ephData(record, ephTags.e)     = str2num(line(23:41));
			ephData(record, ephTags.Cus)   = str2num(line(42:60));
			ephData(record, ephTags.sqrtA) = str2num(line(61:79));
			% Line 4: parameters
			line = fgetl(fileID);
			ephData(record, ephTags.Toe)    = str2num(line( 4:22));
			ephData(record, ephTags.Cic)    = str2num(line(23:41));
			ephData(record, ephTags.Omega0) = str2num(line(42:60));
			ephData(record, ephTags.Cis)    = str2num(line(61:79));
			% Line 5: parameters
			line = fgetl(fileID);
			ephData(record, ephTags.i0)       = str2num(line( 4:22));
			ephData(record, ephTags.Crc)      = str2num(line(23:41));
			ephData(record, ephTags.omega)    = str2num(line(42:60));
			ephData(record, ephTags.OmegaDot) = str2num(line(61:79));
			% Line 6: parameters
			line = fgetl(fileID);
			ephData(record, ephTags.iDot) = str2num(line( 4:22));
			ephData(record, ephTags.week) = str2num(line(42:60));
			% Line 7: parameters
			line = fgetl(fileID);
			ephData(record, ephTags.Tgd) = str2num(line(42:60));
			% Line 8: parameters
			line = fgetl(fileID);
			ephData(record, ephTags.fit) = str2num(line(23:41));
			% No line just auxillary
			ephData(record, ephTags.valid) = 1;
		end % Ephemeris record for
		fclose(fileID);
		delete(ephFile);
		save(strcat(ephFile, '.mat'), 'ephData', 'utcOffset');
		fprintf('%s.mat parsed and saved.\n', ephFile);
	end
end % function

addpath('../File_Processing');
close all

trackTime = 0.25; % plot skymap over trackTime hours
utcHour = 5; % local time zone to UTC hour conversion
rcvrGeo = [-1.9080, 0.6802, 6.3781e6];
rcvrCar = convGeoCar( rcvrGeo );
location = 'Highway 128, UT';
utcTime = datetime( 2010, 10, 10, 13, 0, 0 );

[tags, ephAll, gpstOffset] = getNewEphemeris_GPS( utcTime );
prns = unique( ephAll(:, tags.PRN) );
ephNewest = sortrows( ephAll, tags.Toe, 'descend' );

eph = zeros(length(prns), length(fields(tags)));

% select the newsest ephemeris record for each PRN
prnIndex = 1;
while prnIndex <= length(prns)
	recordIndex = 1;
	while (1)
		if ephNewest(recordIndex, tags.PRN) == prns(prnIndex)
			eph(prnIndex, :) = ephNewest(recordIndex, :);
			break;
		end
		recordIndex = recordIndex + 1;
	end
	prnIndex = prnIndex + 1;
end
timeSamples = (0:18:(trackTime * 3600)) + getGPST( utcTime, gpstOffset ); % 18 second plot intervals
zeniths = zeros( length(prns), length(timeSamples) );
azmuths = zeros( length(prns), length(timeSamples) );

for timeIndex = 1:length(timeSamples)
	sampleCarPos = getSvPos(prns, timeSamples(timeIndex), rcvrCar, eph, tags);
	[sampleEl, sampleAz] = svElvAzm(sampleCarPos, rcvrCar, ...
	 rcvrGeo(1), rcvrGeo(2) );
	zeniths(:, timeIndex) = pi/2 - sampleEl;
	azmuths(:, timeIndex) = sampleAz;
end

polarplot( linspace(0, 2*pi, 500), 85 * ones(1, 500), '--k', ...
 'DisplayName', '5-Deg');
hold on
for prnIndex = 1:length(prns)
	visPoints = find( zeniths(prnIndex, :) < pi/2 );
	if length(visPoints) > 0
		polarscatter( azmuths(prnIndex, visPoints(1)), ...
		 180/pi*zeniths(prnIndex, visPoints(1)), 'filled', 'sk' )
		polarplot( azmuths(prnIndex, visPoints), ...
		 180/pi*zeniths(prnIndex, visPoints) )
		text( azmuths(prnIndex, visPoints(1)), ...
		 180/pi*zeniths(prnIndex, visPoints(1)), sprintf('G%02i', prns(prnIndex)) );
	end
end
set(gca, 'ThetaZeroLocation', 'top');
set(gca, 'ThetaDir', 'clockwise');
title( sprintf('Skymap for %s\n%s UTC', location, utcTime) );
hold off

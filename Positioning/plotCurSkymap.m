addpath('../File_Processing');
close all

trackTime = 1; % plot skymap over trackTime hours
utcHour = 4; % local time zone to UTC hour conversion
%rcvrCar = [-2532493.0840, -4696709.8100, 3483154.416]; % Avalon CA
rcvrCar = [506063.534, -4882262.789, 4059609.661]; % AFIT Antenna
rcvrGeo = convCarGeo( rcvrCar );

utcTime = datetime('now') + duration(utcHour, 0, 0);
[tags, ephAll, gpstOffset] = getNewEphemeris( utcTime );
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
		 180/pi*zeniths(prnIndex, visPoints(1)), 'filled', 'sk', ...
		 'DisplayName', '');
		polarplot( azmuths(prnIndex, visPoints), ...
		 180/pi*zeniths(prnIndex, visPoints), ...
		 'DisplayName', sprintf('PRN %02i', prns(prnIndex)));
%		text( azmuths(prnIndex, visPoints(1)), ...
%		 180/pi*zeniths(prnIndex, visPoints), sprintf('G%02i', prns(prnIndex)) );
	end
end
set(gca, 'ThetaZeroLocation', 'top');
set(gca, 'ThetaDir', 'clockwise');
legend('show')
hold off

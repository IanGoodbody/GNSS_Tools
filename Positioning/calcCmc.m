function cmc = calcCmc(psr, car, freq)
% CALCCMC Calculates the code-carrier divergence for time synchronized samples
% of pseudorange and carrier phase (accumulated Doppler) measurements.  This
% function assumes that all data points are from the same SV
%  cmc = CALCCMC(psr, car)
%
% Parameters:
%   psr  : Pseudoranges (m)
%   car  : Carrier phase measurements (cycles)
%   freq : Signal frequency (Hz) used to convert carrier cycles to distance
% Returns:
%   cmc : Code minus carrier divergence (m)

	c = 2.99792458e8;    % (m s^-1) speed of light
	% Make sure the two data sets have the same dimensions
	psr = psr(:);
	car = car(:);  
	if length(psr) ~= length(car)
		printf('Error: psr and car dimensions must match\n');
		cmc = NaN;
	else
		carDist = car*c/freq;
		bias = psr(1) + carDist(1);
		cmc = psr + carDist - bias;
	end % if
end % function

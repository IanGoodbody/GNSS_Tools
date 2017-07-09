function [cordGeo, N] = convCarGeo(cordCar) % CONVCARGEO converts ECEF cartesian coordinates to their geodedict equivalents
% using the miterative method from section 4.A of Misra & Enge
%   cordGeo = CONVCARGEO(corCar)
% 
% Paramters:
%   cordCar: 1x3 vector of cartesian coordinates [x (m), y (m), z (m)]
% Returns:
%	  cordGeo; 1x3 vector of geodededic coordinates 
%            [lat (rad), lon (rad), height (m)]

  a = 6378137;  		% Equitorial radius (m)
  e2 = 0.00669437999013;  % Eccentricity squared (.)

	lon = atan2(cordCar(2), cordCar(1));
	rho = sqrt(sum(cordCar(1:2).^2));

	% Initial values for the iteration
	epsilon = 1;
	epsilonBound = 1E-12;

	iter = 0;
	iterBound = 10000;

	height = 0;
	lat = 0;

	while( (abs(epsilon) > epsilonBound) .* (iter < iterBound) )
		N = a/sqrt( 1-e2*sin(lat)^2 );
		heightNew = rho/cos(lat) - N;
		lat = atan( cordCar(3) / (rho*( 1 - e2*N/(N+height) )) );

		epsilon = height - heightNew;

		height = heightNew;
		iter = iter+1;
	end

	if iter == iterBound
		printf('Warning: Cartesian to geodedic convesion did not converge after %i iterations', iterBound);
	end

	cordGeo = [lat, lon, height];
end % function

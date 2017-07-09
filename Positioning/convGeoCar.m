function cordCar = convGeoCar(cordGeo) 
% CONVGEOCAR converts ECEF cartesian coordinates to their geodedict equivalents
% using the miterative method from section 4.A of Misra & Enge
%   cordCar = CONVGEOCAR(corGeo)
% 
% Paramters:
%	  cordGeo; 1x3 vector of geodededic coordinates 
%            [lat (rad), lon (rad), height (m)]
% Returns:
%   cordCar: 1x3 vector of cartesian coordinates [x (m), y (m), z (m)]

  a = 6378137;  		% Equitorial radius (m)
  e2 = 0.00669437999013;  % Eccentricity squared (.)

	N = a/sqrt( 1-e2*sin(cordGeo(1))^2 );

	x = (N + cordGeo(3)) * cos(cordGeo(1))*cos(cordGeo(2));
	y = (N + cordGeo(3)) * cos(cordGeo(1))*sin(cordGeo(2));
	z = (N*(1-e2) + cordGeo(3)) * sin(cordGeo(1));
	
	cordCar = [x, y, z];
end

function elevations = svElevation(svPos, rcvrPos, rcvrLat, rcvrLon);
% SVELEVATION Calculates the elevation of sattelites with respect to the horizon
%   elevations = SVELEVATION(svPos, rcvrPos, rcvrLat, rcvrLon);
%
% Paramters:
%   svPos   : Nx3 array of N satellite's Cartesian positions (m)
%   rcvrPos : 1x3 vector of the receiver's ECEF Position (m)
%   rcvrLat : Receiver's latitude (used for local vertical) (rad)
%   rcvrLon : Receiver's longitude (used for local vertical) (rad)
% Returns:
%   elevations : Nx1 vector of satellite elevations above the horizon (rad)

	los = svPos - recvrPos; % line of sight vector
	% Unit vector of the local vertical of the Earth's oblate ellipsoid
	vert = [cos(rcvrLat)*cos(rcvrLon), cos(rcvrLat)*sin(rcvrLon), sin(rcvrLat)];
	% The dot product of the line of sight vectors with the receiver's vertical
	%  divided by a column vector of the LOS magnitudes. Asin gives horizon angle
	elevations = asin(los*vert' ./ sqrt(sum(los.^2, 2)));
end;

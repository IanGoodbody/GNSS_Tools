function [elevation, azmuth] = svElvAzm(svPos, rcvrPos, rcvrLat, rcvrLon);
% SVELVAZM Calculates the elevation of sattelites with respect to the horizon
%   [elevation, azmuth] = SVELVAZM(svPos, rcvrPos, rcvrLat, rcvrLon);
%
% Paramters:
%   svPos   : Nx3 array of N satellite's Cartesian positions (m)
%   rcvrPos : 1x3 vector of the receiver's ECEF Position (m)
%   rcvrLat : Receiver's latitude (used for local vertical) (rad)
%   rcvrLon : Receiver's longitude (used for local vertical) (rad)
% Returns:
%   elevation : Nx1 vector of satellite elevations above the horizon (rad)
%   Azmuth    : Nx1 vector of satellite azmuths relative to local north (rad)

	los = svPos - rcvrPos; % line of sight vector
	losMag = sqrt( sum(los.^2, 2) ); % Nx1 matrix of los vector magnitudes
	localRot = [
	 -sin(rcvrLon), -sin(rcvrLat)*cos(rcvrLon), cos(rcvrLat)*cos(rcvrLon);
	  cos(rcvrLon), -sin(rcvrLat)*sin(rcvrLon), cos(rcvrLat)*sin(rcvrLon);
		           0,               cos(rcvrLat),              sin(rcvrLat)
	];
	
	losLocal = los*localRot; % Convert SV line of sight vectors to ENU coordinates

	elevation = asin(losLocal(:, 3)./losMag);
	azmuth    = atan2(losLocal(:, 1), losLocal(:, 2));
end

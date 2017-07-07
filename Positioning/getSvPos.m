function [svPos, clkErr] = GetSvPos(prn, transTime, rcvrPos, ...
 eph, ephTags)
	
	svEph = zeros(length(prn), length(fields(ephTags)));
	for prnIdx = 1:length(prn)
		svEphAll = eph( find(eph(:, ephTags.PRN) == prn(prnIdx)), :);
		svEph(prnIdx, :) = svEphAll(max(find( ... 
		  abs(svEphAll(:, ephTags.Toe)-transTime) < 7200 ...
		 )), :); % Ephemerides valid within 2 hours of Toe
	end
	svEph = sortrows(svEph, ephTags.PRN);

	mu_e      = 3.986005e14;     % (m^3 s^-2) Gravitation constant for earth
	d_Omega_e = 7.2921151467e-5; % (rad s^-1) Rotation rate of earth
	c         = 2.99792458e8;    % (m s^-1) speed of light
	
	n_0 = sqrt(mu_e)./svEph(:, ephTags.sqrtA).^3; % (rad s^-1) Computed mean motion
	n   = n_0 + svEph(:, ephTags.DeltaN);         % (rad s^-1) Corrected mean motion
	
	orbitTime = transTime - svEph(:, ephTags.Toe); % (s) model propagation time
	% Week rollover check
	orbitTime = orbitTime - (orbitTime >  302400)*604800;
	orbitTime = orbitTime + (orbitTime < -302400)*604800;

	M = svEph(:, ephTags.M0) + n.*orbitTime; % (rad) SV mean anomaly

	% Iteratvely solve for eccentric anomaly (rad) using the x = g(x) method
	epsilon = ones(length(prn), 1);
	epsilonBound = 1e-14;
	
	iterBound = 5000;
	iter = 0;
	
	E0 = M;
	E  = zeros(size(M));
	while( sum(abs(epsilon) > epsilonBound)*(iter < iterBound) )
		E = M + svEph(:, ephTags.e).*sin(E0);
		epsilon = E - E0;
		iter = iter + 1;
		E0 = E;
	end % while
	if(iter >= iterBound)
		disp(sprintf('Warning: Eccentric anomaly did not converge in %i iterations, last delta was %g', iterBound, epsilon))
	end % if

	% Flight path angle (rad) 
	nu = atan2( sqrt(1-svEph(:, ephTags.e).^2).*sin(E),...
	 cos(E)-svEph(:, ephTags.e) );
	Phi = nu + svEph(:, ephTags.omega); % (rad) Argument of latitude

	% Parmeter corrections
	delta_u = svEph(:, ephTags.Cus).*sin(2*Phi) + ...
	          svEph(:, ephTags.Cuc).*cos(2*Phi); % (rad)
	delta_r = svEph(:, ephTags.Crs).*sin(2*Phi) + ...
	          svEph(:, ephTags.Crc).*cos(2*Phi); % (m)
	delta_i = svEph(:, ephTags.Cis).*sin(2*Phi) + ...
	          svEph(:, ephTags.Cic).*cos(2*Phi); % (rad)

	u_sv = delta_u + Phi; % (rad) Corrected argument of latitude
	r_sv = svEph(:, ephTags.sqrtA).^2.*(1-svEph(:, ephTags.e).*cos(E)) + delta_r; % (m) corrected radius
	i_sv = svEph(:, ephTags.i0) + delta_i + svEph(:, ephTags.iDot).*orbitTime; % (rad) corrected inclination

	% Oribital plane coordinates (m)
	x_svp = r_sv.*cos(u_sv);
	y_svp = r_sv.*sin(u_sv);

	Omega = svEph(:, ephTags.Omega0) + (svEph(:, ephTags.OmegaDot)-d_Omega_e).*orbitTime - d_Omega_e*svEph(:, ephTags.Toe); % (rad) Corrected RAAN

	% Unrotated ECEF coodinates (m) 
	x_sv = x_svp.*cos(Omega) - y_svp.*cos(i_sv).*sin(Omega);
	y_sv = x_svp.*sin(Omega) + y_svp.*cos(i_sv).*cos(Omega);
	z_sv = y_svp.*sin(i_sv);
	
	t_p = sqrt(sum(...
	 ([x_sv, y_sv, z_sv]-repmat(rcvrPos, length(prn), 1)).^2, 2 ...
	))/c; % (s) signal prop. time

	% Rotate ECEF coordinates (m)
	svPos = zeros(length(prn), 3);
	for prnInd = 1:length(prn)
		svPos(prnInd, :) = [x_sv(prnInd), y_sv(prnInd), z_sv(prnInd)]*[
		 cos(d_Omega_e.*t_p(prnInd)), -sin(d_Omega_e.*t_p(prnInd)), 0;
		 sin(d_Omega_e.*t_p(prnInd)),  cos(d_Omega_e.*t_p(prnInd)), 0;
		                           0,                            0, 1]; 
	end
	
	delta_t = transTime - svEph(:, ephTags.Toc);
	delta_t = delta_t - 604800*(delta_t >  302400);
	delta_t = delta_t + 604800*(delta_t < -302400);

	delta_t_r = -2*sqrt(mu_e)/c^2 * ...
	 svEph(:, ephTags.e).*svEph(:, ephTags.sqrtA).*sin(E); % (s)
	clkErr = svEph(:, ephTags.A0) + svEph(:, ephTags.A1).*delta_t + ...
	 svEph(:, ephTags.A0).*delta_t.^2 + delta_t_r;
end %function

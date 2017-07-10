function [user_pos_ECEF, clock_error, residuals] = calc_position(pr_vec, ...
  prn_vec, rcvr_time, user_pos0, clock_error0, USE_TWO_FREQ, USE_TROPO_CORR)
% Outputs:
%	user_pos_ECEF	: Calculated ECEF user position (1x3)
%	clock_error	: Calculated receiver clock error (m)
%	residuals	: Measurement residual vector (1xn)
% Inputs:
%	pr_vec	: Vector of pseudoranges at this time epoch (nx1)
%	prn_vec	: Vector of PRNs that correspond to the pseudoranges (nx1)
%	rcvr_time	: Receiver clock time that measuremnts were taken
%	user_pos0	: ECEF position to use as the inital nominal position (3x1)
%	clock_error0	: Clock error to use as the initial nominal clock error

if nargin < 7
	USE_TROPO_CORR = 0;
end
if nargin < 6
	USE_TWO_FREQ = 0;
end

c = 299792458; % Speed of light m/s
n = length(prn_vec); % Number of Satellites
f_L1 = 10.23e6 * 154;
f_L2 = 10.23e6 * 120;

if USE_TWO_FREQ
	% Pass the two pseudorange measurments in as [L1; L2]
	pr_vec = (f_L1^2*pr_vec(:, 1) - f_L2^2*pr_vec(:, 2))./(f_L1^2-f_L2^2);
else
	pr_vec = pr_vec(:, 1);
end

user_pos0_llh = [0, 0, 0];
if USE_TROPO_CORR
	user_pos0_llh = EcefToLlh(user_pos0', 'fast')';
end

x_nom = [user_pos0'; clock_error0]; % \hat{x} 
x_delta = ones(4, 1)*11;            % \Delta \overline{x}
x_sat = zeros(n, 3);                % \overline{x} 

rho_corr = zeros(n, 1);             % \overline{\rho}
rho_nom = zeros(n, 1);              % \hat{\rho}
rho_delta = zeros(n, 1);            % \Delta \overline{\rho}

H = zeros(n, 4);                    % \mathbf{H}

% Calculate SV positions, clock errors, and correct measured pseudoranges
trans_times = rcvr_time - pr_vec/c;
trans_times = trans_times + (trans_times < 0) * 604800;
trans_times = trans_times - (trans_times > 604800) * 604800;

for j = 1:n
	eph_j = current_ephemeris(prn_vec(j));
	approx_delta_t = eph_j.af0 + eph_j.af1*(trans_times(j) - eph_j.toc) + ...
	  eph_j.af2*(trans_times(j) - eph_j.toc);
	trans_times(j) = trans_times(j) - approx_delta_t;  
	if(trans_times(j) < 0)
		trans_times(j) = trans_times(j) + 604800;
	elseif(trans_times(j) > 604800)
		trans_times(j) = trans_times(j) - 604800;
	end % if

	[x_sat_j, sv_clk_err_j] = calc_sv_pos(prn_vec(j), trans_times(j), ...
	  user_pos0);
	x_sat(j, :) = x_sat_j;

	if(USE_TWO_FREQ)
		group_delay = 0;
	else
		group_delay = eph_j.tgd;
	end

	if(USE_TROPO_CORR)
		sat_elev = sv_elevation(user_pos0_llh(1), user_pos0_llh(2), ...
		  user_pos0, x_sat_j);
		tropo_corr = calc_tropo_corr(user_pos0_llh(3), sat_elev, 0.5);
	else
		tropo_corr = 0;
	end
	rho_corr(j) = pr_vec(j) + c*(sv_clk_err_j - group_delay) + tropo_corr;
end % for

it_count = 0;
while( ((x_delta' * x_delta) > 100) * (it_count < 100) )
	it_count = it_count + 1;
	if(it_count == 100)
		disp('Warning: Position solution did not converge');
	end % if;

	for j = 1:n
		rho_nom(j) = sqrt(sum((x_sat(j, :)' - x_nom(1:3)).^2)) + x_nom(4);
		H(j, :) = [(x_sat(j, :) - x_nom(1:3)')./rho_nom(j), -1];
	end % for
	rho_delta = rho_nom-rho_corr;
	x_delta = inv(H'*H) * H' * rho_delta;
	x_nom = x_nom + x_delta;
end % while

for j = 1:n
	rho_nom(j) = sqrt(sum((x_sat(j, :)' - x_nom(1:3)).^2)) + x_nom(4);
end

user_pos_ECEF = x_nom(1:3)';
clock_error = x_nom(4);
residuals = rho_nom-rho_corr;
end % function

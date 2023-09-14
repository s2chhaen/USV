function Hd = filterWindowBohman_p4
%FILTERWINDOWBOHMAN_P4 Returns a discrete-time filter object.

% MATLAB Code
% Generated by MATLAB(R) 9.14 and DSP System Toolbox 9.16.
% Generated on: 14-Sep-2023 15:31:02

% FIR Window Lowpass filter designed using the FIR1 function.

% All frequency values are normalized to 1.

N    = 3;        % Order
Fc   = 0.7;      % Cutoff Frequency
flag = 'scale';  % Sampling Flag

% Create the window vector for the design algorithm.
win = bohmanwin(N+1);

% Calculate the coefficients using the FIR1 function.
b  = fir1(N, Fc, 'low', win, flag);
Hd = dfilt.dffir(b);

% [EOF]

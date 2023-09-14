function Hd = filterWindowParzen_p2
%FILTERWINDOWPARZEN_P2 Returns a discrete-time filter object.

% MATLAB Code
% Generated by MATLAB(R) 9.14 and DSP System Toolbox 9.16.
% Generated on: 12-Sep-2023 13:13:36

% FIR Window Lowpass filter designed using the FIR1 function.

% All frequency values are normalized to 1.

N    = 3;          % Order
Fc   = 0.4;        % Cutoff Frequency
flag = 'noscale';  % Sampling Flag

% Create the window vector for the design algorithm.
win = parzenwin(N+1);

% Calculate the coefficients using the FIR1 function.
b  = fir1(N, Fc, 'low', win, flag);
Hd = dfilt.dffir(b);

% [EOF]

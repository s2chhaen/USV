function Hd = filterWindowRectangular_p3
%FILTERWINDOWRECTANGULAR_P3 Returns a discrete-time filter object.

% MATLAB Code
% Generated by MATLAB(R) 9.14 and Signal Processing Toolbox 9.2.
% Generated on: 14-Sep-2023 19:09:04

% FIR Window Lowpass filter designed using the FIR1 function.

% All frequency values are normalized to 1.

N    = 3;        % Order
Fc   = 0.3;      % Cutoff Frequency
flag = 'scale';  % Sampling Flag

% Create the window vector for the design algorithm.
win = rectwin(N+1);

% Calculate the coefficients using the FIR1 function.
b  = fir1(N, Fc, 'low', win, flag);
Hd = dfilt.dffir(b);

% [EOF]

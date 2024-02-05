% Beschreibung: Implementation vom Test 1 in der Dokumentation

%% Signalerzeugung %%
% Erzeugen des Signals mit Gauß-Rauschen, Nebenbedingung: alle Werte von 
% Geräusch ist großer als 0
snr_dB = 7;
signalPowerConfig = 'measured';
midVal = 400;
repeatedTimeValMid = 150;%Ton = 2*repeatedTimeValMid
tVal = 0:0.5:180;
tComp = 90;
xVal = midVal*rectpuls(tVal-tComp,repeatedTimeValMid)+1;
onIntervalBegin = (180*2-repeatedTimeValMid/0.5)/2 + 1;
onIntervalEnd = onIntervalBegin + repeatedTimeValMid/0.5 -1;
noise = abs(awgn(xVal(onIntervalBegin:onIntervalEnd),snr_dB,signalPowerConfig));
ampCoef = 0.2;
noise = mod(noise,midVal*ampCoef);
xnVal = [xVal(1:onIntervalBegin-1) noise xVal(onIntervalEnd+1:end)];
xnVal = xnVal + xVal;
draw = 1;
if draw == 1
    figure('Name','Das Originalsignal und Signal mit Rauschen');
    plot(tVal,xVal,'-o',tVal,xnVal,'-o');
    legend('Original','mit Geräusch','Location','northeastoutside');
    xlabel('Winkel/Grad');
    ylabel('Radius/dB');
end
%% Berechnung der Wirkung des Filters %%

% slopeO: Steigung des Originalzustandes, slopeF: Steigung nach dem Filtern,
% diff_x: Wirkung des Filters auf einen Bereich

% Das vom Maximally-Flat-Methode-entworfene Filter
phase = 1;
filterObj = filterMaximallyFlatFIR_p4;
output = filterIIR(xnVal, tVal, filterObj, draw, phase, ...
    'Maximally Flat Methode-Filterergebnis');
slopeO = slopeArray(xnVal(onIntervalBegin:onIntervalEnd), ...
    tVal(onIntervalBegin:onIntervalEnd));
slopeF = slopeArray(output(onIntervalBegin:onIntervalEnd), ...
    tVal(onIntervalBegin:onIntervalEnd));
slopeO = rms(slopeO);
slopeF = rms(slopeF);
diff_mf = 20.*log10(slopeF./slopeO);

% Das vom Window-Kaiser-Methode-entworfene Filter
phase = 2;
filterObj = filterWindowKaiser_p10;
output = filterFIR(xnVal, tVal, filterObj, draw, phase, ...
    'Window Kaiser Methode-Filterergebnis');
slopeO = slopeArray(xnVal(onIntervalBegin:onIntervalEnd), ...
    tVal(onIntervalBegin:onIntervalEnd));
slopeF = slopeArray(output(onIntervalBegin:onIntervalEnd), ...
    tVal(onIntervalBegin:onIntervalEnd));
slopeO = rms(slopeO);
slopeF = rms(slopeF);
diff_wk = 20.*log10(slopeF./slopeO);

% Das vom Least-Squares-Methode-entworfene Filter
phase = 2;
filterObj = filterLeastSquares_p11;
output = filterFIR(xnVal, tVal, filterObj, draw, phase, ...
    'Least Squares Methode-Filterergebnis');
slopeO = slopeArray(xnVal(onIntervalBegin:onIntervalEnd), ...
    tVal(onIntervalBegin:onIntervalEnd));
slopeF = slopeArray(output(onIntervalBegin:onIntervalEnd), ...
    tVal(onIntervalBegin:onIntervalEnd));
slopeO = rms(slopeO);
slopeF = rms(slopeF);
diff_lsq = 20.*log10(slopeF./slopeO);

% Das vom Equiripple-Methode-entworfenen Filter
phase = 2;
filterObj = filterEquiripple_p3;
output = filterFIR(xnVal, tVal, filterObj, draw, phase, ...
    'Equiripple Methode-Filterergebnis');
slopeO = slopeArray(xnVal(onIntervalBegin:onIntervalEnd), ...
    tVal(onIntervalBegin:onIntervalEnd));
slopeF = slopeArray(output(onIntervalBegin:onIntervalEnd), ...
    tVal(onIntervalBegin:onIntervalEnd));
slopeO = rms(slopeO);
slopeF = rms(slopeF);
diff_eq = 20.*log10(slopeF./slopeO);

% Das vom Window-Bartlett-Methode-entworfene Filter
phase = 2;
filterObj = filterWindowBarlett_p2;
output = filterFIR(xnVal, tVal, filterObj, draw, phase, ...
    'Window Bartlett Methode-Filterergebnis');
slopeO = slopeArray(xnVal(onIntervalBegin:onIntervalEnd), ...
    tVal(onIntervalBegin:onIntervalEnd));
slopeF = slopeArray(output(onIntervalBegin:onIntervalEnd), ...
    tVal(onIntervalBegin:onIntervalEnd));
slopeO = rms(slopeO);
slopeF = rms(slopeF);
diff_wb = 20.*log10(slopeF./slopeO);

% Das vom Window-Rectangular-Methode-entworfene Filter
phase = 2;
filterObj = filterWindowRectangular_p3;
output = filterFIR(xnVal, tVal, filterObj, draw, phase, ...
    'Window Rectangular Methode-Filterergebnis');
slopeO = slopeArray(xnVal(onIntervalBegin:onIntervalEnd), ...
    tVal(onIntervalBegin:onIntervalEnd));
slopeF = slopeArray(output(onIntervalBegin:onIntervalEnd), ...
    tVal(onIntervalBegin:onIntervalEnd));
slopeO = rms(slopeO);
slopeF = rms(slopeF);
diff_wr = 20.*log10(slopeF./slopeO);

% Das vom Window-Triangular-Methode-entworfenen Filter
phase = 2;
filterObj = filterWindowTriangular_p1;
output = filterFIR(xnVal, tVal, filterObj, draw, phase, ...
    'Window Triangular Methode-Filterergebnis');
slopeO = slopeArray(xnVal(onIntervalBegin:onIntervalEnd), ...
    tVal(onIntervalBegin:onIntervalEnd));
slopeF = slopeArray(output(onIntervalBegin:onIntervalEnd), ...
    tVal(onIntervalBegin:onIntervalEnd));
slopeO = rms(slopeO);
slopeF = rms(slopeF);
diff_wt = 20.*log10(slopeF./slopeO);

clear slopeO slopeF
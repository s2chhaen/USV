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
figureNo = 1;
if draw == 1
    figure(figureNo);
    plot(tVal,xVal,'-o',tVal,xnVal,'-o');
    figureNo = figureNo + 1;
    legend('Original','mit Geräusch','Location','northeastoutside');
end
%% Berechnung der Wirkung des Filters %%

% slopeO: Steigung des Originalzustandes, slopeF: Steigung nach dem Filtern,
% diff_x: Wirkung des Filters auf einen Bereich

% Das vom Maximally-Flat-Methode-entworfene Filter
phase = 1;
filterObj = filterMaximallyFlatFIR_p4;
output = filterIIR(xnVal, tVal, filterObj, draw, phase, figureNo);
if draw == 1
    figureNo = figureNo + 1;
end
slopeO = slopeArray(xnVal(onIntervalBegin:onIntervalEnd), ...
    tVal(onIntervalBegin:onIntervalEnd));
slopeF = slopeArray(output(onIntervalBegin:onIntervalEnd), ...
    tVal(onIntervalBegin:onIntervalEnd));
draw = 0;
if draw == 1
    figure(figureNo);
    hVal = [1:numel(slopeF)];
    plot(hVal,slopeO,'-',hVal,slopeF,'-');
    legend('Original','gefilterten-Signal','Location','northeastoutside');
    figureNo = figureNo + 1;
end
slopeO = rms(slopeO);
slopeF = rms(slopeF);
diff_mf = 20.*log10(slopeF./slopeO);

% Das vom Window-Kaiser-Methode-entworfene Filter
phase = 2;
filterObj = filterWindowKaiser_p10;
output = filterFIR(xnVal, tVal, filterObj, draw, phase, figureNo);
if draw == 1
    figureNo = figureNo + 1;
end
slopeO = slopeArray(xnVal(onIntervalBegin:onIntervalEnd), ...
    tVal(onIntervalBegin:onIntervalEnd));
slopeF = slopeArray(output(onIntervalBegin:onIntervalEnd), ...
    tVal(onIntervalBegin:onIntervalEnd));
draw = 0;
if draw == 1
    figure(figureNo);
    hVal = [1:numel(slopeF)];
    plot(hVal,slopeO,'-',hVal,slopeF,'-');
    legend('Original','gefilterten-Signal','Location','northeastoutside');
    figureNo = figureNo + 1;
end
slopeO = rms(slopeO);
slopeF = rms(slopeF);
diff_wk = 20.*log10(slopeF./slopeO);

% Das vom Least-Squares-Methode-entworfene Filter
phase = 2;
filterObj = filterLeastSquares_p11;
output = filterFIR(xnVal, tVal, filterObj, draw, phase, figureNo);
if draw == 1
    figureNo = figureNo + 1;
end
slopeO = slopeArray(xnVal(onIntervalBegin:onIntervalEnd), ...
    tVal(onIntervalBegin:onIntervalEnd));
slopeF = slopeArray(output(onIntervalBegin:onIntervalEnd), ...
    tVal(onIntervalBegin:onIntervalEnd));
draw = 0;
if draw == 1
    figure(figureNo);
    hVal = [1:numel(slopeF)];
    plot(hVal,slopeO,'-',hVal,slopeF,'-');
    legend('Original','gefilterten-Signal','Location','northeastoutside');
    figureNo = figureNo + 1;
end
slopeO = rms(slopeO);
slopeF = rms(slopeF);
diff_lsq = 20.*log10(slopeF./slopeO);

% Das vom Equiripple-Methode-entworfenen Filter
phase = 2;
filterObj = filterEquiripple_p3;
output = filterFIR(xnVal, tVal, filterObj, draw, phase, figureNo);
if draw == 1
    figureNo = figureNo + 1;
end
slopeO = slopeArray(xnVal(onIntervalBegin:onIntervalEnd), ...
    tVal(onIntervalBegin:onIntervalEnd));
slopeF = slopeArray(output(onIntervalBegin:onIntervalEnd), ...
    tVal(onIntervalBegin:onIntervalEnd));
draw = 0;
if draw == 1
    figure(figureNo);
    hVal = [1:numel(slopeF)];
    plot(hVal,slopeO,'-',hVal,slopeF,'-');
    legend('Original','gefilterten-Signal','Location','northeastoutside');
    figureNo = figureNo + 1;
end
slopeO = rms(slopeO);
slopeF = rms(slopeF);
diff_eq = 20.*log10(slopeF./slopeO);

% Das vom Window-Bartlett-Methode-entworfene Filter
phase = 2;
filterObj = filterWindowBarlett_p2;
output = filterFIR(xnVal, tVal, filterObj, draw, phase, figureNo);
if draw == 1
    figureNo = figureNo + 1;
end
slopeO = slopeArray(xnVal(onIntervalBegin:onIntervalEnd), ...
    tVal(onIntervalBegin:onIntervalEnd));
slopeF = slopeArray(output(onIntervalBegin:onIntervalEnd), ...
    tVal(onIntervalBegin:onIntervalEnd));
draw = 0;
if draw == 1
    figure(figureNo);
    hVal = [1:numel(slopeF)];
    plot(hVal,slopeO,'-',hVal,slopeF,'-');
    legend('Original','gefilterten-Signal','Location','northeastoutside');
    figureNo = figureNo + 1;
end
slopeO = rms(slopeO);
slopeF = rms(slopeF);
diff_wb = 20.*log10(slopeF./slopeO);

% Das vom Window-Rectangular-Methode-entworfene Filter
phase = 2;
filterObj = filterWindowRectangular_p3;
output = filterFIR(xnVal, tVal, filterObj, draw, phase, figureNo);
if draw == 1
    figureNo = figureNo + 1;
end
slopeO = slopeArray(xnVal(onIntervalBegin:onIntervalEnd), ...
    tVal(onIntervalBegin:onIntervalEnd));
slopeF = slopeArray(output(onIntervalBegin:onIntervalEnd), ...
    tVal(onIntervalBegin:onIntervalEnd));
draw = 0;
if draw == 1
    figure(figureNo);
    hVal = [1:numel(slopeF)];
    plot(hVal,slopeO,'-',hVal,slopeF,'-');
    legend('Original','gefilterten-Signal','Location','northeastoutside');
    figureNo = figureNo + 1;
end
slopeO = rms(slopeO);
slopeF = rms(slopeF);
diff_wr = 20.*log10(slopeF./slopeO);

% Das vom Window-Triangular-Methode-entworfenen Filter
phase = 2;
filterObj = filterWindowTriangular_p1;
output = filterFIR(xnVal, tVal, filterObj, draw, phase, figureNo);
if draw == 1
    figureNo = figureNo + 1;
end
slopeO = slopeArray(xnVal(onIntervalBegin:onIntervalEnd), ...
    tVal(onIntervalBegin:onIntervalEnd));
slopeF = slopeArray(output(onIntervalBegin:onIntervalEnd), ...
    tVal(onIntervalBegin:onIntervalEnd));
draw = 0;
if draw == 1
    figure(figureNo);
    hVal = [1:numel(slopeF)];
    plot(hVal,slopeO,'-',hVal,slopeF,'-');
    legend('Original','gefilterten-Signal','Location','northeastoutside');
    figureNo = figureNo + 1;
end
slopeO = rms(slopeO);
slopeF = rms(slopeF);
diff_wt = 20.*log10(slopeF./slopeO);
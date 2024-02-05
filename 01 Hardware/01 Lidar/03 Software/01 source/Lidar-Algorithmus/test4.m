% Beschreibung: Implementation vom Test 3 in der Dokumentation

%% Signalerzeugung %%
midVal = 400;
repeatedTimeValMid = 150;%Ton = 2*repeatedTimeValMid
tVal = 0:0.5:180;
tComp = 90;
xVal = midVal*rectpuls(tVal-tComp,repeatedTimeValMid)+1;
onIntervalBegin = (180*2-repeatedTimeValMid/0.5)/2 + 1;
onIntervalEnd = onIntervalBegin + repeatedTimeValMid/0.5 -1;
minRange = -1;
maxRange = 1;
noise = minRange + (maxRange - minRange).*rand(1,repeatedTimeValMid/0.5);
ah = 4;% erlaubte Höhenabweichung
noise = ah*noise;
xnVal = [xVal(1:onIntervalBegin-1) noise xVal(onIntervalEnd+1:end)];
xnVal = xnVal + xVal;
draw = 1;
if draw == 1
    figure('Name','Das Originalsignal und Signal mit schwächen Rauschen');
    plot(tVal,xVal,'-',tVal,xnVal,'-');
    legend('Original','mit Rauschen','Location','northeastoutside');
    xlabel('Winkel/Grad');
    ylabel('Radius/dB');
end

%% Berechnung der Wirkung des Filters %%
draw = 0;
% Das vom Maximally-Flat-Methode-entworfene Filter
phase = 1;
filterObj = filterMaximallyFlatFIR_p4;
output = filterIIR(xnVal, tVal, filterObj, draw, phase, ...
    'Maximally Flat Methode-Filterergebnis');
% verlorene Punkte aufgrund der Steigung von einem Bereich zu einem
% neuen Bereich: von Diagramm aus dem 2. Versuch (Test2) abgelesen
lostedPointBegin = 1;
lostedPointEnd = 1;
onIntervalBegin = onIntervalBegin + lostedPointBegin;
onIntervalEnd = onIntervalEnd - lostedPointEnd;
diff_mf = output(onIntervalBegin:onIntervalEnd)./xnVal(onIntervalBegin:onIntervalEnd);
diff_mf = 20.*log(diff_mf);
diff_mf = rms(diff_mf);

% Das vom Window-Kaiser-Methode-entworfene Filter
phase = 2;
filterObj = filterWindowKaiser_p10;
output = filterFIR(xnVal, tVal, filterObj, draw, phase, ...
    'Window Kaiser Methode-Filterergebnis');
% verlorene Punkte aufgrund der Steigung von einem Bereich zu einem
% neuen Bereich: von Diagramm aus dem 2. Versuch (Test2) abgelesen
lostedPointBegin = 1;
lostedPointEnd = 2;
onIntervalBegin = onIntervalBegin + lostedPointBegin;
onIntervalEnd = onIntervalEnd - lostedPointEnd;
diff_wk = output(onIntervalBegin:onIntervalEnd)./xnVal(onIntervalBegin:onIntervalEnd);
diff_wk = 20.*log(diff_wk);
diff_wk = rms(diff_wk);

% Das vom Least-Squares-Methode-entworfene Filter
phase = 2;
filterObj = filterLeastSquares_p11;
output = filterFIR(xnVal, tVal, filterObj, draw, phase, ...
    'Least Squares Methode-Filterergebnis');
% verlorene Punkte aufgrund der Steigung von einem Bereich zu einem
% neuen Bereich: von Diagramm aus dem 2. Versuch (Test2) abgelesen
lostedPointBegin = 1;
lostedPointEnd = 2;
onIntervalBegin = onIntervalBegin + lostedPointBegin;
onIntervalEnd = onIntervalEnd - lostedPointEnd;
diff_lsq = output(onIntervalBegin:onIntervalEnd)./xnVal(onIntervalBegin:onIntervalEnd);
diff_lsq = 20.*log(diff_lsq);
diff_lsq = rms(diff_lsq);

% Das vom Equiripple-Methode-entworfenen Filter
phase = 2;
filterObj = filterEquiripple_p3;
output = filterFIR(xnVal, tVal, filterObj, draw, phase, ...
    'Equiripple Methode-Filterergebnis');
% verlorene Punkte aufgrund der Steigung von einem Bereich zu einem
% neuen Bereich: von Diagramm aus dem 2. Versuch (Test2) abgelesen
lostedPointBegin = 1;
lostedPointEnd = 2;
onIntervalBegin = onIntervalBegin + lostedPointBegin;
onIntervalEnd = onIntervalEnd - lostedPointEnd;
diff_eq = output(onIntervalBegin:onIntervalEnd)./xnVal(onIntervalBegin:onIntervalEnd);
diff_eq = 20.*log(diff_eq);
diff_eq = rms(diff_eq);

% Das vom Window-Bartlett-Methode-entworfene Filter
phase = 2;
filterObj = filterWindowBarlett_p2;
output = filterFIR(xnVal, tVal, filterObj, draw, phase, ...
    'Window Bartlett Methode-Filterergebnis');
% verlorene Punkte aufgrund der Steigung von einem Bereich zu einem
% neuen Bereich: von Diagramm aus dem 2. Versuch (Test2) abgelesen
lostedPointBegin = 0;
lostedPointEnd = 1;
onIntervalBegin = onIntervalBegin + lostedPointBegin;
onIntervalEnd = onIntervalEnd - lostedPointEnd;
diff_wb = output(onIntervalBegin:onIntervalEnd)./xnVal(onIntervalBegin:onIntervalEnd);
diff_wb = 20.*log(diff_wb);
diff_wb = rms(diff_wb);

% Das vom Window-Rectangular-Methode-entworfenen Filter
phase = 2;
filterObj = filterWindowRectangular_p3;
output = filterFIR(xnVal, tVal, filterObj, draw, phase, ...
    'Window Rectangular Methode-Filterergebnis');
% verlorene Punkte aufgrund der Steigung von einem Bereich zu einem
% neuen Bereich: von Diagramm aus dem 2. Versuch (Test2) abgelesen
lostedPointBegin = 1;
lostedPointEnd = 2;
onIntervalBegin = onIntervalBegin + lostedPointBegin;
onIntervalEnd = onIntervalEnd - lostedPointEnd;
diff_wr = output(onIntervalBegin:onIntervalEnd)./xnVal(onIntervalBegin:onIntervalEnd);
diff_wr = 20.*log(diff_wr);
diff_wr = rms(diff_wr);

% Das vom Window-Triangular-Methode-entworfenen Filter
phase = 2;
filterObj = filterWindowTriangular_p1;
output = filterFIR(xnVal, tVal, filterObj, draw, phase, ...
    'Window Triangular Methode-Filterergebnis');
% verlorene Punkte aufgrund der Steigung von einem Bereich zu einem
% neuen Bereich: von Diagramm aus dem 2. Versuch (Test2) abgelesen
lostedPointBegin = 1;
lostedPointEnd = 2;
onIntervalBegin = onIntervalBegin + lostedPointBegin;
onIntervalEnd = onIntervalEnd - lostedPointEnd;
diff_wt = output(onIntervalBegin:onIntervalEnd)./xnVal(onIntervalBegin:onIntervalEnd);
diff_wt = 20.*log(diff_wt);
diff_wt = rms(diff_wt);
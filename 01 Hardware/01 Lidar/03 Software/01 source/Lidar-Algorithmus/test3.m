% Beschreibung: Implementation vom Test 2 in der Dokumentation

%% Erzeugung des Testsignals %%
repeatedTimeValMid = 150;%Ton = 2*repeatedTimeValMid
midVal = 20;
%Bereich von 0° zum 180° mit Auflösung von 0.5°
tVal = 0:0.5:180;
tComp = 90;
xVal = midVal*rectpuls(tVal-tComp,repeatedTimeValMid)+1;
figure('Name', 'Das Originalsignal');
plot(tVal,xVal,'-o');
xlabel('Winkel/Grad');
ylabel('Radius/dB');

%% Erstellung des Diagramms zur Bestimmung der Verlustpunkte %%
draw = 1;
% Das Ergebnis-Signal, das von folgenden Filter gefiltert wird

% vom Maximally-Flat-Methode-entworfenen Filter
phase = 1;
filterObj = filterMaximallyFlatFIR_p4;
filterIIR(xVal, tVal, filterObj, draw, phase, figureNo);
% vom Window-Kaiser-Methode-entworfenen Filter
phase = 2;
filterObj = filterWindowKaiser_p10;
filterFIR(xVal, tVal, filterObj, draw, phase, figureNo);
% vom Least-Squares-Methode-entworfenen Filter
phase = 2;
filterObj = filterLeastSquares_p11;
filterFIR(xVal, tVal, filterObj, draw, phase, figureNo);
% vom Equiripple-Methode-entworfenen Filter
phase = 2;
filterObj = filterEquiripple_p3;
filterFIR(xVal, tVal, filterObj, draw, phase, figureNo);
% vom Window-Bartlett-Methode-entworfenen Filter
phase = 2;
filterObj = filterWindowBarlett_p2;
filterFIR(xVal, tVal, filterObj, draw, phase, figureNo);
% vom Window-Rectangular-Methode-entworfenen Filter
phase = 2;
filterObj = filterWindowRectangular_p3;
filterFIR(xVal, tVal, filterObj, draw, phase, figureNo);
% vom Window-Triangular-Methode-entworfenen Filter
phase = 2;
filterObj = filterWindowTriangular_p1;
filterFIR(xVal, tVal, filterObj, draw, phase, figureNo);
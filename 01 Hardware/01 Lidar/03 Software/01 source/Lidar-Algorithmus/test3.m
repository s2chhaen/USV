% Original Signal
% Der Bereich A, deren Werte gleicheinander und ungleich
% 0 sind: Wiederholungsmal immer gerade
repeatedTimeValMid = 150;%Ton = 2*repeatedTimeValMid
midVal = 20;
%Bereich von 0° zum 180° mit Auflösung von 0.5°
tVal = 0:0.5:180;
tComp = 90;
xVal = midVal*rectpuls(tVal-tComp,repeatedTimeValMid)+1;
%plot(tVal,xVal,'-o');

%gefiltertes Signal

figureNo = 1;
%Mit dem Maximally-Flat-Methode-entworfenen Filter
draw = 1;
phase = 1;
filterObj = filterMaximallyFlatFIR_p4;
filterIIR(xVal, tVal, filterObj, draw, phase, figureNo);
figureNo = figureNo + 1;
%Mit dem Window-Kaiser-Methode-entworfenen Filter
draw = 1;
phase = 2;
filterObj = filterWindowKaiser_p10;
filterFIR(xVal, tVal, filterObj, draw, phase, figureNo);
figureNo = figureNo + 1;
%Mit dem Least-Squares-Methode-entworfenen Filter
draw = 1;
phase = 2;
filterObj = filterLeastSquares_p11;
filterFIR(xVal, tVal, filterObj, draw, phase, figureNo);
figureNo = figureNo + 1;
% Mit dem Equiripple-Methode-entworfenen Filter
draw = 1;
phase = 2;
filterObj = filterEquiripple_p3;
filterFIR(xVal, tVal, filterObj, draw, phase, figureNo);
figureNo = figureNo + 1;
% Mit dem Window-Bartlett-Methode-entworfenen Filter
draw = 1;
phase = 2;
filterObj = filterWindowBarlett_p2;
filterFIR(xVal, tVal, filterObj, draw, phase, figureNo);
figureNo = figureNo + 1;
% Mit dem Window-Rectangular-Methode-entworfenen Filter
draw = 1;
phase = 2;
filterObj = filterWindowRectangular_p3;
filterFIR(xVal, tVal, filterObj, draw, phase, figureNo);
figureNo = figureNo + 1;
% Mit dem Window-Triangular-Methode-entworfenen Filter
draw = 1;
phase = 2;
filterObj = filterWindowTriangular_p1;
filterFIR(xVal, tVal, filterObj, draw, phase, figureNo);
figureNo = figureNo + 1;
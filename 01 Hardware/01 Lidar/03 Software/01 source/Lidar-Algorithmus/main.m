% Autor: Thach
% Verwendungszweck: Gesamte Ausführung der Quellcode von anderen Daten
% Erstellt am 23.08.2023
% Version: 1.01
% Revision: 1.00

clear;
clc;
format shortG

% 1. Versuch: zum Erreichen des Überblick und groben Auswahl der Filter-Methode
test1_active = 0;
% 2. Versuch: Zur Untersuchung des Effektes von Filtern 
test2_active = 1;
% 3. Versuch: Zur Untersuchung des Effektes von Filtern im flachen Bereich
test3_active = 0;
% 4. Versuch: Zur Untersuchung des Effektes von Filtern im Bereich mit der 
% schwächen Wirkung der Störgrößen
test4_active = 1;

if test1_active == 1
    test1
end

if test2_active == 1
    test2
end

if test3_active == 1
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
end

if test4_active == 1
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
    draw = 0;
    figureNo = 1;
    if draw == 1
        figure(figureNo);
        plot(tVal,xVal,'-',tVal,xnVal,'-');
        figureNo = figureNo + 1;
        legend('Original','mit Geräusch','Location','northeastoutside');
    end
end





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

if test1_active==1
    test1
end

if test2_active == 1
    % Erzeugen des Signals mit Geräusch, Nebenbedingung: alle Werte von
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
    draw = 0;
    figureNo = 1;
    if draw == 1
        figure(figureNo);
        plot(tVal,xVal,'-o',tVal,xnVal,'-o');
        figureNo = figureNo + 1;
        legend('Original','mit Geräusch','Location','northeastoutside');
    end

    % Mit dem Maximally-Flat-Methode-entworfenen Filter
    draw = 0;
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

    %Mit dem Window-Kaiser-Methode-entworfenen Filter
    draw = 0;
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

    % Mit dem Least-Squares-Methode-entworfenen Filter
    draw = 0;
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

    % Mit dem Equiripple-Methode-entworfenen Filter
    draw = 0;
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

    % Mit dem Window-Bartlett-Methode-entworfenen Filter
    draw = 0;
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

    % Mit dem Window-Rectangular-Methode-entworfenen Filter
    draw = 0;
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

    % Mit dem Window-Triangular-Methode-entworfenen Filter
    draw = 0;
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
end





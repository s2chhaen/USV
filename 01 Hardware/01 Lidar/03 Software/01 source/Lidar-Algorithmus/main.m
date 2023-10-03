% Autor: Thach
% Verwendungszweck: Gesamte Ausführung der Quellcode von anderen Daten
% Erstellt am 23.08.2023
% Version: 1.01
% Revision: 1.00

%clear;
clc;
format shortG

% 1. Versuch: zum Erreichen des Überblick und groben Auswahl der Filter-Methode
test1_active = 0;
% 2. Versuch: Zur Untersuchung des Effektes von Filtern 
test2_active = 1;
% 3. Versuch: Zur Untersuchung des Effektes von Filtern im flachen Bereich
test3_active = 1;
% 4. Versuch: Zur Untersuchung des Effektes vo Filtern im Bereich mit der 
% schwächen Wirkung der Störgrößen
test4_active = 1;

if test1_active==1
    LidarDataExtract; %Ausführung der LidarDataExtract.m Skript
    lenRA = size(dataOutputRA);
    rCol = 1;%Radius
    aCol = 2;%Winkel
    sample = zeros(lenRA(1,1),lenRA(1,2)); %Stichprobe zum Testen


    %Durchschnittberechnung
    for i=1:lenRA(1)
        sample(i,rCol) = (sum(dataOutputRA(i,rCol,:),'all')/lenRA(1,3));
        sample(i,aCol) = (sum(dataOutputRA(i,aCol,:),'all')/lenRA(1,3));
    end

    rSample = sample(:,rCol);
    rSample = transpose(rSample);
    aSample = sample(:,aCol);
    tVal = transpose(aSample);

    printInFile('fixed',rSample);
    printInFile('float',rSample);

    %IIR mit Maximally Flat Entwurfsmethode
    figureNo = 1;
    draw = 1;
    phase = 1;
    filterObj = filterMaximallyFlatFIR_p4;
    output = filterIIR(rSample, tVal, filterObj, draw, phase, figureNo);
    figureNo = figureNo + 1;
    printOutFile('fixed','mf',output);
    printOutFile('float','mf',output);

    %FIR mit Window-Kaiser Entwurfsmethode
    phase = 2;
    filterObj = filterWindowKaiser_p10;
    filterFIR(rSample, tVal, filterObj, draw, phase, figureNo);
    figureNo = figureNo + 1;
    printOutFile('fixed','wk',output);
    printOutFile('float','wk',output);

    %FIR mit Least-Square Entwurfsmethode
    phase = 2;
    filterObj = filterLeastSquares_p11;
    filterFIR(rSample, tVal, filterObj, draw, phase, figureNo);
    figureNo = figureNo + 1;
    printOutFile('fixed','lsq',output);
    printOutFile('float','lsq',output);

    %FIR mit Equiripple Entwurfsmethode
    phase = 2;
    filterObj = filterEquiripple_p3;
    filterFIR(rSample, tVal, filterObj, draw, phase, figureNo);
    figureNo = figureNo + 1;
    printOutFile('fixed','eq',output);
    printOutFile('float','eq',output);

    %FIR mit Window-Bartlett Entwurfsmethode
    phase = 2;
    filterObj = filterWindowBarlett_p2;
    filterFIR(rSample, tVal, filterObj, draw, phase, figureNo);
    figureNo = figureNo + 1;
    printOutFile('fixed','wb',output);
    printOutFile('float','wb',output);

    %FIR mit Window-Rectangular Entwurfsmethode
    phase = 2;
    filterObj = filterWindowRectangular_p3;
    filterFIR(rSample, tVal, filterObj, draw, phase, figureNo);
    figureNo = figureNo + 1;
    printOutFile('fixed','wr',output);
    printOutFile('float','wr',output);

    %FIR mit Window-Triangular Entwurfsmethode
    phase = 2;
    filterObj = filterWindowTriangular_p1;
    filterFIR(rSample, tVal, filterObj, draw, phase, figureNo);
    figureNo = figureNo + 1;
    printOutFile('fixed','wt',output);
    printOutFile('float','wt',output);
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
    %Mit dem Equiripple-Methode-entworfenen Filter
    draw = 1;
    phase = 2;
    filterObj = filterEquiripple_p3;
    filterFIR(xVal, tVal, filterObj, draw, phase, figureNo);
    figureNo = figureNo + 1;
    %Mit dem Window-Bartlett-Methode-entworfenen Filter
    draw = 1;
    phase = 2;
    filterObj = filterWindowBarlett_p2;
    filterFIR(xVal, tVal, filterObj, draw, phase, figureNo);
    figureNo = figureNo + 1;
    %Mit dem Window-Rectangular-Methode-entworfenen Filter
    draw = 1;
    phase = 2;
    filterObj = filterWindowRectangular_p3;
    filterFIR(xVal, tVal, filterObj, draw, phase, figureNo);
    figureNo = figureNo + 1;
    %Mit dem Window-Triangular-Methode-entworfenen Filter
    draw = 1;
    phase = 2;
    filterObj = filterWindowTriangular_p1;
    filterFIR(xVal, tVal, filterObj, draw, phase, figureNo);
    figureNo = figureNo + 1;
end







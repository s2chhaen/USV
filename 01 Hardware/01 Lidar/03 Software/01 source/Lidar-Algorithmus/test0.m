% Beschreibung: Versuch 0: Testen der Filtern in folgenden Fall:
% Ein Buch liegt im Bereich vom 30° zum 60° vom Lidar

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

figureNo = 1;
draw = 1;

if draw == 1
    figure(figureNo)
    plot(tVal,rSample,'-o');
    legend('Original Daten','Location','northeastoutside');
    grid on
    xlabel('Winkel/Grad');
    ylabel('Radius/cm');
    figureNo = figureNo + 1;
end

printInFile('fixed',rSample);
printInFile('float',rSample);

%IIR mit Maximally Flat Entwurfsmethode
draw = 1;
phase = 1;
filterObj = filterMaximallyFlatFIR_p4;
output = filterIIR(rSample, tVal, filterObj, draw, phase, ...
    'Maximally Flat Methode-Filterergebnis');
figureNo = figureNo + 1;
printOutFile('fixed','mf',output);
printOutFile('float','mf',output);

%FIR mit Window-Kaiser Entwurfsmethode
phase = 2;
filterObj = filterWindowKaiser_p10;
filterFIR(rSample, tVal, filterObj, draw, phase, ...
    'Window Kaiser Methode-Filterergebnis');
figureNo = figureNo + 1;
printOutFile('fixed','wk',output);
printOutFile('float','wk',output);

%FIR mit Least-Square Entwurfsmethode
phase = 2;
filterObj = filterLeastSquares_p11;
filterFIR(rSample, tVal, filterObj, draw, phase, ...
    'Least Squares Methode-Filterergebnis');
figureNo = figureNo + 1;
printOutFile('fixed','lsq',output);
printOutFile('float','lsq',output);

%FIR mit Equiripple Entwurfsmethode
phase = 2;
filterObj = filterEquiripple_p3;
filterFIR(rSample, tVal, filterObj, draw, phase, ...
    'Equiripple Methode-Filterergebnis');
figureNo = figureNo + 1;
printOutFile('fixed','eq',output);
printOutFile('float','eq',output);

%FIR mit Window-Bartlett Entwurfsmethode
phase = 2;
filterObj = filterWindowBarlett_p2;
filterFIR(rSample, tVal, filterObj, draw, phase, ...
    'Window Bartlett Methode-Filterergebnis');
figureNo = figureNo + 1;
printOutFile('fixed','wb',output);
printOutFile('float','wb',output);

%FIR mit Window-Rectangular Entwurfsmethode
phase = 2;
filterObj = filterWindowRectangular_p3;
filterFIR(rSample, tVal, filterObj, draw, phase, ...
    'Window Rectangular Methode-Filterergebnis');
figureNo = figureNo + 1;
printOutFile('fixed','wr',output);
printOutFile('float','wr',output);

%FIR mit Window-Triangular Entwurfsmethode
phase = 2;
filterObj = filterWindowTriangular_p1;
filterFIR(rSample, tVal, filterObj, draw, phase, ...
    'Window Triangular Methode-Filterergebnis');
figureNo = figureNo + 1;
printOutFile('fixed','wt',output);
printOutFile('float','wt',output);
%Beschreibung: Funktionstest in der Dokumentation (6.1.1)
format long

afterCommaBits = 15;
tVal = (0:0.5:180);
fileID = fopen('lidarValues_raw.txt','r');
formatSpec = '%lf';
raw = (fscanf(fileID,formatSpec)).';
raw = raw/2^afterCommaBits;
fclose(fileID);

fileID = fopen('lidarValues_filtered.txt','r');
formatSpec = '%lf';
filter = (fscanf(fileID,formatSpec)).';
filter = filter/2^afterCommaBits;
fclose(fileID);

figureNo = 1;
figure('Name','Abstand-Winkel-Diagramm');
ax(1) = subplot(2,1,1);
plot(tVal,raw,'-o',tVal,filter,'-o');
legend('Original Daten','gefilterte Daten','Location','northeastoutside');
grid on
xlabel('Winkel/Grad');
ylabel('Radius/cm');

ax(2) = subplot(2,1,2);
plot(tVal,20.*log10(filter./raw),'-o');
legend('Differenz in dB','Location','northeastoutside');
xlabel('Winkel/Grad');
ylabel('Radius/dB');
figureNo = figureNo + 1;

figure('Name',': Objekte vor und nach dem Filtern');
angleVal = deg2rad(tVal);
polarplot(angleVal,raw,'-o',angleVal,filter,'-o');
legend('Original Daten','gefilterte Daten','Location','northeastoutside');
thetalim([0,180]);

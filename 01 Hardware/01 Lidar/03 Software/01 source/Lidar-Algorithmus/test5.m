%format shortG
%format longE
%format shortE
format long

clear;

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
figure(figureNo)
ax(1) = subplot(2,1,1);
plot(tVal,raw,'-o',tVal,filter,'-o');
legend('Original Daten','Filtered Data','Location','northeastoutside');
grid on
xlabel('Winkel/Grad');
ylabel('Radius/cm');

ax(2) = subplot(2,1,2);
plot(tVal,20.*log10(filter./raw),'-o');
legend('filter/raw in dB','Location','northeastoutside');
xlabel('Winkel/Grad');
ylabel('Radius/dB');
figureNo = figureNo + 1;

figure(figureNo)
angleVal = deg2rad(tVal);
polarplot(angleVal,raw,'-o',angleVal,filter,'-o');
thetalim([0,180]);

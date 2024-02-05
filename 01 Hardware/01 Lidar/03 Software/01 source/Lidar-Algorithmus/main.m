% Autor: Thach
% Verwendungszweck: Gesamte Ausführung der Quellcode von anderen Daten
% Erstellt am 23.08.2023
% Version: 1.01
% Revision: 1.00

clear;
clc;
format shortG

% Setzen dieses Flags zur Aktivierung des Versuch-Skriptes
% 0. Versuch: Überblick über die ausgewählten Filter-Entwurf-Methode
test0_active = 0; % Nicht notwendig
% 1. Versuch: Zur Untersuchung des Effektes von Filtern 
test1_active = 1;
% 2. Versuch: Zur Bestimmung der Verlustpunkte am Rand
test2_active = 1;
% 3. Versuch: Zur Untersuchung des Effektes von Filtern im Bereich mit der 
% schwächen Wirkung der Störgrößen
test3_active = 1;
% 5. Versuche: Anwendung des ausgewählten Filters
test5_active = 1;

%Berechnung der für das Filter-Design notwendigen Parameter
lidarMinRange = 6.25;
lidarMaxRange = 8000;
lidarMaxAngleRange = 180;
lidarResoluation = 0.5;


order = filterOrder(lidarMinRange,lidarMaxRange);
fs = filterSampleFreq(lidarMaxAngleRange,lidarResoluation);

if test0_active == 1
    test0
end

if test1_active == 1
    test1
end

if test2_active == 1
    test2
end

if test3_active == 1
    test3
end

if test5_active == 1
    test5
end





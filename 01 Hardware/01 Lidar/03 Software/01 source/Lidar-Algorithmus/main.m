% Autor: Thach
% Verwendungszweck: Gesamte Ausführung der Quellcode von anderen Daten
% Erstellt am 23.08.2023
% Version: 1.01
% Revision: 1.00

clear;
clc;
format shortG

% 0. Versuch: Überblick über die ausgewählten Filter-Entwurf-Methode
test0_active = 1;
% 1. Versuch: Zur Untersuchung des Effektes von Filtern 
test1_active = 0;
% 2. Versuch: Zur Bestimmung der Verlustpunkte am Rand
test2_active = 0;
% 3. Versuch: Zur Untersuchung des Effektes von Filtern im Bereich mit der 
% schwächen Wirkung der Störgrößen
test3_active = 0;
% 5. Versuche: Anwendung des ausgewählten Filters
test5_active = 0;

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





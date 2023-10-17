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
test2_active = 0;
% 3. Versuch: Zur Untersuchung des Effektes von Filtern im flachen Bereich
test3_active = 0;
% 4. Versuch: Zur Untersuchung des Effektes von Filtern im Bereich mit der 
% schwächen Wirkung der Störgrößen
test4_active = 0;
% checken das Program von C
test5_active = 1;

if test1_active == 1
    test1
end

if test2_active == 1
    test2
end

if test3_active == 1
    test3
end

if test4_active == 1
    test4
end

if test5_active == 1
    test5
end





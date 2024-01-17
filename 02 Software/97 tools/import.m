% Pfad zur JSON-Datei
jsonDateiPfad = 'datablock.json';

% JSON-Datei einlesen
jsonDaten = fileread(jsonDateiPfad);

% JSON-Daten decodieren
struktur = jsondecode(jsonDaten);

% Zugriff auf die Daten in der Struktur
disp(struktur);

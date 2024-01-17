Die Kommunikation zwischen dem Master und den einzelnen Slave-Modulen erfolgt über einen 1024-Byte großen Datenblock. Um die Adresszuordnung flexibel zu gestalten, können die Slave-Module über eine ID auf die jeweilige Adresse zugreifen. Der aktuelle Datenblock ist immer in der Datei „datablock.json“ hinterlegt.

Zugriff in Matlab
% Pfad zur JSON-Datei
jsonDateiPfad = 'datablock.json';

% JSON-Datei einlesen
jsonDaten = fileread(jsonDateiPfad);

% JSON-Daten decodieren
struktur = jsondecode(jsonDaten);

% Zugriff auf die Daten in der Struktur
disp(struktur);


Zugriff in Python
# read JSON-Data
json_file = 'datablock.json'
with open(json_file, 'r') as json_data:
    datablock = json.load(json_data)

function usvAddress = importUSVaddresses()

	% Pfad zur JSON-Datei
	jsonWebPath = 'https://raw.githubusercontent.com/Joe-Grabow/USV/main/02%20Software/97%20tools/datablock.json';
	
	% JSON-Datei einlesen
	data = webread(jsonWebPath);
	
	% JSON-Daten decodieren
	usvAddress = jsondecode(data);
	
	if nargout == 0
		% Zugriff auf die Daten in der Struktur
		disp(usvAddress);
		clear usvAddress
	end
end
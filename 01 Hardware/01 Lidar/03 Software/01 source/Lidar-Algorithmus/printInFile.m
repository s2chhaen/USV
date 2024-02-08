function val = printInFile(type,value)
    val = 0;
    %Anmerkung space = 32, new line = 10, CarriageReturn = 13
    %Erzeugt der "input.txt" Data in C-Daten-Ordner
    if strcmp(type,'fixed')
        fileID = fopen('../FilterTesting/FilterTesting/input_fixed_matlab.txt','w+','l','UTF-8');
        %Umwandlung zur Fixpoint-Form mit 15 Bits für Nachkommateil
        %Fixed Point Format 17.15
        fractionLen = 15;
        fixedPoint = zeros(1,numel(value));
        for i=1:numel(value)
            fixedPoint(i) = floor(value(i)*2^fractionLen);%TI-Form
        end
        %Schreiben der Daten in "input_fixed.txt"
        fprintf(fileID,'%d \n',fixedPoint);
        %Schließen der "input_fixed.txt" Datei
    elseif strcmp(type,'float')
        fileID = fopen('../FilterTesting/FilterTesting/input_float_matlab.txt','w+','l','UTF-8');
        %Schreiben der Daten in "input_fixed.txt"
        fprintf(fileID,'%7.4f \n',value);
        %Schließen der "input_fixed.txt" Datei
    end
    fclose(fileID);

end
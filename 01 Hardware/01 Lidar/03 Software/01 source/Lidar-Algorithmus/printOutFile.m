function val = printOutFile(type,filterName,value)
    val = 0;
    if strcmp(type,'fixed')
        path = strcat('../FilterTesting/FilterTesting/output_',filterName,'_fixed_matlab.txt');
        fileID = fopen(path,'w+','l','UTF-8');
        %Umwandlung zur Fixpoint-Form mit 15 Bits für Nachkommateil
        %Fixed Point Format 17.15
        fractionLen = 15;
        fixedPoint = zeros(1,numel(value));
        for i=1:numel(value)
            fixedPoint(i) = floor(value(i)*2^fractionLen);%TI-Form
        end
        %Schreiben der Daten in "input_fixed.txt"
        fprintf(fileID,'%d \n',fixedPoint);
        %Schließen der Datei
    elseif strcmp(type,'float')
        path = strcat('../FilterTesting/FilterTesting/output_',filterName,'_float_matlab.txt');
        fileID = fopen(path,'w+','l','UTF-8');
        %Schreiben der Daten in "input_fixed.txt"
        fprintf(fileID,'%7.4f \n',value);
        %Schließen der Datei
    end
    fclose(fileID);
end


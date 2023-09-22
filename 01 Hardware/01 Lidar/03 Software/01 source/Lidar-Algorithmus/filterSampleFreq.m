% Autor: Thach
% Verwendungszweck: notwendige Paramter fuer Datenfilter
% Erstellt am 01.09.2023
% Version: 1.00
% Revision: 1.00

function fs = filterSampleFreq(sample)
    if nargin~=1
        error('Eingabe ungültig');
    elseif ~isnumeric(sample)
        error('Eingabe ungültig');
    else
        %Annahme sample = f(x,t)
        fs = numel(sample)/2;
    end
end
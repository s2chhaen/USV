% Autor: Thach
% Verwendungszweck: notwendige Paramter fuer Datenfilter
% Erstellt am 01.09.2023
% Version: 1.00
% Revision: 1.00

function fs = filterSampleFreq(lidarMaxAngleRange,lidarResoluation)
    if nargin~=2
        error('Eingabe ungültig');
    else
        wf = 0.5;
        fs = wf*(lidarMaxAngleRange/lidarResoluation+1);
    end
end
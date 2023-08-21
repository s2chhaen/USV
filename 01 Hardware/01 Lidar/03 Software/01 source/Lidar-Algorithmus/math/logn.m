% Autor: Thach
% Verwendungszweck: Berechnung der Logarithmus mit Base n
% Erstellt am 21.08.2023
% Version: 1.00
% Revision: 1.00

function realVal = logn(base,input)
%LOGN Summary of this function goes here
%   Detailed explanation goes here
    if nargin~=2
        error('nicht genug Parameter');
    elseif isempty(input) || isempty(base)
        error('Eingabe ungültig');
    elseif (base~='e' && ~isnumeric(base)) || (base==0)
        error('Eingabe ungültig');
    elseif input==0
        error('Eingabe ungültig');
    else
        switch base
            case 'e'
                realVal = log(input);
            otherwise
                realVal = log10(input)/log10(base);
        end
    end
end


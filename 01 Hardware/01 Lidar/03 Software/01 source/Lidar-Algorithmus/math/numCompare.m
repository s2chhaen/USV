function compVal = numCompare(a,b)
%NUMCOMPARE Summary of this function goes here
%   Detailed explanation goes here
    if nargin~=2
        error('nicht genug Parameter für Funktion');
    elseif isempty(a)||isempty(b)
        error('Eingabe nicht gültig');
    elseif ~isnumeric(a)||~isnumeric(b)
        error('Eingabe ist keine Nummer');
    else
    if a>b
        compVal = 1;
    elseif a<b
        compVal = -1;
    else
        compVal = 0;
    end
    end
end


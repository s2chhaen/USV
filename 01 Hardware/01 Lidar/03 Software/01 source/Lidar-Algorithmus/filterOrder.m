function order = filterOrder(minIn,maxIn)
    order = 1;
    if nargin ~= 2
        error('Eingabe ungültig');
    elseif ~isnumeric(maxIn) || ~isnumeric(minIn)
        error('Eingabe ungültig');
    else
        if maxIn == 0
            maxIn = 1;
        end
        if minIn == 0
            minIn = 1;
        end

        if maxIn >= minIn
            order = floor(logn(10,maxIn/minIn)+1);
        end
    end
end


% Autor: Thach
% Verwendungszweck: Modellierung eines Stackes
% Erstellt am 25.08.2023
% Version: 1.00
% Revision: 1.00

classdef stackClass  
    properties (Access = private)
        buffer{mustBeNumeric}
    end
    
    methods
        function obj = push(obj,input)
            if nargin ~=2
                error('Anzahl der Eingabe ungültig');
            elseif isempty(obj)||isempty(input)||~isnumeric(input)
                error('Eingabe ungültig');
            else
                obj.buffer = cat(2,input,obj.buffer);
            end
        end

        function [obj,numVal] = pop(obj)
            if nargin ~=1
                error('Anzahl der Eingabe ungültig');
            elseif isempty(obj)
                error('Eingabe ungültig');
            else
                if isempty(obj.buffer)
                    numVal = [];
                else
                    temp = obj.buffer;
                    numVal = obj.buffer(1,1);
                    obj.buffer = temp(1,2:end);
                end
            end
        end

        function numVal = stackLen(obj)
             if nargin ~=1
                error('Anzahl der Eingabe ungültig');
            elseif isempty(obj)
                error('Eingabe ungültig');
             else
                numVal = numel(obj.buffer);
             end
        end

    end
end


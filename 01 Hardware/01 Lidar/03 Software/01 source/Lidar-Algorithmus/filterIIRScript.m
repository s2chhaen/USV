rMax = max(rSample);
rMin = min(rSample);

%Filter param
order = filterOrder(rMin,rMax);
fs = filterSampleFreq(sample);

draw = 0;
filtered = 1;

%IIR - Filter
fiir = filterMaximallyFlatFIR_p4;%Eingetragen ist die Filter Parameter
[b,a] = sos2tf(fiir.sosMatrix);%Faktoren von Übertragungsfunktion

if filtered==1
    result0 = rSample;
    tempR = rSample;

    l = length(a);
    %Dummy-Sample erzeugen um die Phasenverschiebung zu vermeiden
    tempR = [tempR,ones(1,fix(l/2))*tempR(end)];
    %IIR Filter
    result1 = filter(b,a,tempR);
    %Wegwerfen vom Dummy-Sample
    result1 = result1(ceil(l/2):end);
end

if draw==1
    figure(figureNo)
    figureNo = figureNo + 1;
    ax(1) = subplot(2,1,1);
    plot(tVal,result0,'-o',tVal,result1,'-o');
    legend('Original Data','Filtered Data');
    grid on

    xlabel('Winkel/Grad');
    ylabel('Radius/cm');

    ax(2) = subplot(2,1,2);
    plot(tVal,20.*log10(result1./result0),'-o');
    legend('magnitude/dB');
    grid on

    xlabel('Winkel/Grad');
    ylabel('Radius/cm');

    linkaxes(ax,'x')
end
clear draw filtered tempR
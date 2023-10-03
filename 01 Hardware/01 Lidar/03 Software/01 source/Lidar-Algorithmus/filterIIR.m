function outputVal = filterIIR(inputVal, inputTi, filterObj, draw, phase, figureNo)
[b,a] = sos2tf(filterObj.sosMatrix);

temp0 = [inputVal ones(1,phase)*inputVal(end)];
temp0 = filter(b,a,temp0);
temp0 = temp0((phase+1):end);
outputVal = temp0;

if draw == 1
    figure(figureNo);

    ax(1) = subplot(2,1,1);
    plot(inputTi,inputVal,'-o',inputTi,outputVal,'-o');
    legend('Original Data','Filtered Data','Location','northeastoutside');
    grid on
    xlabel('Winkel/Grad');
    ylabel('Radius/cm');

    ax(2) = subplot(2,1,2);
    plot(inputTi,20.*log10(outputVal./inputVal),'-o');
    legend('magnitude/dB','Location','northeastoutside');
    grid on
    xlabel('Winkel/Grad');
    ylabel('Radius/dB');
    linkaxes(ax,'x');
end

end
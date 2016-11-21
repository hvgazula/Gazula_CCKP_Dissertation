function fullData = CCKP_CalculateDifference(myExcelData)

% Generate variable combinations
myDataFirstPart             = allcomb(1000:100:5000, 0.25:0.25:1.00, 0.25:0.25:1.00);
myDataFirstPart(:, [2 3])   = bsxfun(@times, myDataFirstPart(:,1), myDataFirstPart(:, [2 3]));

fullData    = [];
for combNum = 1 : length(myDataFirstPart)
    check       = ismember(myExcelData(:, [2 3 4]), repmat(myDataFirstPart(combNum, :), [656*17 1]), 'rows');
    currData    = myExcelData(check, :);
    
    try
        WithoutCutTime          = currData(1, 7);
        WithoutCutSolStat       = currData(1, 12);
        WithoutCutInfo          = [WithoutCutTime WithoutCutSolStat];
        WithCutInfo             = currData(2:end, [5 7 12]);
        [myMin, myMinIndex]     = min(WithCutInfo(:, 2));
        BestWithCutTime         = WithCutInfo(myMinIndex, 2);
        BestTimeGap             = WithoutCutTime - BestWithCutTime;
        BestPercentReduction    = BestTimeGap/WithoutCutTime;
        BestPercentReduction(~isfinite(BestPercentReduction)) = NaN; % newly added
        
        myData = [myDataFirstPart(combNum, :) WithoutCutInfo ...
            WithCutInfo(myMinIndex, [2 3 1]) BestTimeGap BestPercentReduction];
        
        if isnan(myMin)
            myData = [myData NaN];
        elseif myMinIndex <=8
            myData = [myData 1];
        else
            myData = [myData 0];
        end
        
        fullData = [fullData; myData];
    catch
        continue
    end
end

fullData = fullData(:, [1:8 11 9:10]);
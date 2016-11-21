function CCKP_AverageResults()

close all; clear; clc
ParentFolder = 'C:\Users\harsh\Documents\MATLAB\CCKP_Research_Computations\20160724_CCKP\Final_Results';
ResultSets      = dir(fullfile(ParentFolder, 'All_Results_Set_*'));

for setNum = 1 : length(ResultSets)
    SheetName       = ['Set_' num2str(setNum, '%02d')];
    CurrFileList    = dir(fullfile(ParentFolder, ResultSets(setNum).name, '\*.xls'));
    myExcelData     = CCKP_DLMRead(CurrFileList, ParentFolder, ResultSets(setNum).name);
    
end

myDataFirstPart             = allcomb(1000:100:5000, 0.25:0.25:1.00, 0.25:0.25:1.00);
myDataFirstPart(:, [2 3])   = bsxfun(@times, myDataFirstPart(:,1), myDataFirstPart(:, [2 3]));

CutSizeVec = [0 1:5 10:10:30 1:5 10:10:30]';
CondensedData = [];
for N = 1000 : 100 : 5000
    for M = .25 : .25 : 1.00
        for D = .25 : .25 : 1.00
            part11 = myExcelData1(:, 2) == N;
            part12 = myExcelData1(:, 3) == N*M;
            part13 = myExcelData1(:, 4) == N*D;
            
            part21 = myExcelData2(:, 2) == N;
            part22 = myExcelData2(:, 3) == N*M;
            part23 = myExcelData2(:, 4) == N*D;
            
            part31 = myExcelData3(:, 2) == N;
            part32 = myExcelData3(:, 3) == N*M;
            part33 = myExcelData3(:, 4) == N*D;
            
            part41 = myExcelData4(:, 2) == N;
            part42 = myExcelData4(:, 3) == N*M;
            part43 = myExcelData4(:, 4) == N*D;
            
            CurrSet01 = myExcelData1(part11 & part12 & part13 , :) ;
            CurrSet02 = myExcelData2(part21 & part22 & part23 , :) ;
            CurrSet03 = myExcelData3(part31 & part32 & part33 , :) ;
            CurrSet04 = myExcelData4(part41 & part42 & part43 , :) ;
            
            TimeData = NaN(17, 4);
            TimeData(1:length(CurrSet01(:,7)),1) = CurrSet01(:,7);
            TimeData(1:length(CurrSet02(:,7)),2) = CurrSet02(:,7);
            TimeData(1:length(CurrSet03(:,7)),3) = CurrSet03(:,7);
            TimeData(1:length(CurrSet04(:,7)),4) = CurrSet04(:,7);
            
            TimeAverage = mean(TimeData, 2, 'omitnan');
            AverageData = [repmat([N N*M N*D], size(TimeAverage)) CutSizeVec TimeAverage];
            CondensedData = [CondensedData; AverageData];
        end
    end
end

%% Code to calculate differences in time for each problem
warning('off');
fileName = 'Results_Difference_Test.xlsx';
titleVec = {'N', 'M', 'D', 'Time (w/o cut)', '# of cuts', 'Time (w/cut)','without - with',  '% Reduction', 'cut type'};

% (After all averages)
SheetName   = 'Average';
fullData    = CCKP_CalculateDifference(CondensedData);

xlswrite(fileName, titleVec, SheetName, 'B2');
xlswrite(fileName, fullData, SheetName, 'B3');

end
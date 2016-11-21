close all; clear; clc

ParentFolder = 'C:\Users\harsh\Documents\MATLAB\CCKP_Research_Computations\20160724_CCKP\Final_Results';

ResultSets      = dir(fullfile(ParentFolder, 'All_Results_Set_*'));
SolStatNoCut    = cell(size(ResultSets'));
VarRange        = 1000:100:5000;
TitleVec        = {'N', 'M', 'D', 'Set_01', 'Set_02', 'Set_03', 'Set_04', 'Set_05'};
ExcelFileName   = 'SolutionStatus_0806.xlsx';
SheetName       = 'Sheet2';
SelectIndex     = 1 : 17 : 656*17;

myDataFirstPart = allcomb(VarRange, 0.25:0.25:1.00, 0.25:0.25:1.00);
myDataFirstPart(:, [2 3]) = bsxfun(@times, myDataFirstPart(:,1), myDataFirstPart(:, [2 3]));

for setNum = 1 : length(ResultSets)
    CurrFileList            = dir(fullfile(ParentFolder, ResultSets(setNum).name, '\*.xls'));
    myExcelData             = CCKP_DLMRead(CurrFileList, ParentFolder, ResultSets(setNum).name);
    CurrSolStatData         = myExcelData(:, end);
    reshapedSolStat         = reshape(CurrSolStatData, [17, 656]);
    SolStatNoCut            = reshapedSolStat(1, :)';
    SolStatWithCut          = min(reshapedSolStat(2:end, :))';
    SolStat{setNum}         = [SolStatNoCut SolStatWithCut];
end

fullSolStatData = [myDataFirstPart cell2mat(SolStat)];

xlswrite(fullfile(ParentFolder, ExcelFileName), TitleVec, SheetName, 'B2');
xlswrite(fullfile(ParentFolder, ExcelFileName), fullSolStatData, SheetName, 'B3');

winopen(fullfile(ParentFolder, ExcelFileName));
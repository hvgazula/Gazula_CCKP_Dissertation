% Code written before accumulating all results
% Author        : Harsh Gazula
% Created       : 08/06/2016
% Description   : 

% Preprocessing
close all; clear; clc

% Folder where results are present and their names
% ParentFolder    = 'C:\Users\harsh\Documents\MATLAB\CCKP_Research_Computations\20160724_CCKP\Final_Results';
ParentFolder    = uigetdir(); 
ResultSets      = dir(fullfile(ParentFolder, 'All_Results_Set_*'));
fileName        = 'Results_Normal_Wall_Clock.xlsx';
titleVec        = {'N', 'M', 'D',...
    'Time (w/o cut)', 'Status', ...
    'Time (w/cut)', 'Status', '#_of_cuts', 'Cut_Type', ...
    'Best_Time_Gap',  '%_Reduction',};

warning('off','MATLAB:xlswrite:AddSheet');

% Process results for each folder
for setNum = 1 : length(ResultSets)
    SheetName       = ['Set_' num2str(setNum, '%02d')];
    CurrFileList    = dir(fullfile(ParentFolder, ResultSets(setNum).name, '\*.xls'));
    myExcelData     = CCKP_DLMRead(CurrFileList, ParentFolder, ResultSets(setNum).name);
    fullData        = CCKP_CalculateDifference(myExcelData);
    
    xlswrite(fileName, titleVec, SheetName, 'B2');
    xlswrite(fileName, fullData, SheetName, 'B3');
end
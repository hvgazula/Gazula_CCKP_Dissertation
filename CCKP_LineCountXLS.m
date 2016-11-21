close all;
clear; clc;

myDir = uigetdir();

fileList = dir([myDir '\*.xls']);

myVec = [];
for fileNum = 1 : length(fileList)
    currFile = fileList(fileNum).name;
    
    C = strsplit(currFile, '_');
    
    N = str2double(C{1});
    M = str2double(C{2});
    D = str2double(C{3});
    
    try
        myText = dlmread(fullfile(myDir, currFile));
        lineCount = size(myText, 1);
        myVec = [myVec; N M D lineCount];
    catch
        myVec = [myVec; N M D NaN];
    end
end
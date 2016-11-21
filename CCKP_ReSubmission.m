close all; clear; clc

ParentDir = pwd;

for VarSize = 1000 : 100 : 5000
    CurrDir = ['Problem_Set_'  num2str(VarSize)];
    fprintf('Entering Directory: %s\n', CurrDir);
    cd(CurrDir);
    cd('Results');
    fileList = dir('*.xls');
    
    FailList = cell(0);
    for fileNum = 1 : length(fileList)
        CurrFile = fileList(fileNum).name;
        
        try
            CurrFileText = dlmread(CurrFile);
            if size(CurrFileText, 1) ~= 17
                FailList = [FailList; CurrFile];
            end
        catch
            FailList = [FailList; CurrFile];
        end
    end
    
    cd('..');
    
    for i  = 1 : length(FailList)
        [~, CurrFileName, ~] = fileparts(FailList{i});
        system(['qsub qsub_' CurrFileName '.sh']);
    end
    cd ('..')
end
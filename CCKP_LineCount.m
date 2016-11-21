close all; clear; clc

ParentFolder = pwd;
myExcel = 'myCount.xls';
openID = fopen(fullfile(ParentFolder, 'myCount.xls'), 'w');

for VarSize = 1000 : 100 : 5000
    
    CurrDirName = ['Problem_Set_' num2str(VarSize)];
    fprintf('Entering Directory: %s\n', CurrDirName);
    cd(CurrDirName);
    system('rm -f count.xls');
    fileList = dir('qsub*.sh.o*');
    
    for numFile = 1 : length(fileList)
        CurrFileName = fileList(numFile).name;
        
        [~, result] = system(['wc -l ' CurrFileName]);
        result = strsplit(result);
        [~, CurrFileNameStr, ~] = fileparts(CurrFileName);
        C                       = strsplit(CurrFileNameStr, '_');
        fprintf(openID, '%d\t%d\t%d\t%d\n', str2double(C{2}), str2double(C{3}), ...
            str2double(C{4}), str2double(result{1}));
    end
    cd('..');
end

fclose(openID);
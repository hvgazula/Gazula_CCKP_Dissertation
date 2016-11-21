% File Name       : CCKP_CreateFolders.m
% Created by      : Harshvardhan Gazula
% Created on      : 07/27/2016
% Modified by     : Harshvardhan Gazula

close all; clear; clc

% datestr(date, 'yyyymmdd')
% $HOME
% getenv('USER') or echo $USER

% Changeable
ParentFolder    = pwd; 
ExecFolder      = ParentFolder;
MinVarSize 		= 1000;
MaxVarSize 		= 5000;
VarStepSize 	= 100;
VarSizeRange 	= MinVarSize : VarStepSize : MaxVarSize;

warning('off','all')

system(['mv CCKP_Carcardinality_Cuts.cpp ' fullfile(ParentFolder, 'cardinality2.cpp')]);

unix(['g++ -c -m64 -fPIC -I/$CPLEXINC ' fullfile(ParentFolder, 'cardinality2.cpp') ' -o ' fullfile(ParentFolder, 'cardinality2.o')]);
unix(['g++ -m64 -fpic -I/$CPLEXINC ' fullfile(ParentFolder, 'cardinality2.o') ' -o ' ...
    fullfile(ParentFolder, 'cardinality2.exe') ' -L/$CPLEXLIB -lcplex -lm -pthread']);
delete(fullfile(ParentFolder, 'cardinality2.o'));
delete(fullfile(ParentFolder, 'cardinality2.cpp'));

for VarSize = VarSizeRange
    
    CurrDirName = ['Problem_Set_' num2str(VarSize)];
    fprintf('Entering Folder: %s\n', CurrDirName);
    
    system(['rm -f ' fullfile(ParentFolder, CurrDirName, [CurrDirName '.exe'])]);
    system(['rm -f ' fullfile(ParentFolder, CurrDirName, [CurrDirName '.cpp'])]);
    system(['rm -f ' fullfile(ParentFolder, CurrDirName, ['Set_' num2str(VarSize) '.sh'])]);
    system(['rm -f ' fullfile(ParentFolder, CurrDirName, ['Set_' num2str(VarSize) '.sh.o*'])]);
    

    FinalFileList = dir(fullfile(ParentFolder, CurrDirName, '/*.lp'));
    mkdir(fullfile(ParentFolder, CurrDirName), 'Instances');
    mkdir(fullfile(ParentFolder, CurrDirName), 'Inputs');
    mkdir(fullfile(ParentFolder, CurrDirName), 'Results');
    mkdir(fullfile(ParentFolder, CurrDirName), 'Logs');
    mkdir(fullfile(ParentFolder, CurrDirName), 'Output');
    
    AllSubFileName  = ['All_' num2str(VarSize) '.sh'];
    AllSubFileID    = fopen(fullfile(ParentFolder, CurrDirName, AllSubFileName), 'w');
    fprintf(AllSubFileID, '#!/bin/bash\n');
    fprintf(AllSubFileID, ['cd ' fullfile(ParentFolder, CurrDirName) '\n']);
    
    for FileNum = 1 : length(FinalFileList)
        CurrFileName            = FinalFileList(FileNum).name;
        [~, CurrFileNameStr, ~] = fileparts(CurrFileName);
        InputFileName           = ['Input_'  CurrFileNameStr];
        OutputFileName          = ['Output_' CurrFileNameStr];
        QsubFileName            = ['N_'   CurrFileNameStr '.sh'];
        
        InputFileID             = fopen(fullfile(ParentFolder, CurrDirName, 'Inputs', InputFileName), 'w');
        C                       = strsplit(CurrFileNameStr, '_');
        fprintf(InputFileID, '%d\n', 1);
        fprintf(InputFileID, 'Instances/%s\n', CurrFileName);
        fprintf(InputFileID, '%d\n', str2double(C{1}));
        fprintf(InputFileID, '%d\n', str2double(C{2}));
        fprintf(InputFileID, '%d\n', str2double(C{3}));
        fprintf(InputFileID, '%d\n', 0.2*str2double(C{1}));
        fprintf(InputFileID, '%d\n', str2double(C{4}));
        fclose(InputFileID);
        
        QsubFileID = fopen(fullfile(ParentFolder, CurrDirName, QsubFileName), 'W+t');
        fprintf(QsubFileID, '#!/bin/bash\n');
        fprintf(QsubFileID, '#$ -S /bin/bash\n');
        fprintf(QsubFileID, '#$ -N %s\n', QsubFileName);
        fprintf(QsubFileID, ['#$ -wd ' fullfile(ParentFolder, CurrDirName) '\n']);
        fprintf(QsubFileID, '#$ -V\n');
        fprintf(QsubFileID, '#$ -q normal\n');
        fprintf(QsubFileID, '#$ -pe fill 12\n');
        fprintf(QsubFileID, '#$ -o $JOB_NAME.o$JOB_ID\n');
        fprintf(QsubFileID, '#$ -j y\n');
        fprintf(QsubFileID, '#$ -P hrothgar\n');
        fprintf(QsubFileID, '\n');
        
        fprintf(QsubFileID, 'date\n');
        printString = [fullfile(ParentFolder, 'cardinality2.exe') ' < Inputs/' InputFileName ' > Results/' OutputFileName];
        fprintf(QsubFileID, printString);
        fprintf(QsubFileID, '\ndate');
        fclose(QsubFileID);
        
        fprintf(AllSubFileID, 'qsub %s\n', QsubFileName);
        movefile(fullfile(ParentFolder, CurrDirName, CurrFileName), fullfile(ParentFolder, CurrDirName, 'Instances'));
    end
    
    fclose(AllSubFileID);    
    unix(['. ' fullfile(ParentFolder, CurrDirName, AllSubFileName)]);
end
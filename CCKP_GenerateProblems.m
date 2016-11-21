% File Name     : CCKP_GenerateProblems.m
% Created by    : Harshvardhan Gazula
% Modified on   : 07/29/2016
% To be Done 	: 1) Parallelize code (OpenMPI for C/C++, parfor in MATLAB)

% This code creates one Problem Generator (CCKP_Problem_Generator) for each variable
% size and then creates a job submission file to be run on different processors
% to generate the instances.

close all; clear; clc

ParentFolder = pwd;

MinVarSize 		= 1000;
MaxVarSize 		= 5000;
VarStepSize 	= 100;

% VarSizeRange = MinVarSize : VarStepSize : MaxVarSize;
FileContents = fileread('CCKP_Problem_Generator.cpp');

for VarSize = MinVarSize : VarStepSize : MaxVarSize;
    CurrDirName = ['Problem_Set_' num2str(VarSize)];
    fprintf('Creating Directory: %s\n', CurrDirName);
    mkdir(ParentFolder, CurrDirName)
    
    FileContents1   = strrep(FileContents, '1000', num2str(VarSize));
    GenFileName     = [CurrDirName '.cpp'];
    GenFileID       = fopen(fullfile(ParentFolder, CurrDirName, GenFileName), 'w');
    
    fwrite(GenFileID, FileContents1);
    fclose(GenFileID);
    
    ExeFileName     = [CurrDirName '.exe'];
    unix(['g++ -o ' fullfile(ParentFolder, CurrDirName, ExeFileName) ' ' ...
        fullfile(ParentFolder, CurrDirName, GenFileName)]);
    
    SubmissionFileName  = ['Set_' num2str(VarSize) '.sh'];
    SubmissionFileID    = fopen(fullfile(ParentFolder, CurrDirName, SubmissionFileName), 'w');
    fprintf(SubmissionFileID, '#!/bin/bash\n');
    fprintf(SubmissionFileID, '#$ -S /bin/bash\n');
    fprintf(SubmissionFileID, '#$ -N %s\n', SubmissionFileName);
    fprintf(SubmissionFileID, ['#$ -wd ' fullfile(ParentFolder, CurrDirName) '\n']);
    fprintf(SubmissionFileID, '#$ -V\n');
    fprintf(SubmissionFileID, '#$ -q serial\n');
    fprintf(SubmissionFileID, '#$ -pe fill 1\n');
    fprintf(SubmissionFileID, '#$ -o $JOB_NAME.o$JOB_ID\n');
    fprintf(SubmissionFileID, '#$ -j y\n');
    fprintf(SubmissionFileID, '#$ -P hrothgar\n');
    %     fprintf(SubmissionFileID, '#$ -M id@gmail.com\n');
    %     fprintf(SubmissionFileID, '#$ -m abe\n');
    fprintf(SubmissionFileID, '\n');
    
    fprintf(SubmissionFileID, 'date\n');
    fprintf(SubmissionFileID, fullfile(ParentFolder, CurrDirName, ExeFileName));
    fprintf(SubmissionFileID, '\ndate');
    fclose(SubmissionFileID);
    
    unix(['qsub ' fullfile(ParentFolder, CurrDirName, SubmissionFileName)]);
end

% If you wish to submit all the generator problems at once comment line 47 and
% uncomment lines 52-56
% for VarSize = VarSizeRange
%     CurrDirName = ['Problem_Set_' num2str(VarSize)];
%     SubmissionFileName = ['Set_' num2str(VarSize) '.sh'];
%     unix(['qsub ' fullfile(ParentFolder, CurrDirName, SubmissionFileName)]);
% end
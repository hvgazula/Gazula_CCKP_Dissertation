close all; clear; clc

myDataFirstPart = allcomb(1000:100:5000, 0.25:0.25:1.00, 0.25:0.25:1.00);
myDataFirstPart(:, [2 3]) = bsxfun(@times, myDataFirstPart(:,1), myDataFirstPart(:, [2 3]));

ParentDir = pwd;
currSetNum = ParentDir(end-1:end);

count = 0;
for VarSize = 1000 : 100 : 5000
    CurrDir = ['Problem_Set_'  num2str(VarSize)];
    fprintf('Entering Directory: %s\n', CurrDir);
    cd(CurrDir);
    system('rm -f core.*');
    cd('Results');
    
    fileList = myDataFirstPart(myDataFirstPart(:,1) == VarSize, :);
    %     fileList = dir('*.xls');
    
    FailList = cell(0);
    for fileNum = 1 : length(fileList)
        CurrFile = [num2str(fileList(fileNum, 1), '%04d') '_' num2str(fileList(fileNum, 2), '%04d') '_' num2str(fileList(fileNum, 3), '%04d') '_' currSetNum '.xls'];
        %         CurrFile = fileList(fileNum).name;
        
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
    
    count = count + length(FailList);
    
    for i  = 1 : length(FailList)
        [~, CurrFileName, ~] = fileparts(FailList{i});
        system(['qsub N_' CurrFileName '.sh']);
    end
    cd ('..')
end

fprintf('\n A total of %d file were re-submitted:\n', count);
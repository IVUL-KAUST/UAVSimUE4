function [aveErrCoverage, aveErrCenter,errCoverage, errCenter] = calcSeqErrRobust(rects, rect_anno)

% LineWidth = 2;
% LineStyle = '-';%':';%':' '.-'

% lostCount = zeros(length(seqs), length(trks));
% thred = 0.33;
% 
% errCenterAll=[];
% errCoverageAll=[];

seq_length = min(size(rects,1), size(rect_anno,1));

if size(rects,1) ~= size(rect_anno,1),
 		fprintf('%12s - Number of ground truth frames does not match number of tracked frames.\n', title)
		%just ignore any extra frames, in either results or ground truth
		seq_length = min(size(rects,1), size(rect_anno,1));
		rects(seq_length+1:end,:) = [];
		rect_anno(seq_length+1:end,:) = [];
end


    for i = 2:seq_length
        r = rects(i,:);
        r_anno = rect_anno(i,:);
        if (isnan(r) | r(3)<=0 | r(4)<=0)&(~isnan(r_anno))
            results.res(i,:)=results.res(i-1,:);
%             results.res(i,:) = [1,1,1,1];
        end
    end

centerGT = [rect_anno(:,1)+(rect_anno(:,3)-1)/2 rect_anno(:,2)+(rect_anno(:,4)-1)/2];

rectMat = rects;              
rectMat(1,:) = rect_anno(1,:);

% center = [rectMat(:,1)+(rectMat(:,3)-1)/2 rectMat(:,2)+(rectMat(:,4)-1)/2];
% errCenter = sqrt(sum(((center(1:seq_length,:) - centerGT(1:seq_length,:)).^2),2));
% 
% index = rect_anno>0;
% idx=(sum(index,2)==4);
% % errCoverage = calcRectInt(rectMat(1:seq_length,:),rect_anno(1:seq_length,:));
% tmp = calcRectInt(rectMat(idx,:),rect_anno(idx,:));
% 
% errCoverage=-ones(length(idx),1);
% errCoverage(idx) = tmp;
% errCenter(~idx)=-1;
% 
% aveErrCoverage = sum(errCoverage(idx))/length(idx);
% aveErrCenter = sum(errCenter(idx))/length(idx);

center = [rectMat(:,1)+(rectMat(:,3)-1)/2 rectMat(:,2)+(rectMat(:,4)-1)/2];
errCenter = sqrt(sum(((center(1:seq_length,:) - centerGT(1:seq_length,:)).^2),2));
errCoverage = calcRectInt(rectMat(1:seq_length,:),rect_anno(1:seq_length,:));
aveErrCoverage = sum(errCoverage)/seq_length;
aveErrCenter = sum(errCenter)/seq_length;



%
%  High-Speed Tracking with Kernelized Correlation Filters
%
%  Matthias Mueller 2016
%  https://ivul.kaust.edu.sa/Pages/pub-benchmark-simulator-uav.aspx
%
%  Interface for running the tracker KCF_SMXPC in the simulation environment.
%
%  Based on the code by Joao F. Henriques, 2014

close all, clear all
result_path = 'results/';
tracker_name = 'KCF_SMXPC';
thresholdPrecision = 20;
thresholdOverlap = 0.5;

kernel_type = 'linear';
feature_type = 'hogcolor';
padding = 1.5; 
lambda = 1e-4; 
output_sigma_factor = 0.1;
step = 0.5;
maxFrames = 100;

%Show Matlab visualization (this affects frame rate!)
show_visualization = 0; 
%Show precision plot
show_plots = 1;
%Save images, groundtruth and tracking result to disk
save_results = 1;

%parameters based on the chosen kernel or feature type
kernel.type = kernel_type;

features.gray = false;
features.hog = false;
features.hogcolor = false;

switch feature_type
    case 'gray',
        interp_factor = 0.075;  %linear interpolation factor for adaptation
        kernel_sigma = 0.2;  %gaussian kernel bandwidth
        kernel.sigma = kernel_sigma;
        kernel.poly_a = 1;  %polynomial kernel additive term
        kernel.poly_b = 7;  %polynomial kernel exponent
        cell_factor = 0;        
        hog_orientations = 0;
        features.gray = true;
        
    case 'hog',
        interp_factor = 0.02; 
        kernel_sigma = 0.5;  %gaussian kernel bandwidth
        kernel.sigma = kernel_sigma;
        kernel.poly_a = 1;
        kernel.poly_b = 9;
        cell_factor = 16; 
        hog_orientations = 9; 
        features.hog_orientations = hog_orientations;
        features.hog = true;
        
    case 'hogcolor',
        interp_factor = 0.01; 
        kernel_sigma = 0.5;
        kernel.poly_a = 1;
        kernel.poly_b = 9;
        cell_factor = 16; 
        hog_orientations = 9; 
        features.hog_orientations = hog_orientations;
        features.hogcolor = true;
        
    otherwise
        error('Unknown feature.')
        
end

assert(any(strcmp(kernel_type, {'linear', 'polynomial', 'gaussian'})), 'Unknown kernel.') 

        
		%call tracker function with all the relevant parameters
		[positions, rects, ground_truth, time] = tracker(padding, kernel, lambda, output_sigma_factor, interp_factor, ...
			cell_factor, features, show_visualization,step,maxFrames,save_results);

        close all;
        %calculate and show precision plot
        precisions = precision_plot(positions, ground_truth, 'UE4 Simulator', show_plots);
        %return precisions at a 20 pixels threshold
        %precision = precisions(20);
        
        %calculate precision, overlap and fps
        if (~exist ('rects','var'))
            target_sz_vec = repmat(fliplr(target_sz),[length(positions) 1]);
            rects = [fliplr(positions) - target_sz_vec/2, target_sz_vec];
        end
        
        [aveErrCoverage, aveErrCenter,errCoverage, errCenter] = calcSeqErrRobust(rects, ground_truth);
        overlap = sum(errCoverage > thresholdOverlap)/length(errCoverage);
        precision = sum(errCenter <= thresholdPrecision)/length(errCenter);
        fn = maxFrames;
        fps = maxFrames / time;
        fprintf('Precision(%upx): %.3f, Overlap(%u%%): %.3f, FPS: %.4g,\tFN: %u \n', thresholdPrecision, precision, thresholdOverlap*100, overlap, fps, fn)


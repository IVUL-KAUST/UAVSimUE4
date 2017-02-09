function [best_scale, pos,response] = tracker_multi_KCF(im, pos,kernel,cell_size, ...
    features,window_sz,...
    cos_window,model_xf,model_alphaf,num_scale,scales,scale_weights,w2c)

size_ver = size(cos_window,1);
size_hor = size(cos_window,2);
response_all = zeros(size_ver*size_hor,num_scale);

for j=1:num_scale
        %obtain a subwindow for detection at the position from last
        %frame, and convert to Fourier domain (its size is unchanged)

        patch = get_subwindow(im,pos,round(window_sz*scales(j)));
        if (~features.hogcolor)
            patch = videoResize(patch,window_sz);
        else
            patch = imresize(patch,window_sz,'nearest');
        end
        zf = fft2(get_features(patch, features, cell_size, cos_window,w2c));

        %calculate response of the classifier at all shifts
        switch kernel.type
        case 'gaussian',
            kzf = gaussian_correlation(zf, model_xf, kernel.sigma);
        case 'polynomial',
            kzf = polynomial_correlation(zf, model_xf, kernel.poly_a, kernel.poly_b);
        case 'linear',
            kzf = linear_correlation(zf, model_xf);
        end
        response = real(ifft2(model_alphaf .* kzf)*numel(kzf));  %equation for fast detection
        response_all(:,j) = response(:);
end

        weighted_scales = max(response_all).*scale_weights; %weighted filters
        [~ , scale_ind] =max(weighted_scales);
        best_scale = scales(scale_ind) ;
        response = reshape(response_all(:,scale_ind),[size_ver size_hor]);
end





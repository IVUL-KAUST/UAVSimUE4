%author: Stefan Karlsson, 2013.
% This function performs the same task as for built in matlab function "im resize",
% but is MUCH faster. It remembers its last parameters, and performs the
% same bilinear interpolation faster on succesive calls (assuming the size of the images are the same)
% initialized, but even with the initialization it is faster than imresize.
% Can only handle "bilinear"(requires image acquisition toolbox) or "nearest" neighbour interpolation. 
% This is especially useful for processing video, and this function can
% with ease be made even more efficient. As it is now, it would improve the
% computer vision toolbox simulink modul for resizing video. 

function imOut = videoResize(imIn, newSizeIn, typeStr)
persistent  weights indices oldSize newSize scale typeNumber order;

%initializiation, sets the persistants
if isempty(typeNumber) || nargin >1
    if nargin == 1,
        error('you need to initialize this function with a new output size before running it with one argument');
    end
    if (nargin < 3) || strcmpi(typeStr,'nearest')
        typeNumber = 0;%nearest neighbour by default
    elseif strcmpi(typeStr,'bilinear')
        typeNumber = 1;
    else
        error(['only "bilinear" and "nearest" methods supported, unknown type:' typeStr]);
    end
        
    if typeNumber %bilinear
        if ~(exist('imresizemex') == 3)
            msgbox('To get this function working you need the image processing toolbox. You then need to find the file "imresizemex.???", which is located in a toolbox private folder. See "VideoResize.txt" for details.');
            error('see videoresize.txt for install instructions');
        end
        [weights, indices, order] = imresizeParamsBiLinear(imIn, newSizeIn, 'bilinear');
%         save;error;
    else %nearest neighbour
        newSize = newSizeIn;
        oldSize = size(imIn);  
        scale = newSize./oldSize;    
    end
end %end of initialization


%do the resizing:
if typeNumber
    imOut = imresizemex(imresizemex(imIn, weights{order(1)}', indices{order(1)}', order(1)),weights{order(2)}', indices{order(2)}', order(2));
else
    imOut = imIn(...
        min(round(((1:newSize(1))-0.5)./scale(1)+0.5),oldSize(1)),...
        min(round(((1:newSize(2))-0.5)./scale(2)+0.5),oldSize(2))     );
end






%%%%%% The rest of this m-file is just copy-pasted from "imresize.m", part
%%%%%% of the image processing toolbox. Its been modified to output the
%%%%%% desired initialization structures, and nothing else. It is useless
%%%%%% to anyone, if they do not have the toolbox.

function [weights,indices,order] = imresizeParamsBiLinear(varargin)

params = parseInputs(varargin{:});

A = preprocessImage(params);

% Determine which dimension to resize first.
order = dimensionOrder(params.scale);

% Calculate interpolation weights and indices for each dimension.
weights = cell(1,2);
indices = cell(1,2);
for k = 1:2
    [weights{k}, indices{k}] = contributions(size(A, k), ...
        params.output_size(k), params.scale(k), params.kernel, ...
        params.kernel_width, params.antialiasing);
end

% % if order(1) == 2
%     weights1 = weights{1}';
%     weights2 = weights{2}';
%     indices1 = indices{1}';
%     indices2 = indices{2}';
% else
%     weights1 = weights{2}';
%     weights2 = weights{1}';
%     indices1 = indices{2}';
%     indices2 = indices{1}';
% end

%=====================================================================
function A = preprocessImage(params)
% Convert indexed image to RGB.  Convert binary image to uint8.

if isInputIndexed(params)
    A = ind2rgb8(params.A, params.map);
    
elseif islogical(params.A)
    A = im2uint8(params.A);
    
else
    A = params.A;
end
%---------------------------------------------------------------------

%=====================================================================
function checkForMissingOutputArgument(params, num_output_args)
% If input image is indexed and the colormap option is optimized, the user
% should be calling the function with two output arguments in order to
% capture the new, optimized colormap.  If the user did not use two output
% arguments, issue a warning message.

if isInputIndexed(params) && strcmp(params.colormap_method, 'optimized') && ...
        (num_output_args < 2)

    warning(message('images:imresize:missingOutputArg', 'Second output argument missing for indexed image.', 'When you resize an indexed image, imresize by default', 'calculates a new colormap, which is returned as a second', 'output argument.  You can fix the problem by calling imresize', 'this way:', '    [Y,newmap] = imresize(X,map,...)', 'Or you can resize using the original colormap using this syntax:', '    Y = imresize(X,map,...,''Colormap'',''original'',...)'));
end
%---------------------------------------------------------------------

%=====================================================================
function [B,map] = postprocessImage(B_in, params)
% If input was indexed, convert output back to indexed.
% If input was binary, convert output back to binary.

map = [];
if isInputIndexed(params)
    if strcmp(params.colormap_method, 'original')
        map = params.map;
        B = rgb2ind(B_in, map, params.dither_option);
    else
        [B,map] = rgb2ind(B_in, 256, params.dither_option);
    end
    
elseif islogical(params.A)
    B = B_in > 128;
    
else
    B = B_in;
end
%---------------------------------------------------------------------

%=====================================================================
function params = parseInputs(varargin)
% Parse the input arguments, returning the resulting set of parameters
% as a struct.

narginchk(1, Inf);

% Set parameter defaults.
params.kernel = @cubic;
params.kernel_width = 4;
params.antialiasing = [];
params.colormap_method = 'optimized';
params.dither_option = 'dither';
params.size_dim = []; % If user specifies NaN for an output size, this
                      % parameter indicates the dimension for which the
                      % size was specified.


method_arg_idx = findMethodArg(varargin{:});

first_param_string_idx = findFirstParamString(varargin, method_arg_idx);

[params.A, params.map, params.scale, params.output_size] = ...
    parsePreMethodArgs(varargin, method_arg_idx, first_param_string_idx);

if ~isempty(method_arg_idx)
    [params.kernel, params.kernel_width, params.antialiasing] = ...
        parseMethodArg(varargin{method_arg_idx});
end

warnIfPostMethodArgs(varargin, method_arg_idx, first_param_string_idx);

params = parseParamValuePairs(params, varargin, first_param_string_idx);

params = fixupSizeAndScale(params);

if isempty(params.antialiasing)
    % If params.antialiasing is empty here, that means the user did not
    % explicitly specify a method or the Antialiasing parameter.  The
    % default interpolation method is bicubic, for which the default
    % antialiasing is true.
    params.antialiasing = true;
end
    
%---------------------------------------------------------------------

%=====================================================================
function idx = findMethodArg(varargin)
% Find the location of the method argument, if it exists, before the
% param-value pairs.  If not found, return [].

idx = [];
for k = 1:nargin
    arg = varargin{k};
    if ischar(arg)
        if isMethodString(arg)
            idx = k;
            break;
            
        else
            % If this argument is a string but is not a method string, it
            % must be a parameter string.
            break;
        end
        
    elseif iscell(arg)
        idx = k;
        break;
    end
end
%---------------------------------------------------------------------

%=====================================================================
function tf = isMethodCell(in)
% True of the input argument is a two-element cell array containing a
% function handle and a numeric scalar.

tf = iscell(in) && ...
     numel(in) == 2 && ...
     isa(in{1}, 'function_handle') && ...
     isnumeric(in{2}) && ...
     isscalar(in{2});
%---------------------------------------------------------------------

%=====================================================================
function [A, map, scale, output_size] = parsePreMethodArgs(args, method_arg_idx, ...
                                                  first_param_idx)
% Parse all the input arguments before the method argument.

% Keep only the arguments before the method argument.
if ~isempty(method_arg_idx)
    args = args(1:method_arg_idx-1);
elseif ~isempty(first_param_idx)
    args = args(1:first_param_idx-1);
end

% There must be at least one input argument before the method argument.
if numel(args) < 1
    error(message('images:imresize:badSyntaxMissingImage'));
end

% Set default outputs.
map = [];
scale = [];
output_size = [];

A = args{1};
validateattributes(A, {'numeric', 'logical'}, {'nonsparse', 'nonempty'}, mfilename, 'A', 1);

if numel(args) < 2
    return
end

next_arg = 2;
if size(args{next_arg},2) == 3
    % IMRESIZE(X,MAP,...)
    map = args{next_arg};
    iptcheckmap(map, mfilename, 'MAP', 2);
    next_arg = next_arg + 1;
end

if next_arg > numel(args)
    return
end

next = args{next_arg};

% The next input argument must either be the scale or the output size.
[scale, output_size] = scaleOrSize(next, next_arg);
next_arg = next_arg + 1;

if next_arg <= numel(args)
    error(message('images:imresize:badSyntaxUnrecognizedInput', next_arg));
end
%---------------------------------------------------------------------

%=====================================================================
function [scale, output_size] = scaleOrSize(arg, position)
% Determine whether ARG is the scale factor or the output size.

scale = [];
output_size = [];

if isnumeric(arg) && isscalar(arg)
    % Argument looks like a scale factor.
    validateattributes(arg, {'numeric'}, {'nonzero', 'real'}, mfilename, 'SCALE', ...
                  position);
    scale = double(arg);

elseif isnumeric(arg) && isvector(arg) && (numel(arg) == 2)
    % Argument looks like output_size.
    validateattributes(arg, {'numeric'}, {'vector', 'real', 'positive'}, ...
                  mfilename, '[MROWS NCOLS]', position);
    output_size = double(arg);
    
else
    error(message('images:imresize:badScaleOrSize'));
end


%---------------------------------------------------------------------

%=====================================================================
function first_param_string_idx = findFirstParamString(args, method_arg_idx)
% Find the index of the first parameter string.  It will be the first
% string argument following the method argument.

if isempty(method_arg_idx)
    method_arg_idx = 0;
end

is_class = cellfun('isclass', args(method_arg_idx+1:end), 'char');
first_param_string_idx = find(is_class, 1) + method_arg_idx;
%---------------------------------------------------------------------

%=====================================================================
function [kernel, kernel_width, antialiasing] = parseMethodArg(method)
% Return the kernel function handle and kernel width corresponding to
% the specified method.

[valid_method_names, method_kernels, kernel_widths] = getMethodInfo();

antialiasing = true;

if ischar(method)
    % Replace validatestring here as an optimization. -SLE, 31-Oct-2006
    idx = find(strncmpi(method, valid_method_names, numel(method)));

    switch numel(idx)
      case 0
        error(message('images:imresize:unrecognizedMethodString', method));
        
      case 1
        kernel = method_kernels{idx};
        kernel_width = kernel_widths(idx);
        if strcmp(valid_method_names{idx}, 'nearest')
            antialiasing = false;
        end
        
      otherwise
        error(message('images:imresize:ambiguousMethodString', method));
    end
    
else
    % Cell-array form
    kernel = method{1};
    kernel_width = method{2};
end
%---------------------------------------------------------------------

%=====================================================================
function tf = isMethodString(in)
% Returns true if the input is the name of a method.

if ~ischar(in)
    tf = false;
    
else
    valid_method_strings = getMethodInfo();

    num_matches = sum(strncmpi(in, valid_method_strings, numel(in)));
    tf = num_matches == 1;
end
%---------------------------------------------------------------------

%=====================================================================
function [names,kernels,widths] = getMethodInfo

% Original implementation of getMethodInfo returned this information as
% a single struct array, which was somewhat more readable. Replaced
% with three separate arrays as a performance optimization. -SLE,
% 31-Oct-2006
names = {'nearest', 'bilinear', 'bicubic', 'box', ...
                    'triangle', 'cubic', 'lanczos2', 'lanczos3'};

kernels = {@box, @triangle, @cubic, @box, @triangle, @cubic, ...
           @lanczos2, @lanczos3};

widths = [1.0 2.0 4.0 1.0 2.0 4.0 4.0 6.0];
%---------------------------------------------------------------------

%=====================================================================
function warnIfPostMethodArgs(args, method_arg_idx, first_param_string_idx)
% If there are arguments between the method argument and the first
% parameter string, these must be old-style antialiasing syntaxes that
% are no longer supported.  Issue a warning message.  Note that
% either method_arg_idx and first_param_string_idx may be empty.

if isempty(method_arg_idx)
    method_arg_idx = numel(args) + 1;
end

if isempty(first_param_string_idx)
    first_param_string_idx = numel(args) + 1;
end

if (first_param_string_idx - method_arg_idx) > 1
    warning(message('images:imresize:oldSyntaxesIgnored', 'N and H are now ignored in the old syntaxes IMRESIZE(...,method,N) and IMRESIZE(...,method,H).', 'Use IMRESIZE_OLD if you need the previous behavior.'));
end

%---------------------------------------------------------------------

%=====================================================================
function params = parseParamValuePairs(params_in, args, first_param_string)

params = params_in;

if isempty(first_param_string)
    return
end

if rem(numel(args) - first_param_string, 2) == 0
    error(message('images:imresize:oddNumberArgs'));
end

% Originally implemented valid_params and param_check_fcns as a
% structure which was accessed using dynamic field reference.  Changed
% to separate cell arrays as a performance optimization. -SLE,
% 31-Oct-2006
valid_params = {'Scale', 'Colormap', 'Dither', 'OutputSize', ...
                'Method', 'Antialiasing'};

param_check_fcns = {@processScaleParam, @processColormapParam, ...
                    @processDitherParam, @processOutputSizeParam, ...
                    @processMethodParam, @processAntialiasingParam};

for k = first_param_string:2:numel(args)
    param_string = args{k};
    if ~ischar(param_string)
        error(message('images:imresize:expectedParamString', k));
    end
                  
    idx = find(strncmpi(param_string, valid_params, numel(param_string)));
    num_matches = numel(idx);
    if num_matches == 0
        error(message('images:imresize:unrecognizedParamString', param_string));
    
    elseif num_matches > 1
        error(message('images:imresize:ambiguousParamString', param_string));
        
    else
        check_fcn = param_check_fcns{idx};
        params = check_fcn(args{k+1}, params);

    end
end
%---------------------------------------------------------------------

%=====================================================================
function params = processScaleParam(arg, params_in)

valid = isnumeric(arg) && ((numel(arg) == 1) || (numel(arg) == 2)) && ...
        all(arg > 0);

if ~valid
    error(message('images:imresize:invalidScale'));
end

params = params_in;
params.scale = arg;
%---------------------------------------------------------------------

%=====================================================================
function params = processColormapParam(arg, params_in)

valid = ischar(arg) && (strcmp(arg, 'optimized') || strcmp(arg, 'original'));
if ~valid
    error(message('images:imresize:badColormapOption'));
end

params = params_in;
params.colormap_method = arg;
%---------------------------------------------------------------------

%=====================================================================
function params = processDitherParam(arg, params_in)

valid = (isnumeric(arg) || islogical(arg)) && isscalar(arg);
if ~valid
    error(message('images:imresize:badDitherOption'));
end

params = params_in;
if arg
    params.dither_option = 'dither';
else
    params.dither_option = 'nodither';
end
%---------------------------------------------------------------------

%=====================================================================
function params = processOutputSizeParam(arg, params_in)

valid = isnumeric(arg) && (numel(arg) == 2) && all(isnan(arg) | (arg > 0));
if ~valid
    error(message('images:imresize:badOutputSize'));
end

params = params_in;
params.output_size = arg;
%---------------------------------------------------------------------

%=====================================================================
function params = processMethodParam(arg, params_in)

valid = isMethodString(arg) || isMethodCell(arg);
if ~valid
    error(message('images:imresize:badMethod'));
end

params = params_in;
[params.kernel, params.kernel_width, antialiasing] = parseMethodArg(arg);
if isempty(params.antialiasing)
    % Antialiasing hasn't been set explicity in the input arguments
    % parsed so far, so set it according to what parseMethodArg
    % returns.
    params.antialiasing = antialiasing;
end
    
%---------------------------------------------------------------------

%=====================================================================
function params = processAntialiasingParam(arg, params_in)

valid = (isnumeric(arg) || islogical(arg)) && isscalar(arg);
if ~valid
    error(message('images:imresize:badAntialiasing'));
end

params = params_in;
params.antialiasing = arg;
%---------------------------------------------------------------------

%=====================================================================
function order = dimensionOrder(scale)
% Return the desired dimension order for performing the resize.  The
% strategy is to perform the resize first along the dimension with the
% smallest scale factor.

[junk, order] = sort(scale);
%---------------------------------------------------------------------

%=====================================================================
function tf = isInputIndexed(params)

tf = ~isempty(params.map);
%---------------------------------------------------------------------

%=====================================================================
function params = fixupSizeAndScale(params_in)
% If the scale factor was specified as a scalar, turn it into a
% two-element vector.  If the scale factor wasn't specified, derive it
% from the specified output size.
%
% If the output size has a NaN in it, fill in the value
% automatically. If the output size wasn't specified, derive it from
% the specified scale factor.

params = params_in;

if isempty(params.scale) && isempty(params.output_size)
    error(message('images:imresize:missingScaleAndSize'));
end

params.scale = fixupScale(params.scale);

[params.output_size, params.size_dim] = fixupSize(params);

if isempty(params.scale)
    params.scale = deriveScaleFromSize(params);
end

if isempty(params.output_size)
    params.output_size = deriveSizeFromScale(params);
end

%---------------------------------------------------------------------

%=====================================================================
function scale = fixupScale(scale_in)
% If the input is a scalar, turn it into a two-element vector.

if ~isempty(scale_in) && isscalar(scale_in)
    scale = [scale_in scale_in];
else
    scale = scale_in;
end
%---------------------------------------------------------------------

%=====================================================================
function [output_size, size_dim] = fixupSize(params)
% If params.output_size has a NaN in it, calculate the appropriate
% value to substitute for the NaN.

output_size = params.output_size;
size_dim = [];

if ~isempty(output_size)
    if ~all(output_size)
        error(message('images:imresize:zeroOutputSize'));
    end
    
    if all(isnan(output_size))
        error(message('images:imresize:allNaN'));
    end
    
    if isnan(output_size(1))
        output_size(1) = params.output_size(2) * size(params.A, 1) / size(params.A, 2);
        size_dim = 2;
        
    elseif isnan(output_size(2))
        output_size(2) = params.output_size(1) * size(params.A, 2) / size(params.A, 1);
        size_dim = 1;
        
    end
    
    output_size = ceil(output_size);    
end
%---------------------------------------------------------------------

%=====================================================================
function output_size = deriveSizeFromScale(params)
% Determine the output size from the specified scale factor.

A_2d_size = [size(params.A, 1) size(params.A, 2)];
output_size = ceil(params.scale .* A_2d_size);
%---------------------------------------------------------------------

%=====================================================================
function scale = deriveScaleFromSize(params)
% Determine the scale factor from the specified output size.

if ~isempty(params.size_dim)
    % User specified output size in only one dimension. The other was
    % automatically computed.  The scale factor should be calculated
    % only from the dimension specified, which is params.size_dim.
    
    scale = params.output_size(params.size_dim) / size(params.A, params.size_dim);
    scale = [scale scale];
    
else
    A_2d_size = [size(params.A, 1) size(params.A, 2)];
    scale = params.output_size ./ A_2d_size;
end
%---------------------------------------------------------------------

%=====================================================================
function out = resizeAlongDim(in, dim, weights, indices)
% Resize along a specified dimension
%
% in           - input array to be resized
% dim          - dimension along which to resize
% weights      - weight matrix; row k is weights for k-th output pixel
% indices      - indices matrix; row k is indices for k-th output pixel

if isPureNearestNeighborComputation(weights)
    out = resizeAlongDimUsingNearestNeighbor(in, dim, indices);
    return
end

out_length = size(weights, 1);

size_in = size(in);
size_in((end + 1) : dim) = 1;

if (ndims(in) > 3)
    % Reshape in to be a three-dimensional array.  The size of this
    % three-dimensional array is the variable pseudo_size_in below.
    %
    % Final output will be consistent with the original input.
    pseudo_size_in = [size_in(1:2) prod(size_in(3:end))];
    in = reshape(in, pseudo_size_in);
end

% The 'out' will be uint8 if 'in' is logical 
% Otherwise 'out' datatype will be same as 'in' datatype
% save;
% error
out = imresizemex(in, weights', indices', dim);

if (ndims(in) > 3)
    % Restoring final output to expected size
    size_out = size_in;
    size_out(dim) = out_length;
    out = reshape(out, size_out);
end
%---------------------------------------------------------------------

%=====================================================================
function tf = isPureNearestNeighborComputation(weights)
% True if there is only one column of weights, and if the weights are
% all one.  For this case, the resize can be done using a quick
% indexing operation.

one_weight_per_pixel = size(weights, 2) == 1;
tf = one_weight_per_pixel && all(weights == 1);
%---------------------------------------------------------------------

%=====================================================================
function out = resizeAlongDimUsingNearestNeighbor(in, dim, indices)
% Resize using a multidimensional indexing operation.  Preserve the
% array along all dimensions other than dim.  Along dim, use the
% indices input vector as a subscript vector.

num_dims = max(ndims(in), dim);
subscripts = {':'};
subscripts = subscripts(1, ones(1, num_dims));
subscripts{dim} = indices;
out = in(subscripts{:});

%---------------------------------------------------------------------

%=====================================================================
function out = resizeTwoDimUsingNearestNeighbor(in, indices)
% Resize row and column dimensions simultaneously using a single
% multidimensional indexing operation.

subscripts = indices;
subscripts(3:ndims(in)) = {':'};
out = in(subscripts{:});
%---------------------------------------------------------------------

%=====================================================================
function [weights, indices] = contributions(in_length, out_length, ...
                                            scale, kernel, ...
                                            kernel_width, antialiasing)

% save; 
if (scale < 1) && (antialiasing)
    % Use a modified kernel to simultaneously interpolate and
    % antialias.
    h = @(x) scale * kernel(scale * x);
    kernel_width = kernel_width / scale;
else
    % No antialiasing; use unmodified kernel.
    h = kernel;
end

% Output-space coordinates.
x = (1:out_length)';

% Input-space coordinates. Calculate the inverse mapping such that 0.5
% in output space maps to 0.5 in input space, and 0.5+scale in output
% space maps to 1.5 in input space.
u = x/scale + 0.5 * (1 - 1/scale);

% What is the left-most pixel that can be involved in the computation?
left = floor(u - kernel_width/2);

% What is the maximum number of pixels that can be involved in the
% computation?  Note: it's OK to use an extra pixel here; if the
% corresponding weights are all zero, it will be eliminated at the end
% of this function.
P = ceil(kernel_width) + 2;

% The indices of the input pixels involved in computing the k-th output
% pixel are in row k of the indices matrix.
indices = bsxfun(@plus, left, 0:P-1);

% The weights used to compute the k-th output pixel are in row k of the
% weights matrix.
weights = h(bsxfun(@minus, u, indices));

% Normalize the weights matrix so that each row sums to 1.
weights = bsxfun(@rdivide, weights, sum(weights, 2));

% Clamp out-of-range indices; has the effect of replicating end-points.
indices = min(max(1, indices), in_length);

% If a column in weights is all zero, get rid of it.
kill = find(~any(weights, 1));
if ~isempty(kill)
    weights(:,kill) = [];
    indices(:,kill) = [];
end

%---------------------------------------------------------------------

%=====================================================================
function f = cubic(x)
% See Keys, "Cubic Convolution Interpolation for Digital Image
% Processing," IEEE Transactions on Acoustics, Speech, and Signal
% Processing, Vol. ASSP-29, No. 6, December 1981, p. 1155.

absx = abs(x);
absx2 = absx.^2;
absx3 = absx.^3;

f = (1.5*absx3 - 2.5*absx2 + 1) .* (absx <= 1) + ...
                (-0.5*absx3 + 2.5*absx2 - 4*absx + 2) .* ...
                ((1 < absx) & (absx <= 2));
%---------------------------------------------------------------------

%=====================================================================
function f = box(x)
f = (-0.5 <= x) & (x < 0.5);
%---------------------------------------------------------------------

%=====================================================================
function f = triangle(x)
f = (x+1) .* ((-1 <= x) & (x < 0)) + (1-x) .* ((0 <= x) & (x <= 1));
%---------------------------------------------------------------------

%=====================================================================
function f = lanczos2(x)
% See Graphics Gems, Andrew S. Glasser (ed), Morgan Kaufman, 1990,
% pp. 156-157.

f = (sin(pi*x) .* sin(pi*x/2) + eps) ./ ((pi^2 * x.^2 / 2) + eps);
f = f .* (abs(x) < 2);
%---------------------------------------------------------------------

%=====================================================================
function f = lanczos3(x)
% See Graphics Gems, Andrew S. Glasser (ed), Morgan Kaufman, 1990,
% pp. 157-158.

f = (sin(pi*x) .* sin(pi*x/3) + eps) ./ ((pi^2 * x.^2 / 3) + eps);
f = f .* (abs(x) < 3);
%---------------------------------------------------------------------

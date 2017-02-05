function img = decode_jpeg (byteArray)
% decode_jpeg decodes jpeg to a color image.
% Matthias Mueller, 05.02.2017
%
% INPUT:   byteArray (H*Wx1 uint8)
% OUTPUT:  MATLAB RGB image (HxWx3 uint8)
%
% USAGE:
%   img = decode_jpeg(byteArray);
%
% Reference:
%   http://stackoverflow.com/questions/18659586/from-raw-bits-to-jpeg-without-writing-into-a-file

assert (isa(uint8(byteArray), 'uint8'));

% decode image stream using Java
jImg = javax.imageio.ImageIO.read(java.io.ByteArrayInputStream(byteArray));
h = jImg.getHeight;
w = jImg.getWidth;

% convert Java Image to MATLAB image
p = reshape(typecast(jImg.getData.getDataStorage, 'uint8'), [3,w,h]);
img = cat(3, ...
        transpose(reshape(p(3,:,:), [w,h])), ...
        transpose(reshape(p(2,:,:), [w,h])), ...
        transpose(reshape(p(1,:,:), [w,h])));

end
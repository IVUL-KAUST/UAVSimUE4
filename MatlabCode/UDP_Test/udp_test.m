%IP Address (for sending data)
host='127.0.0.1';

%Remote port
imgPortRx=25000; 
bbPortRx = 25001;
bbPortTx = 25002;
%Maximum packet length
packetLength = 100000;
%Timeout in ms
timeout = 10000; 

%Wait until get Bbox init
bbString = judp('receive',bbPortRx,packetLength,timeout);
bbLength = length(bbString);

frame = 1;
tic
%Loop until nothing sent
if (bbLength > 0)  
    while (true)

%Receive message
byteArray = judp('receive',imgPortRx,packetLength,timeout/10);

%Decode JPEG
img = decode_jpeg (byteArray);

%Display image (disable to see real frame rate)
%imshow(img)

frame = frame +1;
time = toc;
fps = frame/time;
clc
disp(['Framerate: ' num2str(fps)]);

%Send groundtruth back
judp('send',bbPortTx,host,int8(bbString));
    end
end
%IP Address (for sending data)
host='127.0.0.1';

%Remote port
remPort=8890; 
portNumber = 8891;
bbportNumber = 8892;
%Maximum packet length
packetLength = 100000;
%Timeout in ms
timeout = 10000; 

%Setup Figure Display
global KEY_IS_PRESSED
KEY_IS_PRESSED = 0;
gcf
set(gcf, 'KeyPressFcn', @myKeyPressFcn)
drawnow

%Wait until get Bbox init
bbString = judp('receive',portNumber,packetLength,timeout);
bbLength = length(bbString);

%Loop until nothing sent
if (bbLength > 0)  
    while (true)

%Receive message
byteArray = judp('receive',remPort,packetLength,timeout);

%Decode JPEG
img = decode_jpeg (byteArray);

%Display image
imshow(img)

%Send fake Bbox
judp('send',bbportNumber,host,int8(bbString));
    end
end
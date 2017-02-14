clear, clc, close all;
%Enable Visualization (slows down the reception of messages)
bViz = false;

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
        byteArray = judp('receive',imgPortRx,packetLength,timeout);
        
        %Decode JPEG
        img = decode_jpeg (byteArray);
        
        %Display image (disable to see real frame rate)
        if (bViz)
        imshow(img);
        end

        
        %Update frame rate every 30 frames
        if (mod(frame,30)==0)
        fps = 30/toc;
        clc
        disp(['Framerate: ' num2str(fps)]);
        tic
        end
        
        %Send groundtruth back
        judp('send',bbPortTx,host,int8(bbString));
        
        frame = frame +1;
    end
end
remPort=8890;      
host='127.0.0.1';  
%locPort=45455;
%u = udp(host,'RemotePort',remPort,'LocalPort',locPort); 

echoudp('on',remPort)
u = udp(host,'RemotePort',remPort); 

%Open socket
fopen(u)

while (true)
%check if the connection is open and data is received
get (u, 'Status')
get (u, 'ValuesReceived')
if (u.BytesAvailable > 0)
data = fread(u,u.BytesAvailable)
end
pause(1);
end

%Cleanup
echoudp('off')
fclose(u)
delete (u);
clear u
require 'socket'

s = UDPSocket.new
s.setsockopt(Socket::SOL_SOCKET, Socket::SO_BROADCAST, true)
s.send("hello", 0, '255.255.255.255', 10901)

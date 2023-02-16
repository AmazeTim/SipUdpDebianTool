FROM debian:10-slim
ARG DEBIAN_FRONTEND=noninteractive
RUN apt-get update && apt-get install build-essential && apt-get install libpcap-dev && apt-get install uuid-dev
EXPOSE 8088/udp
EXPOSE 9099/udp
RUN gcc -pthread UDPclient.c -o UDPclient -lpcap -luuid
RUN sudo ./UDPclient 0.0.0.0 9099 8088 10 
FROM debian:10-slim
WORKDIR /app
ENV CPORT='8088'
ENV SPORT='9099'
ENV SADDR='127.0.0.1'
ENV RETRY='10'
EXPOSE $SPORT $CPORT
COPY headerFiles.h ./
COPY UDPclient.c ./
RUN echo 'Sipping tool testing!!!'
ARG DEBIAN_FRONTEND=noninteractive
RUN yes Y | apt-get update 
RUN yes Y | apt-get install build-essential
RUN yes Y | apt-get install uuid-dev
RUN yes Y | apt-get install libpcap-dev
RUN gcc -pthread UDPclient.c -o UDPclient -lpcap -luuid
RUN nc -ul -p $SPORT -c 'echo SIP/2.0 200 OK' &
CMD ./UDPclient $SADDR $SPORT $CPORT $RETRY
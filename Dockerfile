FROM ubuntu:22.04

RUN apt-get update
RUN apt-get install -y gcc

COPY *.c /app/
COPY *.h /app/
ADD hostsfile.txt /app/
WORKDIR /app/

RUN gcc *.c -o peer
ENTRYPOINT ["/app/peer"]

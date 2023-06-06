FROM nvcr.io/nvidia/cuda:11.4.3-devel-ubuntu20.04 

RUN apt update
RUN apt install libfreeimage3 libfreeimage-dev wget -y

COPY . /app 
WORKDIR /app/src/imageBlendingNPP
RUN make
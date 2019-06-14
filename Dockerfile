# A Debian image for compiling Nirvana NV14

FROM debian:stretch

# Update and install the required components
RUN DEBIAN_FRONTEND=noninteractive apt-get -y update
RUN DEBIAN_FRONTEND=noninteractive apt-get -y install wget zip bzip2 cmake build-essential python python-pil git lib32ncurses5 mc

# Retrieve and install the required version of the ARM compiler
RUN wget https://launchpad.net/gcc-arm-embedded/4.7/4.7-2013-q3-update/+download/gcc-arm-none-eabi-4_7-2013q3-20130916-linux.tar.bz2 -P /tmp --progress=bar:force
RUN tar -C /tmp -xjf /tmp/gcc-arm-none-eabi-4_7-2013q3-20130916-linux.tar.bz2
RUN mv /tmp/gcc-arm-none-eabi-4_7-2013q3 /opt/gcc-arm-none-eabi
RUN rm /tmp/gcc-arm-none-eabi-4_7-2013q3-20130916-linux.tar.bz2

# Declare the mount point
VOLUME ["/nv14"]

# Set the working directory to /build
WORKDIR /build

# Add the build scripts
COPY build_firmware.py /build

# Update the path
ENV PATH $PATH:/opt/gcc-arm-none-eabi/bin:/nv14/radio/util

# Run the shell script to build the firmware
ENTRYPOINT ["bash", "-c", "python /build/build_firmware.py $CMAKE_FLAGS"]

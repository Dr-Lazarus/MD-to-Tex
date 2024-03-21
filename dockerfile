FROM amd64/ubuntu:22.04

RUN apt-get update && apt-get -yq --no-install-recommends install \
    build-essential \
    clang \
    cmake \
    gdb \
    wget
# Optional: Install additional dependencies or tools you need
# RUN apt-get install -y <your-dependencies-here>
WORKDIR /usr/src/myapp

COPY . .

# and `myapp` with the desired output executable name
RUN gcc -o helloworld main.c
# For C++ use: RUN g++ -o myapp myapp.cpp

# The main file to run
CMD ["./helloworld"]

FROM ubuntu:jammy

ENV CC /usr/bin/clang
ENV CXX /usr/bin/clang++
ENV DISPLAY localhost:0.0 
ENV USER root

# This is a very insecure password, obviously.  Don't trust this container to run long-term without tweaking this
RUN mkdir -p /root/.vnc
RUN echo "pass" > /root/.vnc/passwd
RUN chmod 0600 /root/.vnc/passwd

COPY etc/scripts/docker_packages.sh /setup/docker_packages.sh
RUN /setup/docker_packages.sh

COPY etc/scripts/docker_dependencies.pl /setup/docker_dependencies.pl
RUN /setup/docker_dependencies.pl

COPY ./ /src/updater

RUN mkdir -p /src/updater/out
WORKDIR /src/updater/out

CMD cmake -DCMAKE_BUILD_TYPE=Debug -DSKIP_LINT=false -DCLANG_TIDY_ERRORS=true -DINTEGRATION_TEST=true ../ && make -j6 updater_proto all test


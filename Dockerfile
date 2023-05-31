# ------------------------------------------------------------------------------
# Build Baseline (build_baseline)
# 
# This stage is the base requirements for all of our build stages, including
# the ultimate Scoreboard builder.
# ------------------------------------------------------------------------------
FROM alpine:3.18 AS build_baseline 

RUN apk add --no-cache \
    alpine-sdk \
    clang \
    cmake \
    git

ENV CC /usr/bin/clang
ENV CXX /usr/bin/clang++
ENV USER root

# ------------------------------------------------------------------------------
# Curl (curl_bulid)
#
# Build a custom curl rather than take one from apk.
# ------------------------------------------------------------------------------
FROM build_baseline AS curl_build

RUN apk add --no-cache \
    openssl-dev \
    perl

ENV CURL_VERSION curl-7_79_0

WORKDIR /curl
RUN git clone https://github.com/curl/curl.git .
RUN git fetch --all --tags
RUN git checkout tags/${CURL_VERSION}
RUN git submodule update --init --recursive

WORKDIR /curl/out
RUN cmake .. -DBUILD_SHARED_LIBS=ON
RUN make all
RUN make install

WORKDIR /
RUN tar cvzf curl.tgz \
   /usr/local/lib/libcurl* \
   /usr/local/bin/curl* \
   /usr/local/lib/pkgconfig/libcurl* \
   /usr/local/include/curl \
   /usr/local/lib/cmake/CURL

# ------------------------------------------------------------------------------
# JsonCpp (jsoncpp_bulid)
#
# Build JsonCpp -- mostly because the Alpine packages do not include the
# necessary cmake files for it to work with the scoreboard.
# ------------------------------------------------------------------------------
FROM build_baseline AS jsoncpp_build

ENV JSONCPP_VERSION 1.9.5

WORKDIR /jsoncpp
RUN git clone https://github.com/open-source-parsers/jsoncpp.git .
RUN git fetch --all --tags
RUN git checkout tags/${JSONCPP_VERSION}
RUN git submodule update --init --recursive

WORKDIR /jsoncpp/out
RUN cmake ..
RUN make all
RUN make install

WORKDIR /
RUN tar cvzf jsoncpp.tgz \
    /usr/local/lib/pkgconfig/jsoncpp* \
    /usr/local/lib/cmake/jsoncpp \
    /usr/local/lib/libjsoncpp* \
    /usr/local/lib/objects-Release/jsoncpp_object \
    /usr/local/include/json

# ------------------------------------------------------------------------------
# Googletest (googletest_build)
#
# Build GTest and GMock for testing
# ------------------------------------------------------------------------------
FROM build_baseline AS googletest_build

ENV GTEST_VERSION v1.13.0

WORKDIR /googletest
RUN git clone https://github.com/google/googletest.git .
RUN git fetch --all --tags
RUN git checkout tags/${GTEST_VERSION}
RUN git submodule update --init --recursive

WORKDIR /googletest/out
RUN cmake ..
RUN make all
RUN make install

WORKDIR /
RUN tar cvzf googletest.tgz \
   /usr/local/include/gmock \
   /usr/local/include/gtest \
   /usr/local/lib/libgmock* \
   /usr/local/lib/libgtest* \
   /usr/local/lib/pkgconfig/gmock* \
   /usr/local/lib/pkgconfig/gtest* \
   /usr/local/lib/cmake/GTest

# ------------------------------------------------------------------------------
# Protobuf (protobuf_build)
#
# Build protobuf -- this is partly to keep it lockstep with the version osxcross
# pulls from macports, but also for consistency between builds.
# ------------------------------------------------------------------------------
FROM build_baseline AS protobuf_build

ENV PROTOBUF_VERSION v3.19.3

WORKDIR /protobuf
RUN git clone https://github.com/google/protobuf.git .
RUN git fetch --all --tags
RUN git checkout tags/${PROTOBUF_VERSION}
RUN git submodule update --init --recursive

WORKDIR /protobuf/out
RUN cmake ../cmake
RUN make all
RUN make install

WORKDIR /
RUN tar cvzf protobuf.tgz \
    /usr/local/lib/libproto* \
    /usr/local/bin/proto* \
    /usr/local/lib/pkgconfig/protobuf* \
    /usr/local/include/google/protobuf \
    /usr/local/lib/cmake/protobuf

# ------------------------------------------------------------------------------
# Gui Baseline (gui_build_baseline)
#
# Extra packages for builds that rely on a lot of GUI-based dynamic libs
# (wxWidgets and the scoreboard, notably)
# ------------------------------------------------------------------------------
FROM build_baseline AS gui_build_baseline

RUN apk add --no-cache \
    gtk+3.0-dev \
	libnotify-dev \
	libsecret-dev \
    webkit2gtk-dev

# ------------------------------------------------------------------------------
# wxWidgets (wxwidgets_build)
#
# Build a standard wxWidgets for linux-based build/test.
# ------------------------------------------------------------------------------
FROM gui_build_baseline AS wxwidgets_build

RUN apk add --no-cache \
	libpng-dev \
	openjpeg-dev \
	tiff-dev

ENV WXWIDGETS_VERSION v3.2.2.1

WORKDIR /wxwidgets
RUN git clone https://github.com/wxWidgets/wxWidgets.git .
RUN git fetch --all --tags
RUN git checkout tags/${WXWIDGETS_VERSION}
RUN git submodule update --init --recursive

WORKDIR /wxwidgets/out
RUN cmake .. \
    -DCMAKE_BUILD_TYPE=Debug \
    -DwxBUILD_TOOLKIT=gtk3 \
    -DwxBUILD_STRIPPED_RELEASE=OFF \
    -DwxUSE_XLOCALE=OFF \
    -DwxUSE_WEBVIEW=ON
RUN make all
RUN make install

WORKDIR /
RUN tar cvzf wxwidgets.tgz \
    /usr/local/include/wx-* \
    /usr/local/lib/wx \
    /usr/local/lib/cmake/wxWidgets \
    /usr/local/lib/libwx_* \
    /usr/local/bin/wxrc-* \
    /usr/local/bin/wxrc \
    /usr/local/bin/wx-config

# ------------------------------------------------------------------------------
# Standard Scoreboard Build (standard_build)
#
# The default target stage, this builds and tests the scoreboard.
# ------------------------------------------------------------------------------
FROM gui_build_baseline AS standard_build

RUN apk add --no-cache \
    lxqt-desktop \
    openssl-dev \
    supervisor \
    x11vnc \
    xfce4 \
    xvfb

WORKDIR /root
COPY etc/docker/supervisord.conf  ./

WORKDIR /
# Bring in Curl
COPY --from=curl_build /curl.tgz /
RUN tar xvzf curl.tgz && rm curl.tgz
# Bring in JsonCpp
COPY --from=jsoncpp_build /jsoncpp.tgz /
RUN tar xvzf jsoncpp.tgz && rm jsoncpp.tgz
# Bring in GTest
COPY --from=googletest_build /googletest.tgz /
RUN tar xvzf googletest.tgz && rm googletest.tgz
# Bring in Protobuf
COPY --from=protobuf_build /protobuf.tgz /
RUN tar xvzf protobuf.tgz && rm protobuf.tgz
# Bring in wxWidgets
COPY --from=wxwidgets_build /wxwidgets.tgz /
RUN tar xvzf wxwidgets.tgz && rm wxwidgets.tgz

WORKDIR /root/.vnc
RUN chmod 700 /root/.vnc
COPY etc/docker/vnc_config config
RUN echo "export DISPLAY=$(cat /etc/resolv.conf | grep nameserver | awk '{print $2}'):0" >> /env

COPY . /cszb-scoreboard

WORKDIR /cszb-scoreboard/out
CMD . /env && \
    supervisord -c /root/supervisord.conf && \
    cmake -DCMAKE_BUILD_TYPE=Debug -DSKIP_LINT=false -DCLANG_TIDY_ERRORS=true -DINTEGRATION_TEST=true ../ && \
    make -j6 scoreboard_proto all test


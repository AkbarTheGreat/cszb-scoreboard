﻿# ------------------------------------------------------------------------------
# Build Baseline (build_baseline)
# 
# This stage is the base requirements for all of our build stages, including the
# primary Scoreboard builder.
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
ENV UNATTENDED 1

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
# MacOS Baseline (osxcross_build_baseline)
#
# Extra packages and env vars for builds that build an osx build, including
# as a baseline for osxcross itself.
# ------------------------------------------------------------------------------
FROM build_baseline AS osxcross_build_baseline

RUN apk add --no-cache \
	cairo-dev

ENV OSXCROSS_VERSION 1.4;
ENV MACOSX_DEPLOYMENT_TARGET 10.9
ENV OSXCROSS_OSX_VERSION_MIN 10.9

ENV OSXCROSS_SDK    darwin21.4
ENV OSXCROSS_TARGET darwin21.4
ENV OSXCROSS_HOST x86_64-apple-darwin21.4
ENV OSXCROSS_ROOT_DIR /opt/osxcross
ENV OSXCROSS_TARGET_DIR ${OSXCROSS_ROOT_DIR}

ENV PATH ${OSXCROSS_ROOT_DIR}/bin:$PATH
ENV LD_LIBRARY_PATH ${OSXCROSS_ROOT_DIR}/lib

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

ENV PROTOBUF_VERSION v3.21.12

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
# Osxcross (osxcross_build)
#
# Build osxcross -- This builds our LLVM cross-compiler for MacOS builds.
# ------------------------------------------------------------------------------
FROM osxcross_build_baseline AS osxcross_build

RUN apk add --no-cache \
    bash \
    libxml2-dev \
    musl-fts-dev \
    openssl-dev \
    python3

ENV OSXCROSS_SDK_VERSION 12.3
ENV OSXCROSS_SDK_FILE MacOSX${OSXCROSS_SDK_VERSION}.sdk

ENV OSXCROSS_BASE_DIR      /osxcross
# While we're building osxcross, the target is the location we build into -- 
# after we've installed it, it moves back to OSXCROSS_ROOT_DIR
ENV OSXCROSS_TARGET_DIR    ${OSXCROSS_BASE_DIR}/target
ENV OSXCROSS_SDK           ${OSXCROSS_BASE_DIR}/target/SDK/$OSXCROSS_SDK_FILE
ENV OSXCROSS_SDK_DIR       ${OSXCROSS_BASE_DIR}/target/SDK
ENV OSXCROSS_TARBALL_DIR   ${OSXCROSS_BASE_DIR}/tarballs
ENV OSXCROSS_PATCH_DIR     ${OSXCROSS_BASE_DIR}/patches
ENV OSXCROSS_DIR_SDK_TOOLS ${OSXCROSS_BASE_DIR}/target/SDK/tools
ENV OSXCROSS_BUILD_DIR     ${OSXCROSS_BASE_DIR}/build
ENV OSXCROSS_CCTOOLS_PATH  ${OSXCROSS_BASE_DIR}/target/bin

ENV PATH ${OSXCROSS_BASE_DIR}/target/bin:${PATH}

WORKDIR /osxcross
RUN git clone https://github.com/tpoechtrager/osxcross.git .
RUN git fetch --all --tags
RUN git submodule update --init --recursive

COPY osx_tarballs/${OSXCROSS_SDK_FILE}.tar.bz2 /osxcross/tarballs/

WORKDIR /osxcross
RUN ./build.sh

# Fake Macports Dependencies
RUN osxcross-macports fake-install \
    geoclue2 \
    graphviz \
    py311

# Real Macports dependencies
RUN osxcross-macports install \
    bzip2 \
    curl \
    curl-ca-bundle \
    expat \
    gettext \
    glib2 \
    gtest \
    jsoncpp-devel \
    libedit \
    libffi \
    libiconv \
    libidn2 \
    libpsl \
    libunistring \
    ncurses \
    openssl \
    pcre \
    protobuf3-cpp \
    webkit-gtk \
    zlib

# Patch some macports files that need tweaks
COPY etc/osxcross_patches/ /osxcross/target/

# Move everything to where we'd like it to be in a dependent image
RUN mv ${OSXCROSS_TARGET_DIR} ${OSXCROSS_ROOT_DIR}

# Fix a couple of janky links
RUN rm ${OSXCROSS_ROOT_DIR}/macports/pkgs/opt/local/lib/libssl.a
RUN ln -s ${OSXCROSS_ROOT_DIR}/macports/pkgs/opt/local/libexec/openssl3/lib/libssl.a \
    ${OSXCROSS_ROOT_DIR}/macports/pkgs/opt/local/lib/libssl.a

RUN rm ${OSXCROSS_ROOT_DIR}/macports/pkgs/opt/local/lib/libcrypto.a
RUN ln -s ${OSXCROSS_ROOT_DIR}/macports/pkgs/opt/local/libexec/openssl3/lib/libcrypto.a \
    ${OSXCROSS_ROOT_DIR}/macports/pkgs/opt/local/lib/libcrypto.a

# Tarball it up for consumption
WORKDIR /
RUN tar cvzf /osxcross.tgz /opt/osxcross

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
# wxWidgets for MacOS (wxwidgets_osxcross_build)
#
# Build an Osxcross wxWidgets for macos-based build.
# ------------------------------------------------------------------------------
FROM osxcross_build_baseline AS wxwidgets_osxcross_build

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

# Get osxcross to build against
WORKDIR /
COPY --from=osxcross_build /osxcross.tgz /
RUN tar xvzf osxcross.tgz && rm osxcross.tgz

# WxWidgets doesn't work with an SDK as old as I'm using because of one version-check macro -- while I can,
# I'll patch it to keep my lowest necessary version of MacOS as low as possible.
WORKDIR /wxwidgets

# This fixes an issue that turns into the symbol ___isPlatformVersionAtLeast being unavailable
RUN sed -i 's/__has_builtin(__builtin_available)/__has_builtin(__disabled_builtin_available)/g' \
            include/wx/osx/private/available.h \
            src/png/pngrutil.c

# This constant doesn't exist for x86_64 architecture, so remove it forcefully.  It's gross, but it works
RUN sed -i 's/layer.contentsFormat = kCAContentsFormatRGBA8Uint;//' src/osx/cocoa/window.mm

WORKDIR /wxwidgets/out
RUN cmake .. \
    -DCMAKE_TOOLCHAIN_FILE=${OSXCROSS_ROOT_DIR}/toolchain.cmake \
    -DCMAKE_INSTALL_NAME_TOOL=${OSXCROSS_ROOT_DIR}/bin/${OSXCROSS_HOST}-install_name_tool \
    -DCMAKE_INSTALL_PREFIX=${OSXCROSS_ROOT_DIR}/wxwidgets \
    -DOSXCROSS_TARGET_DIR=${OSXCROSS_ROOT_DIR} \
    -DCMAKE_BUILD_TYPE=Release \
    -DwxBUILD_PRECOMP=OFF \
    -DwxBUILD_SHARED=OFF \
    -DwxUSE_WEBVIEW=ON
RUN make all
RUN make install

WORKDIR /
RUN tar cvzf wxwidgets.tgz \
    ${OSXCROSS_ROOT_DIR}/wxwidgets

# ------------------------------------------------------------------------------
# MacOS Scoreboard Build (macos_build)
#
# Sets up a macos build environment for the scoreboard
# ------------------------------------------------------------------------------
FROM osxcross_build_baseline AS macos_build

RUN apk add --no-cache \
    perl \
    perl-file-which \
    perl-list-allutils \
    zip

WORKDIR /
# Bring in JsonCpp
COPY --from=jsoncpp_build /jsoncpp.tgz /
RUN tar xvzf jsoncpp.tgz && rm jsoncpp.tgz
# Bring in GTest
COPY --from=googletest_build /googletest.tgz /
RUN tar xvzf googletest.tgz && rm googletest.tgz
# Bring in Protobuf
COPY --from=protobuf_build /protobuf.tgz /
RUN tar xvzf protobuf.tgz && rm protobuf.tgz
# Bring in Osxcross
COPY --from=osxcross_build /osxcross.tgz /
RUN tar xvzf osxcross.tgz && rm osxcross.tgz
# Bring in wxWidgets built for MacOS
COPY --from=wxwidgets_osxcross_build /wxwidgets.tgz /
RUN tar xvzf wxwidgets.tgz && rm wxwidgets.tgz

ENV DISPLAY :1

COPY . /cszb-scoreboard

WORKDIR /cszb-scoreboard

CMD echo "Either run macos_test,exec into this container, or run build_osxcross.pl."

# ------------------------------------------------------------------------------
# Standard Scoreboard Build (standard_build)
#
# Sets up a standard build environment for the scoreboard
# ------------------------------------------------------------------------------
FROM gui_build_baseline AS standard_build

RUN apk add --no-cache \
    faenza-icon-theme \
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

ENV DISPLAY :1

COPY . /cszb-scoreboard

WORKDIR /cszb-scoreboard

CMD echo "Either run without a target or exec into this container."

# ------------------------------------------------------------------------------
# Code Clean Worker (code_clean)
#
# A container for use in clean_code.pl, as a method of running various code
# cleanup and static analysis tools.
# ------------------------------------------------------------------------------
FROM standard_build AS code_clean

#    clang-format \
RUN apk add --no-cache \
    bash \
    clang16-extra-tools \
    include-what-you-use \
    perl \
    perl-app-cpanminus \
    py3-pip

RUN cpanm Perl::Tidy

RUN pip install mdformat

COPY . /cszb-scoreboard

WORKDIR /cszb-scoreboard

CMD echo "Please run etc/scripts/clean_code.pl"

# ------------------------------------------------------------------------------
# Build Everything (build_all)
#
# A container which mostly just depends on all of the other final-stage containers
# as a shorthand for building everything (pre-caching builds, basically)
# ------------------------------------------------------------------------------
FROM scratch AS build_all

COPY --from=standard_build /cszb-scoreboard/Dockerfile /standard_docker
COPY --from=macos_build /cszb-scoreboard/Dockerfile /macos_docker
COPY --from=code_clean /cszb-scoreboard/Dockerfile /code_clean_docker

CMD echo "Everything is built.  Enjoy."

# ------------------------------------------------------------------------------
# MacOS Scoreboard Build (macos_test)
#
# Builds the scoreboard for MacOS -- does not test, as this is a cross-compile
# ------------------------------------------------------------------------------
FROM macos_build AS macos_test

WORKDIR /cszb-scoreboard/out
CMD cmake -DCMAKE_OSX_DEPLOYMENT_TARGET=10.12 \
          -DCMAKE_TOOLCHAIN_FILE=${OSXCROSS_ROOT_DIR}/toolchain.cmake \
          -DCMAKE_BUILD_TYPE=Debug \
          -DOPENSSL_ROOT_DIR=${OSXCROSS_ROOT_DIR}/macports/pkgs/opt/local/libexec/openssl3 \
          -DSKIP_LINT=true \
          -DINTEGRATION_TEST=false \
          ../ && \
    make -j6 scoreboard_proto cszb-scoreboard

# ------------------------------------------------------------------------------
# Standard test -- default action (standard_test)
#
# Uses standard_build to build and test the scoreboard
# ------------------------------------------------------------------------------
FROM standard_build

WORKDIR /cszb-scoreboard/out
CMD supervisord -c /root/supervisord.conf && \
    cmake -DCMAKE_BUILD_TYPE=Debug \
          -DSKIP_LINT=false \
          -DCLANG_TIDY_ERRORS=true \
          -DINTEGRATION_TEST=true \
          ../ && \
    make -j6 scoreboard_proto all test


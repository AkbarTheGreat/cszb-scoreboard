# ------------------------------------------------------------------------------
# Build Baseline (build_baseline)
# 
# This stage is the base requirements for all of our build stages, including the
# primary Scoreboard builder.
# ------------------------------------------------------------------------------
FROM alpine:3.18 AS build_baseline 

# Enable community repository for jsoncpp, gtest
RUN echo "https://dl-cdn.alpinelinux.org/alpine/v3.18/community" >> /etc/apk/repositories

RUN apk add --no-cache \
    alpine-sdk \
    bash \
    clang \
    clang-extra-tools \
    cmake \
    git \
    sudo \
    jsoncpp-dev \
    gtest-dev \
    protobuf-dev \
    protobuf

ENV CC=/usr/bin/clang
ENV CXX=/usr/bin/clang++
ENV USER=root
ENV UNATTENDED=1

ENV JSONCPP_VERSION=1.9.5

RUN adduser -Du 1001 jenkins
RUN echo "jenkins ALL=(ALL:ALL) NOPASSWD:ALL" >> /etc/sudoers

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
    webkit2gtk-dev \
    fontconfig \
    ttf-dejavu

# ------------------------------------------------------------------------------
# MacOS Baseline (osxcross_build_baseline)
#
# Extra packages and env vars for builds that build an osx build, including
# as a baseline for osxcross itself.
# ------------------------------------------------------------------------------
FROM build_baseline AS osxcross_build_baseline

RUN apk add --no-cache \
    cairo-dev

ENV OSXCROSS_SDK_VERSION=26.1
ENV MACOSX_DEPLOYMENT_TARGET=14.0
ENV OSXCROSS_OSX_VERSION_MIN=14.0
ENV TARGET_OSXCROSS_SDK=darwin25.1

ENV OSXCROSS_TARGET=${TARGET_OSXCROSS_SDK}
ENV OSXCROSS_HOST=arm64-apple-${TARGET_OSXCROSS_SDK}
ENV OSXCROSS_SDK_FILE=MacOSX${OSXCROSS_SDK_VERSION}.sdk
ENV OSXCROSS_ROOT_DIR=/opt/osxcross
ENV OSXCROSS_TARGET_DIR=${OSXCROSS_ROOT_DIR}
ENV OSXCROSS_SDK=${OSXCROSS_ROOT_DIR}/SDK/$OSXCROSS_SDK_FILE

ENV PATH=${OSXCROSS_ROOT_DIR}/bin:$PATH
ENV LD_LIBRARY_PATH=${OSXCROSS_ROOT_DIR}/lib

# ------------------------------------------------------------------------------
# Osxcross (osxcross_compile)
#
# Build osxcross -- This compiles osxcross itself, see osxcross_build for more.
# ------------------------------------------------------------------------------
FROM osxcross_build_baseline AS osxcross_compile

RUN apk add --no-cache \
    libxml2-dev \
    musl-fts-dev \
    openssl-dev \
    python3

ENV OSXCROSS_SDK_FILE=MacOSX${OSXCROSS_SDK_VERSION}.sdk
ENV OSXCROSS_SDK_ZIP=${OSXCROSS_SDK_FILE}.tar.xz

ENV OSXCROSS_BASE_DIR=/osxcross
ENV OSXCROSS_TARGET_DIR=${OSXCROSS_BASE_DIR}/target
ENV OSXCROSS_SDK=${OSXCROSS_BASE_DIR}/target/SDK/$OSXCROSS_SDK_FILE
ENV OSXCROSS_SDK_DIR=${OSXCROSS_BASE_DIR}/target/SDK
ENV OSXCROSS_TARBALL_DIR=${OSXCROSS_BASE_DIR}/tarballs
ENV OSXCROSS_PATCH_DIR=${OSXCROSS_BASE_DIR}/patches
ENV OSXCROSS_DIR_SDK_TOOLS=${OSXCROSS_BASE_DIR}/target/SDK/tools
ENV OSXCROSS_BUILD_DIR=${OSXCROSS_BASE_DIR}/build
ENV OSXCROSS_CCTOOLS_PATH=${OSXCROSS_BASE_DIR}/target/bin

ENV ENABLE_ARCHS="arm64"
ENV ENABLE_COMPILER_RT_INSTALL=1

ENV PATH=${OSXCROSS_BASE_DIR}/target/bin:${PATH}

ENV OSXCROSS_COMMIT="e6ab3fa7423f9235ce9ed6381d6d3af191b46b59"

WORKDIR ${OSXCROSS_BASE_DIR}
RUN git clone https://github.com/tpoechtrager/osxcross.git .
RUN git fetch --all --tags
RUN git checkout ${OSXCROSS_COMMIT}
RUN git submodule update --init --recursive

WORKDIR ${OSXCROSS_TARBALL_DIR}
RUN wget https://github.com/joseluisq/macosx-sdks/releases/download/${OSXCROSS_SDK_VERSION}/${OSXCROSS_SDK_ZIP}

WORKDIR ${OSXCROSS_BASE_DIR}
RUN ./build.sh 
RUN sed -i 's/print_or_run.*dylib/# removed copy of dylibs/g' build_compiler_rt.sh
RUN ./build_compiler_rt.sh

# ------------------------------------------------------------------------------
# Osxcross (osxcross_build)
#
# Build osxcross -- This builds our complete LLVM cross-compiler for MacOS
#                   builds.  It is broken into two stages for the times when
#                   we need to debug the installation of macports objects.
# ------------------------------------------------------------------------------
FROM osxcross_compile AS osxcross_build

# Fake Macports Dependencies
RUN osxcross-macports fake-install \
    curl-ca-bundle \
    geoclue2 \
    gnutls \
    graphviz \
    libde265 \
    mesa \
    python314 \
    shared-mime-info \
    xorg

# Real Macports dependencies
RUN osxcross-macports install \
    abseil \
    bzip2 \
    expat \
    gettext \
    glib2 \
    gtest \
    libedit \
    libffi \
    libssh2 \
    libiconv \
    libidn2 \
    libpsl \
    libunistring \
    ncurses \
    openssl \
    pcre \
    protobuf3-cpp \
    webkit2-gtk \
    zlib

# Patch some macports files that need tweaks
COPY etc/osxcross_patches/ ${OSXCROSS_TARGET_DIR}

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
RUN tar cvzf /osxcross.tgz /opt/osxcross /usr/lib/llvm16/lib/clang/16/lib/darwin

# ------------------------------------------------------------------------------
# JsonCPP for MacOS (jsoncpp_osxcross_build)
#
# Build an Osxcross jsoncpp for macos-based build.
# OSXCross seems to has stopped shipping a macports jsoncpp package.
# ------------------------------------------------------------------------------
FROM osxcross_build_baseline AS jsoncpp_osxcross_build

# Get osxcross to build against
WORKDIR /
COPY --from=osxcross_build /osxcross.tgz /
RUN tar xvzf osxcross.tgz && rm osxcross.tgz

WORKDIR /jsoncpp
RUN git clone https://github.com/open-source-parsers/jsoncpp.git .
RUN git fetch --all --tags
RUN git checkout tags/${JSONCPP_VERSION}
RUN git submodule update --init --recursive

WORKDIR /jsoncpp/out
RUN cmake .. \
    -DCMAKE_TOOLCHAIN_FILE=${OSXCROSS_ROOT_DIR}/toolchain.cmake \
    -DCMAKE_INSTALL_NAME_TOOL=${OSXCROSS_ROOT_DIR}/bin/${OSXCROSS_HOST}-install_name_tool \
    -DCMAKE_INSTALL_PREFIX=${OSXCROSS_ROOT_DIR}/jsoncpp \
    -DOSXCROSS_TARGET_DIR=${OSXCROSS_ROOT_DIR}
RUN make -j 4 jsoncpp_object jsoncpp_static jsoncpp_lib
RUN make install/fast

WORKDIR /
RUN tar cvzf jsoncpp.tgz \
    ${OSXCROSS_ROOT_DIR}/jsoncpp

# ------------------------------------------------------------------------------
# Include-What-You-Use (iwyu_build)
#
# Build IWYU -- Alpine doesn't seem to have a complete include-what-you-use
# install, so we'll just build our own.
# ------------------------------------------------------------------------------
FROM build_baseline AS iwyu_build

RUN apk add --no-cache \
    clang-dev \
    clang-static \
    llvm-dev \
    llvm-static

ENV IWYU_VERSION=clang_16

WORKDIR /iwyu
RUN git clone https://github.com/include-what-you-use/include-what-you-use.git .
RUN git fetch --all --tags
RUN git checkout ${IWYU_VERSION}
RUN git submodule update --init --recursive

WORKDIR /iwyu/out
RUN cmake \
    -DCMAKE_INSTALL_PREFIX=/iwyu/usr/local \
    ..
RUN make -j 4 all
RUN make install

WORKDIR /iwyu
RUN tar cvzf /iwyu.tgz \
    usr/local

# ------------------------------------------------------------------------------
# MacOS Scoreboard Build (macos_build)
#
# Sets up a macos build environment for the scoreboard
# ------------------------------------------------------------------------------
FROM osxcross_build_baseline AS macos_build

RUN apk add --no-cache \
    libpng-dev \
    openjpeg-dev \
    perl \
    perl-file-which \
    perl-list-allutils \
    tiff-dev \
    zip

WORKDIR /
# Bring in Osxcross
COPY --from=osxcross_build /osxcross.tgz /
RUN tar xvzf osxcross.tgz && rm osxcross.tgz
# Bring in JsonCpp built for MacOS
COPY --from=jsoncpp_osxcross_build /jsoncpp.tgz /
RUN tar xvzf jsoncpp.tgz && rm jsoncpp.tgz

ENV DISPLAY=:1
ENV LD_LIBRARY_PATH='/opt/osxcross/lib'

COPY . /cszb-scoreboard

WORKDIR /cszb-scoreboard

CMD ["/bin/echo", "Either run macos_test,exec into this container, or run build_osxcross.pl."]

# ------------------------------------------------------------------------------
# Standard Scoreboard Build Base (standard_build_base)
#
# Everything except copying source in for standard builds -- this is the base for
# both the standard build as well as the code_clean images -- split up this way to
# best leverage a lot of stage caching.
# ------------------------------------------------------------------------------
FROM gui_build_baseline AS standard_build_base

RUN apk add --no-cache \
    abseil-cpp-dev \
    compiler-rt \
    faenza-icon-theme \
    gcovr \
    llvm \
    mesa-dri-gallium \
    openssl-dev \
    supervisor \
    valgrind \
    x11vnc \
    xfce4 \
    xvfb

WORKDIR /
COPY etc/docker/supervisord.conf  ./

ENV DISPLAY=:1
ENV GCOV='llvm-cov gcov'

EXPOSE 5900

# ------------------------------------------------------------------------------
# Standard Scoreboard Build (standard_build)
#
# Sets up a standard build environment for the scoreboard
# ------------------------------------------------------------------------------
FROM standard_build_base AS standard_build

COPY . /cszb-scoreboard

WORKDIR /cszb-scoreboard

CMD ["/bin/echo", "Either run without a target or exec into this container."]

# ------------------------------------------------------------------------------
# Code Clean Worker (code_clean)
#
# A container for use in clean_code.pl, as a method of running various code
# cleanup and static analysis tools.
# ------------------------------------------------------------------------------
FROM standard_build_base AS code_clean

RUN apk add --no-cache \
    bash \
    perl \
    perl-app-cpanminus \
    py3-pip

RUN cpanm Perl::Tidy

RUN pip install mdformat

WORKDIR /
# Bring in IWYU
COPY --from=iwyu_build /iwyu.tgz /
RUN tar xvzf iwyu.tgz && rm iwyu.tgz

COPY . /cszb-scoreboard

WORKDIR /cszb-scoreboard

CMD ["/bin/echo", "Please run etc/scripts/clean_code.pl"]

# ------------------------------------------------------------------------------
# Dev Container (dev)
#
# A container for use in vscode as a dev container.  Has all of the build
# dependencies as well as code cleanup tools
# ------------------------------------------------------------------------------
FROM mcr.microsoft.com/devcontainers/base:alpine AS dev

RUN echo "https://dl-cdn.alpinelinux.org/alpine/v3.18/community" >> /etc/apk/repositories

RUN apk add --no-cache \
    alpine-sdk \
    clang \
    clang-extra-tools \
    cmake \
    compiler-rt \
    faenza-icon-theme \
    gcovr \
    git \
    gtk+3.0-dev \
    libnotify-dev \
    libsecret-dev \
    lldb \
    llvm \
    mesa-dri-gallium \
    openssl-dev \
    perl \
    perl-app-cpanminus \
    py3-pip \
    valgrind \
    webkit2gtk-4.1-dev \
    xfce4 \
    xvfb \
    jsoncpp-dev \
    gtest-dev \
    protobuf-dev \
    protobuf

RUN cpanm Perl::Tidy

RUN python3 -m venv /mdformat-venv && /mdformat-venv/bin/pip install mdformat

ENV GCOV='llvm-cov gcov'

WORKDIR /
# Bring in IWYU
COPY --from=iwyu_build /iwyu.tgz /
RUN tar xvzf iwyu.tgz && rm iwyu.tgz

COPY . /cszb-scoreboard
WORKDIR /cszb-scoreboard

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
COPY --from=dev /cszb-scoreboard/Dockerfile /dev_docker

CMD ["/bin/echo", "Everything is built.  Enjoy."]

# ------------------------------------------------------------------------------
# MacOS Scoreboard Build (macos_test)
#
# Builds the scoreboard for MacOS -- does not test, as this is a cross-compile
# ------------------------------------------------------------------------------
FROM macos_build AS macos_test

ENV BUILD_PLATFORM=MacOS
ENV BUILD_PRESET=Debug
ENV BUILD_THREADS= 
ENV RUN_TESTS=

CMD ["etc/docker/entrypoint.sh"]

# ------------------------------------------------------------------------------
# Coverage Generation -- (generate_cov)
#
# Uses standard_build to build, test, and generate coverage info for the scoreboard
# You will likely want to map /cszb-scoreboard/out/linux/Coverage as a volume to
# exfiltrate the results.
# ------------------------------------------------------------------------------
FROM standard_build AS generate_cov

ENV BUILD_PLATFORM=Linux
ENV BUILD_PRESET=Coverage
ENV BUILD_THREADS= 
ENV RUN_TESTS=

CMD ["etc/docker/entrypoint.sh"]

# ------------------------------------------------------------------------------
# Standard test -- default action (standard_test)
#
# Uses standard_build to build and test the scoreboard
# ------------------------------------------------------------------------------
FROM standard_build AS standard_test

ENV BUILD_PLATFORM=Linux
ENV BUILD_PRESET=Integration
ENV BUILD_THREADS= 
ENV RUN_TESTS=yes

CMD ["etc/docker/entrypoint.sh"]

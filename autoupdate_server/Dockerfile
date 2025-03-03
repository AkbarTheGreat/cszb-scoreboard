FROM alpine:latest

RUN apk add --no-cache \
    alpine-sdk \
    perl \
    perl-app-cpanminus \
    perl-module-build-tiny \
    perl-mojolicious \
    perl-moose

RUN cpanm \
    Mojolicious::Plugin::AccessLog \
    MooseX::Singleton

COPY ./app /updater

# /releases will likely always be a mounted volume, for persistence, but create a space in case they're not
RUN mkdir -p /releases

WORKDIR /updater

EXPOSE 8080

# TODO -- this may not be appropriate for production deployment -- update if not.
CMD [/usr/bin/perl, autoupdater.pl, daemon, -l, http://*:8080, -m, production]


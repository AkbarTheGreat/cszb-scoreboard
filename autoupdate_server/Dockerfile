FROM alpine:latest

RUN apk add --no-cache \
    alpine-sdk \
    perl \
    perl-app-cpanminus \
    perl-datetime \
    perl-list-allutils \
    perl-module-build-tiny \
    perl-mojolicious \
    perl-sub-identify \
    perl-sub-name \
    perl-moose

RUN cpanm \
    Data::Printer \
    Mojolicious::Plugin::AccessLog \
    MooseX::Privacy \
    MooseX::Singleton \
    Smart::Comments

COPY ./app /updater

# /releases will likely always be a mounted volume, for persistence, but create a space in case they're not
RUN mkdir -p /releases

WORKDIR /updater

EXPOSE 8080

# TODO -- this may not be appropriate for production deployment -- update if not.
CMD [/usr/bin/perl, autoupdater.pl, daemon, -l, http://*:8080, -m, production]


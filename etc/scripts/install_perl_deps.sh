#!/bin/sh

# This just installs dependencies you'll probably want on your host system.  All
# of these may or may not be necessary for every workload, but it's not a big list,
# either.  You likely want to run this as root (sudo).

export PERL_MM_USE_DEFAULT=1

cpan CPAN

cpan  \
  File::Copy::Recursive \
  File::Which \
  HTTP::Tiny \
  JSON \
  List::AllUtils


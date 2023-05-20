#!/usr/bin/perl

=pod

Sets up Docker dependencies which need to be built from a git repository.

Copyright 2020-2023 Tracy Beck

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.


Some miscellaneous notes about using osxcross, from historical script purposes (most of this is probably obsolete in a Docker-based world):

The macports installed with dylibs get bundled them with our application, in the release_osxcross script:  libcrypto.1.1.dylib  libjsoncpp.24.dylib  libprotobuf.23.dylib  libssl.1.1.dylib  libz.1.dylib

Macports protobuf installs the latest protobuf, and the apt version of protobuf will be several versions old, so we do need to rebuild protobuf anyway, to make the protoc/lib versions match, see https://github.com/protocolbuffers/protobuf/blob/master/src/README.md
When you clone protobuf, be sure to checkout the tag which matches the version in osxcross (which can be found with 'x86_64-apple-darwin19-pkg-config --modversion protobuf')

To build the scoreboard after running this:
setenv OSXCROSS_HOST x86_64-apple-darwin19 ; setenv OSXCROSS_TARGET_DIR /opt/osxcross/ ; setenv OSXCROSS_TARGET darwin19 ; setenv OSXCROSS_SDK darwin19 
cmake -DCMAKE_TOOLCHAIN_FILE=/opt/osxcross/toolchain.cmake -DCMAKE_OSX_DEPLOYMENT_TARGET=10.12 -DOPENSSL_ROOT_DIR=/opt/osxcross/macports/pkgs/opt/local/libexec/openssl3 ../..
make scoreboard_proto cszb-scoreboard

=cut

use 5.030;
use Cwd;
use File::Basename        qw{dirname};
use File::Copy::Recursive qw{rcopy rcopy_glob};
use File::Path            qw{mkpath rmtree};
use File::Which;
use FindBin        qw{$RealBin};
use List::AllUtils qw{none};

our $ABSEIL_VERSION    = '20230125.3';
our $GRPC_VERSION      = 'v1.55.0';
our $GTEST_VERSION     = 'v1.13.0';
our $PROTOBUF_VERSION  = 'v3.19.3';

our $BASE_DIR = '/src/';

# Wrapper around system which allows us to die on any system failure easily.
sub sys {
   my (@args) = @_;
   my $retval = system @args;
   return unless $retval;
   die 'System call failed: '
       . "@args\n"
       . '  Died with exit code: '
       . $retval;
}

sub clone_repo {
   my ( $subdir, $repo, $tag ) = @_;
   my $dir = $BASE_DIR . $subdir;
   unless ( -d $dir ) {
      say 'New repo, cloning.';
      sys( '/usr/bin/git', 'clone', $repo, $dir );
   }
   chdir $dir;
   if ($tag) {
      sys( '/usr/bin/git', 'fetch', '--all', '--tags' );
      sys( '/usr/bin/git', 'checkout', 'tags/' . $tag );
   }
   say 'Initializing git submodules.';
   sys( '/usr/bin/git', 'submodule', 'update', '--init', '--recursive' );
   chdir $BASE_DIR;
}

sub cmake {
   my ( $subdir, @args ) = @_;
   my $dir = $BASE_DIR . $subdir;
   mkdir $dir . '/out';
   chdir $dir . '/out';
   my $cmake_dir = '../';
   $cmake_dir = '../cmake' if ( $subdir eq 'protobuf' );
   say 'Running cmake.';
   sys( '/usr/bin/cmake', $cmake_dir, @args );
   chdir $BASE_DIR;
}

sub build {
   my ($subdir) = @_;
   chdir $BASE_DIR . $subdir . '/out';
   say 'Building ' . $subdir;
   sys( '/usr/bin/make', '-j6', 'all' );
}

sub install {
   my ($subdir) = @_;
   chdir $BASE_DIR . $subdir . '/out';
   say 'Installing ' . $subdir;
   sys( '/usr/bin/make', 'install' );
}

sub setup_abseil {
   say 'Setting up Abseil';
   clone_repo( 'absl', 'https://github.com/abseil/abseil-cpp.git',
               $ABSEIL_VERSION );
   cmake('absl');
   build('absl');
   install('absl');
}

sub setup_googletest {
   say 'Setting up Googletest';
   clone_repo( 'googletest', 'https://github.com/google/googletest.git',
               $GTEST_VERSION );
   cmake('googletest');
   build('googletest');
   install('googletest');
}

sub setup_grpc {
   say 'Setting up gRPC';
   clone_repo( 'grpc', 'https://github.com/grpc/grpc.git',
               $GRPC_VERSION );
   cmake('grpc');
   build('grpc');
   install('grpc');
}

sub setup_protobuf {
   say 'Setting up Protobuf';
   clone_repo( 'protobuf', 'https://github.com/google/protobuf.git',
               $PROTOBUF_VERSION );
   cmake('protobuf');
   build('protobuf');
   install('protobuf');
}

sub main {
   my (@args) = @_;
   chdir($BASE_DIR);

   setup_abseil();
   setup_googletest();
   setup_protobuf();
   setup_grpc();

   say 'Successfully completed.';
}

main(@ARGV);


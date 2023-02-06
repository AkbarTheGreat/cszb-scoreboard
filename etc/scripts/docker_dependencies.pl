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

our $PROTOBUF_VERSION  = 'v3.19.3';
our $WXWIDGETS_VERSION = 'v3.1.6-final';
our $CURL_VERSION      = 'curl-7_79_0';
our $GTEST_VERSION     = 'v1.13.0';

our $BASE_DIR = '/src/';

our $OSXCROSS_INSTALL         = '/opt/osxcross';
our $OSX_VERSION              = '10.12';
our $OSXCROSS_VERSION         = 1.4;
our $OSXCROSS_OSX_VERSION_MIN = 10.9;
our $OSXCROSS_TARGET          = 'darwin21.4';
our $OSXCROSS_HOST            = 'x86_64-apple-' . $OSXCROSS_TARGET;
our $OSXCROSS_SDK_VERSION     = '12.3';
our $OSXCROSS_SDK             = 'MacOSX' . $OSXCROSS_SDK_VERSION . '.sdk';
our $OSXCROSS_LIBLTO_PATH     = '/usr/lib/llvm-14/lib';
our $OSXCROSS_LINKER_VERSION  = 609;

our $OSXCROSS_PATCH_FILES = $RealBin . '/osxcross_patches/*';
if ( -d dirname($RealBin) . '/osxcross_patches' ) {

   # For debug runs -- this doesn't come into play in normal docker setups
   $OSXCROSS_PATCH_FILES = dirname($RealBin) . '/osxcross_patches/*';
}

# Macports libraries to be installed.
our @MACPORTS_LIBS = qw(
    bzip2
    curl
    curl-ca-bundle
    expat
    gettext
    glib2
    gtest
    jsoncpp-devel
    libedit
    libffi
    libiconv
    libidn2
    libpsl
    libunistring
    ncurses
    openssl
    pcre
    protobuf3-cpp
    webkit-gtk
    zlib
);

# These libraries don't exist in macports -- faking them seems to work ok?
our @MACPORTS_FAKELIBS = qw(
    geoclue2
    graphviz
);

our $OSXCROSS_REPO  = $BASE_DIR . '/osxcross';
our $WXWIDGETS_REPO = $BASE_DIR . '/wxWidgets';
our $SDK_TARBALL    = '/usr/share/osx_tarballs/' . $OSXCROSS_SDK . '.tar.bz2';

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

sub osxcross_build_env {
   $ENV{'MACOSX_DEPLOYMENT_TARGET'} = $OSXCROSS_OSX_VERSION_MIN;
   $ENV{'OSXCROSS_OSX_VERSION_MIN'} = $OSXCROSS_OSX_VERSION_MIN;
   $ENV{'OSXCROSS_VERSION'}         = $OSXCROSS_VERSION;
   $ENV{'OSXCROSS_TARGET'}          = $OSXCROSS_TARGET;
   $ENV{'OSXCROSS_BASE_DIR'}        = $OSXCROSS_REPO;
   $ENV{'OSXCROSS_SDK'}     = $OSXCROSS_REPO . '/target/SDK/' . $OSXCROSS_SDK;
   $ENV{'OSXCROSS_SDK_DIR'} = $OSXCROSS_REPO . '/target/SDK';
   $ENV{'OSXCROSS_SDK_VERSION'}    = $OSXCROSS_SDK_VERSION;
   $ENV{'OSXCROSS_TARBALL_DIR'}    = $OSXCROSS_REPO . '/tarballs';
   $ENV{'OSXCROSS_PATCH_DIR'}      = $OSXCROSS_REPO . '/patches';
   $ENV{'OSXCROSS_TARGET_DIR'}     = $OSXCROSS_REPO . '/target';
   $ENV{'OSXCROSS_DIR_SDK_TOOLS'}  = $OSXCROSS_REPO . '/target/SDK/tools';
   $ENV{'OSXCROSS_BUILD_DIR'}      = $OSXCROSS_REPO . '/build';
   $ENV{'OSXCROSS_CCTOOLS_PATH'}   = $OSXCROSS_REPO . '/target/bin';
   $ENV{'OSXCROSS_LIBLTO_PATH'}    = $OSXCROSS_LIBLTO_PATH;
   $ENV{'OSXCROSS_LINKER_VERSION'} = $OSXCROSS_LINKER_VERSION;
   $ENV{'PATH'}       = $OSXCROSS_REPO . '/target/bin:' . $ENV{'PATH'};
   $ENV{'UNATTENDED'} = 1;
   $ENV{'CXX'}        = '/usr/bin/clang++';
   $ENV{'CC'}         = '/usr/bin/clang';
}

sub osxcross_dep_env {
   osxcross_build_env();
   $ENV{'OSXCROSS_SDK'}        = $OSXCROSS_TARGET;
   $ENV{'OSXCROSS_TARGET'}     = $OSXCROSS_TARGET;
   $ENV{'OSXCROSS_HOST'}       = $OSXCROSS_HOST;
   $ENV{'OSXCROSS_TARGET_DIR'} = $OSXCROSS_INSTALL;
   $ENV{'CXX'}                 = '/usr/bin/clang++';
   $ENV{'CC'}                  = '/usr/bin/clang';
}

sub build_osxcross {
   chdir $OSXCROSS_REPO;

   # Possibly allow for rebuilding in the future by running:
   # ./cleanup.sh
   say 'Copying SDK to tarballs.';
   say
       "rcopy($SDK_TARBALL, $OSXCROSS_REPO . '/tarballs/' . $OSXCROSS_SDK . '.tar.bz2')";
   rcopy( $SDK_TARBALL,
          $OSXCROSS_REPO . '/tarballs/' . $OSXCROSS_SDK . '.tar.bz2' )
       or die 'Could not copy SDK to tarballs';
   say 'Building osxcross.';
   sys('./build.sh');

   # patch macports for https://github.com/tpoechtrager/osxcross/issues/349
   say 'Patching macports script.';
   my $macports_file      = $OSXCROSS_REPO . '/target/bin/osxcross-macports';
   my $macports_file_orig = $macports_file . '.orig';
   unless ( -e $macports_file_orig ) {
      rename( $macports_file, $macports_file_orig )
          or die 'Could not move macports to ' . $macports_file_orig;
   }
   open my $in_fh, '<', $macports_file_orig
       or die 'Could not open file '
       . $macports_file_orig
       . ' for read: '
       . $!;
   open my $out_fh, '>', $macports_file
       or die 'Could not open file ' . $macports_file . ' for write ' . $!;
   for my $line (<$in_fh>) {
      $line =~ s/\bopenssl rmd160\b/openssl rmd160 -provider legacy/g;
      $line
          =~ s/\bopenssl dgst -ripemd160 -verify\b/openssl dgst -provider default -provider legacy -ripemd160 -verify/g;
      print {$out_fh} $line;
   }
   sys( 'chmod', '755', $macports_file );
}

sub disable_osx_version_checking {
   my ($file) = @_;
   my $file_orig = $file . '.orig';
   if ( -e $file_orig ) {
      unlink $file_orig;
   }
   rename( $file, $file_orig )
       or die 'Could not move ' . $file . ' to ' . $file_orig . ': ' . $!;
   open my $in_fh, '<', $file_orig
       or die 'Could not open file ' . $file_orig . ' for read: ' . $!;
   open my $out_fh, '>', $file
       or die 'Could not open file ' . $file . ' for write ' . $!;
   for my $line (<$in_fh>) {

   # __builtin_available doesn't work, so force-fail checks for it's existence
      $line
          =~ s/__has_builtin\(__builtin_available\)/__has_builtin(__disabled_builtin_available)/g;
      print {$out_fh} $line;
   }
}

sub remove_bad_osx_constant {
   my ($file) = @_;
   my $file_orig = $file . '.orig';
   if ( -e $file_orig ) {
      unlink $file_orig;
   }
   rename( $file, $file_orig )
       or die 'Could not move ' . $file . ' to ' . $file_orig . ': ' . $!;
   open my $in_fh, '<', $file_orig
       or die 'Could not open file ' . $file_orig . ' for read: ' . $!;
   open my $out_fh, '>', $file
       or die 'Could not open file ' . $file . ' for write ' . $!;
   for my $line (<$in_fh>) {
      $line =~ s/layer.contentsFormat = kCAContentsFormatRGBA8Uint;//;
      print {$out_fh} $line;
   }
}

sub patch_wxwidgets_for_osx {

# WxWidgets doesn't work with an SDK as old as I'm using because of one version-check macro -- while I can,
# I'll patch it to keep my lowest necessary version of MacOS as low as possible.
   say 'Patching wxwidgets available.h.';

# At one point, I edited include/wx/osx/private/available.h because the builtin
# checks were passing spuriously and I was getting an undefined symbol.
# This appears to be unnecessary now, but I should keep an eye on it.
   disable_osx_version_checking(
                    $WXWIDGETS_REPO . '/include/wx/osx/private/available.h' );
   disable_osx_version_checking( $WXWIDGETS_REPO . '/src/png/pngrutil.c' );
   remove_bad_osx_constant( $WXWIDGETS_REPO . '/src/osx/cocoa/window.mm' );
}

sub install_macports {
   say 'Removing previously installed macports libraries.';
   rmtree( $OSXCROSS_REPO . '/target/macports' );
   say 'Installing macports libraries.';

   # Install fake libs first, because they're often here due to dependencies
   sys( $OSXCROSS_REPO . '/target/bin/osxcross-macports',
        'fake-install', @MACPORTS_FAKELIBS );

   sys( $OSXCROSS_REPO . '/target/bin/osxcross-macports',
        'install', @MACPORTS_LIBS );
}

sub install_osxcross {
   say 'Removing previously installed osxcross.';
   sys( 'rm', '-Rf', $OSXCROSS_INSTALL );
   say 'Copying osxcross target to ' . $OSXCROSS_INSTALL . '.';
   sys( 'cp', '-R', $OSXCROSS_REPO . '/target', $OSXCROSS_INSTALL );
}

sub move_symlink {
   my ( $link, $src ) = @_;
   sys( 'rm', $link );
   sys( 'ln', '-s', $src, $link );
}

sub patch_files {
   say 'Patching files in osxcross target.';
   rcopy_glob( $OSXCROSS_PATCH_FILES, $ENV{'OSXCROSS_TARGET_DIR'} )
       or die 'Could not copy patch files: ' . $!;
}

sub fix_links {
   say 'Fixing bad symlinks in macports.';
   my $link_root = $OSXCROSS_INSTALL . '/macports/pkgs/opt/local/';
   move_symlink( $link_root . 'lib/libssl.a',
                 $link_root . 'libexec/openssl3/lib/libssl.a' );
   move_symlink( $link_root . 'lib/libcrypto.a',
                 $link_root . 'libexec/openssl3/lib/libcrypto.a' );
}

sub setup_curl {
   say 'Setting up Curl';
   clone_repo( 'curl', 'https://github.com/curl/curl.git', $CURL_VERSION );
   cmake( 'curl', '-DBUILD_SHARED_LIBS=ON' );
   build('curl');
   install('curl');
}

sub setup_googletest {
   say 'Setting up Googletest';
   clone_repo( 'googletest', 'https://github.com/google/googletest.git',
               $GTEST_VERSION );
   cmake('googletest');
   build('googletest');
   install('googletest');
}

sub setup_protobuf {
   say 'Setting up Protobuf';
   clone_repo( 'protobuf', 'https://github.com/google/protobuf.git',
               $PROTOBUF_VERSION );
   cmake('protobuf');
   build('protobuf');
   install('protobuf');
}

sub setup_wxwidgets {
   my ($osxcross) = @_;
   say 'Setting up Wxwidgets';
   clone_repo( 'wxWidgets', 'https://github.com/wxWidgets/wxWidgets.git',
               $WXWIDGETS_VERSION );
   if ($osxcross) {
      osxcross_dep_env();
      patch_wxwidgets_for_osx();
      cmake( 'wxWidgets',
             '-DCMAKE_TOOLCHAIN_FILE='
                 . $OSXCROSS_INSTALL
                 . '/toolchain.cmake',
             '-DCMAKE_INSTALL_NAME_TOOL='
                 . $OSXCROSS_INSTALL . '/bin/'
                 . $OSXCROSS_HOST
                 . '-install_name_tool',
             '-DCMAKE_INSTALL_PREFIX=' . $OSXCROSS_INSTALL . '/wxwidgets',
             '-DOSXCROSS_TARGET_DIR=' . $OSXCROSS_INSTALL,
             '-DCMAKE_BUILD_TYPE=Release',
             '-DwxBUILD_PRECOMP=OFF',
             '-DwxBUILD_SHARED=OFF'
           );
   } else {
      cmake( 'wxWidgets',              '-DCMAKE_BUILD_TYPE=Debug',
             '-DwxBUILD_TOOLKIT=gtk3', '-DwxBUILD_STRIPPED_RELEASE=OFF',
             '-DwxUSE_WEBVIEW=ON'
           );
   }
   build('wxWidgets');
   install('wxWidgets');
}

sub setup_osxcross {
   clone_repo( 'osxcross', 'https://github.com/tpoechtrager/osxcross.git' );
   osxcross_build_env();
   build_osxcross();
   install_macports();
   patch_files();
   install_osxcross();
   fix_links();
}

sub main {
   my (@args) = @_;
   my $osxcross = undef;
   $osxcross = 1 if $args[0] eq 'osxcross';
   chdir($BASE_DIR);

   setup_googletest();
   setup_protobuf();

   if ($osxcross) {
      setup_osxcross();
   } else {
      setup_curl();
   }
   setup_wxwidgets($osxcross);
   say 'Successfully completed.';
}

main(@ARGV);


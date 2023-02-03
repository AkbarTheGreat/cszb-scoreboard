#!/usr/bin/perl

=pod

Currently this is mostly a series of comments tracking my developing
understanding of how to get a Linux machine ready to build via osxcross.  I
hope to eventually turn this into a script that sets osxcross up to be ready
to build the scoreboard.

Copyright 2020-2022 Tracy Beck

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.


This script builds osxcross on one of my (akbar's) machines such that it can build the scoreboard.  YMMV.

Other miscellaneous notes about using osxcross:

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
use Getopt::Long qw{GetOptions};
use File::Basename qw{dirname};
use File::Copy::Recursive qw{rcopy rcopy_glob};
use File::Path qw{mkpath rmtree};
use File::Which;
use FindBin qw{$RealBin};
use List::AllUtils qw{none};

our $OSXCROSS_INSTALL = '/opt/osxcross';
our $OSX_VERSION = '10.12';
our $OSXCROSS_VERSION = 1.4;
our $OSXCROSS_OSX_VERSION_MIN = 10.9;
our $OSXCROSS_TARGET = 'darwin21.4';
our $OSXCROSS_HOST = 'x86_64-apple-' . $OSXCROSS_TARGET;
our $OSXCROSS_SDK_VERSION = '12.3';
our $OSXCROSS_SDK = 'MacOSX' . $OSXCROSS_SDK_VERSION . '.sdk';
our $OSXCROSS_LIBLTO_PATH = '/usr/lib/llvm-14/lib';
our $OSXCROSS_LINKER_VERSION = 609;

our $START_DIR = Cwd::cwd();
our $REPO_BASE = $START_DIR;
our %VALID_ACTIONS = (
   'init' => sub{return init();},
   'osxcross' => sub{return osxcross();},
   'wxwidgets' => sub{return wxwidgets();},
   'install' => sub{return install();},
   'all' => sub{return init() && osxcross() && wxwidgets() && install();},
);
our $OSXCROSS_PATCH_FILES = dirname($RealBin) . '/osxcross_patches/*';

# Macports libraries to be installed as static libs.
our @MACPORTS_LIBS = qw(
  bzip2
  curl
  curl-ca-bundle
  expat
  gettext
  glib2
  gtest
  libedit
  libffi
  libiconv
  libidn2
  libpsl
  libunistring
  ncurses
  openssl
  pcre
  zlib
);

# These libraries only have dylibs, so we have to install those.
our @MACPORTS_DYLIBS = qw(
  jsoncpp-devel
  protobuf3-cpp
);

our $OSXCROSS_REPO = $REPO_BASE . '/osxcross';
our $WXWIDGETS_REPO = $REPO_BASE . '/wxWidgets';
our $WXWIDGETS_BUILD_DIR = $WXWIDGETS_REPO . '/osxcross';
our $OSXCROSS_ORIGIN = 'https://github.com/tpoechtrager/osxcross.git';
our $WXWIDGETS_ORIGIN = 'https://github.com/wxWidgets/wxWidgets.git';
our $SDK_TARBALL = '/usr/share/osx_tarballs/' . $OSXCROSS_SDK . '.tar.bz2';

my ($opt_help, $opt_action);

my %options = (
   'help|?'     => {'val'=>\$opt_help,'help'=>'This help'},
   'action:s'     => {'val'=>\$opt_action,'help'=>'Action to perform, may be: ' . join(', ', sort(keys %VALID_ACTIONS)) . ' (default: all)'},
);

sub usage {
   say $0 . ': Setup OSXCross for use in developing the scoreboard.';
   for my $opt (keys %options) {
      say "\t" . $opt . ': ' . $options{$opt}{'help'};
   }
   exit 0;
}

sub valid_args {
   if (none  {$_ eq $opt_action} keys %VALID_ACTIONS) {
      warn 'Action "' . $opt_action . '" unrecognized.' . "\n";
      return undef;
   }
   if (!-d $OSXCROSS_REPO || !-d $WXWIDGETS_REPO) {
      if ($opt_action ne 'init' && $opt_action ne 'all') {
         warn 'Repos not initialized in current directory, run init first or change into the base path for all repos.' . "\n";
	 return undef;
      }
   }
   return 1;
}

sub parse_options {
   my %parseable_options;
   for my $key (keys %options) {
      $parseable_options{$key} = $options{$key}{'val'};
   }
   GetOptions(%parseable_options) or usage();
   usage() if $opt_help;
   $opt_action //= 'all';
   usage() unless valid_args();
}

sub setup_osxcross_build_env {
   $ENV{'MACOSX_DEPLOYMENT_TARGET'} = $OSXCROSS_OSX_VERSION_MIN;
   $ENV{'OSXCROSS_OSX_VERSION_MIN'} = $OSXCROSS_OSX_VERSION_MIN;
   $ENV{'OSXCROSS_VERSION'} = $OSXCROSS_VERSION;
   $ENV{'OSXCROSS_TARGET'} = $OSXCROSS_TARGET;
   $ENV{'OSXCROSS_BASE_DIR'} = $OSXCROSS_REPO;
   $ENV{'OSXCROSS_SDK'} = $OSXCROSS_REPO . '/target/SDK/' . $OSXCROSS_SDK;
   $ENV{'OSXCROSS_SDK_DIR'} = $OSXCROSS_REPO . '/target/SDK';
   $ENV{'OSXCROSS_SDK_VERSION'} = $OSXCROSS_SDK_VERSION;
   $ENV{'OSXCROSS_TARBALL_DIR'} = $OSXCROSS_REPO . '/tarballs';
   $ENV{'OSXCROSS_PATCH_DIR'} = $OSXCROSS_REPO . '/patches';
   $ENV{'OSXCROSS_TARGET_DIR'} = $OSXCROSS_REPO . '/target';
   $ENV{'OSXCROSS_DIR_SDK_TOOLS'} = $OSXCROSS_REPO . '/target/SDK/tools';
   $ENV{'OSXCROSS_BUILD_DIR'} = $OSXCROSS_REPO . '/build';
   $ENV{'OSXCROSS_CCTOOLS_PATH'} = $OSXCROSS_REPO . '/target/bin';
   $ENV{'OSXCROSS_LIBLTO_PATH'} = $OSXCROSS_LIBLTO_PATH;
   $ENV{'OSXCROSS_LINKER_VERSION'} = $OSXCROSS_LINKER_VERSION;
   $ENV{'PATH'} = $OSXCROSS_REPO . '/target/bin:' . $ENV{'PATH'};
   $ENV{'UNATTENDED'} = 1;
   $ENV{'CXX'} = '/usr/bin/clang++';
   $ENV{'CC'} = '/usr/bin/clang';
}

sub setup_osxcross_dep_env {
   setup_osxcross_build_env();
   $ENV{'OSXCROSS_SDK'} = $OSXCROSS_TARGET;
   $ENV{'OSXCROSS_TARGET'} = $OSXCROSS_TARGET;
   $ENV{'OSXCROSS_HOST'} = $OSXCROSS_HOST;
   $ENV{'OSXCROSS_TARGET_DIR'} = $OSXCROSS_INSTALL;
   $ENV{'CXX'} = '/usr/bin/clang++';
   $ENV{'CC'} = '/usr/bin/clang';
}

# Wrapper around system which allows us to die on any system failure easily.
sub sys {
	my (@args) = @_;
	my $retval = system @args;
	return unless $retval;
	die 'System call failed: ' . "@args\n" . '  Died with exit code: ' . $retval . "\n  " . $?;
}

sub update_repo {
   my ($repo, $origin) = @_;
   if (-d $repo) {
      say 'Repo exists, resetting.';
      chdir $repo;
      sys('git', 'reset', '--hard');
   } else {
      say 'New repo, cloning.';
      chdir $REPO_BASE;
      mkpath $repo;
      sys('git', 'clone', $origin);
   }
   chdir $repo;
   say 'PUlling repo for freshness.';
   sys('git', 'pull');
   say 'Initializing git submodules.';
   sys('git', 'submodule', 'update', '--init', '--recursive');
}

sub build_osxcross {
   chdir $OSXCROSS_REPO;
   # Possibly allow for rebuilding in the future by running:
   # ./cleanup.sh
   say 'Copying SDK to tarballs.';
   say "rcopy($SDK_TARBALL, $OSXCROSS_REPO . '/tarballs/' . $OSXCROSS_SDK . '.tar.bz2')";
   rcopy($SDK_TARBALL, $OSXCROSS_REPO . '/tarballs/' . $OSXCROSS_SDK . '.tar.bz2') or die 'Could not copy SDK to tarballs'; 
   say 'Building osxcross.';
   sys('./build.sh');
   # patch macports for https://github.com/tpoechtrager/osxcross/issues/349
   say 'Patching macports script.';
   my $macports_file = $OSXCROSS_REPO . '/target/bin/osxcross-macports';
   my $macports_file_orig = $macports_file . '.orig';
   unless (-e $macports_file_orig) {
   	rename ($macports_file, $macports_file_orig) or die 'Could not move macports to ' . $macports_file_orig;
   }
   open my $in_fh, '<', $macports_file_orig or die 'Could not open file ' . $macports_file_orig . ' for read: ' . $!;
   open my $out_fh, '>', $macports_file or die 'Could not open file ' . $macports_file . ' for write ' . $!;
   for my $line (<$in_fh>) {
      $line =~ s/\bopenssl rmd160\b/openssl rmd160 -provider legacy/g;
      $line =~ s/\bopenssl dgst -ripemd160 -verify\b/openssl dgst -provider default -provider legacy -ripemd160 -verify/g;
      print {$out_fh} $line;
   }
   sys('chmod', '755', $macports_file);
}

sub disable_osx_version_checking {
   my ($file) = @_;
   my $file_orig = $file . '.orig';
   if (-e $file_orig) {
      unlink $file_orig;
   }
   rename ($file, $file_orig) or die 'Could not move ' . $file . ' to ' . $file_orig . ': ' . $!;
   open my $in_fh, '<', $file_orig or die 'Could not open file ' . $file_orig . ' for read: ' . $!;
   open my $out_fh, '>', $file or die 'Could not open file ' . $file . ' for write ' . $!;
   for my $line (<$in_fh>) {
      # __builtin_available doesn't work, so force-fail checks for it's existence
      $line =~ s/__has_builtin\(__builtin_available\)/__has_builtin(__disabled_builtin_available)/g;
      print {$out_fh} $line;
   }
}

sub remove_bad_osx_constant {
   my ($file) = @_;
   my $file_orig = $file . '.orig';
   if (-e $file_orig) {
      unlink $file_orig;
   }
   rename ($file, $file_orig) or die 'Could not move ' . $file . ' to ' . $file_orig . ': ' . $!;
   open my $in_fh, '<', $file_orig or die 'Could not open file ' . $file_orig . ' for read: ' . $!;
   open my $out_fh, '>', $file or die 'Could not open file ' . $file . ' for write ' . $!;
   for my $line (<$in_fh>) {
	   $line =~ s/layer.contentsFormat = kCAContentsFormatRGBA8Uint;//;
      print {$out_fh} $line;
   }
}

sub patch_wxwidgets {
   # WxWidgets doesn't work with an SDK as old as I'm using because of one version-check macro -- while I can,
   # I'll patch it to keep my lowest necessary version of MacOS as low as possible.
   say 'Patching wxwidgets available.h.';
   disable_osx_version_checking($WXWIDGETS_REPO . '/include/wx/osx/private/available.h');
   disable_osx_version_checking($WXWIDGETS_REPO . '/src/png/pngrutil.c');
   remove_bad_osx_constant($WXWIDGETS_REPO . '/src/osx/cocoa/window.mm');
}

sub build_wxwidgets {
   # At one point, I edited include/wx/osx/private/available.h because the builtin
   # checks were passing spuriously and I was getting an undefined symbol.
   # This appears to be unnecessary now, but I should keep an eye on it.
   chdir($WXWIDGETS_REPO);
   mkpath($WXWIDGETS_BUILD_DIR);
   chdir($WXWIDGETS_BUILD_DIR);
   say 'Running cmake.';
   sys('cmake',
      '-DCMAKE_TOOLCHAIN_FILE=' . $OSXCROSS_INSTALL . '/toolchain.cmake',
	  '-DCMAKE_INSTALL_NAME_TOOL=' . $OSXCROSS_INSTALL . '/bin/' . $OSXCROSS_HOST . '-install_name_tool',
	  '-DCMAKE_INSTALL_PREFIX=' . $OSXCROSS_INSTALL . '/wxwidgets',
	  '-DOSXCROSS_TARGET_DIR=' . $OSXCROSS_INSTALL,
	  '-DCMAKE_BUILD_TYPE=Release',
	  '-DwxBUILD_PRECOMP=OFF',
	  '-DwxBUILD_SHARED=OFF',
	  '..');
   say 'Building wxwidgets.';
   sys('/usr/bin/make', '-j6', 'all');
   return 1;
}


sub install_macports {
   say 'Removing previously installed macports libraries.';
   rmtree($OSXCROSS_REPO . '/target/macports');
   say 'Installing macports libraries.';
   # (The -s strips out dylibs for us).
   sys($OSXCROSS_REPO . '/target/bin/osxcross-macports', 'install', '-s', @MACPORTS_LIBS);
   # These libs appear to only have dylibs.  So we don't strip them out.
   sys($OSXCROSS_REPO . '/target/bin/osxcross-macports', 'install', @MACPORTS_DYLIBS);

}

sub install_osxcross {
   say 'Removing previously installed osxcross.';
   sys('rm', '-Rf', $OSXCROSS_INSTALL);
   say 'Copying osxcross target to ' . $OSXCROSS_INSTALL . '.';
   sys('cp', '-R', $OSXCROSS_REPO . '/target', $OSXCROSS_INSTALL);
}

sub install_wxwidgets {
   say 'Installing wxwidgets.';
   chdir($WXWIDGETS_BUILD_DIR);
   sys('/usr/bin/make', 'install');
   return 1;
}

sub move_symlink {
   my ($link, $src) = @_;
   sys('rm', $link);
   sys('ln', '-s', $src, $link);
}

sub patch_files {
   say 'Patching files in osxcross target.';
   rcopy_glob($OSXCROSS_PATCH_FILES, $ENV{'OSXCROSS_TARGET_DIR'});
}

sub fix_links {
   say 'Fixing bad symlinks in macports.';
   my $link_root = $OSXCROSS_INSTALL . '/macports/pkgs/opt/local/';
   move_symlink($link_root . 'lib/libssl.a',
                $link_root . 'libexec/openssl3/lib/libssl.a');
   move_symlink($link_root . 'lib/libcrypto.a',
                $link_root . 'libexec/openssl3/lib/libcrypto.a');
}

sub init {
   say 'Getting freshest osxcross repo.';
   update_repo($OSXCROSS_REPO, $OSXCROSS_ORIGIN);
   say 'Getting freshest wxwidgets repo.';
   update_repo($WXWIDGETS_REPO, $WXWIDGETS_ORIGIN);
   return 1;
}

sub osxcross {
   setup_osxcross_build_env();
   build_osxcross();
   install_macports();
   patch_files();
   install_osxcross();
   fix_links();
   return 1;
}

sub wxwidgets {
   setup_osxcross_dep_env();
   patch_wxwidgets();
   build_wxwidgets();
   install_wxwidgets();
   return 1;
}

sub install {
return 1;
}

sub main {
   parse_options();
   unless($VALID_ACTIONS{$opt_action}->()) {
      die 'An error occurred performing action ' . $opt_action . "\n";
   }
   say 'Successfully completed.';
}

main();


#!/usr/bin/perl

=pod

This script builds a MacOS app bundle of the scoreboard using osxcross on a
Linux machine.  The built application is in out/osxcross_package

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

=cut

use 5.030;
use Cwd;
use File::Basename qw(basename);
use File::Copy;
use File::Path qw(mkpath);
use File::Which;
use Getopt::Long   qw{GetOptions};
use List::AllUtils qw(any);
use FindBin;

our $BUILD_PATH    = 'out/osxcross';
our $PACKAGE_PATH  = 'out/osxcross_package';
our $BASE_DIR      = Cwd::cwd();
our $OSX_VERSION   = '10.12';
our $APP_CONTAINER = $PACKAGE_PATH . '/CszbScoreboard.app/Contents';
our $APP_BIN       = $APP_CONTAINER . '/MacOS';
our $APP_RESOURCES = $APP_CONTAINER . '/Resources';

our $DOCKER_TAG       = 'osxcross_release';
our $DOCKER_CONTAINER = 'osxcross_release_exec';

my ( $opt_help, $opt_version, $opt_dry_run, $opt_test_build, $opt_internal );

my %options = (
   'help|?'    => { 'val' => \$opt_help, 'help' => 'This help' },
   'version=s' =>
       { 'val' => \$opt_version, 'help' => 'Version to release (required)' },
   'dry_run' => { 'val'  => \$opt_dry_run,
                  'help' => 'Print steps, take no action.'
                },
   'test_build' => { 'val'  => \$opt_test_build,
                     'help' => 'Build a debug version for testing.'
                   },
   'internal_run' => {
      'val'  => \$opt_internal,
      'help' =>
          'Used to signify that this is inside of a docker container and should do actual work.'
   },
);

sub usage {
   say $0
       . ': This builds an osxcross release, utilizing the osxcross Docker container.';
   for my $opt ( keys %options ) {
      say "\t" . $opt . ': ' . $options{$opt}{'help'};
   }
   exit 0;
}

sub parse_options {
   my %parseable_options;
   for my $key ( keys %options ) {
      $parseable_options{$key} = $options{$key}{'val'};
   }
   GetOptions(%parseable_options) or usage();
   usage() if $opt_help;
   usage unless $opt_version;
}

sub run_cmd {
   my @cmd = @_;
   if ($opt_dry_run) {
      say '>', join ' ', @cmd;
   } else {
      system @cmd;
      return $?;
   }
   return 0;
}

sub build_release {
   say 'Building release in ' . $BUILD_PATH;
   mkpath $BUILD_PATH;
   chdir $BUILD_PATH;
   my $release_type = $opt_test_build ? 'Debug' : 'Release';
   my $result = run_cmd( 'cmake',
                         '-DCMAKE_OSX_DEPLOYMENT_TARGET=' . $OSX_VERSION,
                         '-DCMAKE_TOOLCHAIN_FILE='
                             . $ENV{'OSXCROSS_TARGET_DIR'}
                             . '/toolchain.cmake',
                         '-DCMAKE_BUILD_TYPE=' . $release_type,
                         '-DOPENSSL_ROOT_DIR='
                             . $ENV{'OSXCROSS_TARGET_DIR'}
                             . '/macports/pkgs/opt/local/libexec/openssl3',
                         $BASE_DIR
                       );
   return $result if $result;
   return run_cmd( 'make', '-j5', 'scoreboard_proto', 'cszb-scoreboard' );
}

sub plist_content {
   my ($version) = @_;
   say 'Updating PList';
   my $plist_prefix = q{<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple Computer//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
  <key>CFBundleGetInfoString</key>
  <string>ComedySportz Scoreboard</string>
  <key>CFBundleExecutable</key>
  <string>scoreboard.sh</string>
  <key>CFBundleIdentifier</key>
  <string>dev.akbar.cszb-scoreboard</string>
  <key>CFBundleName</key>
  <string>cszb-scoreboard</string>
  <key>CFBundleIconFile</key>
  <string>scoreboard</string>
  <key>CFBundleShortVersionString</key>
  <string>};
   my $plist_suffix = q{</string>
  <key>CFBundleInfoDictionaryVersion</key>
  <string>6.0</string>
  <key>CFBundlePackageType</key>
  <string>APPL</string>
</dict>
</plist>
};
   return $plist_prefix . $version . $plist_suffix;
}

sub fix_dylibs {
   my ( $process_binary, @processed_libs ) = @_;

   # Avoid doubling back, because that way lies infinite recursion.
   next if any { $_ eq $process_binary } @processed_libs;
   push @processed_libs, $process_binary;

   my $otool
       = $ENV{'OSXCROSS_TARGET_DIR'} . '/bin/'
       . $ENV{'OSXCROSS_HOST'}
       . '-otool';
   my $install_name_tool
       = $ENV{'OSXCROSS_TARGET_DIR'} . '/bin/'
       . $ENV{'OSXCROSS_HOST'}
       . '-install_name_tool';

   my @libraries = `$otool -L $process_binary`;
   for my $found_lib (@libraries) {
      chomp($found_lib);
      $found_lib =~ s/\s+\(.*//;
      my $lib = $found_lib;
      next unless $lib =~ s#^\s+/opt/local/##;
      my $libname = basename($lib);

      my $source_lib
          = $ENV{'OSXCROSS_TARGET_DIR'} . '/macports/pkgs/opt/local/' . $lib;
      my $target_lib = $APP_BIN . q{/} . $libname;

      die 'Library ' . $source_lib . ' not found!' unless -e $source_lib;

      unless ( -e $APP_BIN . q{/} . $lib ) {
         copy( $source_lib, $target_lib )
             or die 'Copy of library ' . $lib . ' failed: ' . $!;
      }
      my $result
          = run_cmd( $install_name_tool, '-change', $found_lib,
                     '@executable_path/' . $libname,
                     $process_binary );
      return $result if $result;
      return fix_dylibs( $target_lib, @processed_libs );
   }

}

sub copy_libraries {
   say 'Copying dynamic libraries into package.';
   return fix_dylibs( $APP_BIN . '/cszb-scoreboard' );
}

sub create_app_package {
   my ($version) = @_;
   say 'Creating Package';
   chdir $BASE_DIR;
   mkpath $APP_BIN;
   mkpath $APP_RESOURCES;
   open my $out_fh, '>', $APP_CONTAINER . '/Info.plist';
   print {$out_fh} plist_content($version);
   copy( $BUILD_PATH . '/cszb-scoreboard', $APP_BIN . '/cszb-scoreboard' )
       or die 'Copy of binary failed: ' . $!;
   copy( $BASE_DIR . '/resources/scoreboard.icns',
         $APP_RESOURCES . '/scoreboard.icns' )
       or die 'Copy of icon failed: ' . $!;
   copy( $BASE_DIR . '/resources/app_package/scoreboard.sh',
         $APP_BIN . '/scoreboard.sh' )
       or die 'Copy of bootstrap script failed: ' . $!;
   copy( $ENV{'OSXCROSS_TARGET_DIR'}
             . '/macports/pkgs/opt/local/share/curl/curl-ca-bundle.crt',
         $APP_BIN . '/cert.pem'
       )
       or die 'Copy of SSL certs failed: ' . $!;
   chmod 0777, $APP_BIN . '/cszb-scoreboard';
   chmod 0777, $APP_BIN . '/scoreboard.sh';
   return copy_libraries();
}

sub zip_package {
   say 'Zipping package for Github';
   chdir $PACKAGE_PATH;
   return run_cmd( 'zip', '-r', 'CszbScoreboard', 'CszbScoreboard.app' );
}

sub internal_build {
   build_release();
   my $result = create_app_package($opt_version);
   return $result if $result;
   say 'Release ' . $opt_version . ' created';
   return zip_package();
}

sub build_docker {
   say 'Building Docker image.';
   return
       run_cmd( 'docker', 'build', '-f', $BASE_DIR . '/Dockerfile.osxcross',
                '-t', $DOCKER_TAG, $BASE_DIR );
}

sub start_docker {
   say 'Starting Docker container.';
   mkpath( $BASE_DIR . q{/} . $PACKAGE_PATH );
   return
       run_cmd( 'docker',
                'run',
                '-v',
                $BASE_DIR . q{/}
                    . $PACKAGE_PATH
                    . ':/src/cszb-scoreboard/'
                    . $PACKAGE_PATH,
                '--name',
                $DOCKER_CONTAINER,
                '-dit',
                $DOCKER_TAG,
                '/bin/sh'
              );
}

sub shutdown_docker {
   say 'Shutting down Docker container.';
   return run_cmd( 'docker', 'rm', '-f', $DOCKER_CONTAINER );
}

sub docker_cmd {
   my @cmd = @_;
   return
       run_cmd( 'docker', 'exec', '-w', '/src/cszb-scoreboard', '-it',
                $DOCKER_CONTAINER, @cmd );
}

sub run_docker {
   my @args = ( '--version', $opt_version, '--internal_run' );
   push @args, '--test_build' if $opt_test_build;

# All of these commands need to warn instead of die so that the docker shutdown can attempt to run regardless.
   my $error = undef;
   if ( build_docker() != 0 ) {
      warn 'Error building docker container: ' . $!;
      $error = 1;
   }
   if ( !$error && start_docker() != 0 ) {
      warn 'Error starting docker container: ' . $!;
   }

   say 'Running build inside of Docker image.';
   if ( !$error && docker_cmd( 'etc/scripts/' . basename($0), @args ) != 0 ) {
      warn 'Error running docker command: ' . $!;
   }

   if ( shutdown_docker() != 0 ) {
      die 'Error shutting docker down: ' . $!;
   }
   return $error;
}

sub main {
   parse_options();
   if ($opt_internal) {
      if ( internal_build() != 0 ) {
         die 'Error running internal build: ' . $!;
      }
   } else {
      if ( run_docker() != 0 ) {
         die 'Error starting internal docker run: ' . $!;
      }
   }
   say 'Process complete.';
}

main();


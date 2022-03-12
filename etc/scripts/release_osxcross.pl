#!/usr/bin/perl

=pod

This script builds a MacOS app bundle of the scoreboard using osxcross on a
Linux machine.

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

=cut


use 5.030;
use Cwd;
use File::Copy;
use File::Path qw(mkpath);
use File::Which;
use Getopt::Long qw{GetOptions};
use List::AllUtils qw(any);

use FindBin;
use lib "$FindBin::RealBin";

# Project local libraries
use GitHub;

our $BUILD_PATH = 'out/osxcross';
our $BASE_DIR = Cwd::cwd();
our $OSX_VERSION = '10.12';
our $APP_CONTAINER = 'CszbScoreboard.app/Contents';
our $APP_BIN = $APP_CONTAINER . '/MacOS';
our $APP_RESOURCES = $APP_CONTAINER .  '/Resources';

my ($opt_help, $opt_version, $opt_dry_run, $opt_test_build);

my %options = (
    'help|?'     => {'val'=>\$opt_help,'help'=>'This help'},
    'version=s'  => {'val'=>\$opt_version,'help'=>'Version to release (required)'},
    'dry_run'    => {'val'=>\$opt_dry_run,'help'=>'Dry run only. (do not upload to git)'},
    'test_build' => {'val'=>\$opt_test_build,'help'=>'Build a debug version for testing. (implies dry_run)'},
);

sub usage {
    say $0 . ': Release the Kraken!  Err, scoreboard.';
    for my $opt (keys %options) {
        say "\t" . $opt . ': ' . $options{$opt}{'help'};
	}
    exit 0;
}

sub parse_options {
    my %parseable_options;
    for my $key (keys %options) {
        $parseable_options{$key} = $options{$key}{'val'};
	}
    GetOptions(%parseable_options) or usage();
    usage() if $opt_help;
    usage unless $opt_version;
    $opt_dry_run = 1 if $opt_test_build;
}


sub setup_env {
  my $osxcross_path = which 'osxcross';
  $osxcross_path =~ s#/bin/osxcross$##;
  $ENV{'OSXCROSS_SDK'} = 'darwin19';
  $ENV{'OSXCROSS_TARGET'} = $ENV{'OSXCROSS_SDK'};
  $ENV{'OSXCROSS_HOST'} = 'x86_64-apple-' . $ENV{'OSXCROSS_SDK'};
  $ENV{'OSXCROSS_TARGET_DIR'} = $osxcross_path;
}

sub build_release {
  setup_env();
  mkpath $BUILD_PATH;
  chdir $BUILD_PATH;
  my $release_type = $opt_test_build?'Debug':'Release';
  system 'cmake'
    . ' -DCMAKE_OSX_DEPLOYMENT_TARGET=' . $OSX_VERSION
    . ' -DCMAKE_TOOLCHAIN_FILE=' . $ENV{'OSXCROSS_TARGET_DIR'}
                                 . '/toolchain.cmake'
    . ' -DCMAKE_BUILD_TYPE=' . $release_type
    . ' ' . $BASE_DIR;
  system 'make cszb-scoreboard';
}

sub plist_content {
  my ($version) = @_;
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
  my ($process_binary, @processed_libs) = @_;

  # Avoid doubling back, because that way lies infinite recursion.
  next if any {$_ eq $process_binary} @processed_libs;
  push @processed_libs, $process_binary;

  my $otool = $ENV{'OSXCROSS_TARGET_DIR'} . '/bin/' . $ENV{'OSXCROSS_HOST'}
    . '-otool';
  my $install_name_tool = $ENV{'OSXCROSS_TARGET_DIR'} . '/bin/'
    . $ENV{'OSXCROSS_HOST'} . '-install_name_tool';

  my @libraries = `$otool -L $process_binary`;
  for my $lib (@libraries) {
    chomp($lib);
    next unless $lib =~ s#^\s+/opt/local/lib/##;
    $lib =~ s/\s+\(.*//;

    my $source_lib = $ENV{'OSXCROSS_TARGET_DIR'}
      . '/macports/pkgs/opt/local/lib/' . $lib;
    my $target_lib = $APP_BIN . q{/} . $lib;

    unless (-e $APP_BIN . q{/} . $lib) {
      copy($source_lib, $target_lib)
        or die 'Copy of library ' . $lib . ' failed: ' . $!;
    }
    system($install_name_tool . ' -change /opt/local/lib/' . $lib
      . ' @executable_path/' . $lib . q{ } . $process_binary);
    fix_dylibs($target_lib, @processed_libs);
  }

}

sub copy_libraries {
  fix_dylibs($APP_BIN . '/cszb-scoreboard');
}

sub create_app_package {
  my ($version) = @_;
  chdir $BUILD_PATH;
  mkpath $APP_BIN;
  mkpath $APP_RESOURCES;
  open my $out_fh, '>', $APP_CONTAINER . '/Info.plist';
  print {$out_fh} plist_content($version);
  copy('./cszb-scoreboard', $APP_BIN . '/cszb-scoreboard')
    or die 'Copy of binary failed: ' . $!;
  copy($BASE_DIR . '/resources/scoreboard.icns', $APP_RESOURCES . '/scoreboard.icns')
    or die 'Copy of icon failed: ' . $!;
  copy($BASE_DIR . '/resources/app_package/scoreboard.sh', $APP_BIN . '/scoreboard.sh')
    or die 'Copy of bootstrap script failed: ' . $!;
  copy($ENV{'OSXCROSS_TARGET_DIR'} . '/macports/pkgs/opt/local/share/curl/curl-ca-bundle.crt', 
    $APP_BIN . '/cert.pem')
    or die 'Copy of SSL certs failed: ' . $!;
  chmod 0777, $APP_BIN . '/cszb-scoreboard';
  chmod 0777, $APP_BIN . '/scoreboard.sh';
  copy_libraries();
}

sub zip_package {
  chdir $BUILD_PATH;
  system 'zip -r CszbScoreboard CszbScoreboard.app';
}

sub upload_to_github {
  my ($version) = @_;

  chdir $BUILD_PATH;

  my $upload_path = GitHub::find_existing_release($version);
  unless ($upload_path) {
    die 'Error finding release from GitHub ' . $!;
  }
  if (GitHub::upload_binary(
      $upload_path,
      'CszbScoreboard.zip',
      'MacOS',
      './CszbScoreboard.zip') != 0) {
    die 'Error adding file to release at GitHub ' . $!;
  }
}

sub main {
  parse_options();
  build_release();
  create_app_package($opt_version);
  say 'Release ' . $opt_version . ' created';

  if ($opt_dry_run) {
    say 'Dry run enabled, skipping upload to GitHub.';
  } else {
    say 'Uploading to GitHub.';
    zip_package();
    upload_to_github($opt_version);
  }

  say 'Process complete.';
}

main();


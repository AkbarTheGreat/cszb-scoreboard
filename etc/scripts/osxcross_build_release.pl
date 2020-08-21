#!/usr/bin/perl

=pod

This script builds a MacOS app bundle of the scoreboard using osxcross on a
Linux machine.

Copyright 2020 Tracy Beck

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
use List::AllUtils qw(any);

our $BUILD_PATH = 'out/osxcross';
our $BASE_DIR = Cwd::cwd();
our $OSX_VERSION = '10.12';
our $APP_CONTAINER = 'CszbScoreboard.app/Contents';
our $APP_BIN = $APP_CONTAINER . '/MacOS';
our $APP_RESOURCES = $APP_CONTAINER .  '/Resources';

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
  system 'cmake'
    . ' -DCMAKE_OSX_DEPLOYMENT_TARGET=' . $OSX_VERSION
    . ' -DCMAKE_TOOLCHAIN_FILE=' . $ENV{'OSXCROSS_TARGET_DIR'}
                                 . '/toolchain.cmake'
    . ' -DCMAKE_BUILD_TYPE=Release'
    . ' ../..';
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
  <string>cszb-scoreboard</string>
  <key>CFBundleIdentifier</key>
  <string>dev.akbar.cszb-scoreboard</string>
  <key>CFBundleName</key>
  <string>cszb-scoreboard</string>
  <key>CFBundleIconFile</key>
  <string>scoreboard.icns</string>
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
  my $source_lib = $ENV{'OSXCROSS_TARGET_DIR'} . '/macports/pkgs/opt/local/lib/'
    . $lib;
  my $target_lib = $APP_BIN . q{/} . $lib;

  my @libraries = `$otool -L $process_binary`;
  for my $lib (@libraries) {
    chomp($lib);
    next unless $lib =~ s#^\s+/opt/local/lib/##;
    $lib =~ s/\s+\(.*//;
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
  chmod 0777, $APP_BIN . '/cszb-scoreboard';
  copy_libraries();
}

sub main {
  my ($version) = @_;
  build_release();
  create_app_package($version);
  say 'Release ' . $version . ' created.';
}

die 'Requires exactly one argument, the version' unless @ARGV == 1;

my $version = shift;
main($version);


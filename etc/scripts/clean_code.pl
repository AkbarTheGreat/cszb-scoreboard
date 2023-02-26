#!/usr/bin/perl

=pod

This script runs include-what-you-use and then clang-format on the entire
codebase, cleaning up any messy human stuff that might've made our pretty
code ugly.  It's only tested on Ubuntu and some stuff would probably need
to be changed to make it work on other platforms.

Copyright 2021-2023 Tracy Beck

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
use Getopt::Long   qw{GetOptions};
use List::AllUtils qw(any);

use FindBin;
use lib "$FindBin::RealBin";
use Docker;

our $BASE_DIR          = Cwd::cwd();
our $DOCKER_BUILD_PATH = '/src/out/iwyu';
our $BUILD_PATH        = $BASE_DIR . q{/out/iwyu};
my $DOCKER_ROOT = '/src/cszb-scoreboard';

our @MARKDOWN_FILES = qw(
    CODE_OF_CONDUCT.md
    CONTRIBUTING.md
    README.md
    doc/developers.md
    doc/kiosk_setup.md
    doc/users.md
    include/config/swx/README.md
    include/ui/widget/swx/README.md
);

our $IS_WSL = undef;

my ( $opt_help, $opt_docker );
my $opt_procs = 2;

# No real options yet, but it makes pretty boilerplate.
my %options = (
      'help|?'      => { 'val' => \$opt_help, 'help' => 'This help' },
      'processes=i' => {
         'val'  => \$opt_procs,
         'help' => 'The number of jobs to run per build execution (default 2)'
      },
      'docker' => {
            'val'  => \$opt_docker,
            'help' => 'Run commands inside of the standard docker container.',
      },
);

sub sys {
   my ( $docker, @args ) = @_;
   if ($docker) {
      $docker->cmd(@args);
   } else {
      system @args;
   }
}

# Sys, with redirect capabilities (pipe, file io, etc), by wrapping in a bash script (and so a bit wonkier)
sub sys_io {
   my ( $docker, @args ) = @_;
   sys( $docker, 'bash', '-c', join q{ }, @args );
}

sub usage {
   say $0 . ': Make code pretty. (runs perltidy, iwyu, and clang-format)';
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
}

sub status {
   my (@msg) = @_;
   say '=' x 50;
   say @msg;
   say '=' x 50;
}

sub docker {
   return undef unless $opt_docker;
   my %volumes = ( $BASE_DIR => $DOCKER_ROOT );
   my $docker = Docker->new( 'build'   => 'standard',
                             'name'    => 'code_clean',
                             'verbose' => 1,
                             'volumes' => \%volumes,
                           );
   $docker->cmd( 'apt', '-y', 'install', 'iwyu', 'clang-format', 'pip' );

# This is a hack to deal with iwyu being compiled against a different clang version.
   $docker->cmd( 'ln',   '-s', '/usr/lib/clang/14', '/usr/lib/clang/13.0.1' );
   $docker->cmd( 'cpan', 'install', 'Perl::Tidy' );
   $docker->cmd( 'pip',  'install', 'mdformat-gfm' );
   return $docker;
}

sub workdir {
   my ( $docker, $dir ) = @_;
   if ($docker) {
      if ($dir) {
         $docker->cmd( 'mkdir', '-p', $dir );
      }
      $docker->workdir($dir);
   } else {
      if ( $dir !~ /^\// ) {
         $dir = $BASE_DIR . q{/} . $dir;
      }
      mkpath $dir;
      chdir $dir;
   }
}

sub cmake {
   my ($docker)   = @_;
   my $iwyu       = '/usr/bin/iwyu';
   my $code_path  = $BASE_DIR;
   my $build_path = $BUILD_PATH;
   if ($docker) {
      $code_path  = $DOCKER_ROOT;
      $build_path = $DOCKER_BUILD_PATH;
   } else {
      $ENV{'CC'}  = '/usr/bin/clang';
      $ENV{'CXX'} = '/usr/bin/clang++';
      if ($IS_WSL) {
         $iwyu = '/usr/local/bin/include-what-you-use';
      }
   }
   workdir( $docker, $build_path );
   sys( $docker,
        'cmake',
        '-DSKIP_LINT=true',
        '-DCMAKE_CXX_INCLUDE_WHAT_YOU_USE='
            . $iwyu
            . ';-Xiwyu;any;-Xiwyu;iwyu;-Xiwyu;args',
        $code_path
      );
}

sub run_perltidy {
   my ($docker) = @_;
   workdir( $docker, 'etc/scripts' );
   sys( $docker, 'perltidy', '-pro=.perltidy', '*.pl', '*.pm' );
   unlink( glob( $BASE_DIR . '/etc/scripts/*.bak' ) );
}

sub run_iwyu {
   my ($docker) = (@_);

   cmake($docker);

# Don't capture any iwyu output for protobuf generated files, but redirect it to /dev/null to quiet things down.
   sys_io( $docker, 'make', '-j' . $opt_procs,
           'clean', 'scoreboard_proto', '2>/dev/null' );
   sys_io( $docker, 'make', '-j' . $opt_procs, 'all', '2>iwyu_data.txt' );
   return $docker;
}

sub run_fix_include {
   my ($docker) = @_;
   sys_io( $docker, 'cat', 'iwyu_data.txt', '|', '/usr/bin/fix_include',
           '--comments', '--nosafe_headers' );
}

sub run_clangformat {
   my ($docker) = @_;
   cmake($docker);
   sys( $docker, 'make', '-j' . $opt_procs, 'clangformat' );
}

sub run_mdformat {
   my ($docker) = @_;
   workdir( $docker, q{} );
   for my $file (@MARKDOWN_FILES) {
      sys( $docker, 'mdformat', '--wrap=100', $file );
   }
}

sub check_wsl {
   my $uname = `uname -a`;
   if ( $uname =~ /WSL2/ ) {
      $IS_WSL = 1;
   }
}

sub main {
   parse_options();
   check_wsl();
   status('Setting up Docker (if requested)');
   my $docker = docker();
   status('Running perltidy');
   run_perltidy($docker);
   status('Running include-what-you-use');
   run_iwyu($docker);
   status('iwyu complete, fixing includes in files');
   run_fix_include($docker);
   status('Includes fixed, auto-formatting all files');
   run_clangformat($docker);
   status('Source formatted, auto-formatting markdown files');
   run_mdformat($docker);
   status(   'Process complete.' . "\n"
           . 'Any changes will need to be submitted via git to be preserved.'
         );
}

main();


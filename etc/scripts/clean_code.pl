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

our $BASE_DIR   = Cwd::cwd();
our $BUILD_PATH = $BASE_DIR . '/out/iwyu';

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
            'help' => 'Run commands inside of the standard docker container. (Unimplemented)',
      },
);

sub sys {
   my @args = @_;
   system @args;
}

sub sys_tick {
   my @args = @_;
   push @args, '2>&1', '1>/dev/null';
   my $cmd = join ' ', @args;
   return `$cmd`;
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

sub cmake {
   mkpath $BUILD_PATH;
   chdir $BUILD_PATH;
   $ENV{'CC'}  = '/usr/bin/clang';
   $ENV{'CXX'} = '/usr/bin/clang++';
   my $iwyu = '/usr/bin/iwyu';
   if ($IS_WSL) {
      $iwyu = '/usr/local/bin/include-what-you-use';
   }
   sys( 'cmake',
        '-DSKIP_LINT=true',
        '-DCMAKE_CXX_INCLUDE_WHAT_YOU_USE="'
            . $iwyu
            . ';-Xiwyu;any;-Xiwyu;iwyu;-Xiwyu;args"',
        $BASE_DIR
      );
}

sub run_perltidy {
   chdir $BASE_DIR . '/etc/scripts';
   sys( 'perltidy', '-pro=.perltidy', '*.pl', '*.pm' );
   unlink( glob('*.bak') );
}

sub run_iwyu {
   cmake();
   return sys_tick( 'make', '-j' . $opt_procs, 'clean', 'all' );
}

sub run_fix_include {
   my (@iwyu) = @_;
   open my $proc, '|-', '/usr/bin/fix_include --comments --nosafe_headers';
   say {$proc} $_ for @iwyu;
}

sub run_clangformat {
   cmake();
   sys( 'make', '-j' . $opt_procs, 'clangformat' );
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
   status('Running perltidy');
   run_perltidy();
   status('Running include-what-you-use');
   my @iwyu = run_iwyu();
   status('iwyu complete, fixing includes in files');
   run_fix_include(@iwyu);
   status('Includes fixed, auto-formatting all files');
   run_clangformat();
   status(   'Process complete.' . "\n"
           . 'Any changes will need to be submitted via git to be preserved.'
         );
}

main();


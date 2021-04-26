#!/windows/perl/install/perl

=pod

This script manages creating a release to Github.  It's only expected to be run
by myself at the present time, so compatability with anyone else's system is
not guaranteed.

Copyright 2019-2021 Tracy Beck

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

# Sample command line:
# C:\Strawberry\perl\bin\perl.exe .\etc\scripts\release.pl -v 0.0.3

use 5.030;
use strict;

use File::Path qw{make_path rmtree};
use Getopt::Long qw{GetOptions};

use FindBin;
use lib "$FindBin::RealBin";

# Project local libraries
use GitHub;

our $TEMP_ROOT = 'C:/temp';
our $GIT_REPO = 'git@github.com:AkbarTheGreat/cszb-scoreboard.git';
our $VERSION_FILE = '/include/ScoreboardCommon.h';

# These should be the parts that are very specific to my machine
our $VCPKG_CMAKE = 'C:/Users/akbar/Software/vcpkg/scripts/buildsystems/vcpkg.cmake',
our $GIT_CMD = 'C:/Program Files/Git/cmd/git.exe';
our $CMAKE_ROOT = 'C:/PROGRAM FILES (X86)/MICROSOFT VISUAL STUDIO/2019/COMMUNITY/COMMON7/IDE/COMMONEXTENSIONS/MICROSOFT/CMAKE/CMake/bin/';
our $CMAKE_CMD = $CMAKE_ROOT . 'cmake.exe';
our $CTEST_CMD = $CMAKE_ROOT . 'ctest.exe';

my ($opt_help, $opt_version, $opt_dry_run, $opt_keep_dir, $opt_skip_git);

my $repo_path;

my %options = (
    'help|?'    => {'val'=>\$opt_help,'help'=>'This help'},
    'version=s' => {'val'=>\$opt_version,'help'=>'Version to release (required)'},
    'dry_run'   => {'val'=>\$opt_dry_run,'help'=>'Dry run only'},
    'keep_dir'  => {'val'=>\$opt_keep_dir,'help'=>'Keep the temporary directory created by this script'},
    'skip_git'  => {'val'=>\$opt_skip_git,'help'=>'Skip checking in, tagging, and pushing in git.  (Not recommended unless a second run fixing a failure.)'},
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

sub get_repo {
    die 'Incorrect number of arguments to get_repo' if (@_ != 0);
    return run_cmd($GIT_CMD, 'clone', $GIT_REPO, $repo_path);
}

sub update_version {
    die 'Incorrect number of arguments to update_version' if (@_ != 0);

    return 0 if $opt_dry_run;

    my $version_file = $repo_path . $VERSION_FILE;
    my @file_lines;
    open my $if, '<', $version_file or die 'Could not open ' . $version_file . ' for read: ' . $!;
    while (<$if>) {
        push @file_lines, $_;
    }
    close $if or die 'Broken pipe reading from ' . $version_file . ': ' . $!;

    open my $of, '>', $version_file or die 'Could not open ' . $version_file . ' for write: ' . $!;
    for (@file_lines) {
        s/(#define SCOREBOARD_VERSION ")[\w\d\.]+"/${1}${opt_version}"/;
        print {$of} $_;
	}
    close $of or die 'Broken pipe writing to ' . $version_file . ': ' . $!;
    return 0;
}

sub cmake {
    die 'Incorrect number of arguments to cmake' if (@_ != 0);

    unless ($opt_dry_run) {
        make_path($repo_path . '/out/build');
        chdir($repo_path . '/out/build');
    }

    my @cmake_args = (
        '-G', 'Visual Studio 16 2019',
        '-A', 'x64',
        '-DCMAKE_INSTALL_PREFIX:PATH="' . $repo_path . '/out/install"',
        '-DCMAKE_TOOLCHAIN_FILE="' . $VCPKG_CMAKE . q{"},
        '-DVCPKG_TARGET_TRIPLET:STRING="x64-windows-static"',
        '-DCMAKE_BUILD_TYPE:STRING="Release"',
        '-DCMAKE_CONFIGURATION_TYPES="Release"',
    );

    return run_cmd($CMAKE_CMD, @cmake_args, $repo_path);
}

sub make {
    die 'Incorrect number of arguments to make' if (@_ != 0);
    
    return run_cmd($CMAKE_CMD, '--build', '.', '--config', 'Release')
}

sub test {
    die 'Incorrect number of arguments to test' if (@_ != 0);
    
    return run_cmd($CTEST_CMD);
}

sub commit_and_tag {
    die 'Incorrect number of arguments to commit_and_tag' if (@_ != 0);
    
    my $retval = run_cmd($GIT_CMD, 'add', $repo_path . $VERSION_FILE);
    return $retval if $retval;

    my $retval = run_cmd($GIT_CMD, 'commit', '-m', 'Version auto-commit via release script');
    return $retval if $retval;

    my $retval = run_cmd($GIT_CMD, 'tag', '-a', 'release_' . $opt_version, '-m', 'Version auto-commit via release script');
    return $retval if $retval;

    return run_cmd($GIT_CMD, 'push');
}

sub main {
    parse_options();
    $repo_path = $TEMP_ROOT . '/' . $$;

    if (get_repo() != 0) {
        die 'Error cloning repo: ' . $!;
	}
    if (update_version() != 0) {
        die 'Error updating version: ' . $!;
	}
    if (cmake() != 0) {
        die 'Error running cmake: ' . $!;
	}
    if (make() != 0) {
        die 'Error building: ' . $!;
	}
    if (test() != 0) {
        die 'Error running tests: ' . $!;
	}
    unless ($opt_skip_git) {
        if (commit_and_tag() != 0) {
			die 'Error updating repo: ' . $!;
		}
    }

    my $upload_path = GitHub::create_release($opt_version);
    unless ($upload_path) {
        die 'Error creating release at Github: ' . $!;
	}
    if (GitHub::upload_binary(
        $upload_path,
        'cszb-scoreboard.exe',
        'Win64',
        $repo_path . '/out/build/Release/cszb-scoreboard.exe',
        $repo_path . '/out/build/Release/copy_test.exe') != 0) {
          die 'Error adding file to release at Github: ' . $!;
	}
    
    # Clean up now that we're done.
    unless ($opt_dry_run || $opt_keep_dir) {
        chdir($TEMP_ROOT);
        rmtree($repo_path);
    }
    say 'Release ' . $opt_version . ' created.  Available at /https://github.com/AkbarTheGreat/cszb-scoreboard/releases/tag/release_' . $opt_version;
}

main();

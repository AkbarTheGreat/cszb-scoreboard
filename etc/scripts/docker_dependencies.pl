#!/usr/bin/perl

use 5.022;
use Cwd qw(getcwd);

our $PROTOBUF_VERSION = 'v3.19.3';
our $WXWIDGETS_VERSION = 'v3.1.6-final';
our $CURL_VERSION = 'curl-7_79_0';
our $GTEST_VERSION = 'v1.13.0';

our $BASE_DIR = '/src/';

# Wrapper around system which allows us to die on any system failure easily.
sub sys {
	my (@args) = @_;
	my $retval = system @args;
	return unless $retval;
	die 'System call failed: ' . "@args\n" . '  Died with exit code: ' . $retval;
}

sub clone_repo {
	my ($subdir, $repo, $tag) = @_;
	my $dir = $BASE_DIR . $subdir;
	unless (-d $dir) {
		sys('/usr/bin/git', 'clone', $repo, $dir);
	}
	chdir $dir;
	if ($tag) {
		sys('/usr/bin/git', 'fetch', '--all', '--tags');
		sys('/usr/bin/git', 'checkout', 'tags/' . $tag);
	}
	sys('/usr/bin/git', 'submodule', 'update', '--init', '--recursive');
	chdir $ENV{'HOME'};
}

sub cmake {
	my ($subdir, @args) = @_;
	my $dir = $BASE_DIR . $subdir;
	mkdir $dir . '/out';
	chdir $dir . '/out';
	my $cmake_dir = '../';
	$cmake_dir = '../cmake' if ($subdir eq 'protobuf');
	sys('/usr/bin/cmake', $cmake_dir, @args);
	chdir $ENV{'HOME'};
}

sub build {
	my ($subdir) = @_;
	chdir $BASE_DIR . $subdir . '/out';
	sys('/usr/bin/make', '-j4', 'all');
}

sub install {
	my ($subdir) = @_;
	chdir $BASE_DIR . $subdir . '/out';
	sys('/usr/bin/make', 'install');
}

sub setup_curl {
	say 'Setting up Curl';
	clone_repo('curl', 'https://github.com/curl/curl.git', $CURL_VERSION);
	cmake('curl', '-DBUILD_SHARED_LIBS=ON');
	build('curl');
	install('curl');
}

sub setup_googletest {
	say 'Setting up Googletest';
	clone_repo('googletest', 'https://github.com/google/googletest.git', $GTEST_VERSION);
	cmake('googletest');
	build('googletest');
	install('googletest');
}

sub setup_protobuf {
	say 'Setting up Protobuf';
	clone_repo('protobuf', 'https://github.com/google/protobuf.git', $PROTOBUF_VERSION);
	cmake('protobuf');
	build('protobuf');
	install('protobuf');
}

sub setup_wxwidgets {
	say 'Setting up Wxwidgets';
	clone_repo('wxWidgets', 'https://github.com/wxWidgets/wxWidgets.git', $WXWIDGETS_VERSION);
	cmake('wxWidgets', '-DCMAKE_BUILD_TYPE=Debug', '-DwxBUILD_TOOLKIT=gtk3', '-DwxBUILD_STRIPPED_RELEASE=OFF', '-DwxUSE_WEBVIEW=ON');
	build('wxWidgets');
	install('wxWidgets');
}

sub main {
	chdir($BASE_DIR);
	setup_curl();
	setup_googletest();
	setup_wxwidgets();
	setup_protobuf();
}

main(@ARGV);


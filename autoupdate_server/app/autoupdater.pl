#!/usr/bin/perl

use 5.022;
use lib '.';
use Mojolicious::Lite -signatures;
use CszbScoreboard::UpdaterAPI;

if ($ENV{'LOCAL_TEST'}){
	plugin AccessLog => {};
} else {
	plugin AccessLog =>  {log => '/logs/access.log'};
}

my $api = CszbScoreboard::UpdaterAPI->new();

get '/' => {text => 'I ♥ Mojolicious!'};

get '/versions' => {json => $api->versions()};

app->start;

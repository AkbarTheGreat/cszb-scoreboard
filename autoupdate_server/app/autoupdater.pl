#!/usr/bin/perl

use 5.022;
use lib '.';
use Mojolicious::Lite -signatures;
use CszbScoreboard::UpdaterAPI;
use CszbScoreboard::Config;

my $config = CszbScoreboard::Config->instance();

if ($ENV{'LOCAL_TEST'}){
	plugin AccessLog => {};
} else {
	plugin AccessLog =>  {log => $config->access_log()};
}

my $api = CszbScoreboard::UpdaterAPI->new();

get '/' => {text => 'I ♥ Mojolicious!'};

get '/versions'     => sub($c) {$c->render(json => $api->dispatch($c, 'versions'))};
get '/version_info' => sub($c) {$c->render(json => $api->dispatch($c, 'version_info'))};
get '/latest'       => sub($c) {$c->render(json => $api->dispatch($c, 'latest'))};

app->start;


=pod

A library which handles communicating with GitHub for the purposes of releasing
the cszb-scoreboard.

If anyone is reading this hoping for a general purpose library, please be aware
that this is absolutely not what you're looking for.

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

package GitHub {

   use 5.030;

   use FindBin;
   use HTTP::Tiny;
   use JSON;

# This needs to be populated with a valid Github token.  Mine is not checked in for obvious reasons.
   our $GIT_TOKEN = read_token();

   sub create_release {
      die 'Incorrect number of arguments to create_release' if ( @_ != 1 );
      my ($version) = @_;

      my $json = encode_json {
                 'tag_name' => 'release_' . $version,
                 'name'     => $version,
                 'body' => "Released via script.  No better description yet.",
                 'draft'      => $JSON::true,
                 'prerelease' => $JSON::false,
      };

      my $http = HTTP::Tiny->new( default_headers => {
                                     'Authorization' => 'Token ' . $GIT_TOKEN,
                                     'Content-Type'  => 'application/json',
                                  }
                                );

      my $response
          = $http->post(
         'https://api.github.com/repos/AkbarTheGreat/cszb-scoreboard/releases'
             => { content => $json, } );

      if ( $response->{'success'} && $response->{'reason'} eq 'Created' ) {
         my $content = decode_json $response->{'content'};
         return $content->{'upload_url'};
      }
      $? = $response->{'reason'};
      return undef;
   }

   sub find_existing_release {
      die 'Incorrect number of arguments to find_existing_release'
          if ( @_ != 1 );
      my ($version) = @_;

      my $http = HTTP::Tiny->new( default_headers => {
                                     'Authorization' => 'Token ' . $GIT_TOKEN,
                                     'Content-Type'  => 'application/json',
                                  }
                                );

      my $response
          = $http->get(
         'https://api.github.com/repos/AkbarTheGreat/cszb-scoreboard/releases'
          );

      unless ( $response->{'success'} && $response->{'reason'} eq 'OK' ) {
         $? = $response->{'reason'};
         return undef;
      }

      my $content = decode_json $response->{'content'};

      for my $release ( @{$content} ) {
         if ( $release->{'name'} eq $version ) {
            return $release->{'upload_url'};
         }
      }

      return undef;
   }

   sub read_token {
      die 'Incorrect number of arguments to release_token' if ( @_ != 0 );

      my $token_file = $FindBin::RealBin . '/release_token';
      open my $fh, '<', $token_file
          or die 'Cannot open ' . $token_file . ' for read: ' . $!;
      return <$fh>;
   }

   sub upload_binary {
      die 'Incorrect number of arguments to upload_binary'
          if ( @_ < 4 || @_ > 5 );

      my ( $url, $name, $label, $origin_file, $copy_test ) = @_;

      my $binary;

      $url =~ s/\{\?name,label}/?name=${name}&label=${label}/;

      open my ($fh), '<:raw', $origin_file;
      for (<$fh>) { $binary .= $_; }
      close $fh;

# Write the binary back out (if the script asks for it) in order to prove that the binary isn't corrupted.
      if ($copy_test) {
         open my ($out_fh), '>:raw', $copy_test;
         print {$out_fh} $binary;
         close $out_fh;
      }

      my $http = HTTP::Tiny->new(
                              default_headers => {
                                 'Authorization' => 'Token ' . $GIT_TOKEN,
                                 'Content-Type' => 'application/octet-stream',
                              }
      );

      my $response = $http->post( $url => { content => $binary, } );

      if ( $response->{'success'} && $response->{'reason'} eq 'Created' ) {
         return 0;
      }
      $? = $response->{'reason'};
      return 1;
   }

}

1;

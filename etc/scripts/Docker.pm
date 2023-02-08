
=pod

A library which handles building and running commands against a Docker container.

This could possibly be useful for other folks wanting to do this sort of thing.  But also, there's probably something better on CPAN already.

Copyright 2023 Tracy Beck

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

package Docker {

   use 5.030;
   use FindBin        qw();
   use File::Basename qw(dirname);

   sub new {
      my ( $class, %args ) = @_;
      die 'Docker requires the "name" argument.' unless ( $args{'name'} );
      $args{'dockerfile'} //= 'Dockerfile';
      $args{'context'}    //= '.';
      $args{'volumes'}    //= {};

      if ( $args{'build'} ) {
         $args{'context'} = dirname( dirname($FindBin::RealBin) );
         $args{'dockerfile'}
             = $args{'context'} . '/Dockerfile.' . $args{'build'};
      }

      my $self = bless { 'name'    => $args{'name'},
                         'verbose' => $args{'verbose'},
                       }, $class;
      $self->build( $args{'dockerfile'}, $args{'context'} );
      $self->start();
      return $self;
   }

   sub log {
      my ( $self, @msg ) = @_;
      say @msg if $self->{'verbose'};
   }

   sub container {
      my ($self) = @_;
      return $self->{'name'} . '_exec';
   }

   sub build {
      my ( $self, $file, $context ) = @_;
      $self->log('Building Docker image.');
      if ( system( 'docker', 'build',         '-f', $file,
                   '-t',     $self->{'name'}, $context )
         )
      {
         die 'Error building Docker image: ' . $? . "\n";
      }
   }

   sub start {
      my ( $self, $volumes ) = @_;
      $self->log('Starting Docker container.');
      my @cmd = ( 'docker', 'run' );
      for my $src ( keys %{$volumes} ) {
         push @cmd, '-v', $src . q{:} . $volumes->{$src};

      }
      push @cmd,
          '--name',
          $self->container(),
          '-dit',
          $self->{'name'},
          '/bin/sh';
      if ( system(@cmd) ) {
         die 'Error starting Docker image: ' . $? . "\n";
      }
   }

   sub DESTROY {
      local ( $., $@, $!, $^E, $? );
      my ($self) = @_;
      $self->log('Shutting down Docker container.');
      system( 'docker', 'rm', '-f', $self->container() );
   }

   sub cmd {
      my ( $self, @cmd ) = @_;
      system( 'docker', 'exec', '-w', '/src/cszb-scoreboard', '-it',
              $self->container(), @cmd );
      return $?;
   }

}

1;

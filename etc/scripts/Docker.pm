
=pod

A library which handles building and running commands against a Docker container.

This could possibly be useful for other folks wanting to do this sort of thing.  But also, there's probably something better on CPAN already.

Copyright 2023-2025 Tracy Beck

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
   use File::Which    qw();

=pod
    Constructor arguments:

    Requred
    ------------
    name - The name of the image to build.  The container will always be named ${name}_exec

    Optional
    ------------
    build - If set, the name of the cszb_scoreboard image to load.
    dockerfile - The name of the dockerfile to load.  Only useful if build unset.  Defaults to "Dockerfile"
    context - The context to run Docker in.  Defaults to either the current directory or, in case of a build argument, the root of the repository the current script is running from.
    target - The target to build in the Dockerfile.  Ignored if build is used, if undef it builds the default target.
    skip_build - Skip building this container.  This should only be done in scripts where the container is known to be built earlier in execution.
    volumes - A map of volumes to mount at construction time.

=cut

   our @DOCKER_CMD;

   sub docker_cmd {
      if (@DOCKER_CMD) {
         return @DOCKER_CMD;
      }

      for my $cmd (qw(docker podman)) {
         if ( File::Which::which($cmd) ) {
            @DOCKER_CMD = ($cmd);
            return @DOCKER_CMD;
         }
      }
      die 'Could not find docker on your system.';
   }

   sub new {
      my ( $class, %args ) = @_;
      die 'Docker requires the "name" argument.' unless ( $args{'name'} );
      $args{'dockerfile'} //= 'Dockerfile';
      $args{'context'}    //= '.';
      $args{'volumes'}    //= {};

      my $root;
      if ( $args{'build'} ) {
         $args{'context'}    = dirname( dirname($FindBin::RealBin) );
         $args{'target'}     = $args{'build'};
         $args{'dockerfile'} = $args{'context'} . '/Dockerfile';
         $root               = '/cszb-scoreboard/';
      }
      $root //= '/';

      my $self = bless { 'name'    => $args{'name'},
                         'root'    => $root,
                         'verbose' => $args{'verbose'},
                       }, $class;
      $self->workdir(q{});
      $self->build( $args{'dockerfile'}, $args{'context'}, $args{'target'} )
          unless $args{'skip_build'};
      $self->start( $args{'volumes'} );
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
      my ( $self, $file, $context, $target ) = @_;
      $self->log('Building Docker image.');
      my @cmd = ( docker_cmd(), 'buildx', 'build' );
      push @cmd, '-q' unless $self->{'verbose'};
      if ($target) {
         push @cmd, '--target=' . $target;
      }
      push @cmd, ( '-f', $file, '-t', $self->{'name'}, $context );
      if ( system(@cmd) ) {
         die 'Error building Docker image: ' . $? . "\n";
      }
   }

   sub start {
      my ( $self, $volumes ) = @_;
      $self->log('Starting Docker container.');
      my @cmd = ( docker_cmd(), 'run' );
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

   # Sets the current working directory for commands
   sub workdir {
      my ( $self, $dir ) = @_;
      if ( $dir !~ '^/' ) {
         $dir = $self->{'root'} . $dir;
      }
      $self->{'workdir'} = $dir;
   }

   sub DESTROY {
      local ( $., $@, $!, $^E, $? );
      my ($self) = @_;
      $self->log('Shutting down Docker container.');
      system( docker_cmd(), 'rm', '-f', $self->container() );
   }

   sub cmd {
      my ( $self, @cmd ) = @_;
      system( docker_cmd(), 'exec', '-w', $self->{'workdir'}, '-it',
              $self->container(), @cmd );
      return $?;
   }
}

1;

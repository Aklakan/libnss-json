#include <unistd.h>
#include <errno.h>

int main( int argc, char ** argv, char ** envp )
{
              if( setgid(getegid()) ) perror( "setgid" );
              if( setuid(geteuid()) ) perror( "setuid" );
              envp = 0; /* blocks IFS attack on non-bash shells */
              system( "/etc/nss-json-impl", argv, envp );
              perror( argv[0] );
              return errno;
}


#include <iostream>
#include <cstdlib>
#include <cerrno>
#include <cstring>

#include <unistd.h>
#include <sys/types.h>
#include <sys/inotify.h>

#define EVENT_SIZE ( sizeof (struct inotify_event) )
#define EVENT_BUF_LEN ( 1024 * ( EVENT_SIZE + 16 ) )

int main( int argc, char* argv[] )
{
    if ( argc != 3 )
    {
        std::cerr << "Error: invalid arguments!" << std::endl;
        std::cerr.flush();
        return 1;
    }

    std::string fileName(argv[1]);
    std::string cmd(argv[2]);

    std::cout << "fmon started!" << std::endl;
    while ( 1 )
    {
        int fd = inotify_init();
        if ( fd < 0 ) 
        {
            std::cout << "inotify_init error" << std::endl;
            std::cout.flush();
            usleep(1000000); // 1s
        }
        else
        {
            int wd = inotify_add_watch( fd, fileName.c_str(), IN_OPEN );
            if ( wd < 0 )
            {
                std::cout << "inotify_add_watch error" << std::endl;
                std::cout.flush();
                usleep(1000000); // 1s
            }
            else
            {
                int length = 0;
                char buffer[EVENT_BUF_LEN];
repeat:
                length = read( fd, buffer, EVENT_BUF_LEN );
                if ( length < 0 )
                {
                    if ( errno == EINTR ) goto repeat;
                    std::cout << "read error: " << strerror(errno) << std::endl;
                    std::cout.flush();
                    usleep(1000000); // 1s
                }
                else
                {
                    int i = 0;
                    while ( i < length )
                    {
                        struct inotify_event *event = ( struct inotify_event * ) &buffer[ i ];
                        if ( event->mask & IN_OPEN )
                        {
                            std::cout << "start " << cmd << std::endl;
                            std::cout.flush();
                            int ret = system( cmd.c_str() );
                            std::cout << "finished. return code: " << ret << std::endl;
                            std::cout.flush();
                            break;
                        }
                        i += EVENT_SIZE + event->len;
                    }
                }
            }        
        }
        close( fd );
    }
}

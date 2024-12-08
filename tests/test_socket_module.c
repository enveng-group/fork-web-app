#include "../include/socket_module.h"
#include <assert.h>
#include <stdio.h>

void
test_socket_creation ()
{
    Socket *sock = create_socket ();
    assert (sock != NULL);
    close_socket (sock);
}

void
test_socket_connection ()
{
    Socket *sock = create_socket ();
    int result = connect_socket (sock, "127.0.0.1", 8080);
    assert (result == 0);
    close_socket (sock);
}

void
test_socket_send_receive ()
{
    Socket *sock = create_socket ();
    connect_socket (sock, "127.0.0.1", 8080);
    const char *message = "Hello";
    send_message (sock, message);
    char buffer[1024];
    receive_message (sock, buffer, sizeof (buffer));
    assert (strcmp (buffer, message) == 0);
    close_socket (sock);
}

int
main ()
{
    test_socket_creation ();
    test_socket_connection ();
    test_socket_send_receive ();
    printf ("All tests passed!\n");
    return 0;
}

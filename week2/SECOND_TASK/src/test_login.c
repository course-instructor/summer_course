// test_login.c
#include <stdio.h>
#include <string.h>
#include "s_handle_request.h"
#include "s_rooms.h"

int main(void) {
    // 1) Prepare a fake client object
    client_s fake_client = { .status = NOT_CONNECTED };
    
    // 2) Call handle_login()
    const char *user = "alice";
    const char *pass = "secret123";
    int result = handle_login(&fake_client, user, pass);
    
    // 3) Observe and report
    if (result == CONNECTED) {
        printf("✅ handle_login: SUCCESS (client->status=%d)\n", fake_client.status);
    } else {
        printf("❌ handle_login: FAILURE (returned %d, status=%d)\n",
               result, fake_client.status);
    }
    return 0;
}

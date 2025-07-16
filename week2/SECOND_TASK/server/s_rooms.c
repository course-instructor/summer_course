typsef struct 
{
    int sockfd;
    enum connection_status status;
    char * name;
    int room_num = -1;
    room_s room;
} * client_s;

typdef struct 
{
    client  top;
    client * next;
} * node;


typdef struct 
{
    node clients;
} * room_s;

node g_clients = malloc(sizeof(node));
g_clients -> top = NULL;
g_clients -> next = NULL;


room [ROOM_COUNT] g_rooms ;
for(int i = 0; i < ROOM_COUNT; i++)
{
    rooms[i] = malloc(sizeof(room));
    rooms[i] -> clients = NULL;
}




void add_client(client_list_t head, client_ptr_t new_client)
{
    client_list_t current = head;
    while (current -> next)
    {
        if (current -> next -> client && current -> next -> client -> sockfd == new_client -> sockfd) 
        {
            printf("Server: Client %d already in list.\n", new_client -> sockfd);
            return;
        }
        current = current->next;
    }

    client_list_t new_node = (client_list_t) malloc (sizeof(struct clients_node));
    if (!new_node)
    {
        perror("malloc add_client node");
        return;
    }
    new_node -> client = new_client; 
    new_node -> next = NULL;

    current -> next = new_node; 
    printf("Server: Client %d added to list.\n", new_client -> sockfd);
}

int rem_client(client_list_t head, client_ptr_t  remove_client)
{
    int ret = 0;

    client_list_t current = head;
    client_list_t prev = NULL;

    if(head -> client == remove_client)
    {
        head = head -> next;
        free(current);
    }

    while (current) 
    {
        if (current -> client == remove_client)
        {
            prev -> next = current -> next;
            free(current);
            ret = 1; // remove_client was found...
            break;
        }
        prev = current;
        current = current -> next;
    }
    return(ret);
}

void broadcast(client_list_t head, client_ptr_t ignore, const char *payload)
{
    pthread_mutex_lock(& clients_mutex);
    char sender[INET6_ADDRSTRLEN];
    inet_ntop(ignore -> addr.ss_family, get_in_addr((struct sockaddr*) & ignore -> addr), sender, sizeof sender);

    char out[1200];
    int len = snprintf(out, sizeof out, "%s: %s\n", sender, payload);

    for (client_list_t cur = head -> next; cur; cur = cur->next)
    {
        if (cur -> client != ignore)
        {
            send(cur -> client -> sockfd, out, len, 0);
        }
    }
    pthread_mutex_unlock(& clients_mutex);
    fflush(stdout);
}

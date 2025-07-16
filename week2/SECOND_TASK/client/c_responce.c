void handle_message(char * buf, int num)
{
    int i = 0;
    switch (num)
    {
        case (SIGN_UP_RESPONSE):
            if(buf[0] == 0) //success
            {
                printf("signed up sucssesfully\n");
            }

            else//failed (-1)
            {
                printf("signed up un-sucssesfully\n");
            }
            break;

        case (LOG_IN_RESPONSE):
            if(buf[i] == '0') //success
            {
                pthread_mutex_lock(& g_level);
                printf("loged in sucssesfully\n");
                g_level = LOGGED_IN;
                pthread_mutex_unlock(& g_level);
            }

            else//failed (-1)
            {
                printf("loged in un-sucssesfully\n");
            }
            break;

        case (LIST_OF_ROOMS_RESPONSE):
            if(buf[i] != "-1") //success
            {
                printf("availble rooms: \n");
                int len;
                while (1)
                {
                    printf("room: %s\n", buf);
                    len = strlen(buf);

                    if(buf[len - 1] == '\n') //if last char in the param is \n then its the end...
                    {
                        break;
                    }

                    buf += len; //next param

                }
                printf("\n\n");
            }
            else // failed
            {
                printf("get rooms unsuccessfull: %s", buf[i]);
            }
            break;
        
        case (ENTER_ROOM_RESPONSE):
            if(buf[i] == '0') //success
            {
                pthread_mutex_lock(& g_level);
                printf("entered room sucssesfully\n");
                g_level = IN_ROOM;
                pthread_mutex_unlock(& g_level);
            }

            else//failed (-1)
            {
                printf("entered room un-sucssesfully\n");
            }
            break;

        case (MESSAGE_FROM_CLIENT):            
            char * name;
            char * message;

            get_param(buf, name);
            get_param(buf, message);

            printf("%s: %s\n", name, message);
            break;

        case (MESSAGE_FROM_SERVER):
            printf("server: %s\n" , buf);
            break;

        case (EXIT_ROOM_RESPONSE):
            if(buf[i] == '0') //success
            {
                pthread_mutex_lock(& g_level);
                printf("loged in sucssesfully\n");
                g_level = LOGGED_IN;
                pthread_mutex_unlock(& g_level);
            }

            else//failed (-1)
            {
                printf("loged in un-sucssesfully\n");
            }
            break;

        default:
            perror("invalid message number\n");
            break;
    }
}


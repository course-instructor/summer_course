#include <stdio.h>
#include <termios.h>
#include "../include/my_sniffer.h"

#define START_KEY   's'
#define KILL_KEY    'k'
#define INSPECT_KEY 'i'
#define ERASE_KEY   'e'
#define DUMP_KEY    'd'
#define HELP_KEY    'h'

#define ESCAPE_KEY 27

extern uint64_t packet_count;
extern bool is_sniffing;

static struct termios oldt;

void input_listener(void)
{
    int ch;
    pthread_t sniffer_thread;

    input_terminal_raw_mode_enable();
    printf("(press h for help!!)\n");


    while((ch = getchar())) 
    {
        if (ch < 0) {
            if (ferror(stdin)) 
            { 
                break;
                /* there was an error... */ 
            }
            clearerr(stdin);
        } 
        else 
        {
            switch (ch)
            {
            case START_KEY:
                input_start(&sniffer_thread);
                break;

            case KILL_KEY:
                input_kill(&sniffer_thread);
                break;
            
            case INSPECT_KEY:
                input_inspect();
                break;

            case ERASE_KEY:
                input_erase();
                break;
            case DUMP_KEY:
                input_dump();
                break;
            case HELP_KEY:
                input_help();
                break;
            
            default:
                break;
            }

        }
    }
}

void input_help(void)
{
    printf("\n*** Sniffer Program Commands***\n\n");
    printf("[%c] Shows this page\n"             ,HELP_KEY);
    printf("[%c] Starts sniffing\n"             ,START_KEY);
    printf("[%c] Kill the sniffing process\n"   ,KILL_KEY);
    printf("[%c] Inspect a specific packet\n"   ,INSPECT_KEY);
    printf("[%c] Dump the data into a file\n"   ,DUMP_KEY);
    printf("[%c] Erase the unsaved data\n"      ,ERASE_KEY);
    printf("\n*******************************\n\n");

}
void input_terminal_raw_mode_enable(void)
{
    struct termios newt;

    /*tcgetattr gets the parameters of the current terminal
    STDIN_FILENO will tell tcgetattr that it should write the settings
    of stdin to oldt*/
    tcgetattr( STDIN_FILENO, &oldt);
    /*now the settings will be copied*/
    newt = oldt;

    /*ICANON normally takes care that one line at a time will be processed
    that means it will return if it sees a "\n" or an EOF or an EOL*/
    newt.c_lflag &= ~(ICANON | ECHO);          

    /*Those new settings will be set to STDIN
    TCSANOW tells tcsetattr to change attributes immediately. */
    tcsetattr( STDIN_FILENO, TCSANOW, &newt);
    
}

void input_terminal_raw_mode_disable(void)
{
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);  /*Restore original settings*/ 
}

void input_start(pthread_t * sniffer_thread)
{
    if(is_sniffing == false)
    {
        is_sniffing = true;
        pthread_create(sniffer_thread,NULL,(void*)sniffer_start,NULL);
        printf("Starting sniffer...\n");

    }
    else
    {
        printf("Sniffer is alredy runnning.\n");

    }

}
void input_kill(pthread_t * sniffer_thread)
{
    if(is_sniffing == true)
    {
        is_sniffing = false; /*Tell thread to finish after next recive*/
        printf("Stopping sniffer...\n");
        close(5);
        pthread_join(*sniffer_thread,NULL); /*wait until thread finished*/
        printf("Done sniffing\n");

    }
    else
    {
        printf("Sniffer is not runnning currently.\n");
    }

}

void input_inspect(void)
{

    status input_status = SUCCESS;
    FILE * tmp_file = NULL;
    FILE * offset_file = NULL;
    bool is_inspect_mode = true;
    uint64_t input_id;

    if(is_sniffing == true)
    {
        printf("[Can't inspect while still sniffing (press [%c] to stop sniffing)]\n",KILL_KEY);
    }
    else if((tmp_file = fopen(TEMPORARY_FILE_PATH,"rb")) == NULL)
    {
        perror("Error opening file");
    }
    else if((offset_file = fopen(OFFSET_FILE_PATH,"rb")) == NULL)
    {
        perror("Error opening file");

    }
    else
    {

        printf("Plase enter an id of a packt you would like to inspect. (Press ESC to exit INSPECT MODE)\n");
        while(is_inspect_mode == true)
        {
            printf("[INSPECT MODE] ");
            input_status = input_get_id(&input_id);
            if(input_status == FAILURE)
            {
                printf("inavild input\n");

            }
            else if(input_status == DISCONNECTED)
            {
                printf("Exiting Inspect Mode...\n");
                is_inspect_mode = false;

            }
            else if(input_status == SUCCESS)
            {
                if(sniffer_read_packet(input_id,tmp_file,offset_file,stdout)== FAILURE)
                {
                    fprintf(stderr,"\nInavlid ID : %ld invalid range\n",input_id);
                }    
            }
        }

    }
    if(tmp_file != NULL)
    {
        fclose(tmp_file);
    }
    if(offset_file != NULL)
    {
        fclose(offset_file);
    }


}
void input_erase(void)
{   

    packet_count = 0;

    fclose(fopen(OFFSET_FILE_PATH, "wb"));
    fclose(fopen(TEMPORARY_FILE_PATH, "wb"));
    printf("Data Erased\n");
    


    


}

void input_dump(void)
{
    FILE * tmp_file;
    FILE * offset_file;
    FILE * dump_file;
    char dump_file_path[FILENAME_MAX] = {0};

    time_t rawtime;
    struct tm * timeinfo;
    uint64_t count_id = 0;

    if(packet_count == 0) /* Check if there any packets in temporery file*/
    {
        printf("No data to save.\n");
    }
    else
    {
        /* get time */
        time ( &rawtime );
        timeinfo = localtime ( &rawtime );
        /*Format dump file name*/
        strftime(dump_file_path, FILENAME_MAX, "./saves/my_sniffer_(%Y-%m-%d)_(%H:%M:%S).txt", timeinfo);

        if((tmp_file = fopen(TEMPORARY_FILE_PATH,"rb")) == NULL)
        {
            perror("Error opening tmp file");
        }
        else if((offset_file = fopen(OFFSET_FILE_PATH,"rb")) == NULL)
        {
            perror("Error opening offset file");
        }
        else if((dump_file = fopen(dump_file_path,"w")) == NULL)
        {
            perror("Error opening dump file");
        }
        else
        {
            while(sniffer_read_packet(count_id,tmp_file,offset_file,dump_file) == SUCCESS)
            {
                count_id++;
            }
            printf("Data saved at [%s].\n",dump_file_path);
        }
        fclose(tmp_file);
        fclose(offset_file);
        fclose(dump_file);
        
    }
    

}


status input_get_id(uint64_t * ptr_input_id)
{
    char buffer[BUFSIZ] = {0};
    char *endptr;
    char ch;
    int offset = 0;
    status input_status = SUCCESS;



    while(input_status != DISCONNECTED && (ch = fgetc(stdin)) != '\n')
    {
        
        if(ch == ESCAPE_KEY) /* For exiting from inspect mode even mid writing an input, there is a small downside tho, I cant use backspace*/
        {
            input_status = DISCONNECTED;
        }
        else
        {
            printf("%c",ch);
            buffer[offset] = ch;
            offset++;
        }

    }
    if(input_status == SUCCESS)
    {
        *ptr_input_id = strtol(buffer, &endptr, 10);
        if (endptr == buffer)
        {
            // no character was read
            printf("No input\n");
            input_status = FAILURE;
        }
        if (*endptr && *endptr != '\n')
        {
            /* *endptr is neither end of string nor newline,
            so we didn't convert the *whole* input*/
            printf("input is not an integer\n");
            input_status = FAILURE;
        }

    }
    return input_status;
    

}
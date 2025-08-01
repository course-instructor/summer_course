#include <stdio.h>
#include "my_sniffer.h"

#define START_KEY   's'
#define KILL_KEY    'k'
#define INSPECT_KEY 'i'
#define ERASE_KEY   'e'
#define DUMP_KEY    'd'
#define HELP_KEY    'h'
#define ESCAPE_KEY  27

#define HELP_MESSAGE              "(press h for help!!)"
#define MSG_STARTING_SNIFFER      "Starting sniffer...\n"
#define MSG_ALREADY_RUNNING       "Sniffer is already running.\n"
#define MSG_STOPPING_SNIFFER      "Stopping sniffer...\n"
#define MSG_DONE_SNIFFING         "Done sniffing\n"
#define MSG_NOT_RUNNING           "Sniffer is not running currently.\n"
#define MSG_CANT_INSPECT_RUNNING  "[Can't inspect while still sniffing (press [k] to stop sniffing)]\n"
#define MSG_ENTER_ID              "Please enter an id of a packet you would like to inspect. (Press ESC to exit INSPECT MODE)\n"
#define MSG_INSPECT_MODE          "[INSPECT MODE] "
#define MSG_INVALID_INPUT         "Invalid input\n"
#define MSG_EXITING_INSPECT       "Exiting Inspect Mode...\n"
#define MSG_INVALID_ID_FMT        "\nInvalid ID : %ld invalid range\n"
#define MSG_DATA_ERASED           "Data Erased\n"
#define MSG_NO_DATA_TO_SAVE       "No data to save.\n"
#define MSG_DATA_SAVED_FMT        "Data saved at [%s].\n"

#define MSG_ERR_OPENING_FILE      "Error opening file"
#define MSG_ERR_OPEN_TMP_FILE     "Error opening tmp file"
#define MSG_ERR_OPEN_OFFSET_FILE  "Error opening offset file"
#define MSG_ERR_OPEN_DUMP_FILE    "Error opening dump file"

#define DUMP_FILENAME_FORMAT      "./saves/my_sniffer_(%Y-%m-%d)_(%H:%M:%S).txt"

extern uint64_t packet_count;
extern bool is_sniffing;

static struct termios oldt;

void input_listener(void)
{
    int ch;
    pthread_t sniffer_thread;

    input_terminal_raw_mode_enable();
    printf("%s\n", HELP_MESSAGE);

    while ((ch = getchar()))
    {
        if (ch < 0)
        {
            if (ferror(stdin))
                break;
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

    input_terminal_raw_mode_disable();
}

void input_help(void)
{
    printf("\n*** Sniffer Program Commands***\n\n");
    printf("[%c] Shows this page\n", HELP_KEY);
    printf("[%c] Starts sniffing\n", START_KEY);
    printf("[%c] Kill the sniffing process\n", KILL_KEY);
    printf("[%c] Inspect a specific packet\n", INSPECT_KEY);
    printf("[%c] Dump the data into a file\n", DUMP_KEY);
    printf("[%c] Erase the unsaved data\n", ERASE_KEY);
    printf("\n*******************************\n\n");
}

void input_terminal_raw_mode_enable(void)
{
    struct termios newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
}

void input_terminal_raw_mode_disable(void)
{
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
}

void input_start(pthread_t *sniffer_thread)
{
    if (!is_sniffing)
    {
        is_sniffing = true;
        pthread_create(sniffer_thread, NULL, (void *)sniffer_start, NULL);
        printf(MSG_STARTING_SNIFFER);
    }
    else
    {
        printf(MSG_ALREADY_RUNNING);
    }
}

void input_kill(pthread_t *sniffer_thread)
{
    if (is_sniffing)
    {
        is_sniffing = false;
        printf(MSG_STOPPING_SNIFFER);
        close(5);
        pthread_join(*sniffer_thread, NULL);
        printf(MSG_DONE_SNIFFING);
    }
    else
    {
        printf(MSG_NOT_RUNNING);
    }
}

void input_inspect(void)
{
    status input_status = SUCCESS;
    FILE *tmp_file = NULL;
    FILE *offset_file = NULL;
    bool is_inspect_mode = true;
    uint64_t input_id;

    if (is_sniffing)
    {
        printf(MSG_CANT_INSPECT_RUNNING);
    }
    else if ((tmp_file = fopen(TEMPORARY_FILE_PATH, "rb")) == NULL)
    {
        perror(MSG_ERR_OPENING_FILE);
    }
    else if ((offset_file = fopen(OFFSET_FILE_PATH, "rb")) == NULL)
    {
        perror(MSG_ERR_OPENING_FILE);
    }
    else
    {
        printf(MSG_ENTER_ID);
        while (is_inspect_mode)
        {
            printf(MSG_INSPECT_MODE);
            input_status = input_get_id(&input_id);

            if (input_status == FAILURE)
            {
                printf(MSG_INVALID_INPUT);
            }
            else if (input_status == DISCONNECTED)
            {
                printf(MSG_EXITING_INSPECT);
                is_inspect_mode = false;
            }
            else if (input_status == SUCCESS)
            {
                if (sniffer_read_packet(input_id, tmp_file, offset_file, stdout) == FAILURE)
                {
                    fprintf(stderr, MSG_INVALID_ID_FMT, input_id);
                }
            }
        }
    }

    if (tmp_file != NULL) fclose(tmp_file);
    if (offset_file != NULL) fclose(offset_file);
}

void input_erase(void)
{
    packet_count = 0;
    fclose(fopen(OFFSET_FILE_PATH, "wb"));
    fclose(fopen(TEMPORARY_FILE_PATH, "wb"));
    printf(MSG_DATA_ERASED);
}

void input_dump(void)
{
    FILE *tmp_file;
    FILE *offset_file;
    FILE *dump_file;
    char dump_file_path[FILENAME_MAX] = {0};
    time_t rawtime;
    struct tm *timeinfo;
    uint64_t count_id = 0;

    if (packet_count == 0)
    {
        printf(MSG_NO_DATA_TO_SAVE);
    }
    else
    {
        time(&rawtime);
        timeinfo = localtime(&rawtime);
        strftime(dump_file_path, FILENAME_MAX, DUMP_FILENAME_FORMAT, timeinfo);

        if ((tmp_file = fopen(TEMPORARY_FILE_PATH, "rb")) == NULL)
        {
            perror(MSG_ERR_OPEN_TMP_FILE);
        }
        else if ((offset_file = fopen(OFFSET_FILE_PATH, "rb")) == NULL)
        {
            perror(MSG_ERR_OPEN_OFFSET_FILE);
        }
        else if ((dump_file = fopen(dump_file_path, "w")) == NULL)
        {
            perror(MSG_ERR_OPEN_DUMP_FILE);
        }
        else
        {
            while (sniffer_read_packet(count_id, tmp_file, offset_file, dump_file) == SUCCESS)
            {
                count_id++;
            }
            printf(MSG_DATA_SAVED_FMT, dump_file_path);
        }

        if (tmp_file) fclose(tmp_file);
        if (offset_file) fclose(offset_file);
        if (dump_file) fclose(dump_file);
    }
}

status input_get_id(uint64_t *ptr_input_id)
{
    char buffer[BUFSIZ] = {0};
    char *endptr;
    char ch;
    int offset = 0;
    status input_status = SUCCESS;

    while (input_status != DISCONNECTED && (ch = fgetc(stdin)) != '\n')
    {
        if (ch == ESCAPE_KEY)
        {
            input_status = DISCONNECTED;
        }
        else
        {
            printf("%c", ch);
            buffer[offset++] = ch;
        }
    }

    if (input_status == SUCCESS)
    {
        *ptr_input_id = strtol(buffer, &endptr, 10);
        if (endptr == buffer)
        {
            printf("No input\n");
            input_status = FAILURE;
        }
        else if (*endptr && *endptr != '\n')
        {
            printf("input is not an integer\n");
            input_status = FAILURE;
        }
    }

    return input_status;
}

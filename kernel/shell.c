#include <kernel.h>
#define UP_ARROW    1
#define LEFT_ARROW  2
#define RIGHT_ARROW 3
#define DOWN_ARROW  4
#define TAB_SIZE 4

#define MAX_COMMANDS 20
#define buffer_inp_MAX_LENGTH 160
#define SHELL_HISTORY_SIZE 20
#define TRUE 1
#define FALSE 0
#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 25
#define WINDOW_OFFSET(wnd_inp, x_inp, y_inp) (((wnd_inp)->y + (y_inp)) * SCREEN_WIDTH + ((wnd_inp)->x + (x_inp)))
#define MAX(a,b) (((a)>(b))? (a):(b))

typedef struct _command
{
    char *name;
    int id;
} command;

typedef struct _buffer_inp
{
    BOOL used;
    int length;
    char buffer[buffer_inp_MAX_LENGTH + 1];
} buffer_inp;

typedef struct _hist_buffer
{
    int len;
    char *arg_array[buffer_inp_MAX_LENGTH];
    buffer_inp in_buf;
} hist_buffer;

hist_buffer arg;
command shell_cmd[MAX_COMMANDS + 1];
buffer_inp history[SHELL_HISTORY_SIZE];
buffer_inp *history_head; // always points to a new buffer
buffer_inp *current_hist; // always points to the buffer displayed on screen
/*************************************************************/
static WINDOW shell_window_def = {0, 10, 60, 14, 0, 0, '_'};
WINDOW* shell_wnd = &shell_window_def;
/*************for debug ********************/
static WINDOW shell_window_def2 = {0, 1, 60, 14, 0, 0, '>'};
WINDOW* shell_wnd2 = &shell_window_def2;
/*******************************************/
static WINDOW train_window=  {0, 0, 80, 10, 0, 0, ' '};

void win_printfc(WINDOW *wnd,char c);

typedef struct _buffer_window
{
    unsigned char buffer[2 * SCREEN_WIDTH * SCREEN_HEIGHT];
} buffer_window;

buffer_window kernel_buffer_window;

unsigned char peek_byte(int offset)
{
    return peek_b((MEM_ADDR)((WORD*)kernel_buffer_window.buffer + offset));
}
void cmd_init(char *name, command *cmd,int id)
{
    cmd->name = name;
    cmd->id = id;
}
int str_to_int(const char* str)
{
    int num = 0;
    BOOL found_num = FALSE;
    while (*str != '\0')
    {
        if (*str >= '0' && *str <= '9')
        {
            num = num * 10 + *str - '0';
            found_num = TRUE;
        }//wait for a   string
        else if (!found_num && (*str == ' ' || *str == '\t'))
        {
            continue;
        }
        else
        {
            break;
        }
        str++;
    }
    return num;
}

int k_strcmp(const char* str1, const char* str2)
{
    int i;
    while(*str1 != '\0' && *str2 != '\0')
    {
        i = *str1++ - *str2++;
        if (i != 0) return i;
    }
    return *str1 - *str2;
}
void *set_memory(void *dst, int c, int len)
{
    char* mem = (char*)dst;
    const char* end = mem + len;
    while(mem < end) *mem++ = (char)c;
    return dst;
}
void empty_buffer(buffer_inp *in_buf)
{
    set_memory(in_buf->buffer, 0, buffer_inp_MAX_LENGTH + 1);
  
    in_buf->length = 0;
    in_buf->used = FALSE;
}
int check_for_white_space(buffer_inp *in_buf)
{
    int i = 0;
    int j = 0;
    int in_arg = FALSE;

    while (j < in_buf->length)
    {
        if (in_buf->buffer[j] == ' ' ||
                in_buf->buffer[j] == '\t' ||
                in_buf->buffer[j] == '\n' ||
                in_buf->buffer[j] == '\r' ||
                in_buf->buffer[i] == '\0')
        {
            // skip white space
            if (in_arg == TRUE)
            {
                in_buf->buffer[i++] = '\0';
            }
            in_arg = FALSE;
            j++;
        }
        else
        {
            in_buf->buffer[i++] = in_buf->buffer[j++];
            in_arg = TRUE;
        }
    }
    in_buf->buffer[i] = '\0';
    in_buf->length = i;
    return i;
}

// prints characters or removes characters if backspace was pressed
void wnd_printf(WINDOW *wnd, char c)
{
    if (c == '\b')
    {
        remove_char(wnd);
    }
    else
    {
        if (c == '\0') c = ' ';
        wprintf(wnd, "%c", c);
    }
}
 //This function prints Author information
 void print_about()
 {
     wprintf(shell_wnd,"This shell is created by Anoja Rajalakshmi \n");
 }

 //This function prints all commands implemented by author
 void print_help()
 {
     //wprintf(shell_wnd2,"Welcome to HELP\n");
     wprintf(shell_wnd,"Following are the shell commands you may use:\n");
     wprintf(shell_wnd,"NOTE: Type the commands or Use UP and DOWN Arrow keys\n");
     wprintf(shell_wnd,"help                  - Displays help information\n");
     wprintf(shell_wnd,"about                 - Author information\n");
     wprintf(shell_wnd,"clear                 - Clear window\n");
     wprintf(shell_wnd,"echo                  - Echo the string passed\n");
     wprintf(shell_wnd,"procs                 - Lists all processes\n");
     wprintf(shell_wnd,"tstart                - Start the train\n");
   //  wprintf(shell_wnd,"tstop                 - Stop the train\n");
     wprintf(shell_wnd,"sleep <duration>      - Sleep for a duration\n");
   
 }

void echo_shell()
{
    int i = 1;
    while (i < arg.len)
        wprintf(shell_wnd, "%s ", arg.arg_array[i++]);
    if (arg.len > 1)
        wprintf(shell_wnd, "\n");
  
}
// causes the shell process to sleep for arg_array[1] amount of time
void sleep_shell(/*int len, char **arg_array*/)
{
    if (arg.len > 1)
    {
        wprintf(shell_wnd, "Sleeping: ");
        sleep(str_to_int(arg.arg_array[1]));
        wprintf(shell_wnd, "woke after %d\n", str_to_int(arg.arg_array[1]));
    }
    else
    {
        wprintf(shell_wnd, "Usage: sleep num_of_ticks\n");
    }
}

void train_func()
{
    //wprintf(shell_wnd,"The Train commands are under construction!\n");
	init_train(&train_window);
}
void remove_char(WINDOW* wnd)
{
    volatile int check;
    DISABLE_INTR(check);

    unsigned char c;

    if (wnd->cursor_x != 0)
    {
        // removing within a single line
        c = peek_byte(WINDOW_OFFSET(wnd, wnd->cursor_x - 1, wnd->cursor_y));
        // remove single character
        remove_cursor(wnd);
        move_cursor(wnd, wnd->cursor_x - 1, wnd->cursor_y);

        if (c == '\t')
        {
            c = peek_byte(WINDOW_OFFSET(wnd, wnd->cursor_x - 1, wnd->cursor_y));
            // removing tabs
            while (c == '\t' && wnd->cursor_x % TAB_SIZE != 0)
            {
                remove_cursor(wnd);
                move_cursor(wnd, wnd->cursor_x - 1, wnd->cursor_y);
                c = peek_byte(WINDOW_OFFSET(wnd, wnd->cursor_x - 1, wnd->cursor_y));
            }
        }
    }
    else if (wnd->cursor_y > 0)
    {
        // removing on previous line if it exists
        c = peek_byte(WINDOW_OFFSET(wnd, wnd->width - 1, wnd->cursor_y - 1));
        // move cursor to end of new line, removing single character
        remove_cursor(wnd);
        move_cursor(wnd, wnd->width - 1, wnd->cursor_y - 1);

        if (c == '\n' || c == '\r')
        {
            c = peek_byte(WINDOW_OFFSET(wnd, wnd->cursor_x - 1, wnd->cursor_y));
            // removing newlines
            while ((c == '\n' || c == '\r') && wnd->cursor_x > 0)
            {
                remove_cursor(wnd);
                move_cursor(wnd, wnd->cursor_x - 1, wnd->cursor_y);
                c = peek_byte(WINDOW_OFFSET(wnd, wnd->cursor_x - 1, wnd->cursor_y));
            }
        }
        else if (c == '\t')
        {
            c = peek_byte(WINDOW_OFFSET(wnd, wnd->cursor_x - 1, wnd->cursor_y));
            // removing tabs
            while (c == '\t' && wnd->cursor_x % TAB_SIZE != 0)
            {
                remove_cursor(wnd);
                move_cursor(wnd, wnd->cursor_x - 1, wnd->cursor_y);
                c = peek_byte(WINDOW_OFFSET(wnd, wnd->cursor_x - 1, wnd->cursor_y));
            }
        }
    }

    show_cursor(wnd);

    ENABLE_INTR(check);
}
void history_up()
{
    int i;
    buffer_inp *next = history + ((current_hist - history - 1) % SHELL_HISTORY_SIZE);
    if (next->used && next != history_head)
    {
        // if next buffer used and not back to head

        // clear screen of current data
        for(i = 0; i < current_hist->length; i++)
        {
            wnd_printf(shell_wnd, '\b');
        }

        // add new data
        current_hist = next;
        for(i = 0; i < current_hist->length; i++)
        {
            wnd_printf(shell_wnd, current_hist->buffer[i]);
        }
    }
}

void arrow_down()
{
    int i;
    buffer_inp *previous = history + ((current_hist - history + 1) % SHELL_HISTORY_SIZE);
    if (current_hist != history_head)
    {
        // if not back to head

        // clear screen of current data
        for(i = 0; i < current_hist->length; i++)
        {
            wnd_printf(shell_wnd, '\b');
        }

        // add new data
        current_hist = previous;
        for(i = 0; i < current_hist->length; i++)
        {
            wnd_printf(shell_wnd, current_hist->buffer[i]);
        }
    }
}

void refresh_hist()
{
    buffer_inp *history_prev = history + (history_head - history - 1)  % SHELL_HISTORY_SIZE;
    // copy current to head if necessary
    if (history_head != current_hist)
    {
        *history_head = *current_hist;
    }

    if (history_prev->used == TRUE)
    {
        if (k_memcmp(history_prev->buffer, history_head->buffer, MAX(history_prev->length, history_head->length)) == 0)
        {
            // previous same as head
            empty_buffer(history_head);
        }
        else
        {
            // previous different from head, move head
            history_head = history + ((history_head - history + 1) % SHELL_HISTORY_SIZE);
        }
    }
    else
    {
        // no history, move head
        history_head = history + ((history_head - history + 1) % SHELL_HISTORY_SIZE);
    }
    // set current to head
    current_hist = history_head;
}
void setup_args(buffer_inp* in_buf, hist_buffer *arg)
{
    int i, j;
    int in_arg = FALSE;
    arg->in_buf = *in_buf;

    for (i = j = 0; i < arg->in_buf.length; i++)
    {
        if (arg->in_buf.buffer[i] != '\0')
        {
            // reading through part of an argument, add to arg_array if this is the first char
            if (in_arg == FALSE)
            {
                arg->arg_array[j++] = (char *)&arg->in_buf.buffer[i];
                in_arg = TRUE;
            }
        }
        else
        {
            // found a null char, no longer in an argument
            in_arg = FALSE;
        }
    }
    arg->len = j;
}
void clear_args(hist_buffer *arg)
{
    set_memory(&arg->arg_array[0], 0, buffer_inp_MAX_LENGTH*sizeof(char *));
}

void execute_command(int id)
{
    switch(id)
    {
        case 1:
                    print_help();break;
        case 2:
                    print_all_processes(shell_wnd);break;
        case 3:
                    sleep_shell();break;
        case 4:
               	    clear_window(shell_wnd);break;
        case 5:
                    echo_shell();break;
        case 6:
                    train_func();break;
        case 7:
                    print_about();break;
	case 8: 
		    /*remove_ready_queue(active_proc);  //this function has to be executed ib ttrain.c
   		    resign();*/  //but  should be invoked from here
		    break;
        default:
                    wprintf(shell_wnd,"Unknown Command: %s \n",current_hist->buffer);break;
    }
}
void shell_process(PROCESS proc, PARAM param)
{
    //command *cmd;
    clear_window(kernel_window);
    wprintf(shell_wnd, "************** Welcome to TOS shell ***************\n");
    //wprintf(shell_wnd, "Type help for more information!\n");
    command *current_cmd;
    while(TRUE)
    {
            wprintf(shell_wnd, "$");
            empty_buffer(current_hist);
            /********** receive input ***************/
            char c;
            Keyb_Message msg;
            msg.key_buffer = &c;

            current_hist->used = TRUE;

            do
            {
                    // get new character
                send(keyb_port, (void *)&msg);
                if (c == '\b' && current_hist->length > 0)
                {
                    // backspace received and can backspace
                    current_hist->buffer[--current_hist->length] = '\0';
                    wnd_printf(shell_wnd, c);
                }
                else if (((c >= 32 && c < 127) || c == '\t')
                        && current_hist->length < buffer_inp_MAX_LENGTH)
                {
                    // writable caracter received and can print
                    current_hist->buffer[current_hist->length++] = c;
                    wnd_printf(shell_wnd, c);
                }
                else
                {
                    // non-writable
                    switch (c)
                    {
                        case UP_ARROW:
                            history_up();
                            break;
                        case DOWN_ARROW:
                            arrow_down();
                            break;
                    }
                }
            }
  
            while(c != '\n' && c != '\r');

            wprintf(shell_wnd, "\n");

            current_hist->buffer[current_hist->length] = '\0';
        /****************************************/
        check_for_white_space(current_hist); 
      
        for (current_cmd=0; current_cmd->id != 99; current_cmd++)
        {
            if (k_strcmp(current_cmd->name, current_hist->buffer) == 0)
            {
                break;
            }
        }
        wprintf(shell_wnd2,"%d",current_cmd->id);
        if (current_hist->length <= 0)
        {
            // empty line, don't print
            continue;
        }
        else if (current_cmd->id == 99)
        {
            wprintf(shell_wnd, "Unknown Command: %s\n", current_hist->buffer);
        }
        else
        {
            clear_args(&arg);
            setup_args(current_hist, &arg);
            execute_command(current_cmd->id/*,arg.len,&arg.arg_array[0]*/);//,current_hist->buffer);
        }
        refresh_hist();
      
    }
}
void init_shell()
{
    int i = 0;

    // init used commands
    cmd_init("help",&shell_cmd[i++], i);
    cmd_init("procs",&shell_cmd[i++],i);
    cmd_init("sleep",&shell_cmd[i++],i);
    cmd_init("clear",&shell_cmd[i++],i);
    cmd_init("echo",&shell_cmd[i++], i);
    cmd_init("tstart",&shell_cmd[i++],i);
    cmd_init("about",&shell_cmd[i++],i);
   // cmd_init("tstop",&shell_cmd[i++],i);
    //cmd_init("tstop",&shell_cmd[i++],i);

    // init unused commands
    while (i < MAX_COMMANDS)
    {
        cmd_init("----",&shell_cmd[i++],i);
    }
    shell_cmd[MAX_COMMANDS].name = "NULL";
    shell_cmd[MAX_COMMANDS].id = 99;

    // init shell command history
    for (i = 0; i < SHELL_HISTORY_SIZE; i++)
    {
        empty_buffer(history + i);
    }
    history_head = history;
    current_hist = history;

    create_process (shell_process, 3, 0, "Shell process");

    resign();

    //init_train(train_wnd);
}





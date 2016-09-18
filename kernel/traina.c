#include <kernel.h>
#define CR "\015"
//#define L "L\0"
//#define NUL  '\0'
#define TRAIN_NUMBER "20\0"
#define NO_VEHICLE 0
#define FOUND_VEHICLE 1
//#define CHANGE_DIR "L20D\0"
static WINDOW train_window=  {0, 0, 80, 10, 0, 0, ' '};

int CONFIG_NO=0;
int ZAM_DIR_LEFT =0;
int ZAM_DIR_RIGHT=0;
int ZAM=0;

void run_config1_without_zam();//Running config 1 and 2 without Zamboni
void run_config3_without_zam();//Running config 3 without Zamboni
void run_config4_without_zam();//Running config 4 without Zamboni
void run_config1_with_zam();// Running config 1 with Zamboni
void run_config2_with_zam();//Running config 2 with Zamboni
void run_config3_with_zam();// Running config 3 with Zamboni
void run_config4_with_zam();//Running config 4 with Zamboni 
/*
typedef struct _COM_Message
{
 char* output_buffer;
 char* input_buffer;
 int len_input_buffer;
} COM_Message;
*/
COM_Message msg;
void send_to_train(char* command)
{
   // strcat(command,CR);
    int len=0;
    char inp[10];
    msg.output_buffer =command;
    msg.input_buffer=inp; 
    msg.len_input_buffer=len;
    //wprintf(&train_window,"%s",command);
    sleep(10);
    send(com_port,&msg);
}

int strlen( char *str )
{
    char *start;
    start = str;
    while( *str != 0 )
    {
        ++str;
    }
    return str - start;
}
void append(char* s, char c)
{
    int len = strlen(s);
    s[len] = c;
    s[len+1] = '\0';
}

/*
void com1_example ()
{
 char buffer [12]; // 12 == strlen ("Hello World!")
 COM_Message msg;
 int i;
 msg.output_buffer = "Hello World!";
 msg.input_buffer = buffer;
 msg.len_input_buffer = 12;
 send (com_port, &msg);
 for (i = 0; i < 12; i++)
 kprintf (“%c”, buffer[i]);
}
*/
void strcat(char* dst, char* src)
{
    while (*dst!='\0')
        dst++;
    while (*src!='\0')
    {
        *dst = *src;
        dst++;
        src++;
    }
    *dst='\0';
}

//L20D\015”
void change_direction()
{
    char message[10];
    append(message,'L');
    strcat(message,TRAIN_NUMBER);
    append(message,'D');
    strcat(message,CR);
}

void change_switch(char switchno,char color)
{
    char message[10];
    append(message,'M');
    append(message,switchno);
    append(message,color);
    strcat(message,CR);
    send_to_train(message);
}
//L#S#CR
void change_speed(char speed)
{
    char message[10];
    append(message,'L');
    strcat(message,TRAIN_NUMBER);
    append(message,'S');
    append(message,speed);
    strcat(message,CR);
    send_to_train(message);
}
void clear_memory_buffer()
{
    char message[5];
    append(message,'R');
    strcat(message,CR);
    send_to_train(message);
}
//Get the status of a contact.“*1\015” is returned if there is a vehicle on the contact.
//Otherwise, “*0\015” is returned.
int switch_status(char switch_no)
{
    clear_memory_buffer();
    char message[10];
    append(message,'C');
    append(message,switch_no);
    strcat(message,CR);
    msg.output_buffer = message;
    send(com_port,&msg);
    if(msg.input_buffer[1]=='1')
        return NO_VEHICLE;
    else
        return FOUND_VEHICLE;

}

void run_config1_without_zam()
{
    change_speed('4');
    change_switch('4','R');
    change_switch('3','G');
    while(switch_status('1'));
    change_speed('0');
    change_direction();
    change_speed('4');
    change_switch('5','R');
    change_switch('6','R');
    while(switch_status('8'));
    change_speed('0');
}
void run_config3_without_zam()
{
    change_switch(3, 'R');
    change_switch(4, 'R');
    change_switch(5, 'R');
    change_switch(6, 'G');
    change_switch(7, 'G');
    change_speed('4');

    while(switch_status('12'));
    change_speed('0');
    change_direction();

    change_switch(7, 'R');
    change_switch(8, 'R');
    change_speed('4');

    while(switch_status('13'));
    change_speed('0');
    change_direction();

    change_switch(8, 'G');
    change_speed('4');

    while(switch_status('5'));
    change_speed('0');

}
void run_config4_without_zam()
{
    change_switch(3, 'R');
    change_switch(4, 'R');
    change_switch(5, 'G');
    change_switch(8, 'G');
    change_switch(9, 'G');
    change_speed('4');

    while(switch_status('14'));
    sleep(50);

    change_speed('0');
    change_direction();
    change_speed('4');

    while(switch_status('7'));
    change_speed('0');

    change_switch(5, 'R');
    change_switch(6, 'R');

    change_direction();
    change_speed('4');

    while(switch_status('8'));
    sleep(50);
    change_speed('0');
}
void start_train()
{
    switch(CONFIG_NO)
    {
        case 1: 
        case 2:run_config1_without_zam();
                wprintf(&train_window,"Running config 1/2 without Zamboni\n");break;
        case 3:run_config3_without_zam();
                wprintf(&train_window,"Running config 3 without Zamboni\n");break;
        case 4:run_config4_without_zam();
                wprintf(&train_window,"Running config 4 without Zamboni\n");break;
        case 5://run_config1_with_zam();
                wprintf(&train_window,"Running config 1 with Zamboni\n");break;
        case 6://run_config2_with_zam();
                wprintf(&train_window,"Running config 2 with Zamboni\n");break;
        case 7://run_config3_with_zam();
                wprintf(&train_window,"Running config 3 with Zamboni\n");break;
        case 8://run_config4_with_zam();
                wprintf(&train_window,"Running config 4 with Zamboni\n");break; 
    }
            
}
void check_for_zamboni()
{
    if(switch_status('4'))
    {    
        ZAM=TRUE;
        if(switch_status('7'))
        ZAM_DIR_LEFT=1;
        else
        ZAM_DIR_RIGHT=1;
    }
    else
    ZAM=FALSE;
    
}
void switch_initialization()
{
    //M3G, M6G, M9R, M5G, M8G, M1G, M4G 
    change_switch('3','G');
    change_switch('6','G');
    change_switch('9','R');
    change_switch('5','G');
    change_switch('8','G');
    change_switch('1','G');
    change_switch('4','G');
}

void check_for_config()
{
    if( switch_status('8')&& switch_status('2'))
    {
        CONFIG_NO=1;
        if(ZAM && ZAM_DIR_LEFT)
        CONFIG_NO=5; //config 1 with zamboni
        else if(ZAM && ZAM_DIR_RIGHT)
        CONFIG_NO=6; //config 2 with zamboni
    }

    else if( switch_status('5') && switch_status('11'))
    {
        CONFIG_NO=3;
        if(ZAM)
        CONFIG_NO=7; //config 3 with zamboni
    }
    else if( switch_status('5')&& switch_status('16'))
    {    
        CONFIG_NO=4;
        if(ZAM)
        CONFIG_NO=8; //config 4 with zamboni
    }
    else
    CONFIG_NO=0;
    
        wprintf(&train_window,"Configuration is %d\n",CONFIG_NO);
    
}
void Train_process(PROCESS self, PARAM param)
{
    clear_window(&train_window);
    wprintf(&train_window,"Starting Train Application\n");
    wprintf(&train_window,"Initializing Switches\n");
    switch_initialization();
    wprintf(&train_window,"Looking for Zamboni\n");
    check_for_zamboni();
    wprintf(&train_window,"Analyzing configuration\n");
    check_for_config();
    //start_train();
    //change_speed('4');
    while(1);
}



void init_train(WINDOW* wnd)
{
    //
   // wprintf(&train_window,"init");
        create_process(Train_process, 3, 0, "Train Process");
        //resign();
}



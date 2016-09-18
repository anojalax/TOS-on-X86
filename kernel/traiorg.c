
#include <kernel.h>

#define TRAIN_NUMBER "20\0"

static WINDOW train_window=  {0, 0, 80, 10, 0, 0, ' '};
int sleep_time = 10;
int config =0;
int running =0;//This is used to disable the use of Train go and Train Stop inside shell if the main App is running
int dead_end_wait = 600;//This is the amount of ticks to sleep for the Dead-End Track. - Varies on diffirent Machines

void clear_memory_buffer(void);
void init_switches(void);
void strcat(char*,char*);
void send_to_train(char*);
void setSwitch(char*,char*);
void change_direction(void);
void change_speed(char*);
int get_switch_status(char*);
//void debug_command(void);
void config1(void);
COM_Message msg;
int zam_found=0;
int zam_dir=0;

void strcat(char* s1, char* s2){
    while (*s1!='\0') s1++;
    while (*s2!='\0') {
        *s1 = *s2;
        s1++;
        s2++;
    }
    *s1='\0';

}

//Sends the Command to the Train
void send_to_train(char* string)
{
    int len;
    char command[15];
    char inp[10];
    command[0] = '\0';
    strcat(command,string);
    strcat(command,"\015");
    msg.output_buffer =command;
    msg.len_input_buffer = len;
    msg.input_buffer = inp;
    sleep(sleep_time);
    send(com_port,&msg);

}

//M#x{CR}
void setSwitch(char* switch_no, char* color)
{
    char message[5];
    message[0] = 'M';
    message[1] = switch_no;
    message[2] = color;
    message[3] = '\0';
    send_to_train(message);
}

//Initializing all the switches for Zamboni.
 
void init_switches(){
    setSwitch('5','G');
    setSwitch('8','G');
    setSwitch('9','R');
    setSwitch('1','G');
    setSwitch('4','G');
}

//L#D{CR}
void change_direction(){
    char message[5];

    message[0] = 'L';
    message[1] = '\0';
    strcat(message,TRAIN_NUMBER);
    message[3] = 'D';
    message[4] = '\0';
    send_to_train(message);
}

//L#S#{CR}
void change_speed(char* s){
    char message[6];

    message[0] = 'L';
    message[1] = '\0';
    strcat(message,TRAIN_NUMBER);
    message[3] = 'S';
    message[4] = s;
    message[5] = '\0';
    send_to_train(message);
}

//Clear the s88 memory buffer.
//R{CR} 

void clear_memory_buffer(){
    char message[5];

    message[0] = 'R';
    message[1] = '\0';
    send_to_train(message);
}
//Get the status of a contact.“*1\015” is returned if there is a vehicle on the contact.
//Otherwise, “*0\015” is returned.

int get_switch_status(char* c)
{
    char message[10];
    char* input;
    int len=3;
    clear_memory_buffer();
	
	/************  probe message  *************/
    message[0] = 'C';
    message[1] = '\0';
    strcat(message,c);
    strcat(message,"\015");
	/*****************************************/
    
    msg.output_buffer = message;
    msg.len_input_buffer = len;
    msg.input_buffer = input;
    sleep(sleep_time);  // wait before sending message to train
    send(com_port,&msg);
    if(msg.input_buffer[1]=='1') 
		return 1;
    else 
		return 0;
}
//Running config 1 and 2 without Zamboni
void run_config1_without_zam()  
{
    change_speed('4');
    setSwitch('4','R');
    setSwitch('3','G');
    while(get_switch_status("1")==0);
    change_speed('0');
    change_direction();
    change_speed('4');
    setSwitch('5','R');
    setSwitch('6','R');
    while(get_switch_status("8")==0);
    change_speed('0');
    wprintf(&train_window,"Home :)\n");
}
//Running config 3 without Zamboni
void run_config3_without_zam()
{
    change_speed('4');
    //setSwitch('3','R');
    setSwitch('5','R');
    setSwitch('6','G');
    setSwitch('7','R');
    while(!get_switch_status("12"));
    change_speed('0');
    change_direction();
    change_speed('4');
    while(!get_switch_status("13"));
    change_speed('0');
    setSwitch('8','G');
    change_direction();
    change_speed('4');
    setSwitch('5','G');
    setSwitch('4','R');
    setSwitch('3','R');
    while(!get_switch_status("5"));
    change_speed('0');
    wprintf(&train_window,"Home :)\n");
    }
////Running config 4 without Zamboni
void run_config4_without_zam(){

    change_speed('4');
    setSwitch('5','G');
    setSwitch('8','G');
    setSwitch('9','G');
    while(!get_switch_status("14"));
    change_speed('0');
    change_speed('4');
    while(!get_switch_status("16"));
    sleep(dead_end_wait);
    change_speed('0');

    change_direction();
    change_speed('4');
    setSwitch('4','R');
    setSwitch('3','R');
    while(!get_switch_status("5"));
    change_speed('0');
    wprintf(&train_window,"Home :)\n");
}

// Running config 1 with Zamboni
void run_config1_with_zam(){
    wprintf(&train_window,"Config 1 With Zamboni\n");
    setSwitch('7','R');
    setSwitch('2','R');
    while(!get_switch_status("10"));
    setSwitch('1','R');
    run_config1_without_zam();
}

//Running config 2 with Zamboni
void run_config2_with_zam(){
    wprintf(&train_window,"Config 2 With Zamboni\n");
    setSwitch('7','R');
    setSwitch('2','R');
    while(!get_switch_status("3"));
    setSwitch('8','R');
    setSwitch('1','R');
    run_config1_without_zam();
}


// Running config 3 with Zamboni
void run_config3_with_zam(){
    wprintf(&train_window,"Config 3 With Zamboni\n");
    setSwitch('4','R');
    setSwitch('3','R');
    while(!get_switch_status("10"));
    change_speed('5');
    setSwitch('5','R');
    setSwitch('6','G');
    while(!get_switch_status("9"));
    setSwitch('5','G');
    while(!get_switch_status("12"));
    change_speed('0');
    change_direction();
    setSwitch('7','R');
    
    while(!get_switch_status("13"));
    
    setSwitch('2','R');
    change_speed('5');

    while(!get_switch_status("3"));
    setSwitch('1','R');
    
    while(!get_switch_status("12"));
    change_speed('0');
    setSwitch('1','G');
    setSwitch('7','G');
    change_speed('4');
    while(!get_switch_status("9"));
    change_speed('0');

    //trapping zamboni
    setSwitch('7','R');
    setSwitch('2','R');
    while(!get_switch_status("10"));
    setSwitch('1','R');

    //moving to home
    
    change_speed('4');
    setSwitch('4','R');
    setSwitch('3','R');
    while(!get_switch_status("5"));
    change_speed('0');
    wprintf(&train_window,"Home :)\n");
}

//Running config 4 with Zamboni
void run_config4_with_zam(){
    wprintf(&train_window,"Config 4 With Zamboni\n");

    while(!get_switch_status("14"));
    setSwitch('8','R');
    change_speed('4');
    while(!get_switch_status("6"));
    change_speed('0');
    change_direction();
    setSwitch('4','R');
    setSwitch('3','G');
    change_speed('4');

    while(!get_switch_status("1"));
    change_speed('0');

    while(!get_switch_status("12"));
    setSwitch('8','G');
    while(!get_switch_status("13"));
    change_speed('5');
    setSwitch('4','G');
    setSwitch('9','G');
    while(!get_switch_status("14"));
    change_speed('0');
    change_direction();
    
    change_speed('4');
    while(!get_switch_status("16"));
 
    sleep(dead_end_wait);
   
    change_speed('0');
    setSwitch('3','R');
    setSwitch('9','R');
    while(!get_switch_status("13"));
    change_direction();


    change_speed('4');
    while(!get_switch_status("14"));

    change_speed('0');
    change_direction();

    
    change_speed('5');
    setSwitch('2','G');
    setSwitch('1','R');
    while(!get_switch_status("1"));
    change_speed('0');
    
    while(!get_switch_status("14"));

    setSwitch('8','R');


    change_speed('4');
    while(!get_switch_status("6"));

    change_speed('0');

    setSwitch('4','R');
    setSwitch('3','R');

    change_direction();
    change_speed('4');
    while(!get_switch_status("5"));

    change_speed('0');


    wprintf(&train_window,"Home :)\n");
}


/**
 * Checks for Zamboni
 * @return [returns the switch at which zamboni was found]
 */
int check_zamboni()
{
    int i;
   // int found=0;
	zam_found=0; zam_dir=0;
    for(i=0;i<50;i++)
	{
        sleep(sleep_time);
	wprintf(&train_window,".");
        if(get_switch_status("4"))
	    {
                zam_found = TRUE;
		wprintf(&train_window,"\nZamboni found\n");
                for(i=0;i<50 && (zam_dir==0);i++)
		{
		if(get_switch_status("7"))
			{
				sleep(sleep_time);
				return zam_dir=7;

			}
				
		else if(get_switch_status("13"))
			{
				sleep(sleep_time);
				return zam_dir=13;
			}
		}
            }
	//else
	         
	
        }
	if(!zam_found)
	wprintf(&train_window,"\nZamboni not found :(\n");
	return 0;
}

int check_config()
{
    if((get_switch_status("8"))&&(get_switch_status("2")))
	{
		if(zam_found && zam_dir == 7)
			config=5; //Running config 1 with Zamboni
		else if(zam_found && zam_dir == 13)
			config=6; //Running config 2 with Zamboni
		else
			config=1; //Running config 1 and 2 without Zamboni
   	 }
	else if((get_switch_status("5"))&&(get_switch_status("11")))
	{
		if(zam_found)
			config=7; // Running config 3 with Zamboni
		else
			config=3; //Running config 3 without Zamboni
		
    	}
	else if((get_switch_status("5"))&&(get_switch_status("16")))
	{
		if(zam_found)
			config=8;  //Running config 4 with Zamboni
		else
			config= 4;  //Running config 4 without Zamboni
        }
        else 
	{
   	     config=0;
        }

        return config;
}

void train_process(PROCESS self, PARAM param)
{
    clear_window(&train_window);
    wprintf(&train_window,"Starting Train Application\n");
    wprintf(&train_window,"Initializing all Switches\n");
    wprintf(&train_window,"Looking for Zamboni\n");
    int config=0;

    init_switches();
    clear_memory_buffer();
    check_zamboni();
    config=check_config();
    
    switch(config)
    {
        case 0:
            wprintf(&train_window,"Config not zam_found\n");
            break;
        case 1:
            wprintf(&train_window,"Running config 1 and 2 without Zamboni\n");
            run_config1_without_zam();
            break;
        case 3:
            wprintf(&train_window,"Running config 3 without Zamboni\n");
            run_config3_without_zam();
            break;
        case 4:
            wprintf(&train_window,"Running config 4 without Zamboni\n");
            run_config4_without_zam();
            break;
	case 5:
	    wprintf(&train_window,"Zamboni running Clockwise\n"); 
            wprintf(&train_window,"Running config 1 with Zamboni\n");
            run_config1_with_zam();
            break;
        case 6:
	    wprintf(&train_window,"Zamboni running Anti-clockwise\n"); 
            wprintf(&train_window,"Running config 2 with Zamboni\n");
            run_config2_with_zam();
            break;
        case 7:
            wprintf(&train_window,"Running config 3 with Zamboni\n");
            run_config3_with_zam();
            break;
	case 8:
            wprintf(&train_window,"Running config 4 with Zamboni\n");
            run_config4_with_zam();
            break;
    }

    wprintf(&train_window,"Shutting down Train App\n");
    running=0;
    remove_ready_queue(active_proc);
    resign();
    while(1);
}

void init_train(WINDOW* wnd)
{
    create_process(train_process, 4, 0, "Train Process");
}

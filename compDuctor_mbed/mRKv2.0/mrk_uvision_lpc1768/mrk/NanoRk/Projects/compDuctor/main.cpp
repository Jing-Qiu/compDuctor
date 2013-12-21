#include "mbed.h"                 
#include <nrk.h>
#include <include.h>
#include <ulib.h>
#include <stdio.h>
#include <hal.h>
#include <nrk_error.h>
#include <nrk_timer.h>
#include <nrk_stack_check.h>
#include <nrk_stats.h>
#include <string>
#include <cctype>
#include <algorithm>    // std::copy
#include <vector>       // std::vector

/* Fredy Monterroza
	 Ye Yang
	 Jing Qiu
	 ESE 519: CompDuctor
	 main.cpp
*/



/* Left Hand = servo/leftFinger (p21, p16)
	 Middle Hand = servo2/middleFinger (p22, p17)
	 Right Hand = servo3/rightFinger (p23, p18)
*/

Serial bt(p9,p10);   //tx, rx      Bluetooth read in   //can also be USBSerial

PwmOut servo(p21);
PwmOut servo2(p22); //24 good on , p22 doesn't work on m125 mBed to drive servo, but can get the right waveform on oscilloscope
PwmOut servo3(p23);
DigitalOut leftFinger(p14); 
DigitalOut middleFinger(p15);
DigitalOut rightFinger(p16);

int i = 0;
int j = 0;
int iM = 0;
int jM = 0;
int iR = 0;
int jR = 0;

//float shiftAngleByDuty = 0.00011666;

float initialPWM = .000830;
//char keyboardIn[2];
char keyboardIn;

int keyCount = 0; //Do Re ...Ti 4% to 11% OR FIND APPROPRIATE LEFT RIGHT OR RIGHT LEFT
float keyPWMVal[12];

Timer tim;

//Task Control (Locks) Variables
bool turnPermit = 1;
bool taskFlag1 = 1;
bool taskFlag2 = 1;
bool taskFlag3 = 1;
bool taskFlag4 = 1;

//bool turnPermitM = 1;
bool taskFlag1M = 1;
bool taskFlag2M = 1;
bool taskFlag3M = 1;
bool taskFlag4M = 1;

//bool turnPermitR = 1;
bool taskFlag1R = 1;
bool taskFlag2R = 1;
bool taskFlag3R = 1;
bool taskFlag4R = 1;



// Variables to ensure timing of music and physical constraints of servos. 
int pressingTime;
int depressingTime;
int startTurningTime;
int lastTurningTime;
int endTurningTime;

int pressingTimeM;
int depressingTimeM;
int startTurningTimeM;
int lastTurningTimeM;
int endTurningTimeM;

int pressingTimeR;
int depressingTimeR;
int startTurningTimeR;
int lastTurningTimeR;
int endTurningTimeR;


NRK_STK Stack1[NRK_APP_STACKSIZE];
nrk_task_type TaskOne;
void Task1(void);

NRK_STK Stack2[NRK_APP_STACKSIZE];
nrk_task_type TaskTwo;
void Task2 (void);

NRK_STK Stack3[NRK_APP_STACKSIZE];
nrk_task_type TaskThree;
void Task3 (void);

NRK_STK Stack4[NRK_APP_STACKSIZE];
nrk_task_type TaskFour;
void Task4 (void);

/*nrk_task_type RX_TASK;
NRK_STK rx_task_stack[NRK_APP_STACKSIZE];
void rx_task (void);

nrk_task_type TX_TASK;
NRK_STK tx_task_stack[NRK_APP_STACKSIZE];
void tx_task (void);



char tx_buf[RF_MAX_PAYLOAD_SIZE];
char rx_buf[RF_MAX_PAYLOAD_SIZE];


// You do not need to modify this function
struct __FILE { int handle; };
*/

void nrk_create_taskset();


int register_1 = 0;
int register_2 = 0;

//Marry Had a Little Lamb  
//#define mhallSize 26*3
#define mhallSizeL 1
#define mhallSizeM (14*3 )+ 1
#define mhallSizeR (13*3) + 1
//float mhallFile[mhallSize];
			float mhallFileL[1] = {11}; 
			float mhallFileM[mhallSizeM] = {12,1,0, 10,1,.5, 8,1,.5, 10,4,.5, 10,1,.5, 10,1,.5, 10,5,.5, 10,1,.5, 8,1,.5, 10,5,.5, 10,1,.5, 10,2,.5, 10,1, .5, 8,1,.5, 11};    
			float mhallFileR[mhallSizeR] = {2,4,.5, 2,1,.5, 2,1,.5, 2,4,.5, 2,1,.5, 5,1,.5, 5,1,.5, 2,4,.5, 2,1,.5, 2,1,.5, 2,1,.5, 2,3,.5, 2,1,.5, 11};  	  


//Jingle Bells
#define jblSize (9*3) + 1
#define jbmSize (16*3) + 1
#define jbrSize (40*3) + 1

			 float jingleBellsL[jblSize] =  {12,8,.5, 12,6,.5, 8,10,.5, 5,7,.5, 12,8,.5, 12,6,.5, 8,8,.5, 5,4,.5, 10,2,2, 11};
			 float jingleBellsM[jbmSize] =  {12,4,0, 3,6,.5, 8,1,.5, 10,1,.5, 3,7,.5, 3,6,.5, 10,1,.5, 10,2,.5, 10,7,.5, 3,6,.5, 8,1,.5, 10,1,.5, 3,6,.5, 3,7,.5, 10,1,.5, 8,2,2, 11};
	     float jingleBellsR[jbrSize] =  {2,1,.5, 2,1,.5, 2,2,.5, 2,1,.5, 2,1,.5, 2,2,.5, 2,1,.5, 5,3,.5, 2,2,.5, 3,1,.5, 3,1,.5, 3,2,.5, 3,1,.5, 3,1,.5,
																				2,1,.5, 2,2,.5, 2,1,.5, 2,3,.5, 2,3,.5, 5,1,.5, 2,1,.5, 2,1,.5, 2,2,.5, 2,1,.5, 2,1,.5, 2,2,.5, 2,1,.5, 5,3,.5,
																				2,2,.5, 3,1,.5, 3,1,.5, 3,1,.5, 3,1,.5, 3,1,.5, 2,1,.5, 2,1,.5, 2,1,.5, 5,1,.5, 5,1,.5, 3,1,.5, 11};
//Cloud Atlas
#define calSize (23*3) + 1
#define camSize (24*3) + 1
#define carSize (24*3) + 1
				 
			 float cloudAtlasL[calSize] = {6,2,.5, 3,3,.5, 10,2,.5, 1,2,.5, 6,4,.5, 10,2,.5, 8,2,.5, 10,2,.5, 3,2,.5, 10,2,.5, 1,2,.5,
																		 3,2,.5, 10,2,.5, 8,2,.5, 10,4,.5, 10,3,.5, 6,3,.5, 3,1,.5, 10,5,.5, 1,3,.5, 6,3,.5, 12,2,.5, 8,1,.5, 11};
			 float cloudAtlasM[camSize] = {3,1,.5, 8,2,.5, 8,3,.5, 7,4,.5, 8,4,.5, 8,2,.5, 5,1,.5, 3,1,.5, 8,4,.5, 7,6,.5,   
																			8,7,.5, 3,7,.5, 3,2,.5, 10,1,.5, 8,1,.5, 7,1,.5, 3,1,.5, 8,1,.5, 7,1,.5, 5,1,.5, 10,1,.5, 8,1,.5, 7,2,.5, 5,1,.5,11};
			 float cloudAtlasR[carSize] = {12,5,.5, 5,1,.5, 5,3,.5, 5,1,.5, 5,3,.5, 5,1,.5, 5,7,.5, 5,1,.5, 5,3,.5,
																	   5,1,.5, 5,2,.5, 5,3,.5, 7,1,.5, 5,1,.5, 13,1,.5, 10,1,.5, 9,1,.5, 5,1,.5, 10,1,.5, 9,1,.5, 7,1,.5, 13,1,.5, 10,1,.5, 7,1,.5, 11}; 
				 
class SongLib {
	public:
		struct Song {
			
			char *name;
			int songlSize;
			int songmSize;
			int songrSize;
			float *songlFile;
			float *songmFile;
			float *songrFile;
			
		};	
		
		Song *song_list;
		int size;
		
		SongLib(){};
		SongLib(int s) {
			size = s;
			song_list = (Song*)malloc(sizeof(Song)*size);
			//default_song();
		}
		void default_song(){
			
			size = 3;
			song_list = (Song*)malloc(sizeof(Song)*size);
			
			song_list[0].name = (char*)malloc(100);
			strcpy(song_list[0].name, "Mary Had a Little Lamb!");
			song_list[0].songlSize = mhallSizeL;
			song_list[0].songmSize = mhallSizeM;
			song_list[0].songrSize = mhallSizeR;
			song_list[0].songlFile = (float*)malloc(sizeof(float)*song_list[0].songlSize);
			song_list[0].songmFile = (float*)malloc(sizeof(float)*song_list[0].songmSize);
			song_list[0].songrFile = (float*)malloc(sizeof(float)*song_list[0].songrSize);

			for(int i = 0; i < mhallSizeL; i++) 
				song_list[0].songlFile[i] = mhallFileL[i];
			
			for(int i = 0; i < mhallSizeM; i++) 
				song_list[0].songmFile[i] = mhallFileM[i];
			
			for(int i = 0; i < mhallSizeR; i++) 
				song_list[0].songrFile[i] = mhallFileR[i];
			
			
			song_list[1].name = (char*)malloc(100);
			strcpy(song_list[1].name, "Jingle Bell!");
			song_list[1].songlSize = jblSize;
			song_list[1].songmSize = jbmSize;
			song_list[1].songrSize = jbrSize;
			song_list[1].songlFile = (float*)malloc(sizeof(float)*song_list[1].songlSize);
			song_list[1].songmFile = (float*)malloc(sizeof(float)*song_list[1].songmSize);
			song_list[1].songrFile = (float*)malloc(sizeof(float)*song_list[1].songrSize);
			
			for(int i = 0; i < jblSize; i++) 
				song_list[1].songlFile[i] = jingleBellsL[i];
			
			for(int i = 0; i < jbmSize; i++) 
				song_list[1].songmFile[i] = jingleBellsM[i];
			
			for(int i = 0; i < jbrSize; i++) 
				song_list[1].songrFile[i] = jingleBellsR[i];
				
				
			song_list[2].name = (char*)malloc(100);
			strcpy(song_list[2].name, "Cloud Atlas!");
			song_list[2].songlSize = calSize;
			song_list[2].songmSize = camSize;
			song_list[2].songrSize = carSize;
			song_list[2].songlFile = (float*)malloc(sizeof(float)*song_list[2].songlSize);
			song_list[2].songmFile = (float*)malloc(sizeof(float)*song_list[2].songmSize);
			song_list[2].songrFile = (float*)malloc(sizeof(float)*song_list[2].songrSize);
			
			for(int i = 0; i < calSize; i++) 
				song_list[2].songlFile[i] = cloudAtlasL[i];
			
			for(int i = 0; i < camSize; i++) 
				song_list[2].songmFile[i] = cloudAtlasM[i];
			
			for(int i = 0; i < carSize; i++) 
				song_list[2].songrFile[i] = cloudAtlasR[i];
		}

		void initial_lib(char *buffer, int i);
		
		void printSong(int j) {
			
			printf("Song Number: #%d\r\n", j);
			printf("Song name %s\r\n", song_list[j].name);
			
			printf("Left Hand: \r\n");
			for(int i = 0; i < song_list[j].songlSize; i++) 
				printf(" %f", song_list[j].songlFile[i]);
			
			printf("\r\nMiddle Hand: \r\n");
			for(int i = 0; i < song_list[j].songmSize; i++) 
				printf(" %f", song_list[j].songmFile[i]);
			
			printf("\r\nRight Hand: \r\n");
			for(int i = 0; i < song_list[j].songrSize; i++) 
				printf(" %f", song_list[j].songrFile[i]);
			printf("\r\n");
		}
		
		void printLibrary() {
			
			printf("Song Content: \r\n");
			for(int song = 0; song < size; song++) {
				
				printSong(song);
				
			}
			
		}
		
		void copyFile(int j, float *filel, float *filem, float *filer) {
			
			for(int i = 0; i < song_list[j].songlSize; i++) 
				filel[i] = song_list[j].songlFile[i];
			
			for(int i = 0; i < song_list[j].songmSize; i++) 
				filem[i] = song_list[j].songmFile[i];
			
			for(int i = 0; i < song_list[j].songrSize; i++) 
				filer[i] = song_list[j].songrFile[i];
			
		}
};
//Little Dummer Boy

//Heart and Soul

//Cloud Atlas Sextet

/*
float file[jblSize];// = {6,6,.5};// = {6,6,.5, 6,5,.5, 8,9,.5, 5,6,.5, 6,6,.5, 6,5,.5, 8,7,.5, 5,1,.5, 10,5,3};     
float fileM[mhallSizeM] = {12,1,0, 10,1,.5, 8,1,.5, 10,3,.5, 10,1,.5, 10,1,.5, 10,4,.5, 10,1,.5, 8,1,.5, 10,5,.5, 10,3,.5, 10,1,.5, 8,1,.5}; 
float fileR[mhallSizeR] = {2,4,.5, 2,1,.5, 2,1,.5, 2,4,.5, 2,1,.5, 5,1,.5, 5,1,.5, 2,4,.5, 2,1,.5, 2,1,.5, 2,1,.5, 2,2,.5, 2,1,.5, 2,1,.5}; 
*/	

float file[(40*3) + 1];// = {6,6,.5};// = {6,6,.5, 6,5,.5, 8,9,.5, 5,6,.5, 6,6,.5, 6,5,.5, 8,7,.5, 5,1,.5, 10,5,3};     
float fileM[(40*3) + 1];// = {12,1,0, 10,1,.5, 8,1,.5, 10,3,.5, 10,1,.5, 10,1,.5, 10,4,.5, 10,1,.5, 8,1,.5, 10,5,.5, 10,3,.5, 10,1,.5, 8,1,.5}; 
float fileR[(40*3) + 1];// = {2,4,.5, 2,1,.5, 2,1,.5, 2,4,.5, 2,1,.5, 5,1,.5, 5,1,.5, 2,4,.5, 2,1,.5, 2,1,.5, 2,1,.5, 2,2,.5, 2,1,.5, 2,1,.5};

int fileSize = 0;
int fileMSize = 0;
int fileRSize = 0;


void calibrateHand(void);
void setServos(void);
void pressKey(void);
void depressKey(void);
void setNextKey(void);

	
bool musicSelectionOver = 0;
void musicSelection(bool);    // Selecting music
                              //true for read in the keyboard or bluetooth number
															//false for read the global number current_song
bool updateLib();	

static SongLib song_lib;

static int current_song = 0;
int nextSongNumber();

//bool variable type for pause_flag, play_flag, stop_flag, next_flag
static bool pause_flag = false;
static bool play_flag = false;
static bool stop_flag = false;
static bool next_flag = false;

InterruptIn pause_button(p5);
InterruptIn play_button(p6);
InterruptIn stop_button(p7);
InterruptIn next_button(p8);

void pause_press() {
	  
	  printf("Pause botton being pressed\r\n");
    pause_flag = true;
	  
}
void play_press() {
	
	printf("Play botton being pressed\r\n");
    pause_flag = false;
		stop_flag = false;
		//next_flag = false;
	  
	  play_flag = true;
}
void stop_press() {
	printf("Stop botton being pressed\r\n");
	play_flag = false;
  stop_flag = true;
}
void next_press() {
	
	printf("Next botton being pressed\r\n");
	play_flag = true;
  next_flag = true;
}

int main(void)

  {
      pause_button.rise(&pause_press);
			play_button.rise(&play_press);
			stop_button.rise(&stop_press);
			next_button.rise(&next_press);
			/*
			servo.period(0.02); 
			servo2.period(0.02);
			servo3.period(0.02);
			servo.pulsewidth(.001140); 
			servo2.pulsewidth(.000830); 
			servo3.pulsewidth(.001420); 	
      calibrateHand();
      */
		
			
			bt.baud(115200);      //Sets up baud rate for bloetooth module communication, for USBSerial this is not necessary
		
      //Start Timer     
			tim.start();
				
			//50 Hz Period (All Servos Periods are tied together, however, the duty cycles can be independent.
			servo.period(0.02); 
			servo2.period(0.02);
			servo3.period(0.02);
				
			//Bring Servos to Middle (Position Hands)
			servo.pulsewidth(.001140); 
			servo2.pulsewidth(.000690); 
			servo3.pulsewidth(.001210); 
			
			//Set GPIO's for solenoids controlling fingers	
			leftFinger = 0;
			middleFinger = 0;
			rightFinger = 0;
			
			
			while(!updateLib()) {
				
				printf("waiting blue tooth update\r\n");
				
			}
			
			song_lib.printLibrary();
			
			//song_lib.default_song();
			while(bt.readable()) {
				
				printf("%c\r\n", bt.getc());
				
			}
			
			 while(1)
       {
				 while(!musicSelectionOver) {
					 
						//printf("play_flag : %d \r\n", play_flag);
						//printf("over : %d \r\n", musicSelectionOver);
				 
						musicSelection(true);

					}
					
						pause_flag = false;
						stop_flag = false;
						next_flag = false;
						play_flag = false;
					  tim.reset();
					
						while(musicSelectionOver){		
							
							if(!pause_flag) {
								//printf("song play session \r\n");
									setServos();
									pressKey();
									depressKey();
									setNextKey();			
							}
							
								if(stop_flag) {
									printf("stop playing\r\n");
									musicSelectionOver = false;
									//break;
								}
								
								if(next_flag) {
									printf("next song\r\n");
									musicSelectionOver = false;
									nextSongNumber();
									//break;
									
								}
            }
						
						continue;
			}
			printf("error!");

		
			/*
			while(1){
				
					setServos();
					pressKey();
					depressKey();
					setNextKey();
				
			}
			*/
			

			/*
			nrk_setup_ports();
				
			nrk_init();
		  //bmac_task_config();
			nrk_create_taskset();
			nrk_start();
			*/
			
			return 0;

  }
      


 
 
		//This task is for turning the servo
 void Task1()                 
 {
                     
		//int key;
					 
		while(1){   
							
			//This will set the left, middle and right servos to the next positions.
			setServos(); 
			
			/*
			//printf("task1a\r\n");
			servo.pulsewidth(.001020);
			servo2.pulsewidth(.000800);
			servo3.pulsewidth(.000600);
			//printf("task1b\r\n");
			*/					
			nrk_wait_until_next_period();
 
		}
}

 
 
 void Task2()                    //This task is for pressing the key
 {

      
      while(1)
      {
                
				//This handles the pressing of left/middle/right solenoids.
				pressKey();
		 
        nrk_wait_until_next_period();
                  
     
     }
    }
      
      
  void Task3()   //Task3 is for depressing the key
    {

          
        while(1)
        {
              
						depressKey();
						nrk_wait_until_next_period();
      
         }
            
     
    }
      

  
void Task4()                   //This task is for setting the pwm
 {
         
       while(1)
       {
              
					//This function checks if it's time to press the next key.
					setNextKey();                  
				  nrk_wait_until_next_period();
      }
        
         
      
      
 }
    

void nrk_create_taskset()

{
 
    nrk_task_set_entry_function( &TaskOne, Task1);
    nrk_task_set_stk( &TaskOne, Stack1, NRK_APP_STACKSIZE);
    TaskOne.prio = 1;
    TaskOne.FirstActivation = TRUE;
    TaskOne.Type = BASIC_TASK;
    TaskOne.SchType = PREEMPTIVE;
    TaskOne.period.secs = 0;
    TaskOne.period.nano_secs = 50*NANOS_PER_MS;//100*NANOS_PER_MS;//500*NANOS_PER_MS;
    TaskOne.cpu_reserve.secs = 0;
    //TaskOne.cpu_reserve.nano_secs = 100*NANOS_PER_MS;
    TaskOne.cpu_reserve.nano_secs = 0;
    TaskOne.offset.secs = 0;
    TaskOne.offset.nano_secs= 0;
    nrk_activate_task (&TaskOne);

		nrk_task_set_entry_function( &TaskTwo, Task2);
		nrk_task_set_stk( &TaskTwo, Stack2, NRK_APP_STACKSIZE);
		TaskTwo.prio = 2;
		TaskTwo.FirstActivation = TRUE;
		TaskTwo.Type = BASIC_TASK;
		TaskTwo.SchType = PREEMPTIVE;
		TaskTwo.period.secs = 0;
		TaskTwo.period.nano_secs = 50*NANOS_PER_MS;//500*NANOS_PER_MS;
		TaskTwo.cpu_reserve.secs = 0;
		//TaskTwo.cpu_reserve.nano_secs = 100*NANOS_PER_MS;
		TaskTwo.cpu_reserve.nano_secs = 0;
		TaskTwo.offset.secs = 0;
		TaskTwo.offset.nano_secs= 0;
		nrk_activate_task (&TaskTwo);
					
		nrk_task_set_entry_function( &TaskThree, Task3);
		nrk_task_set_stk( &TaskThree, Stack3, NRK_APP_STACKSIZE);
		TaskThree.prio = 3;
		TaskThree.FirstActivation = TRUE;
		TaskThree.Type = BASIC_TASK;
		TaskThree.SchType = PREEMPTIVE;
		TaskThree.period.secs = 0;
		TaskThree.period.nano_secs = 50*NANOS_PER_MS;//500*NANOS_PER_MS;
		TaskThree.cpu_reserve.secs = 0;
		//TaskThree.cpu_reserve.nano_secs = 100*NANOS_PER_MS;
		TaskThree.cpu_reserve.nano_secs = 0;
		TaskThree.offset.secs = 0;
		TaskThree.offset.nano_secs= 0;
		nrk_activate_task (&TaskThree);
					
		nrk_task_set_entry_function( &TaskFour, Task4);
		nrk_task_set_stk( &TaskFour, Stack4, NRK_APP_STACKSIZE);
		TaskFour.prio = 4;
		TaskFour.FirstActivation = TRUE;
		TaskFour.Type = BASIC_TASK;
		TaskFour.SchType = PREEMPTIVE;
		TaskFour.period.secs = 0;
		TaskFour.period.nano_secs = 50*NANOS_PER_MS;//500*NANOS_PER_MS;
		TaskFour.cpu_reserve.secs = 0;
		//TaskFour.cpu_reserve.nano_secs = 100*NANOS_PER_MS;
		TaskFour.cpu_reserve.nano_secs = 0;
		TaskFour.offset.secs = 0;
		TaskFour.offset.nano_secs= 0;
		nrk_activate_task (&TaskFour);
												
}


void calibrateHand() {
	
		//Code to 'calibrate' the servo/structure: Centered at Sol, 3.25 inches from keyboard and 2.75 inches high
		servo2.period(.02);
		//servo3.pulsewidth(.0008);
		servo2.pulsewidth(.00830);
		while(keyCount < 12) {
				//const char* constIn;
			
				//printf("inside while keycount\r\n");
				//cin>>keyboardIn;
				//gets(keyboardIn);
				keyboardIn = getchar();
				//putchar(keyboardIn);
				printf("printing keyboardIn: %c and count is: %d \r\n", keyboardIn, keyCount);
				servo2.pulsewidth(initialPWM);
				
				//strcpy(keyboardIn, constIn);
			
				if (keyboardIn == 'w') { //increase duty by a small number
						//printf("w: %f\r\n", initialPWM);
						initialPWM += .00001;
						servo2.pulsewidth(initialPWM);
						printf("w: %f\r\n", initialPWM);
				}
				if (keyboardIn == 's') {
				//if (!strcmp(keyboardIn, "d")){  
						initialPWM = initialPWM - .00001;
						servo2.pulsewidth(initialPWM);
						printf("d: %f\r\n", initialPWM);
				}
				if (keyboardIn == 'x') { //save
						printf("PWM Value to be Saved: %f \r\n", initialPWM);
						keyPWMVal[keyCount] = initialPWM;
						keyCount++;
						//initialPWM = .0008; 
				}	
		}
		for (int herp = 0; herp < 12; herp++) {
				printf("%f\r\n", keyPWMVal[herp]);
		}
	
}


//Each Finger/Hand/Servo covers a range of 6 Keys
void setServos() {
	int key = 0;
	int keyM = 0;
	int keyR = 0;
	

	//Go home if you've finished each hand/finger's part.
	//if (i >= jblSize && iM >= jbmSize && iR >= jbrSize) {
	if (file[i] == 11 && fileM[iM] == 11 && fileR[iR] == 11) {	
			printf("~~~ End of Music ~~~ \r\n");
			
			//Reset Indices for New Music
			i = 0;
			iM = 0;
			iR = 0;
			//Center the Servos/Fingers
		  servo.pulsewidth(.001140); 
			servo2.pulsewidth(.000690);  
			servo3.pulsewidth(.001420); 
		
			musicSelectionOver = 0;       // At the end of the music, wait for the next one.
      taskFlag1 = 0;
			taskFlag2 = 0;
			taskFlag3 = 0;
			taskFlag4 = 0;
			
		  taskFlag1M = 0;
			taskFlag2M = 0;
			taskFlag3M = 0;
			taskFlag4M = 0;
			
			taskFlag1R = 0;
			taskFlag2R = 0;
			taskFlag3R = 0;
			taskFlag4R = 0;
	}
	
	//====Left Servo: Range from Sol - Mi, Centered at Ti/Do Boundary====
	if(taskFlag1)
	{                          
							 if(i < fileSize - 1){
								key = file[i];
								switch(key) {
										case 1: {    
													servo.pulsewidth(.001440);												
													break; //Sol
										}
										case 2: {
													servo.pulsewidth(.001380);
													break;//Sol#
										}
										case 3: {
														servo.pulsewidth(.001310);																				
														break;//La
										}
										case 4: {
														servo.pulsewidth(.001250); 
														break;//La#
										}
										case 5: {
														servo.pulsewidth(.001190); 
														break; //Ti
										}
										case 6: {
														servo.pulsewidth(.001110);            
														break;//Do
										}
										case 7: {
														servo.pulsewidth(.001060);
														break;//Do#
										}
										case 8: {
														servo.pulsewidth(.001010); 
														break;//Re
										}
										case 9: {
														servo.pulsewidth(.000950);
														break;//Re#
										}
										case 10: {
														servo.pulsewidth(.000880);
														break;//Mi
										}
										case 11: { //Stop Bit
														//servo.pulsewidth(.000830); //These are covered by middle servo.
														break;//Fa
										}
										case 12:{													
														//servo.pulsewidth(.000300); //Ditto												
														break;//Fa#
										}
										default: {
											break;
										}																																							
								}						
								//printf("Left Key: %d \r\n", key);											
								startTurningTime = tim.read_ms();
								taskFlag1 = 0;
								taskFlag2 = 1;
								taskFlag3 = 0;
								taskFlag4 = 0;
								//printf("startTurning time %d \r\n", startTurningTime);									
							} //close if
							else
							{
										taskFlag1 = 0;
										taskFlag2 = 0;
										taskFlag3 = 0;
										taskFlag4 = 0;
										printf("End of Left Hand \r\n");						
							}										 
		}
													
	//====Middle Servo: Range from Fa - Re, Centered at La/Ti Boundary====
	if(taskFlag1M)
							{                          
							 if(iM < fileMSize - 1){
								keyM = fileM[iM];
								switch(keyM) {
										case 1: {
													servo2.pulsewidth(.001090);
													break;//Fa
										}
										case 2: {
													servo2.pulsewidth(.001030);
													break;//Fa#
										}
										case 3: {
														servo2.pulsewidth(.000950);																				
														break;//Sol
										}
										case 4: {
														servo2.pulsewidth(.000920);
														break;//Sol#
										}
										case 5: {
														servo2.pulsewidth(.000840);
														break; //La
										}
										case 6: {
														servo2.pulsewidth(.000780);             
														break;//La#
										}
										case 7: {
														servo2.pulsewidth(.000720);
														break;//Ti
										}
										case 8: {
														servo2.pulsewidth(.000660);
														break;//Do
										}
										case 9: {
														servo2.pulsewidth(.000620);
														break;//Do#
										}
										case 10: {
														servo2.pulsewidth(.000540);
														break;//Re
										}
										case 11: { //Stop Bit
														//servo2.pulsewidth(.000390); //Covered by right hand
														break;//Re#
										}
										case 12:{
														//servo2.pulsewidth(.000300); //Ditto
														break;//Mi
										}
										default: {
											break;
										}
																																														
								}												
								//printf("Middle Key: %d \r\n", keyM);			
								startTurningTimeM = tim.read_ms();
								taskFlag1M = 0;
								taskFlag2M = 1;
								taskFlag3M = 0;
								taskFlag4M = 0;
								//printf("Middle startTurning time %d \r\n", startTurningTimeM);			
								} //close if
								else
								{
											taskFlag1M = 0;
											taskFlag2M = 0;
											taskFlag3M = 0;
											taskFlag4M = 0;
											printf("End of Middle Hand Part \r\n");							
								}
										
												 
		}

	//====Right Servo: Range from Re# - Do, Centered at Sol/La Boundary====
			if(taskFlag1R)
							{
							 if(iR < fileRSize - 1){
								keyR = fileR[iR];
								
								switch(keyR) {
										case 1: {
													//servo3.pulsewidth(.001580);
													servo3.pulsewidth(.001550);
													break;//Re#
										}
										case 2: {
													//servo3.pulsewidth(.001510);
													servo3.pulsewidth(.001480);
													break;//Mi
										}
										case 3: {
														//servo3.pulsewidth(.001420);	
													  servo3.pulsewidth(.001400);
														break;//Fa
										}
										case 4: {
														//servo3.pulsewidth(.001370);
													  servo3.pulsewidth(.001340);
														break;//Fa#
										}
										case 5: {
														//servo3.pulsewidth(.001300);
													  servo3.pulsewidth(.001280);
														break; //Sol
										}
										case 6: {
														//servo3.pulsewidth(.001240);  
													  servo3.pulsewidth(.001210);
														break;//Sol#
										}
										case 7: {
														//servo3.pulsewidth(.001170);
														servo3.pulsewidth(.001150);
														break;//La
										}
										case 8: {
														//servo3.pulsewidth(.001110);
														servo3.pulsewidth(.001080);
														break;//La#
										}
										case 9: {
														//servo3.pulsewidth(.0011050);
														servo3.pulsewidth(.001020);
														break;//Ti
										}
										case 10: {
														//servo3.pulsewidth(.000960);
														servo3.pulsewidth(.000950);
														break;//Do
										}
										case 11: { //Stop Bit
														//servo3.pulsewidth(.001080); //Unused
														break;//Do#
										}
										case 12:{
														//servo3.pulsewidth(.000330); //Ditto
														break;//Re
										}
										case 13:{
														servo3.pulsewidth(.000820); //Ditto
														break;//Re
										}
										default: {
											break;
										}
																																														
								}
													
								//printf("Right Key: %d \r\n", keyR);								
								startTurningTimeR = tim.read_ms();
								taskFlag1R = 0;
								taskFlag2R = 1;
								taskFlag3R = 0;
								taskFlag4R = 0;					
								//printf("Right startTurning time %d \r\n", startTurningTimeR);		
								} //close if
								else
								{
											taskFlag1R = 0;
											taskFlag2R = 0;
											taskFlag3R = 0;
											taskFlag4R = 0;
											printf("End of Right Hand Part \r\n");	//For MHHL this is larger file so it wont print  this statement			
								}																					 
		}
}

		
void pressKey() {
	
	  int currentTime;
	  currentTime = tim.read_ms();
			
		//printf("pressKey\r\n");

		//====Left Solenoid====
		//if((currentTime - startTurningTime) >= 200)       //Time for Servo to Reach next Key, then Solenoid can press key.
		if((currentTime - startTurningTime) >= 200)
		{					
						if(taskFlag2)
						 {							 
								 if (file[i] != 12){
									 leftFinger = 1; //press, only if not a 'buffer key'
								 }
								 pressingTime = tim.read_ms() - 200;
								 //endTurningTime = tim.read_ms();
								 turnPermit = 0;  // press flag on
								 taskFlag1 = 0;
								 taskFlag2 = 0;
								 taskFlag3 = 1;
								 taskFlag4 = 0;							
							  //printf("Pressing time %d \r\n", pressingTime);
						 }				 
		 }
			 
		 //====Middle Solenoid====		 
		 //if((currentTime - startTurningTimeM) >= 200)       //Time for Servo to Reach next Key, then Solenoid can press key.
			if((currentTime - startTurningTimeM) >= 200) 
			{						
							if(taskFlag2M)
							 {
								 if(fileM[iM] != 12){
										 middleFinger = 1; //press, only if not a 'buffer key'
								 }
								 pressingTimeM = tim.read_ms() - 200;
								 //endTurningTimeM = tim.read_ms();
								 //turnPermitM = 0;  // press flag on
								 taskFlag1M = 0;
								 taskFlag2M = 0;
								 taskFlag3M = 1;
								 taskFlag4M = 0;						
								 //printf("Middle Pressing time %d \r\n", pressingTimeM);
							 }					 
			 }
				 
			 //====Right Solenoid====																	 
			//if((currentTime - startTurningTimeR) >= 200)       //Time for Servo to Reach next Key, then Solenoid can press key.
			if((currentTime - startTurningTimeR) >= 200)
			{			
							if(taskFlag2R)
							 {
									 //printf("PRESSING KEY\r\n");
								 if(fileR[iR] != 12){	
									 rightFinger = 1; //press
								 }
								 pressingTimeR = tim.read_ms() - 200;
								 //endTurningTimeR = tim.read_ms();
								 //turnPermitR = 0;  // press flag on
								 taskFlag1R = 0;
								 taskFlag2R = 0;
								 taskFlag3R = 1;
								 taskFlag4R = 0;				
								 //printf("Right Pressing time %d \r\n", pressingTimeR);
							 }								 
			 }
		 
				 
	
	
}
	
void depressKey() {
		int currentTime;
		currentTime = tim.read_ms();  
    
		//printf("depressKey\r\n");

	
		//check the press duration, so check the difference between the time of press (of piano key) and current time, if this is
		//larger than the "press duration" then depress the key, i.e. set the solenoid GPOI to 0.
		if( (currentTime - pressingTime) >= (file[i+2])*1000 )    																																																																		
		{
				if (taskFlag3)
				{   
						leftFinger = 0; // de - press
						depressingTime = tim.read_ms();
						taskFlag1 = 0;
						taskFlag2 = 0;
						taskFlag4 = 1;
						taskFlag3 = 0;
						//printf("depressing time %d \r\n", depressingTime);						
				 }                
      }

		//====Middle Solenoid====	
		if( (currentTime - pressingTimeM) >= (fileM[iM+2])*1000 )    																																																																	
		{
				if (taskFlag3M)
				{  
						middleFinger = 0; // de - press
						depressingTimeM = tim.read_ms();
						taskFlag1M = 0;
						taskFlag2M = 0;
						taskFlag4M = 1;
						taskFlag3M = 0;
						//printf("Middle depressing time %d \r\n", depressingTimeM);					
				 }            
      }
		
		//====Right Solenoid====		
		if( (currentTime - pressingTimeR) >= (fileR[iR+2])*1000 )    																																																																		
		{
				if (taskFlag3R)
				{  
						rightFinger = 0; // de - press
						depressingTimeR = tim.read_ms();
						taskFlag1R = 0;
						taskFlag2R = 0;
						taskFlag4R = 1;
						taskFlag3R = 0;
						//printf("Right depressing time %d \r\n", depressingTimeR);										
				 }                              
      }
}


void setNextKey() {
	
	int currentTime;
	currentTime = tim.read_ms();
     
	//printf("setNextKey\r\n");	
     
	 // set the next pwm. So, after time to next key - press duration has elapsed, and this is taken as the
	 // difference between current time and the time at which the key was depressed, since a task is used to
	 // check if key press duration has elapsed.
	
	
	//====Left Key====
	
	
	if (file[i] != 11){ //Don't increment index after reached stop bit
		if((currentTime - depressingTime)  > (file[i+1]-file[i+2])*1000)  
			{   
					 if(taskFlag4)
					 {   
										//printf("i %d\r\n", i);
										i = i+3;
										turnPermit = 1;
										taskFlag1 = 1;
										taskFlag2 = 0;
										taskFlag3 = 0;
										taskFlag4 = 0;
									
					 }
			}
}
		
		
	//====Middle Key====
  
	if (fileM[iM] != 11){
		if((currentTime - depressingTimeM)  > (fileM[iM+1]-fileM[iM+2])*1000)  
			{   
					 if(taskFlag4M)
					 {        
									//printf("iM %d\r\n", iM);
										iM = iM+3;
										//turnPermitM = 1;
										taskFlag1M = 1;
										taskFlag2M = 0;
										taskFlag3M = 0;
										taskFlag4M = 0;
									
					 }
			}
	}
		
		//====Right Servo====
	
	if (fileR[iR] != 11){
		if((currentTime - depressingTimeR)  > (fileR[iR+1]-fileR[iR+2])*1000)  
			{   
					 if(taskFlag4R)
					 {                   
									//printf("iR %d\r\n", iR);
										iR = iR+3;
										//turnPermitR = 1;
										taskFlag1R = 1;
										taskFlag2R = 0;
										taskFlag3R = 0;
										taskFlag4R = 0;
									
					 }
			}
	}
		
}


void musicSelection(bool mode)
{
  int musicNumber = -1;
	char keyboardIN;
	
	
			if (bt.readable()) {
            char tmp = bt.getc();
           
            //pc.putc(tmp);
            //rx_buf[i++] = tmp;
				   	musicNumber =  tmp - 48;	
			      musicNumber--; 
						current_song = musicNumber;
						//printf("derp2!\r\n");
            //end = tmp;
       }
	
			if(play_flag) {
				printf("in musicSelection, play button\r\n");
				
				if(next_flag) {wait(2);}
				musicNumber = current_song;
			}

	
	
	if(musicNumber < song_lib.size && musicNumber > -1) {
		
			printf("Play %s\r\n", song_lib.song_list[musicNumber].name);
			fileSize = song_lib.song_list[musicNumber].songlSize;
			fileMSize = song_lib.song_list[musicNumber].songmSize;
			fileRSize = song_lib.song_list[musicNumber].songrSize;

		song_lib.copyFile(musicNumber, file, fileM, fileR);
		//song_lib.printSong(musicNumber);
		
		  for(int j=0;j < fileMSize; j++)
		  {
      printf("%f ", fileM[j]);
      }
			printf("\r\n");
		
			i = 0;
			iM = 0;
			iR = 0;
			
			
			musicSelectionOver = 1;
			//Task Control (Locks) Variables
			turnPermit = 1;
			taskFlag1 = 1;
			taskFlag2 = 1;
			taskFlag3 = 1;
			taskFlag4 = 1;

			//bool turnPermitM = 1;
			taskFlag1M = 1;
			taskFlag2M = 1;
			taskFlag3M = 1;
			taskFlag4M = 1;

			//bool turnPermitR = 1;
			taskFlag1R = 1;
			taskFlag2R = 1;
			taskFlag3R = 1;
			taskFlag4R = 1;	
			musicSelectionOver = 1;
	}
	else {
		//printf("Do nothing!\r\n");
      musicSelectionOver = 0;
	}


}

int nextSongNumber() {
	
	//current_song++;
	current_song = current_song < (song_lib.size-1)? current_song : -1;
  current_song++;	
	
}
int toInt(char *buf, int start, int end) {

    

    int result = 0;

    

    //int i = 0;

    

    while(start < end) {

        

        result = result*10 + (buf[start] - 48);

        start++;

    }
		
    return result;

}


float toFloat(char *start, int size) {

    

    int part_int = 0;

    

    int i = 0;

    

    while(i < size && start[i] != '.') {

        part_int = part_int*10 + (start[i] - 48);

        i++;

    }

    

    i++;

    float part_float = 0;

    float tmp = 0.1;

    //int j = size - 1;

    while(i < size) {

        part_float = part_float + tmp * (start[i] - 48);

        i++;

    }

    return part_int + part_float;

}


bool updateLib() {

		if(!bt.readable()) {
			printf("waiting for blue tooth!");
			return false;
		}
	
		char *buf = (char*)malloc(6000);
		int i = 0;
		char tmp = ' ';
		
		while(tmp != '!') {
			while(bt.readable() && i < 6000 && tmp != '!') {
				tmp = bt.getc();
				buf[i] = tmp;
				i++;
			}
		}
		while(bt.readable()){
			printf("%c", bt.getc());
			}
		
		printf("received message %s\r\n", buf);
		song_lib.initial_lib(buf, i); 
		
		return true;
		
}

inline void SongLib::initial_lib(char *buffer, int length) {

    

    if(buffer[0] == '#') {

        printf("choose not to update library");
        song_lib.default_song();
			
        return;
			
    }

    
    int i = 0;  

    //get the size of the library
    int start = i;
    while(buffer[i] != '!' && isdigit(buffer[i])) {

        i++;

    }
    size = toInt(buffer, start, i);
    song_list = (Song*)malloc(sizeof(Song)*size);

    
    for(int j = 0; j < size; j++) {

        //Assign song name to a song
			
        //buffer[i] here is first ", skip this one
				i++;
        string name = "";

        while(buffer[i] != '!' && buffer[i] != '*') {

            name = name + buffer[i];

            i++;

        }
        //buffer[i] here is second ", skip this one
        i++;
        //song_list[j].name = (char*)malloc(100);
        song_list[j].name = new char[100];
        strcpy(song_list[j].name, name.c_str());

        //start read in l, m, r
        //skip L(

        i = i + 2;
        start = i;
        while(buffer[i] != '!' && isdigit(buffer[i])) {

            i++;

        }
        song_list[j].songlSize = toInt(buffer, start, i);

        //skip ){
        i = i + 2;
        int il = 0;
        song_list[j].songlFile = (float*)malloc(sizeof(float)*song_list[j].songlSize);
        while(buffer[i] != '}') {
					
            start = i;
            while(buffer[i] != ',' && buffer[i] != '}') {
							
                i++;
							
            }

            song_list[j].songlFile[il] = toFloat(&buffer[start], i - start);

            if(buffer[i] == ',')
                i++;

            il++;

        }

        //skip }M(

        i = i + 3;
        start = i;
        while(buffer[i] != '!' && isdigit(buffer[i])) {

            i++;

        }
        song_list[j].songmSize = toInt(buffer, start, i);

        //skip ){
        i = i + 2;
        int im = 0;
        song_list[j].songmFile = (float*)malloc(sizeof(float)*song_list[j].songmSize);
        while(buffer[i] != '}') {

            start = i;

            while(buffer[i] != ',' && buffer[i] != '}') {
                i++;
            }

            song_list[j].songmFile[im] = toFloat(&buffer[start], i - start);

            if(buffer[i] == ',')
                i++;

            im++;

        }

        //skip }R(
        i = i + 3;
        start = i;

        while(buffer[i] != '!' && isdigit(buffer[i])) {

            i++;

        }
        song_list[j].songrSize = toInt(buffer, start, i);

        //skip ){

        i = i + 2;

        int ir = 0;

        song_list[j].songrFile = (float*)malloc(sizeof(float)*song_list[j].songrSize);

        while(buffer[i] != '}') {

            start = i;

            while(buffer[i] != ',' && buffer[i] != '}') {
                i++;
            }

            song_list[j].songrFile[ir] = toFloat(&buffer[start], i - start);

            if(buffer[i] == ',')
                i++;
            ir++;

        }
				
        //skip };

        i = i + 2;

    }

    

    printf("update library successfully!");

    

}





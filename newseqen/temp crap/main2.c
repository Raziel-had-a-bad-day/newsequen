/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#define ARM_MATH_CM4
#include "main.h"
#include "arm_math.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;
DMA_HandleTypeDef hdma_adc1;

I2C_HandleTypeDef hi2c2;

SPI_HandleTypeDef hspi2;

TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim4;

/* USER CODE BEGIN PV */



// STM32F411

// if folding problem , space after // 


// flash start


// flash start 0x08000000    , prog size  59B0  22,960 bytes  end   0x080059B0 , 64k ends at 0x08010000 , maybe after or D2F0 which is 54k so 0x0800D2F0
// source , select mask type , speed of playback

const uint32_t flash_start=0x0800D2F0; // flash start address , 2048 page saize or 0x800
const uint32_t flash_page=0x800;



uint32_t x;  // calc timer note
uint32_t y;
uint16_t pwmVel;
uint16_t pwmVelB;
//const uint8_t tempoLUT[];  // lookup table for tempo instead of calculate
const uint8_t MajorNote[]= { 0,2,4,6,7,9,11,13,14,16,18,19,21,23,25,26,28,30,31,33,35,37,38,40,42,43,45} ;  // major
const uint8_t MinorNote[]={ 0,2,3,5,7,8,10,12,14,15,17,19,20,22,24,26,27,29,31,32,34,36,38,39,41,43,44,46}; // minor
const uint8_t ChromNote[]={0,2,3,5,6,8,9,11,12,14,15,17,18,20,21}; //chromatic, diminished
//const uint8_t noteReference[] = {10,10,10,10,0,10,0,8,1,10,1,8,2,10,3,10,3,8,4,10,4,8,5,10,5,8,6,10,0,11,0,8,1,11,1,8,2,11,3,11,3,8,4,11,4,8,5,9,5,8,6,9,10,11,10,20,10,11,10,12,10,13,10,14,10,15,10,16,10,17,10,18,10,19,11,20,11,11 };// cant read last
const uint16_t timerValues[]= {34400,32469,30647,28927,27303,25771,24324,22959,21670,20454,19306,18222,17200,16234,15323,14463,13651,12885,12162,11479,10835,10227,9653,9111,8600,8117,7661
,7231,6825,6442,6081,5739,5417,5113,4826,4555,4300,4058,3830,3615,3412,3221,3040,2869,2708,2556,2413,2277} ;   // timer values C2-C6
const uint16_t sample_Noteadd[50]= { 2059, 2181, 2311, 2448, 2594, 2748, 2912, 3085, 3268, 3462, 3668, 3886, 4117, 4362, 4622, 4896, 5188, 5496, 5823, 6169, 6536, 6925, 7336, 7773, 8235, 8725, 9243, 9793,
		10375, 10992, 11646, 12338, 13072, 13850,
		14673, 15546, 16470, 17449, 18487, 19586, 20751, 21985, 23292, 24677, 26145, 27699, 29346};  // 35khz add c2-c6  >>12 for correct value , 0-255 samples sawtooth , /2 for extra notes ,prefer circular , replaced first 2059 with 0

//static unsigned short playWave;
const uint16_t freq_lut[]={4186,4434,4698,4978,5274,5587,5919,6271,6644,7039,7458,7902,8371,8869,9397,9955,10547,11175,11839,12543,13289,14079,14916,15803,16743,17739,
		18794,19911,21095,22350,23679,25087,26578,28160,29834,31608,33488,35479,37589,39824,42192,44701,47359,50175,53158,56319,59668,63216};  // freq lookup x64  64hz_>987hz , C2-C6 Note 1-48
//const uint8_t disp_lut [18] [16]= {                    // menu look up
const uint16_t disp_lut [18] [16]= {							 // menu look up using char


			//	{208,209,210,211,212,213,214,215,216,217,218,219,220,221,222,223}, // 16 notes for drums and other tones
		// {70,50,64,158,159,64,227,226,64,201,64,64,236,143,64,161},  // filter 2 settings and loop start
		// {70,49,64,156,157,64,225,224,64,200,202,64,235,142,64,160},  // filter 1 settings and loop start
		// {65 ,68 ,64 ,150 ,151 ,64,152,153,64,64,64,64,64,64,237,64},	//adr for drums and other notes



		{208,209,210,211,212,213,214,215,216,217,218,219,220,221,222,223}, // 16 notes for drums and other tones 80 - 95
		{'P',64,200,64,201,64,202,64,64,64,64,64,64,64,64,64},  // filter 2 settings and loop start
		{128,129,130,131,132,133,134,135,136,137,138,139,140,141,142,143},  // 16 notes 0-15 
		{'A' ,'S' ,64 ,148 ,149 ,64,64,64,64,64,64,64,64,'T',237,64},	// 20,21    attack sustain  maybe follow a diff system ,attach+decay then sustain+release







		// {224,225,64,226,227,64,64,68,101,84,64,228,229,230,231,232},				//p7 filter adr and detuner
		{'S',64,258,64,259,64,260,64,261 ,64,262,64,263,64,264,64},	// lfo speed 1-6

		{'D',64,268,64,269,64,270,64,271,64,271,64,272,64,273,64},   // lfo depth
		{'G',64,64,64,64,64,64,64,264,64,64,64,64,64,64,64},	// lfo gain


		{'F',64, 278, 279, 280,64, 64,64,64,64,64,64,64,64,64,64},   // feedback



		{84 ,121, 112, 101, 78, 111, 116, 64,240,241,242,243,244,245,246,247},	//feedback

				{176,177,178,64,179,180,181,64,182,183,184,64,185,186,187,64},						//p4 tone mods

			//	{98 ,97, 114, 64, 80, 105, 116, 99, 104,64,200,201,202,203,64,64}, //p5  bug with 66?

		{192,64,193,64,194,64,195,64,196,64,197,64,198,64,199,64},					// keychange
				{64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64},					//p6 empty

		{237,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64},							// unsure

		{64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64},
		{64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64},					//p6
		{64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64},
		{64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64}			//p8




};

const char popup_lut [13][9]= { "Pitch    " , "Attack   ","Sustain  ","LFO Speed","LFO Gain ","Tempo    " ,"LPF1     ","LPF2     ","Pitch 1  " ,"Pitch 2  ","         ","empty    "}; // info line 
const uint8_t popup_ref[8][16]={	// reference for feedback
	{11,11,10,10,10,10,10,10,10,10,10,10,10,10,10,10}, 
{10,10,0,10,0,10,0,10,10,10,10,10,10,10,10,10},
{10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10}, 
{10,10,10,1,2,10,10,10,10,10,10,10,10,10,5,5}, 
{10,10,3,10,3,10,3,10,3,10,3,10,3,10,10,10}, 
{10,10,4,10,4,10,4,10,4,10,4,10,4,10,10,10}, 
{10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10}, 
{10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10} }; 




int _write(int file, char *ptr, int len)
{
	int DataIdx;

	for (DataIdx = 0; DataIdx < len; DataIdx++)
	{
	   ITM_SendChar( *ptr++ );
	}

	return len;
}

const char menuList[129] = "_Notelist1234V___Note_2_Speed_Key_LFOSSWITCHESREPEAT__KEY#____Time____NTE2_____________";   // top menu

const char mainNote[29] = "_CDEFGABCDEF>_1234567890+>_x" ; //note string values 7 notes and flat and sharp which are double length ? 11=1  20=0 21-

// const char mainNoteLow[] = "cdefgab_^: " ; // note string for off values
const uint8_t noteSwitch[] = { 255,0,204,51,170,85,240,15,17,252,0 };  // note switching using bitread , alternating pattern on, off, 11001100, 00110011,10101010,01010101,11110000,00001111,00010001,11101110
const uint16_t lfoGainValues[8] = {}; // gain adder values
const uint8_t  waveSaw[17] = {  10,31,47,63,79,95,111,127,0,31,47,63,79,95,111,127}; //lfo saw values lut ,not used

const uint8_t waveTri[17] = { 0,15,31,47,63,79,95,111,127,111,95,79,63,47,32,15 }; //lfo triangle values lut ,not used

uint8_t potValues [256];  //low res values mostly for display

uint8_t potSource[256]; // high res version of potValues used when needed 40-0, gonna change to 160 just o break things, need more res for lfo
// lfos speed=150-160 ,lfo_depth=160-170,lfo_gain=170-180;lfo_target=180-190;  80-95=notes,

uint8_t notePos2;
//static unsigned short lfoNext;

uint8_t noteTiming;  // set timing shift
static unsigned short noteTimeFlag;
uint8_t noteTimebit[]= {0,170,85,240,15,204,51,153,102,165,90,0}   ;   // bit switch for adjustimg rythm
uint16_t noteAccu; //adder for left over timing
uint16_t tempAccuA;  // accu for pots
volatile uint8_t promValue; //eprom
volatile uint8_t promValues[64]; // hold original potvalues here
uint8_t noteLength=16; // lenght of note 16 ticks default
uint8_t noteDelay; // initial delay for trigger

uint8_t potPosition=0; // potValues pointer
uint8_t modSwitches [9] ={0,0,0,0,0,0,0,0 }; // function switch storage ,all values are 0-2
 uint8_t ccValues[6]={}; //cc memories storage
uint8_t currentValues [10] = {  };  //note switches on,stop,off  storage 15=on 0=off 1=loop
uint8_t currentValuesB [10] = {  };  // note switch  storage for lcd
uint8_t pitchValues[9] = {  }; // remember pitch values of key pot
uint8_t noteDuration[9] = {  }; // note length array
uint8_t displayMem[9] = { }; // past stored values for lcd
uint8_t displayMemVelocity[9] = {};
uint8_t lcdBufferString; // string storage
uint8_t lcdBufferCounter;  // counter in lcd buffer
uint8_t lcdBufferCounterB;  // counter in lcd buffer
char lcdBufferMem[33] = {};  // string memory for lcdbuffering
char lcdBufferMemNew[33] = {};  // string memory for lcdbuffering incoming
uint8_t memNote[10] = { 0,0,0,0,0,0,0,0,0 };  // current note values for lcd
uint8_t tempValue[127]={ }; // last value is menu control
uint8_t tempValueB[127]={ };
uint16_t tempValueA;
uint8_t potRange=54; // range of potvalues 8/14
// uint8_t modMatrix[65]={};  // storage for all modulation options, new
const char modTop[] = "Nt_WvRtWdTyDp"; // modulation top lcd row : Note(1-8),LFO wave type(0-3) 0 is off , Rate(1-8), Pulse width(1-8) , velocity pitch or time modulation(1-3) , depth goes +- for inversion (+5 -5)
unsigned short modEnable=0;   // true when menu pot above 0
uint8_t menuSelect;     // menu selector pot
uint8_t menuSelectB;  // old menu data
uint8_t menuSelectX; // menu horizontal
uint8_t menuSelectX2; // old menu
 unsigned short firstRound=1;
//static unsigned short modMenu=0;
uint8_t modRate; // lfo rate
uint8_t modPhase; // lfo phase
uint8_t modDelay; // lfo initialdelay
uint8_t modPosition; // like notePosition but freewheel
uint8_t modPositionB; // like modPosition but shifted
uint16_t modGain;// moddepth setting ,not sure yet can be -
uint16_t modPitch;// pitch modifier
uint16_t modBarPitch; // changes bar pitch , 8 bars
uint8_t modPointer; // mod pointer
uint8_t modOut; // mod output
//static unsigned short modStep;// mod up count
//static unsigned short centreLed;  // locate positions with led
uint8_t oldNote; //store previous note
uint8_t oldNote2;
uint8_t eventCounter=0; // event timer
//static unsigned short writeCycle=1; // switch write and read
uint8_t notePosition; // seq position for noteplayback 8x8 for now 0-63
uint8_t lfoWave[4]={};
uint8_t lfolastccA[4]={};
uint8_t lfoPhase;
//static unsigned short displayFirst=1; // display loop firs round
uint16_t isrDivide; // counter for isrcount since notimer for now
uint8_t  Note=0;
uint8_t NoteB;
//static unsigned short loopMode=0;
uint8_t octKey = 0;
//static unsigned short NoteSlide=0;
uint8_t NoteBuffer=0;
uint8_t patternType=0;
uint8_t keyPot=0; // make it real time only from now
uint8_t keyPotB=5;
uint8_t keyPotC=0; // storage for analogeread
 uint8_t counterVar=0;
uint16_t counterVarB;
uint8_t switchOne=0;
//static unsigned short outNote;  // detect note process
uint8_t patternEditPos=1; // bar position for pattern edit
uint16_t valueOriginal; // stored value  needs signed
uint16_t valueCurrent; //current setting
uint8_t valueReturn; // final value out
uint8_t valueChange[9]={1,1,1,1,1,1,1,1}; // enable writing
uint8_t barCount=0;  // counts +1 per 8 bars ie
uint8_t barCountB=1;// dec 1-8
uint8_t barCountC=1;
uint16_t barCountTotal=0;  // total note position in 32 bars 95 * 8 , 16bit
uint16_t lastbarCountB=0; // ( dec signed barcountB-2)  0-7 pointer
uint8_t nextbarCountB=0; // next barcounter
//static unsigned short firstbarWrite=1;  // write during first 8 bars
uint8_t firstbarLoop;
uint8_t firstbarLoopB;// bit shifted
volatile int  midiTimer =0; // timing from midiclock to notes

volatile uint16_t isrTemp;
uint8_t isrCountC;

uint16_t isrPoint;
uint16_t isrPointB;  // for noteBar jump by 4
uint16_t isrCount; // ISR counter tcik 32 bit
volatile uint16_t isrCountB; //  512-820 per tick 16bit
volatile uint16_t isrMask;  // tempo mask for tick  16 bit
uint8_t lcdCounter=0;
uint8_t electPitch=64; // Electribe pitch
 uint8_t tempo=20; //delay timer
uint8_t tempoTemp;
uint32_t timerVar ;  // multipler for timer1 lenght for freq , too big needs to be 16bit
uint32_t timerVar2; // timer 2
uint8_t noteMem=0;
uint8_t midiMemory[70]={}; //midi storage for incoming
uint8_t midSettings[]={1,1,0,0}; // midi channel and lfo cc setting
uint8_t midiBarcount=0; // note off var rollover on 8
volatile int  t =0;
uint32_t timExe=0;
uint8_t timeCount=0; //time execute schedule once per note
uint8_t noteTraffic[4]={1,48,0,0}; // remember if note was sent from midi or sequencer : switchone , outNote, midi stored value ,0
uint8_t changeNote=2; // note on off flags
uint16_t sine_counter;  // up counter for sine reading
uint16_t sine_counterB;  // up counter for sine reading ,fractional * 8
int32_t sine_out;     // generated sine output 9 bit
uint16_t sine_temp2;
volatile uint8_t lcdstep_toggle;
uint8_t disp_bit;
uint32_t lcd_holder;
uint8_t lcd_return[16];
uint8_t feedback_temp;
uint16_t note_tuned[11];
	uint32_t tune_Accu;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_ADC1_Init(void);
static void MX_SPI2_Init(void);
static void MX_TIM3_Init(void);
static void MX_TIM4_Init(void);
static void MX_I2C2_Init(void);
static void MX_TIM2_Init(void);
/* USER CODE BEGIN PFP */
void display_init(void);

void adc_read(void);
void analogInputloop(void);
void displayLoop(void);
void displayBuffer(void);
void timerNote(void);
void timerNote2(void);
void sine_count(void);
void sampling(void);
void noteCalc(void);
void lcd_feedback(void);
void lfo(void);
void print_pot(void);
void analoginputloopb(void);
void adsr(void);
void display_gfx(void);
void SPI_command(void);
void display_update(void);

//void velocityLFO(void);
uint8_t seq_pos; // sequencer position linked to isrCount for now but maybe change

void  mask_calc(uint8_t mask_select,uint8_t mask_speed);

 uint16_t noteBar[257]={0,12,12,12,12,12,12,12,12,12,1,22,1};  //   8 bar data , start , end ,vel,wave * 8  3*wave note length cant be uint32_ter than next start
uint8_t NoteC; // second channel note
 uint8_t spell[322] = { 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78,
		79, 254,192, 65, 66, 67, 68, 65, 70, 71, 72, 70, 74, 75, 76, 77, 78,79,79,254,128,0 }; //16x2 chars
uint8_t spellB[42]; // temp store
const uint16_t sine_block2[514]={20000,20245,20491,20736,20981,21226,21471,21716,21960,22204,22448,22692,22935,23177,23419,23661,23902,24142,24382,24621,24860,25097,25334,25570,25806,26040,26274,26506,26738,26968,27198,27426,27654,27880,28105,28329,28551,28772,28992,29211,29428,29644,29858,30071,30282,30492,30700,30906,31111,31315,31516,31716,31914,32110,32305,32497,32688,32877,33063,33248,33431,33612,33791,33968,34142,34315,34485,34653,34819,34983,35144,35303,35460,35615,35767,35917,36064,36209,36352,36492,36629,36764,36897,37027,37155,37279,37402,37521,37638,37753,37864,37973,38080,38183,38284,38382,
		38478,38570,38660,38747,38831,38912,38991,39066,39139,39209,39276,39340,39401,39459,39514,39566,39616,39662,39706,39746,39784,39818,39850,39878,39904,39926,39946,39962,39976,39986,39994,39998,40000,39998,39994,39986,39976,39962,39946,39926,39904,39878,39850,39818,39784,39746,39706,39662,39616,39566,39514,39459,39401,39340,39276,39209,39139,39066,38991,38912,38831,38747,38660,38570,38478,38382,38284,38183,38080,37973,37864,37753,37638,37521,37402,37279,37155,37027,36897,36764,36629,36492,36352,36209,36064,35917,35767,35615,35460,35303,35144,34983,34819,34653,34485,34315,
		34142,33968,33791,33612,33431,33248,33063,32877,32688,32497,32305,32110,31914,31716,31516,31315,31111,30906,30700,30492,30282,30071,29858,29644,29428,29211,28992,28772,28551,28329,28105,27880,27654,27426,27198,26968,26738,26506,26274,26040,25806,25570,25334,25097,24860,24621,24382,24142,23902,23661,23419,23177,22935,22692,22448,22204,21960,21716,21471,21226,20981,20736,20491,20245,20000,19755,19509,19264,19019,18774,18529,18284,18040,17796,17552,17308,17065,16823,16581,16339,16098,15858,15618,15379,15140,14903,14666,14430,14194,13960,13726,13494,13262,13032,12802,12574,
		12346,12120,11895,11671,11449,11228,11008,10789,10572,10356,10142,9929,9718,9508,9300,9094,8889,8685,8484,8284,8086,7890,7695,7503,7312,7123,6937,6752,6569,6388,6209,6032,5858,5685,5515,5347,5181,5017,4856,4697,4540,4385,4233,4083,3936,3791,3648,3508,3371,3236,3103,2973,2845,2721,2598,2479,2362,2247,2136,2027,1920,1817,1716,1618,1522,1430,1340,1253,1169,1088,1009,934,861,791,724,660,599,541,486,434,384,338,294,254,216,182,150,122,96,74,54,38,24,14,6,2,
		0,2,6,14,24,38,54,74,96,122,150,182,216,254,294,338,384,434,486,541,599,660,724,791,861,934,1009,1088,1169,1253,1340,1430,1522,1618,1716,1817,1920,2027,2136,2247,2362,2479,2598,2721,2845,2973,3103,3236,3371,3508,3648,3791,3936,4083,4233,4385,4540,4697,4856,5017,5181,5347,5515,5685,5858,6032,6209,6388,6569,6752,6937,7123,7312,7503,7695,7890,8086,8284,8484,8685,8889,9094,9300,9508,9718,9929,10142,10356,10572,10789,11008,11228,11449,11671,11895,12120,
		12346,12574,12802,13032,13262,13494,13726,13960,14194,14430,14666,14903,15140,15379,15618,15858,16098,16339,16581,16823,17065,17308,17552,17796,18040,18284,18529,18774,19019,19264,19509,19755,20000,20000}; //sine 16bit , 512 samples 68.66HZ  /note sample_noteadd >>12
unsigned char kick[202]={0x7f,0x82,0x97,0xae,0xc1,0xd3,0xe2,0xef,0xf2,0xf7,0xf7,0xf7,0xf5,0xef,0xea,0xe5,0xe0,0xd8,0xd0,0xcb,0xc3,0xbb,0xb6,0xae,0xa9,0xa4,0x9f,0x99,0x94,0x92,0x8f,0x8d,0x8a,0x87,0x87,0x87,0x85,0x85,0x85,0x87,0x82,0x7d,0x75,0x70,0x6b,0x66,0x60,0x5e,0x5b,0x58,0x59,0x56,0x56,0x56,0x59,0x58,0x58,0x5b,0x5e,0x5e,0x60,0x63,0x63,0x65,0x68,0x68,0x6b,0x6b,0x6d,0x6d,0x6d,0x6d,0x6d,0x6d,0x6d,0x6d,0x6d,0x6d,0x6b,0x6b,0x6b,0x68,0x68,0x65,0x63,0x63,0x60,0x60,0x5e,0x5b,0x5b,0x58,0x56,0x56,0x53,0x51,0x51,0x4e,
		0x4b,0x4c,0x49,0x49,0x46,0x44,0x44,0x41,0x41,0x3e,0x3f,0x3e,0x3c,0x3c,0x39,0x39,0x39,0x37,0x37,0x37,0x34,0x34,0x34,0x34,0x32,0x31,0x32,0x31,0x31,0x32,0x2f,0x2f,0x2f,0x2f,0x2f,0x2f,0x2f,0x2f,0x2f,0x2f,0x2f,0x2f,0x2f,0x2f,0x2f,0x2f,0x2f,0x2f,0x32,0x31,0x32,0x31,0x32,0x34,0x34,0x34,0x34,0x37,0x37,0x37,0x39,0x39,0x3c,0x3c,0x3c,0x3e,0x3e,0x41,0x41,0x44,0x44,0x46,0x46,0x49,0x49,0x4b,0x4e,0x4e,0x51,0x53,0x53,0x56,0x59,0x58,0x5b,0x5e,0x5e,0x60,0x63,
		0x66,0x65,0x68,0x6b,0x6d,0x70,0x70,0x72,0x75,0x78,0x7a,0x7d,0x7f,0x7f,0x7f};

const uint16_t sine_block3[601]={20059,20270,20479,20686,20891,21093,21297,21503,21711,21917,22124,22330,22534,22736,22937,23135,23333,23527,23721,23912,24102,24290,24476,24660,24841,25021,25200,25376,25550,25722,25892,26060,26227,26391,26553,26714,26873,27029,27183,27336,27487,27635,27782,27927,28069,28210,28350,28486,28621,28754,28885,29014,29142,29268,29392,29514,29634,29754,29871,29986,30100,30212,30323,30432,30540,30646,30751,30854,30958,31058,31158,31257,31354,31451,31546,31641,31735,31827,31919,32009,32100,32190,32278,32367,32454,32541,32628,32713,32798,32883,32968,33052,33136,33220,33302,33384,33467,33548,
		33630,33711,33792,33872,33951,34031,34110,34189,34267,34346,34423,34501,34577,34653,34728,34804,34879,34952,35026,35098,35170,35242,35313,35382,35452,35521,35588,35654,35720,35785,35849,35912,35973,36034,36093,36152,36208,36264,36319,36372,36424,36474,36525,36576,36629,36683,36740,36799,36863,36931,37004,37082,37166,37254,37348,37447,37551,37659,37772,37888,38007,38128,38250,38375,38497,38621,38741,38860,38975,39088,39193,39296,39391,39481,39565,39641,39711,39772,39828,39874,39913,39946,39970,39987,39996,39999,39996,39987,39970,39948,39919,39886,39847,39803,39755,39702,39644,39583,39517,39448,39374,
		39298,39216,39133,39044,38953,38858,38759,38657,38550,38441,38328,38210,38089,37964,37834,37702,37565,37424,37278,37129,36976,36818,36656,36490,36320,36146,35968,35785,35599,35409,35215,35018,34816,34611,34402,34192,33977,33759,33539,33316,33090,32862,32632,32399,32166,31930,31693,31453,31214,30973,30731,30489,30245,30003,29759,29515,29272,29029,28786,28544,28303,28061,27821,27582,27343,27106,26870,26635,26401,26168,25938,25708,25480,25253,25029,24805,24583,24362,24144,23926,23712,23498,23286,23075,22867,22660,22455,22252,22050,21849,21650,21454,21259,21065,20872,20683,20494,20306,20119,19928,19734,
		19541,19348,19155,18962,18772,18582,18394,18205,18019,17832,17647,17463,17281,17100,16919,16740,16560,16382,16206,16030,15854,15679,15506,15333,15160,14989,14817,14647,14476,14307,14136,13967,13799,13630,13463,13294,13126,12959,12791,12623,12456,12289,12121,11954,11787,11619,11452,11285,11117,10949,10782,10614,10447,10279,10111,9943,9775,9607,9438,9271,9103,8936,8768,8600,8433,8267,8099,7933,7769,7603,7438,7274,7111,6948,6786,6625,6465,6305,6147,5991,5834,5680,5526,5374,5223,5075,4927,4781,4638,4495,4356,4218,4081,3947,3815,3686,3558,3433,3311,3191,3074,2960,2849,2740,2634,2531,2431,2335,2241,2151,
		2064,1980,1899,1823,1749,1680,1613,1549,1490,1434,1382,1334,1290,1249,1211,1177,1148,1122,1099,1081,1066,1054,1046,1043,1043,1046,1053,1064,1078,1096,1118,1143,1171,1203,1238,1276,1318,1363,1411,1462,1515,1572,1631,1694,1759,1827,1898,1971,2047,2123,2204,2286,2370,2457,2545,2635,2728,2821,2916,3014,3112,3211,3313,3415,3519,3624,3730,3837,3945,4053,4163,4272,4384,4495,4609,4720,4833,4941,5048,5150,5248,5340,5426,5505,5576,5640,5695,5743,5782,5814,5839,5856,5867,5875,5875,5873,5868,5862,5854,5847,5840,5837,5836,5837,5845,5858,5875,5900,5931,5969,6014,6067,6126,6193,6268,6349,6437,6531,6633,6739,6852,
		6969,7091,7217,7349,7484,7621,7761,7904,8049,8197,8347,8499,8652,8806,8963,9120,9280,9441,9601,9766,9930,10098,10266,10436,10607,10781,10956,11133,11313,11495,11679,11865,12054,12244,12438,12633,12832,13032,13235,13440,13647,13856,14068,14281,14496,14714,14932,15153,15374,15597,15821,16046,16271,16498,16724,16951,17178,17404,17632,17858,18083,18307,18531,18754,18976,19196,19413,19631,19847
}; // next wave form, organ


const uint16_t sine_block[601]={20112,20707,21302,21880,22447,23026,23624,24233,24838,25438,26044,26667,27301,27930,28544,29145,29741,30339,30936,31519,32086,32638,33181,33713,34225,34711,35178,35630,36070,36486,36874,37241,37590,37920,38219,38488,38737,38972,39186,39369,39521,39651,39763,39851,39916,39962,39992,39999,39970,39903,39822,39755,39705,39647,39544,39393,39220,39063,38928,38790,38617,38406,38187,37989,37813,37629,37413,37172,36936,36721,36517,36300,36063,35824,35603,35397,35186,34955,34720,34496,34289,34080,33847,33600,33365,33156,32961,32753,32527,32302,32098,31909,31709,31490,31270,31070,30887,30699,30497
		,30300,30122,29954,29773,29574,29382,29216,29063,28898,28715,28531,28368,28219,28065,27902,27744,27601,27465,27320,27170,27025,26890,26757,26617,26474,26341,26220,26098,25964,25824,25693,25576,25465,25349,25223,25097,24976,24863,24753,24644,24532,24413,24289,24167,24056,23955,23855,23743,23618,23490,23372,23265,23163,23055,22938,22814,22693,22581,22474,22367,22252,22132,22013,21901,21791,21677,21556,21432,21313,21201,21086,20967,20844,20721,20605,20493,20380,20264,20140,20010,19882,19758,19645,19534,19421,19299,19169,19039,18914,18797,18681,18560,18428,18293,18167,18051,17943,17828,17706,17578,17455,17335,
		17214,17089,16965,16849,16736,16622,16501,16380,16263,16149,16028,15900,15773,15657,15549,15436,15312,15182,15058,14947,14841,14731,14610,14485,14364,14249,14137,14019,13893,13766,13644,13528,13411,13291,13170,13049,12932,12812,12691,12572,12455,12335,12205,12070,11942,11830,11724,11604,11467,11328,11205,11096,10983,10854,10717,10589,10474,10359,10236,10106,9978,9855,9733,9608,9483,9362,9243,9119,8989,8862,8745,8633,8514,8385,8251,8126,8016,7910,7795,7667,7537,7422,7322,7225,7118,6999,6880,6775,6683,6592,6497,6395,6293,6195,6104,6019,5936,5850,5758,5668,5588,5521,5459,5389,5314,5242,5183,5137,5096,5053,5010,
		4968,4930,4894,4864,4843,4832,4824,4816,4812,4814,4828,4847,4868,4895,4930,4974,5029,5092,5162,5241,5326,5418,5524,5643,5773,5908,6049,6205,6387,6592,6807,7026,7255,7508,7789,8091,8401,8715,9041,9389,9760,10148,10553,10977,11422,11880,12347,12826,13325,13845,14379,14919,15466,16028,16610,17203,17794,18380,18974,19586,20209,20826,21425,22010,22595,23187,23778,24357,24921,25471,26011,26540,27051,27541,28008,28456,28886,29299,29692,30063,30411,30735,31033,31307,31556,31782,31984,32161,32311,32438,32541,32628,32698,32750,32776,32773,32746,32704,32653,32589,32501,32391,32267,32136,31997,31841,31663,31468,31270,31068,
		30859,30638,30411,30178,29942,29701,29456,29211,28964,28707,28436,28160,27892,27640,27389,27126,26846,26565,26297,26048,25804,25555,25295,25031,24769,24517,24273,24032,23788,23538,23290,23059,22846,22638,22421,22191,21961,21747,21548,21353,21149,20941,20740,20549,20367,20182,19999,19824,19658,19500,19340,19177,19014,18849,18684,18518,18361,18220,18088,17951,17798,17637,17491,17368,17256,17133,16989,16841,16711,16602,16495,16370,16228,16089,15972,15870,15768,15651,15522,15399,15291,15195,15093,14976,14855,14744,14643,14539,14423,14300,14186,14086,13990,13885,13772,13661,13561,13464,13357,13243,13133,13036,12944,12845,
		12735,12620,12513,12416,12328,12241,12147,12045,11941,11846,11763,11686,11605,11517,11426,11341,11260,11183,11107,11033,10963,10897,10835,10780,10731,10680,10629,10581,10544,10516,10491,10466,10447,10441,10449,10462,10479,10499,10527,10562,10606,10659,10726,10807,10903,11010,11124,11247,11376,11519,11677,11849,12030,12226,12443,12683,12941,13204,13473,13762,14078,14414,14758,15107,15476,15872,16291,16720,17156,17606,18073,18556,19058,19590
}; // piano


const uint16_t sine_block5[601]={ 20037, 20533, 20996, 21339, 21610, 21874, 22165, 22508, 22874, 23264, 23679, 24069, 24386, 24592, 24703, 24777, 24859, 24964, 25095, 25244, 25399, 25522, 25586, 25716, 25912, 26134, 26391, 26655, 26929, 27216, 27493, 27768, 28034, 28300, 28546, 28779, 29006, 29208, 29394, 29569, 29718, 29851, 29968, 30062, 30138, 30196, 30235, 30253, 30254, 30236, 30203, 30153, 30082, 30000, 29902, 29788, 29657, 29517, 29365, 29195, 29018, 28831, 28626, 28417, 28198, 27963, 27726, 27484, 27232, 26964, 26698, 26427, 26139, 25857, 25568, 25266, 24969, 24669, 24354, 24047, 23738, 23427, 23100, 22787, 22428, 21713, 20446, 18728, 16764, 15066, 13888,
13278, 13118, 13168, 13074, 12444, 11381, 10228, 9415, 8866, 8480, 8523, 8792, 9173, 9577, 9937, 10257, 10545, 10808, 10918, 10636, 9776, 8463, 6937, 5592, 4855, 4684, 4936, 5409, 5977, 6563, 7095, 7462, 7702, 7891, 8079, 8313, 8620, 8976, 9386, 9849, 10328, 10825, 11337, 11820, 11928, 11394, 10244, 8871, 7658, 6959, 6897, 7295, 7998, 8804, 9645, 10473, 11300, 12070, 12816, 13574, 14292, 15002, 15731, 16422, 17106, 17802, 18457, 19097, 19717, 20340, 20905, 21372, 21706, 21918, 22083, 22258, 22456, 22691, 22965, 23254, 23554, 23857, 24167, 24455, 24730, 24995, 25227, 25434, 25616, 25745, 25814, 25905, 26046, 25968, 25376, 23939, 21463,
18659, 16339, 15211, 15117, 15746, 16696, 17735, 18726, 19647, 20428, 21121, 21768, 22342, 22880, 23404, 23880, 24326, 24757, 25131, 25463, 25739, 25932, 26055, 26226, 26285, 26155, 25756, 25088, 24018, 22543, 21085, 20124, 19778, 19931, 20407, 20995, 21600, 22187, 22700, 23161, 23589, 23957, 24281, 24558, 24723, 24893, 25170, 25489, 25830, 26183, 26563, 26934, 27299, 27613, 27783, 27836, 27824, 27807, 27816, 27871, 27970, 28099, 28250, 28421, 28590, 28758, 28928, 29082, 29226, 29364, 29470, 29372, 28745, 27473, 25904, 24347, 23136, 22569, 22519, 22815, 23256, 23727, 24177, 24551, 24865, 25128, 25363, 25560, 25737, 25900, 26044, 26171,
26289, 26386, 26468, 26538, 26528, 26137, 25091, 23386, 21446, 19645, 18347, 17738, 17607, 17769, 18053, 18369, 18679, 18946, 19183, 19398, 19603, 19793, 19976, 20161, 20338, 20509, 20681, 20840, 20991, 21134, 21270, 21375, 21388, 21258, 21029, 20763, 20500, 20291, 20130, 20014, 19939, 19890, 19860, 19842, 19829, 19822, 19816, 19811, 19804, 19793, 19779, 19762, 19740, 19665, 18989, 16827, 13071, 9465, 5777, 2019, 235, 0, 761, 1977, 3307, 4607, 5719, 6693, 7558, 8376, 9115, 9823, 10539, 11212, 11872, 12539, 13165, 13770, 14370, 14922, 15432, 15806, 15903, 15712, 15346, 14926, 14594, 14393, 14338, 14415, 14595, 14862, 15174, 15524, 15898,
16304, 16704, 17106, 17523, 17916, 18301, 18682, 19029, 19348, 19602, 19618, 19249, 18453, 17370, 16427, 15938, 16004, 16469, 17138, 17880, 18559, 19164, 19710, 20161, 20549, 20873, 21113, 21319, 21596, 21901, 22210, 22519, 22840, 23131, 23245, 22892, 22083, 21156, 20329, 19361, 18746, 18584, 18759, 19102, 19504, 19911, 20268, 20587, 20864, 21111, 21301, 21436, 21621, 21839, 22068, 22320, 22566, 22818, 23076, 23322, 23561, 23752, 23839, 23807, 23709, 23593, 23505, 23456, 23445, 23469, 23517, 23585, 23662, 23744, 23829, 23914, 23996, 24073, 24123, 23896, 23134, 21895, 20606, 19603, 19126, 18917, 18957, 19176, 19406, 19599, 19838, 20078, 20293, 20488, 20677, 20846, 21010, 21173, 21325, 21472, 21613, 21752, 21879, 21997, 22113, 22208, 22237, 22142, 21926, 21368, 20047, 18347, 15857, 12558, 9844, 8454, 8196, 8693, 9542, 10491, 11442, 12271, 13010, 13702, 14318, 14904, 15468, 16041, 16585, 17119, 17662, 18174, 18670, 19171, 19634, 20071, 20422, 20548, 20451, 20220, 19949, 19741, 19623, 19606, 19678, 19817, 20015, 20239, 20485, 20756, 21026, 21294, 21560, 21829, 22078, 22314, 22544, 22749, 22937, 23064, 22921, 22337, 21268, 19995, 18856, 18207, 18130, 18470, 19022, 19655, 20235, 20753, 21218, 21610, 21953, 22258, 22542, 22787, 23005, 23200, 23353, 23460, 23526, 23624, 23618, 23319, 22739, 21638, 20233, 18953, 18220, 17998, 18153, 18501, 18919, 19359, 19757, 20125, 20446, 20463, 19927, 18893, 17570, 16408, 15718, 15635, 16030, 16691, 17482, 18168, 18416, 17937, 16877, 15540, 14355, 13679, 13634, 14041, 14744, 15540, 16356, 17183, 17943, 18665, 19382,

}; // more organ




uint16_t sine_length=600; //holds sample size

uint16_t	sine_lut[]= { 33,33, 36, 38, 41, 43, 46, 48, 51, 54, 57, 61, 65, 68, 72, 77, 81, 86, 91, 97, 102, 108, 115, 122, 129, 137, 145, 153, 163, 172, 182, 193, 205, 217, 230, 244,
		258, 273, 290, 307, 325, 344, 365, 387, 410, 434, 460, 487, 516};  // sine + step *32   ,current
uint16_t sine_adder; // sine_lut return
  // sine lookup  ,  multiplier for midi >>14



uint8_t command[5]={254,191,254,128}; // lcd setcursor command
//uint8_t command[5]={168,128,168,128}; // lcd setcursor command

uint8_t command_toggle; // points to lcd command
uint16_t store_c=0;
uint16_t n = 0; // lcd counter
uint8_t rs_toggle = 1; // lcd rs toggle
uint8_t lcddata = 0; // lcd data var
 unsigned short row_toggle=0; // lcd row select
 int32_t sample_Accu[7] ={0,0,0,0,0,0,0};   // accu for sample output or lookup
uint32_t sample_AccuB=0;   // second tone accu
uint16_t sample_Note;  // hold sampleNoteadd value
uint8_t loop_counter; // basic loop schedule

uint16_t sample_pointB; //circular pointer

 uint16_t i;
//static unsigned short lcdit_toggle=1;
 unsigned short lcd_blink; // lcd blinker
uint8_t n_lcd; // lcd var
const char words[10] = "HELLOWORLD";
uint8_t adc_values[5]={15,15,15,15,0} ;  // adc values storage
uint8_t position; // lcd cursor
  uint16_t play_sample[1025]={} ;  //sample storage  , big better
volatile uint16_t sample_point; // pointer for sample array
volatile uint8_t lcd_isr;
 volatile  uint16_t play_hold;
 uint16_t n_calc=0;

 uint16_t sample_pointD;  // global store buffer start

 uint16_t sampling_counter; //  chase counter for sampling
 unsigned short bank_write=1;


uint16_t adsr_time[11];
uint16_t note_channel[31]={ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};  // 10 notes , 10 velocity 20->16bit , 10 mask toggle
 unsigned short mask_result;

 uint16_t adc_source[4] ;
const uint8_t lfo_mask[20]={255,170,85,252,240,192,128,136,238,15,0,0,0,0,0,0,0}; // lfo dif masks
uint16_t sample_lp;  //low pass accu
uint8_t sample_lpbuf[512]; // sample average buff


uint8_t adsr_counter[11]; // hold count from 0-31 max 10+10+10 =ADR,  +1 on posedge then stop when above count value
const uint16_t adsr_lut[41]= {0,1,2,3,4,5,6,8,9,10,12,14,17,20,24,28,32,37,42,47,53,59,65,72,79,86,96,104,114,128,144,178,228,342,409,455,512,585,682,819,1024}; //count up 1=10 , /9 for output

uint8_t adsr_step;
uint8_t adsr_up; //up counter for adsr
uint16_t adsr_isr; // holds adsr isrCount
uint8_t adsr_test=0;   // select if enable upcount
unsigned short adsr_retrigger[11]={0,0,0,0,0,0,0,0,0,0,0}; // adsr auto retrigger or wait for note start , if 0 zero big problem
uint8_t note_enabled=0;  // simple flag for note on seq_pos
uint16_t next_isr; //isrcount ahead
uint8_t adsr_toggle[11]={0,0,0,0,0,0,0,0};   // needs to be 0 or it never starts
uint16_t  note_vol[11]={0,0,0,0,0,0,0,0,0,0};  // note volumes and mask toggle
uint8_t note_holdA;
uint8_t note_holdB;

int32_t sample_accub;
int32_t filter_out[7]={0,0,0,0,0,0,0};	// big out
int32_t filter_in[5]={0,0,0,0,0};  // small values come in

int32_t sample_accus[6]={0,0,0,0,0,0};  // sample array
uint8_t filter_counter=0;
uint8_t flip_list=0;  // line flipping for display
							// 5,6 volume
//uint8_t pot_stored[64] = {114,66,1,50,103,0,160,0,106,106,138,132,122,138,0,0,20,34,20,96,0,0,0,0,22,80,17,2,65,50,0,0,0,0,0,48,49,48,0,0,48,128,9,0,1,0,151,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,16,};
uint8_t pot_stored[130] ={29,15,26,19,12,16,15,0,7,16,1,0,1,1,1,0,40,19,40,17,35,40,40,31,40,24,40,36,29,1,39,24,7,2,4,0,4,17,16,7,0,0,0,0,0,0,0,0,16,23,3,19,19,18,18,22,22,10,25,8,0,0,0,0,5,0,4,0,4,1,4,0,9,0,10,29,0,0,0,0,0,16,0,16,0,0,16,0,0,16,0,0,12,16,0,0,10,23,36,27,1,7,0,4,0,39,28,30,20,14,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0};
// seems to send odd data with no audio output for lcd ????????
uint8_t one_shot; // up counter for attack sample
uint8_t cust_char[64];  // holder for custom characters 8x5bit x8 ,Send command: 0x40 (Set pointer to the first CGRAM address).

uint16_t enc_temp;  // hold in for tim2
uint16_t enc_tempB;  // hold old in
uint16_t enc2_temp;  // hold in for tim4
uint16_t enc2_tempB;  // hold old in2
int16_t enc2_dir;    // enc direction2
int16_t enc_dir;    // enc direction
uint16_t tempo_count=0; // up counter for tempo reset
 uint8_t bsrr_hold[128]; //store to hold bsrr data ready for transfer in a loop
uint8_t bs_count=0; //count to 40 on bsrrr

uint8_t menu_store=1;
uint16_t cursor_pos[2];
unsigned short lcd_send=0;
//spi pins
unsigned short cs_pin=1;
uint8_t clk_pin=0;
unsigned short mosi_pin=0;
unsigned short spi_enable=0;
unsigned short spi_state=0; //select data or command 0= command
uint8_t bsr_out;
uint8_t spi_counter=0; // tracks spi progress
uint8_t spi_byte; //incoming data
uint16_t spi_hold; // temp hold for incoming
uint8_t spi_send=0; // keeps track of send process
uint8_t init=0; //init counter
uint8_t samp_temp=0;
uint32_t bsrr_long;
uint32_t bsrr_longB;
uint8_t seq_spi;
uint8_t init_b;
uint32_t bsrr_seq[49]; // holds bits for one byte spi
uint8_t bsrr_counter;
uint8_t cursor_menu[3]; // holds cursor position
uint8_t spi_toggle; // toggle between command a data for writing characters
uint8_t cursor_blink=0; // blink position  blink 1 or 2 or off
uint16_t lfo_base[3]; // new lfo multi
uint32_t lfo_send[10];
uint8_t mem_count; //just count mem address
uint32_t loop_counter2; //long counter
uint8_t buffer[4];
uint8_t mem_buf;
uint8_t menu_page[3]; // switch between pages, keep track when flipping
uint8_t seq_loop[7]; //loop positions

//new stuff///////////////////////////////////////////////////////////////////////////////////////////////////////
float filter_accus[15];  // hold floats for filter
float filter_hold[5];  //holds some feedback related stuff
float freq_point[4]; // multiplier coeff
uint16_t lfo_value[5]={0,0,0,0}; //  lfo value hold
int16_t lfo_output[5]; // lfo out 0-2047 normally
float freq_lookup; // calculate step from midi frequencies for samples

uint16_t lfo_accu[10]; //holds last lfo value , simple 0-255 upcount for now,will change;
int16_t lfo_out[10];
uint16_t tempo_lut[162]; // tempo look up 40-200bpm
////////////////////////////////////////////////////////////////////////////////////////////////////////////GFX
//uint8_t gfx_ram[64][16] ; //holds data for lcd 64*256bit , display 16kbyte/sec so 8fps , maybe use dma for memory transfers
char gfx_ram[1025] ;


// tryin mem with char
const char gfx_char[1024]={   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0x00,     // 8*5 fonts vert aligned
	     0x0E, 0x11, 0x13, 0x15, 0x19, 0x11, 0x0E,  0x00,
	     0x04, 0x0C, 0x04, 0x04, 0x04, 0x04, 0x0E,  0x00,
	     0x0E, 0x11, 0x01, 0x02, 0x04, 0x08, 0x1F,  0x00,
	     0x1F, 0x02, 0x04, 0x02, 0x01, 0x11, 0x0E,  0x00,
	     0x02, 0x06, 0x0A, 0x12, 0x1F, 0x02, 0x02,  0x00,
	     0x1F, 0x10, 0x1E, 0x01, 0x01, 0x11, 0x0E,  0x00,
	     0x06, 0x08, 0x10, 0x1E, 0x11, 0x11, 0x0E,  0x00,
	     0x1F, 0x11, 0x01, 0x02, 0x04, 0x04, 0x04,  0x00,
	     0x0E, 0x11, 0x11, 0x0E, 0x11, 0x11, 0x0E,  0x00,
	     0x0E, 0x11, 0x11, 0x0F, 0x01, 0x02, 0x0C,  0x00,
	     0x00, 0x0C, 0x0C, 0x00, 0x0C, 0x0C, 0x00,  0x00,
	     0x00, 0x0C, 0x0C, 0x00, 0x0C, 0x04, 0x08,  0x00,
	     0x02, 0x04, 0x08, 0x10, 0x08, 0x04, 0x02,  0x00,
	     0x00, 0x00, 0x1F, 0x00, 0x1F, 0x00, 0x00,  0x00,
	     0x08, 0x04, 0x02, 0x01, 0x02, 0x04, 0x08,  0x00,
	     0x0E, 0x11, 0x01, 0x02, 0x04, 0x00, 0x04,  0x00,
	     0x0E, 0x11, 0x01, 0x0D, 0x15, 0x15, 0x0E,  0x00,
	     0x0E, 0x11, 0x11, 0x11, 0x1F, 0x11, 0x11,  0x00,
	     0x1E, 0x11, 0x11, 0x1E, 0x11, 0x11, 0x1E,  0x00,
	     0x0E, 0x11, 0x10, 0x10, 0x10, 0x11, 0x0E,  0x00,
	     0x1C, 0x12, 0x11, 0x11, 0x11, 0x12, 0x1C,  0x00,
	     0x1F, 0x10, 0x10, 0x1E, 0x10, 0x10, 0x1F,  0x00,
	     0x1F, 0x10, 0x10, 0x1E, 0x10, 0x10, 0x10,  0x00,
	     0x0E, 0x11, 0x10, 0x17, 0x11, 0x11, 0x0F,  0x00,
	     0x11, 0x11, 0x11, 0x1F, 0x11, 0x11, 0x11,  0x00,
	     0x0E, 0x04, 0x04, 0x04, 0x04, 0x04, 0x0E,  0x00,
	     0x07, 0x02, 0x02, 0x02, 0x02, 0x12, 0x0C,  0x00,
	     0x11, 0x12, 0x14, 0x18, 0x14, 0x12, 0x11,  0x00,
	     0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x1F,  0x00,
	     0x11, 0x1B, 0x15, 0x15, 0x11, 0x11, 0x11,  0x00,
	     0x11, 0x11, 0x19, 0x15, 0x13, 0x11, 0x11,  0x00,
	     0x0E, 0x11, 0x11, 0x11, 0x11, 0x11, 0x0E,  0x00,
	     0x1E, 0x11, 0x11, 0x1E, 0x10, 0x10, 0x10,  0x00,
	     0x0E, 0x11, 0x11, 0x11, 0x15, 0x12, 0x0D,  0x00,
	     0x1E, 0x11, 0x11, 0x1E, 0x14, 0x12, 0x11,  0x00,
	     0x0F, 0x10, 0x10, 0x0E, 0x01, 0x01, 0x1E,  0x00,
	     0x1F, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,  0x00,
	     0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x0E,  0x00,
	     0x11, 0x11, 0x11, 0x11, 0x11, 0x0A, 0x04,  0x00,
	     0x11, 0x11, 0x11, 0x15, 0x15, 0x15, 0x0A,  0x00,
	     0x11, 0x11, 0x0A, 0x04, 0x0A, 0x11, 0x11,  0x00,
	     0x11, 0x11, 0x11, 0x0A, 0x04, 0x04, 0x04,  0x00,
	     0x1F, 0x01, 0x02, 0x04, 0x08, 0x10, 0x1F,  0x00,
	     0x1C, 0x10, 0x10, 0x10, 0x10, 0x10, 0x1C,  0x00,
	     0x11, 0x0A, 0x1F, 0x04, 0x1F, 0x04, 0x04,  0x00,
	     0x0E, 0x02, 0x02, 0x02, 0x02, 0x02, 0x0E,  0x00,
	     0x04, 0x0A, 0x11, 0x00, 0x00, 0x00, 0x00,  0x00,
	     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F,  0x00,
	     0x08, 0x04, 0x02, 0x00, 0x00, 0x00, 0x00,  0x00,
	     0x00, 0x00, 0x0E, 0x01, 0x0F, 0x11, 0x0F,  0x00,
	     0x10, 0x10, 0x16, 0x19, 0x11, 0x11, 0x1E,  0x00,
	     0x00, 0x00, 0x0E, 0x10, 0x10, 0x11, 0x0E,  0x00,
	     0x01, 0x01, 0x0D, 0x13, 0x11, 0x11, 0x0F,  0x00,
	     0x00, 0x00, 0x0E, 0x11, 0x1F, 0x10, 0x0E,  0x00,
	     0x06, 0x09, 0x08, 0x1C, 0x08, 0x08, 0x08,  0x00,
	     0x00, 0x0F, 0x11, 0x11, 0x0F, 0x01, 0x0E,  0x00,
	     0x10, 0x10, 0x16, 0x19, 0x11, 0x11, 0x11,  0x00,
	     0x04, 0x00, 0x0C, 0x04, 0x04, 0x04, 0x0E,  0x00,
	     0x02, 0x00, 0x06, 0x02, 0x02, 0x12, 0x0C,  0x00,
	     0x10, 0x10, 0x12, 0x14, 0x18, 0x14, 0x12,  0x00,
	     0x0C, 0x04, 0x04, 0x04, 0x04, 0x04, 0x0E,  0x00,
	     0x00, 0x00, 0x1A, 0x15, 0x15, 0x11, 0x11,  0x00,
	     0x00, 0x00, 0x16, 0x19, 0x11, 0x11, 0x11,  0x00,
	     0x00, 0x00, 0x0E, 0x11, 0x11, 0x11, 0x0E,  0x00,
	     0x00, 0x00, 0x1E, 0x11, 0x1E, 0x10, 0x10,  0x00,
	     0x00, 0x00, 0x0D, 0x13, 0x0F, 0x01, 0x01,  0x00,
	     0x00, 0x00, 0x16, 0x19, 0x10, 0x10, 0x10,  0x00,
	     0x00, 0x00, 0x0E, 0x10, 0x0E, 0x01, 0x1E,  0x00,
	     0x08, 0x08, 0x1C, 0x08, 0x08, 0x09, 0x06,  0x00,
	     0x00, 0x00, 0x11, 0x11, 0x11, 0x13, 0x0D,  0x00,
	     0x00, 0x00, 0x11, 0x11, 0x11, 0x0A, 0x04,  0x00,
	     0x00, 0x00, 0x11, 0x15, 0x15, 0x15, 0x0A,  0x00,
	     0x00, 0x00, 0x11, 0x0A, 0x04, 0x0A, 0x11,  0x00,
	     0x00, 0x00, 0x11, 0x11, 0x0F, 0x01, 0x0E,  0x00,
	     0x00, 0x00, 0x1F, 0x02, 0x04, 0x08, 0x1F,  0x00,
	     0x02, 0x04, 0x04, 0x08, 0x04, 0x04, 0x02,  0x00,
	     0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,  0x00,
	     0x08, 0x04, 0x04, 0x02, 0x04, 0x04, 0x08,  0x00,
	     0x00, 0x04, 0x02, 0x1F, 0x02, 0x04, 0x00,  0x00,
	     0x00, 0x04, 0x08, 0x1F, 0x08, 0x04, 0x00,  0x00
	      }; //various alphabet and other characters,for now 5*8 , 26*8 bytes+others , will update

//uint8_t gfx_char[2048]={};
uint16_t adsr_countup[11];  //holds isr count on notes , 
	float adsr_level[11]; //float for vol envelope  ,ps 20 21
float midi_lut[51]={65.4064,0}; //start with C2-c6
float sample_build[11]; // float based sample accu for interpolation
uint8_t popup_ref2; // store text pointer for feedback


uint16_t gfx_counter[6]={0,0,0,0,0}; // just upcounter for gfx ram bytes
uint8_t gfx_skip=1;  // important
uint8_t gfx_blink=0; // blinker counter
uint16_t lcd_out3; //for feedback
//  USE THE BREAK WITH SWITCH STATEMENT MORON!!!


/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_ADC1_Init();
  MX_SPI2_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  MX_I2C2_Init();
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */

  const volatile uint8_t *userConfig=(const volatile uint8_t *)0x0800D2F0;





//LL_SPI_Enable(SPI2);

  HAL_SPI_Init(&hspi2);
  // lcd_init(); // keep this late or have issues
//HAL_TIM_Base_Start_IT(&htim1);  // This needs to work for irq   ,disbling tim1 made loop a lot faster
//TIM1->CCER=0;
HAL_TIM_Base_Start_IT(&htim3);  // This needs to work for irq
TIM3->CCER=0;
HAL_TIM_Base_Start(&htim2);
HAL_TIM_Base_Start(&htim4);
//HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_1);  // they both work fine together
HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_3);
//HAL_TIM_PWM_Start_DMA(&htim3, TIM_CHANNEL_4,pData, 63);
TIM2->CNT=32000;
HAL_ADC_Start(&hadc1);
HAL_ADC_Start_DMA(&hadc1, adc_source, 3); //dma start ,needs this and adc start


HAL_I2C_MspInit(&hi2c2);

/*
for(i=0;i<128;i++){			// read values from stored
mem_buf=0;
HAL_I2C_Mem_Read(&hi2c1, 160, ((1+(i>>6))<<6)+(i&63), 2,  &mem_buf, 1, 1000); // add 160 is correct  // only single byte works for now
potSource[i]=mem_buf;
potValues[i]=mem_buf>>2;

} // reads stored values for potvalues
*/

uint8_t potSource2[65];
uint8_t potSource3[65];
HAL_I2C_Mem_Read(&hi2c2, 160, (1<<6), 2,&potSource, 64,1000);		// all good
HAL_I2C_Mem_Read(&hi2c2, 160, (2<<6), 2,&potSource2,64,1000);
HAL_I2C_Mem_Read(&hi2c2, 160, (3<<6), 2,&potSource3,64,1000);

for(i=0;i<1024;i++){
	gfx_ram[i]=gfx_char[((i&15)<<3)+((i>>4)&7)+8+(((i>>7)*128)&511)]; // ok
//	gfx_ram[i&63] [i>>6]  =gfx_char[8+(i&7)+(((i>>3)*8)&63)]; //last one
//gfx_ram[i]  =240; // 0-16 16-32 
}

for(i=0;i<64;i++){
	potSource[i+64]=potSource2[i];
}

for(i=0;i<64;i++){
	potSource[i+128]=potSource3[i];
}

for(i=0;i<180;i++){
	potValues[i]=potSource[i]>>4;
}

float tempo_hold;  // calculate tempo look up
for (i=0;i<161;i++) {

tempo_hold=(i+180)*0.0166666666;

tempo_hold=	1/tempo_hold;
tempo_hold=	tempo_hold*2187.6;	

tempo_lut[i]=tempo_hold;
}
isrMask=571; // def tempo 571=180bpm , 20 ms /isrcount

noteTiming=24;


for (i=1;i<51;i++) {   //midi note lookup freq table/float 0-c2(65.hz)   , careful
midi_lut[i]=midi_lut[i-1]*1.059463;	//basic note freq calc 
	
}



for (i=0;i<320;i++)	{	// write C into whole section,useful
	spell[i]=67;

}

for (n=0;n<64;n++)	{

	//menuSelect=n*2;menuSelectX=i;displayBuffer();
init_b=n;

displayBuffer();
}

menuSelect=0;
// fill up sample
firstbarLoop=0;



  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  loop_counter++ ;
	  loop_counter2++;//
	  // if (menu_page<320) lcd_feedback();  //curious no issues with lcd without this  , maybe spell writing
	  if (loop_counter2==9096) {    //   4096=1min=32bytes so 4mins per 128 bank or 15 writes/hour

	  	mem_buf=potSource[mem_count];
	  	// read values from stored
	  	HAL_I2C_Mem_Write(&hi2c2, 160, ((1+(mem_count>>6))<<6)+(mem_count&63), 2, &mem_buf, 1, 1000);
	  	HAL_Delay(5);
	  	if (mem_count==255) mem_count=0; else mem_count++;  // write to first
	  	loop_counter2=0; //reset

	  }

	  displayBuffer();
	  analoginputloopb(); // no delay
	//  for (i=0;i<7;i++) {display_init();}
	  if (init<6)
{
	  for (i=0;i<6;i++) {display_init();}  //1-2ms ?  change length if flickering ,maybe initial data
} else display_update();

	  ///////////////////////////////////////////////////////////////////////////////

	  if (loop_counter & 255)	{ // grab adc readings + 3ms , 32 step
	  	for (i=0;i<3;i++) {

	  	adc_values[2-i]= (adc_source[i]>>7) &31;
	  }
	  	loop_counter=0;
	  }

	  if ((seq_pos==7) && (lcd_send==0)) {lcd_send=1;} // runs just once
	  /*



	   if (promValue<64) promValue=promValue+1 ; else promValue=0;  // fetch eeprom   nogo
	  	  if ((promValues[promValue] ) !=(potValues[promValue]))  EEPROM.write(promValue,(potValues[promValue]));   //  not too happy can totally kill speed  will have to put elsewhere
	  	  promValues[promValue] =potValues[promValue];
	  	   */

	  	     HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin,(seq_pos & 1)); // easy skip ?
	  	    // very inconsistent

	  if (sample_point>511) sample_pointD=0; // loop when zero cross , good but limited, works ok
	  if (sample_point<511)  sample_pointD=512;


	  if (sample_pointB!=sample_pointD) bank_write=1; // set start of buffer ,grab , works ok
	  if (bank_write){

	  	sampling();
	  /*	HAL_SPI_Transmit(&hspi2,(uint8_t *)248,1,1);
	  	HAL_SPI_Transmit(&hspi2,(uint8_t *)64,1,1);
	  	HAL_SPI_Transmit(&hspi2,(uint8_t *)0,1,1);
	  */

	  } //works ok, write opp bank





	    }  // while loop





  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 100;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */
  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.ScanConvMode = ENABLE;
  hadc1.Init.ContinuousConvMode = ENABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 3;
  hadc1.Init.DMAContinuousRequests = ENABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SEQ_CONV;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_3;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_480CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_4;
  sConfig.Rank = 2;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_5;
  sConfig.Rank = 3;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief I2C2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C2_Init(void)
{

  /* USER CODE BEGIN I2C2_Init 0 */

  /* USER CODE END I2C2_Init 0 */

  /* USER CODE BEGIN I2C2_Init 1 */

  /* USER CODE END I2C2_Init 1 */
  hi2c2.Instance = I2C2;
  hi2c2.Init.ClockSpeed = 400000;
  hi2c2.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c2.Init.OwnAddress1 = 0;
  hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c2.Init.OwnAddress2 = 0;
  hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C2_Init 2 */

  /* USER CODE END I2C2_Init 2 */

}

/**
  * @brief SPI2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI2_Init(void)
{

  /* USER CODE BEGIN SPI2_Init 0 */

  /* USER CODE END SPI2_Init 0 */

  /* USER CODE BEGIN SPI2_Init 1 */

  /* USER CODE END SPI2_Init 1 */
  /* SPI2 parameter configuration*/
  hspi2.Instance = SPI2;
  hspi2.Init.Mode = SPI_MODE_MASTER;
  hspi2.Init.Direction = SPI_DIRECTION_2LINES;
  hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi2.Init.NSS = SPI_NSS_SOFT;
  hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_128;
  hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi2.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI2_Init 2 */

  /* USER CODE END SPI2_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_Encoder_InitTypeDef sConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 0;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 65535;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV4;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  sConfig.EncoderMode = TIM_ENCODERMODE_TI1;
  sConfig.IC1Polarity = TIM_ICPOLARITY_FALLING;
  sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC1Filter = 2;
  sConfig.IC2Polarity = TIM_ICPOLARITY_FALLING;
  sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC2Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC2Filter = 2;
  if (HAL_TIM_Encoder_Init(&htim2, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 0;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 2857;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM2;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */
  HAL_TIM_MspPostInit(&htim3);

}

/**
  * @brief TIM4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM4_Init(void)
{

  /* USER CODE BEGIN TIM4_Init 0 */

  /* USER CODE END TIM4_Init 0 */

  TIM_Encoder_InitTypeDef sConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM4_Init 1 */

  /* USER CODE END TIM4_Init 1 */
  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 0;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 65535;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  sConfig.EncoderMode = TIM_ENCODERMODE_TI1;
  sConfig.IC1Polarity = TIM_ICPOLARITY_FALLING;
  sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC1Filter = 2;
  sConfig.IC2Polarity = TIM_ICPOLARITY_FALLING;
  sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC2Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC2Filter = 2;
  if (HAL_TIM_Encoder_Init(&htim4, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM4_Init 2 */

  /* USER CODE END TIM4_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA2_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA2_Stream0_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : LED_Pin */
  GPIO_InitStruct.Pin = LED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
/* USER CODE BEGIN 4 */
void SPI_command(void){
// pins are all good , SPI is always the problem!!!
if ((spi_enable==0)&& (spi_send==1)){				//if data sent and next byte is ready ,creates the actual bytes sent

uint8_t spi_store[5];

	//clk_pin=(i&1)^1;
		//bsrr_long=0;
		if (spi_hold>>8) spi_byte=248; else {spi_byte=250;}  //start with msb ,dont forget flip around at end, 250 =data ,248= command if below 8bit
/*		bsrr_long=spi_byte<<16; // top byte for command 24-16, then 2 bytes of data 16-0
		spi_hold=(spi_hold&255);
		bsrr_long=bsrr_long+((spi_hold>>4)<<12); // 4 bits to top
		bsrr_long=bsrr_long+((spi_hold&15)<<4);	// 4 bits to top
*/
	spi_store[0]=spi_byte&255;
	spi_store[1]=((spi_hold>>4)<<4)&255;
	spi_store[2]=((spi_hold&15)<<4)&255;

// send this to spi for now
		HAL_SPI_Transmit(&hspi2, (uint8_t *)spi_store, 3, 100);  // working good


//HAL_Delay(10);
	spi_enable=1; }
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)    // unreliable

{

	sample_point=sample_point & 1023;// this is 1
	play_hold=play_sample[sample_point]; // this is 2

		if(TIM3==htim->Instance)			// nothing here is consistent
{


TIM3->CCR3=play_hold ;  // keep readin sample storage

/* if ((sample_point&7)==6) {


	if (spi_send==2){ GPIOB->BSRR =bsrr_seq[bsrr_counter];bsrr_counter++;}  // send spi when loaded up , this works , too fast however will fail

	if ((bsrr_counter==49) && (spi_send==2)) {bsrr_counter=0; spi_send=0;}
}


*/





//if (sample_point&1) {  // this is needed   , slow down also read only when not written to

	//bsr_out=bsrr_hold[bs_count+ (menu_store*80)]; // send port data to holder

	// if (lcd_send) GPIOB->BSRR =(bsr_out<<10) + (~(bsr_out) <<26);  // write to port but not too early

//if (spi_enable) {GPIOB->BSRR =(bsrr_hold[samp_temp]<<13)+(~(bsrr_hold[samp_temp])<<29); }  // send spi data when enabled

			//	if (spi_enable){ HAL_GPIO_WritePin(MOSI_GPIO_Port, MOSI_Pin,((bsrr_hold[samp_temp]>>1))); HAL_GPIO_WritePin(CLK_GPIO_Port, CLK_Pin,((bsrr_hold[samp_temp])&1));}


//samp_temp++;
//if ((samp_temp)==48) {spi_enable=0;spi_send=0;samp_temp=0;}  // finish , enable next char
			//	 HAL_GPIO_WritePin(RS1_GPIO_Port, RS1_Pin,1);
		 	 	//  HAL_Delay(1);




//if (bs_count==255
	//	)  { bs_count=0; menu_store=(menuSelect>>2); } else bs_count++; // menu select, higher count just spills over  159 chamge to 128 2*64
//}
sample_point++; //this needs to be here or too fast and wrong sample rate

}

}


 void adc_read(void){		HAL_ADC_Start(&hadc1);				//disabled
		if (HAL_ADC_PollForConversion(&hadc1, 1) == HAL_OK)      // 1ms time out
				{
		adc_values[2] = HAL_ADC_GetValue(&hadc1);
					}  // get value
		 HAL_ADC_Start(&hadc1); // need for every conversion
		if (HAL_ADC_PollForConversion(&hadc1, 1) == HAL_OK)      // 1ms time out
				{
		adc_values[1] = HAL_ADC_GetValue(&hadc1);
					}  // get value
	 	HAL_ADC_Start(&hadc1);
		if (HAL_ADC_PollForConversion(&hadc1, 1) == HAL_OK)      // 1ms time out
				{
			adc_values[0] = HAL_ADC_GetValue(&hadc1);
				}  // get value this works
		HAL_ADC_Stop(&hadc1);
for (i=0;i<3;i++){ adc_values[i]= (adc_values[i]>>8) &15; } // reduce value to 4 bit

}

void analoginputloopb(void){  //works fine still
uint16_t menu_holder;


	//menuSelect = (7 - (adc_values[0] >>2))<<1;		//x *16  main menu select
	//menuSelect = (15 - (adc_values[0] >>1));		//x *7  main menu select
	//menuSelectX=(31 - adc_values[1])>>1;  // Y select inside page
	menuSelect = cursor_menu[2]>>4;		//x *7  main menu select
		menuSelectX=cursor_menu[2]&15;  // Y select inside page




	//	counterVarB = menuSelectX + menuSelect; // select mem page 10*8  and location pointer  77
//		if (adc_values[1]>15)	menu_holder=disp_lut [(menuSelect)	+1] [31-adc_values[1]]; // change back to 0-15
//	else menu_holder=disp_lut [(menuSelect)	] [15-adc_values[1]];  // grab disp lut value for pointer if valid then write for now

		//menu_holder=disp_lut [(menuSelect)	] [(15-adc_values[1])];   // value from disp lut
		menu_holder=disp_lut [menuSelect] [menuSelectX];   // value from disp lut
		cursor_menu[1]=0;
		//cursor_menu[2]=cursor_lookup[enc2_dir];
		// cursor_menu[2]=cursor_lookup[enc2_dir]; //needed for correct line sequence ,obsolete
		cursor_menu[2]=enc2_dir;
		
		//cursor_menu[2]=(16-adc_values[1]);



		if (menu_holder>127)	counterVarB=menu_holder-128; //  points to actual potvalues location from dsip_lut when value is higher than 127 , works ok problem with menu display
				enc_dir=potSource[counterVarB];

	enc_temp=(TIM2->CNT)>>1;  // read counter tim2
	enc2_temp=(TIM4->CNT)>>1;  // read counter tim4
	if  (enc_temp>enc_tempB)	 enc_dir++;   // start settle timer , will do 2 times per turn always
	if (enc_temp<enc_tempB)	 enc_dir--;

			if (enc_dir>160) enc_dir=160;
			if (enc_dir<0) enc_dir=0;
			potSource[counterVarB]=enc_dir;
					enc_dir=0;
			enc_tempB=enc_temp;




			if  (enc2_temp>enc2_tempB)	 enc2_dir++;   // start settle timer , will do 2 times per turn always
			if (enc2_temp<enc2_tempB)	 enc2_dir--;

					if (enc2_dir>126) enc2_dir=126;
					if (enc2_dir>63) menu_page[1]=1; else menu_page[1]=0;
					if (enc2_dir<0) enc2_dir=0;

							//enc2_dir=0;
					enc2_tempB=enc2_temp;


potValues[counterVarB]=(potSource[counterVarB]>>4) & 15 ;  // reduce values for now ,use original for others

}
void display_init(void){
	//uint8_t sp2_command=0xf8+(0<<1);


		 // uint16_t disp[]={  304,304,304,270,257,268,384,258,51,51,52,53,54,55,56,57,58,0};  // normal characters
		 uint16_t disp[]={  304,268,257,262,308,310,310,51,51,52,53,54,55,56,57,58,0};  // this should work for gd
		  if (spi_send==0){         // sets data byte

if ((enc2_dir>63) && (menu_page[2]==0))  {init=4;menu_page[2]=1;}  //restart page draw for second page , might need home pos init
if  ((init==76) && (menu_page[2]==1)) menu_page[2]=2; //after drawing second page and finish
if ((enc2_dir<63) && (menu_page[2]==2))  {init=4;menu_page[2]=0;}

switch(init){     //Remember every line advances +char on display

case 77: spi_hold=384+(init_b>>1);init=init+((init_b&1)*2);break; // no delay ,jump to either blink sets

case 78: spi_hold=10;break; //write one set then jump
case 79: spi_hold=spell[(init_b&62)+1];init=81;break;
case 80: spi_hold=spell[init_b&62];break; // or write other set then jump
case 81: spi_hold=10;init=81;break;


case 82: spi_hold=384+(init_b>>1);break;  // finish writes
case 83: spi_hold=spell[init_b&62];break;
case 84 : spi_hold=spell[(init_b&62)+1];init_b=cursor_menu[2]&63;displayBuffer ();init=76; break; //update cursor and displaybuffer
default : init_b=init-6;displayBuffer ();spi_hold=spell[init_b];break; //initial menu write either page ,skip after
}


if (init<6)	{HAL_Delay(30);spi_hold=disp[init] ;  init_b=cursor_menu[2]; } //delay needs to go

init++;
spi_send=1;SPI_command();spi_send=0;spi_enable=0;
		  }
/////////////////////////////////////////////////////////////////////////////
		  /*		  if (spi_enable)			// load up one byte , works good
		  {
			 for (samp_temp=0;samp_temp<48;samp_temp++){      // convert bytes to bits

			  clk_pin=(samp_temp&1)^1; // start on high

		  bsrr_longB=bsrr_long>>(((48-samp_temp)>>1));   // need to repeat bit for clock
		  bsr_out=bsrr_longB&1;
		  bsr_out=(bsr_out<<2)+clk_pin; //chmaged for other pin

		 // {GPIOB->BSRR =(bsr_out<<13)+(~(bsr_out)<<29); }  // send spi data when enabled
		  {bsrr_seq[samp_temp] =(bsr_out<<13)+(~(bsr_out)<<29); }  // send spi data when enabled ,fills up bsrr hold
		//  samp_temp++;
		  //	if ((samp_temp)==48)
			 }
				 spi_enable=0;spi_send=2;samp_temp=0; // finish , enable next char
  spi_send=0; // bypass the bsrr stuff
		  /*
		 while((spi_send==2) && ((sample_point & 3)==3))   {    //send bits for a single command , seem to time alright
		  //if (spi_send==2){ GPIOB->BSRR =bsrr_seq[bsrr_counter];bsrr_counter++;}  // send spi when loaded up , this works , too fast however will fail
	// GPIOB->BSRR =bsrr_seq[bsrr_counter];bsrr_counter++;  // send spi when loaded up , this works , too fast however will fail ,disable for now
			 bsrr_counter++; // add this or stuck
		  //if ((bsrr_counter==49) && (spi_send==2)) {bsrr_counter=0; spi_send=0;}
		  if (bsrr_counter==49)  {bsrr_counter=0; spi_send=0;}

		 }

*/

}
void display_gfx(void){   // new display driver , all gfx based



}

void display_update(void){				//spi display updater code , all gfx , works pretty ok

	uint8_t spi_store[5];
/*
switch (gfx_skip) { // 1-17  one line


case 1 : {spi_hold=((gfx_counter[0])&31)+384;gfx_counter[4]=(gfx_counter[0]>>5);  gfx_counter[0]=(gfx_counter[0]+1)&63 ;gfx_skip++;break ;}
case 2  : {	spi_hold=384+(gfx_counter[4]*8);gfx_skip++;break ;  }
case 3   : gfx_counter[3]=0; gfx_skip++;break;
case 18 : gfx_counter[2]=(gfx_counter[2]+1) &63; gfx_counter[3]=0;gfx_skip=1;break;
default :  gfx_counter[3]=(gfx_counter[3]+1)&15;gfx_skip++;break;

}

 original good one
*/

switch (gfx_skip) { // 1-17  one line


case 1 : {spi_hold=((gfx_counter[0])&31)+384;gfx_counter[4]=(gfx_counter[0]>>5);  gfx_counter[0]=(gfx_counter[0]+1)&63 ;gfx_skip++;break ;} // 0-63 lines 0-31 *2 
case 2  : {	spi_hold=384+(gfx_counter[4]*8);gfx_skip++;break ;  } // after 32 lines it's +8 
case 3   : gfx_counter[3]=0; gfx_skip++;break;  // not good
//case 18 : gfx_counter[2]=(gfx_counter[2]+1) &63; gfx_counter[3]=0;gfx_skip=1;break; // jumps to next line but it also skips print so last byte twice
default :  gfx_counter[3]=(gfx_counter[3]+1)&15;gfx_skip++;break; // problem starts on 1 

}

if ((gfx_skip==2) || (gfx_skip==3) || (gfx_skip==1)) spi_hold=spi_hold; else spi_hold=gfx_ram[gfx_counter[3]+(gfx_counter[2]*16)] ;  // all good , last repeat though still or first repeats
if (gfx_skip==19) {gfx_counter[2]=(gfx_counter[2]+1) &63; gfx_counter[3]=0;gfx_skip=1;}  //advance line , all good
		if (spi_hold>>8) spi_byte=248; else {spi_byte=250;}  //start with msb ,dont forget flip around at end, 250 =data ,248= command if below 8bit

		spi_store[0]=spi_byte&255;
		spi_store[1]=((spi_hold>>4)<<4)&255;
		spi_store[2]=((spi_hold&15)<<4)&255;

	// send this to spi for now
			HAL_SPI_Transmit(&hspi2, (uint8_t *)spi_store, 3, 100);  // working good ,blocking


}



void displayBuffer (void){        //  only 1 char per round for now ,works good
//const char *popup_lut [20]= { "Pitch" , "Attack","Sustain","LFO Speed","LFO Gain","Tempo" ,"LPF1","LPF2","Pitch 1" ,"Pitch 2","      "}; // info line 

uint8_t d_count;
uint8_t init_x=((init_b>>4)<<3);  // /16 *16 so +16 after every 16 bytes
uint8_t init_y=(init_b&15); //0-15 cont char

uint16_t store_x;


store_c= disp_lut [(init_b+(cursor_menu[2]&128))>>4]  [init_y] ;  //gets potvalues pointer from menus ,works
if (init_b==enc2_dir) {lcd_out3=potSource[store_c-128];popup_ref2=popup_ref[init_b>>4] [init_b&15]; }// get pointer for ref


	 if (store_c==64) store_c=47;
	if ((store_c>127)&& (store_c<255))  {store_c= potValues[store_c&127]+48;}		// sets data or stored
	if (store_c>254){store_c= potValues[store_c-128]+48;}

if (init_b>117)  store_c=popup_lut[popup_ref2][init_b-118]; // copy text 

store_c=store_c-47; store_c = store_c &127;	  // spell no longer ?

if ((seq_pos&1) && (init_b==enc2_dir)) {if (store_c) {  store_c=0;} else store_c=48;} 


store_x=(store_c*8);


//uint64_t gfx_src;
//uint64_t gfx_dest;

for (d_count=0;d_count<8;d_count++){
					//gfx_ram[d_count+init_x] [init_y] = gfx_char[d_count+store_x]; //write character to ram maybe change to memcpy

gfx_ram[((d_count+init_x)*16)+init_y] =gfx_char[d_count+store_x]; //ok 

}
// gfx_src=gfx_char[d_count+store_x]<<(d_count*8);
	
	
	
	//memcpy(*(gfx_dest+init_y),  *(gfx_src+store_x), 8);

	if (init_b==127) init_b=0; else init_b++;   // make sure its &15 ok or will scroll
	
}


void lcd_feedback(void){      // works

if (menuSelect==4){
feedback_temp= lfo_mask[potValues[((menuSelectX/4)*2)+32] & 15 ]; // grab lfo_mask byte 2,6,10=0,2,4

}

if (menuSelect==6){
feedback_temp= lfo_mask[potValues[((menuSelectX/4)*3)+48] & 15]; // grab lfo_mask byte

}
for(n=0;n<8;n++){
	lcd_return[n]=(feedback_temp>>n)&1;  	 // write bits to array , works
spell[108+n]=(lcd_return[n]+48)& 63;			// works
spell[148+n]=spell[108+n];
}
}


void sampling(void){						// 18 ms of data


uint8_t t_ac;

uint8_t mask_i;
uint8_t mask_k;
uint8_t adsr_mult[5];
//adc_read();
//uint16_t isr_tempo=isrMask; // get tempo value
sample_pointB=sample_pointD;
unsigned short tempo_start=0;  // enabled when i=isrMask;
static unsigned short tempo_end=0;   // last count before note start
uint16_t i_total;
uint16_t tempo_mod=tempo_lut[potSource[109]];  // set tempo,speed from lut 40-200bpm

uint8_t l;			// 35.002khz(0.02857ms) sample, 1 sample is temp count (16x=0.00045712) , *16=1 note ,at 300 (437bpm),(1/(0.00002857*tempo count*16)=1beat in s
float freq_temp;	// (1/(bpm/60)) /0.00045712=tempo count ie 1093.8 for 120bpm
float freq2_temp;
float freq_adder;
float tempo_sync=16384/((tempo_mod*16)/512) ; // 8000 at slowest 15.625 updates to lfo at 1 note 16384/15.625=1048.576+ per update  at setting 80 
tempo_sync=tempo_sync/80;


for (l=0;l<10;l++){
	
	
	freq_temp=potSource[130+l];
	freq2_temp=freq_temp*tempo_sync; //correction to one note per cycle 
	//freq2_temp=freq_temp;
	freq_temp=lfo_accu[l]+ freq2_temp;// get lfo value plus rate , will try to get related to tempo for easier sync , at potS 80?/8192/8notes/ 1 bar 
	// temp0*16isr*8notes=(1/tempo)*128		@80 rate is 1 bar/8note?  @ tempo*128 count ie@300 temp : 38400tempo countis at freq_temp=8192 freq_temp+1=4.6

if ((tempo_count<50) && ((next_isr&15)==15)) freq_temp=0;   // trying retrigger
if (freq_temp>16384) lfo_accu[l]=freq_temp-16384; else lfo_accu[l]=freq_temp; // write back value
freq_temp=lfo_accu[l]; // 0-255 limit + above zero
freq_temp=freq_temp*0.000383495;  // 0-255 , chang this for depth
freq2_temp =arm_sin_f32(freq_temp); // seems to be working
freq_temp=freq2_temp*potSource[140+l]*51;
lfo_out[l]=freq_temp+8195; // all ok

} // lfo gen : 0=f1 , 1=tempo,2=pitch


	float k_hold=lfo_out[0];
	
	//freq_point[0]=(3.6*k_hold) - (1.6*k_hold*k_hold) -1; // tune for a linear input,dunno ,not much ?
	
	freq_point[0]=k_hold*0.00006435; //sine seem to overload at fully open but only with filter engaged 

freq_point[2]=lfo_out[3]*0.00006435;;
	//potSource[150]=(freq_point[0])*100; //0-2

//float lcd_out2;



//lcd_out3=potSource[130]; // 3 digit read out , works ok
//lcd_out3=lcd_out3+180;
potSource[150]=(lcd_out3/100)*16;
potSource[151]=((lcd_out3 %100)/10)*16;		 // 0-160 to 0-10
potSource[152]=(lcd_out3%10)*16;


unsigned short  sine_zero;  // zero cross
note_holdA=0;

int32_t filter_Accu;

//tempo_mod=tempo_mod-63+(lfo_out[1]>>7);
//if (tempo_mod<450) tempo_mod=((tempo_mod-200)>>1) +200; // more res lower
uint8_t note_plain;
int8_t ring_mod=0;
//sample_Accu2=0;
//printf ("crap");
// some good phasin and delays here
uint8_t cross_fade[2];
uint8_t fader[17]={0,1,5,11,19,28,39,51,64,76,88,99,108,116,122,126,127}; // sine curve for cross fade
if(adc_values[2]&16)	{cross_fade[1]=127-fader[adc_values[2]&15]; cross_fade[2]=127;}  else {cross_fade[2]=fader[adc_values[2]&15]; cross_fade[1]=127;} //calculate crossfader
// doing lfo calc here as it is slow only for now


///////////////////////////////////////////////////////////////

for (i=0;i<512;i++) {    // this should write 512 bytes , or about 15ms buffer ,works fine

	i_total=i+sample_pointB;

	note_plain=potValues[seq_pos & 7 ];
potValues[i&255]=potSource[i&255]>>4; //just to update values 
	if (tempo_count>=tempo_mod) { next_isr=(next_isr+1) & 4095;tempo_count=0;adsr();  }  else {tempo_count++; }  //trigger next note , actual next step for isrCount(future)  8ms,trying to fix slow down here  8000 too  much, adsr clears note info
// tempo_count is about 1000-400 
	tempo_start=0;
	if ((next_isr>>4) != seq_pos) { 					// next note step 140ms
		seq_pos=next_isr>>4; // seq pos =256 max , isr = 1/16 of a note
		tempo_start=1;

}


	if(tempo_start  )    // this helps alot to slow down,only on note change 16xisr maybe clear everythign before hand  and sample and hold and zero means no change
	{
	//printf("\n");//	ITM_SendChar( 65 );   //  Send ASCII code 65 = ’A’
	//printf("%d" ,note_channel[10]);


		potValues[32]=adc_values[0]>>1; //assigned pots to start of loopers 0-16,works
		potValues[33]=adc_values[1]>>1;


		seq_loop[2]=((potValues[32]+(seq_pos&7))&15); // calc  8 note loop positions sets looping point in sequence
		
		//seq_loop[3]=(potValues[33]+(( seq_pos&31 ) >>2)) & 15;  // quater speed
			seq_loop[3]=((potValues[33]+(seq_pos&15))&15); //sets looping point in sequence this is full 16 note
		
			seq_loop[4]=((potValues[32]+(seq_pos&7))&15);
			
		//seq_loop[4]=((potValues[32]+((seq_pos&15)>>1))&15); // half speed

		note_channel[2]=potValues[80+seq_loop[2]]+potValues[72];  //loop 8 notes from pos and x times
		note_channel[3]=potValues[seq_loop[3]];  //loop 8 notes from pos and x times ,might disable normal adsr completely
	if (note_channel[3]) note_channel[3]=note_channel[3]+(potSource[73]/4)+(lfo_out[2]>>11); // stay at zero for off
	
	
	if (note_channel[3]>48) note_channel[3]=48; //limit
	if (note_channel[3])  freq_lookup=512/(36137/midi_lut[MajorNote[note_channel[3]]]); // sample playback rate =samples/(36137/note hz)
	
	



	note_channel[5]=potValues[80+(seq_pos&15)];  // sample


	if ((note_channel[5]) && (adsr_toggle[5]==2)) {note_holdB=note_channel[5]; one_shot=0;}  // grab note when on ,one shot also

	
	note_holdB=potValues[80+seq_loop[2]]+(potValues[74]);  // 
	
	note_holdB=(note_holdB-4)+(lfo_out[2]>>11);
	note_holdB=MajorNote[note_holdB];

	sine_adder=sine_lut[note_holdB];	//sets freq ,1.0594  * 16536 =17518  ,
	sine_adder= (sine_adder*1200)>>10;  // modify different sample size , just need single cycle length and thats it
		mask_result =0;


		sample_Accu[0]=0; // reset to 0 mani sample hold
		sample_Accu[1]=0; // reset to 0 mani sample hold
		sample_Accu[2]=0; // reset to 0 mani sample hold
		sample_Accu[3]=0; // reset to 0 mani sample hold
		sample_Accu[4]=0; // reset to 0 mani sample hold
		sample_Accu[5]=0; // reset to 0 mani sample hold
	for (mask_i=0;mask_i<5;mask_i++)	{							// calc detune , slow ,also creates notes

		if (note_channel[mask_i]) {tune_Accu=sample_Noteadd[MajorNote[note_channel[mask_i]]];   note_tuned[mask_i]=(tune_Accu);}
		
	}

 
	}

  // calc freq 1/isr or 1/16 per note ,need for pitch bend and so on , change depending on decay

	// every step   1,110,928   >>20  ,per note
// New oscillators , sync, trigger input , waveshape ,zero cross
		sample_accus[0] = sample_accus[0] + note_tuned[0]; //careful with signed bit shift,better compare

		if (sample_accus[0]>524287) sample_accus[0] =-sample_accus[0] ; // faster >  than &  ,strange



		sample_accus[1] = sample_accus[1] + note_tuned[1];  // normal adder full volume
			//	if (!(note_channel[0]))   sample_accus[1] =0;  // turn off with vel now , maybe use mask
				if (sample_accus[1]>524287) sample_accus[1] =-sample_accus[1] ; // faster >  than &  ,strange

				sample_accus[2] = sample_accus[2] + note_tuned[2];
				//		if (!(note_channel[0]))   sample_accus[2] =0;  // turn off with vel now , maybe use mask
						if (sample_accus[2]>524287) sample_accus[2] =-sample_accus[2] ; // faster >  than &  ,strange

						sample_accus[3] = sample_accus[3] + note_tuned[3]; // bouncing somewhere
						//sample_accus[3] = sample_accus[3] +4000;
						//	if (!(note_channel[0]))   sample_accus[3] =0;  // turn off with vel now , maybe use mask
								if (sample_accus[3]>524287) sample_accus[3] =-sample_accus[3] ; // faster >  than &  ,strange

								sample_accus[4] = sample_accus[4] + note_tuned[4];
									//	if (!(note_channel[4]))   sample_accus[4] =0;  // turn off with vel now , maybe use mask
										if (sample_accus[4]>524287) sample_accus[4] =-sample_accus[4] ; // faster >  than &  ,strange

										sample_Accu[2] = 0;sample_Accu[0] =0;sample_Accu[3] =0; //all zeroed
										//if (sample_accus[2]<0) sample_Accu[2]=+sample_accus[2]; else sample_Accu[2]=sample_accus[2]; // convert to triangle ?
										sample_Accu[0]=sample_accus[2]>>7; // needs cut a bit
							
								sample_Accu[0] = ((sine_out+sample_Accu[0])*cross_fade[1]);
								
										
										
										
										
									 // sample section with float
										float s_temp; // hold float out 0-512 different speeds , i is ref pos
										float freq_jump= freq_lookup+sample_build[0];  //add to accu pointer
										sample_build[0]=freq_jump;  //store
										
										uint32_t freq_jump2=freq_jump; // int value 
										float s_frac= freq_jump2-freq_jump; // get frac part
										
										if ((freq_jump2&511)==0) sample_build[0]=0; //overflow reset ,random,but will reset if any reason note is 0
									 
										int16_t s_bit1=sine_block2[freq_jump2&511]-20000; // first sample  ,might wanna change to something better
										int16_t s_bit2=sine_block2[(freq_jump2+1)&511]-20000; //next sample
									
									s_temp= s_bit1+ (s_frac* (s_bit2-s_bit1));  // y1-y0 , good
										sample_accus[3]= s_temp;
									
										
										
										
										
										//sample_Accu[3]=0;
										sample_Accu[3]=sample_accus[3]<<6; // push up
										sample_Accu[2] = (sample_Accu[3]*cross_fade[2]);			//27b, 2 out f2  might do a crossfade here using pot 3

										//	sample_Accu[5] = sample_Accu[4]+ (sample_accus[4]*4);			// drum and envelope

		//	sample_Accu=sample_Accu-(1<<21);


	if (sine_counterB==0) 	sine_temp2=sine_adder;

		sine_counterB=sine_counterB+sine_temp2 ;  // sine up counter per cycle , however sine adder nees to wait
		if (sine_counterB>>7) sine_zero=0; else sine_zero=1;

if (sine_counterB>(sine_length<<5)) sine_counterB=0; //fixed for now
	sine_count(); // calc sine

// filter 1


int32_t feedback_out=filter_out[3];
//if (feedback_out>0xFFFF) feedback_out=0xFFFF; else if (feedback_out<-65535) feedback_out=-65535;  // limiter to 16 bits

sample_Accu[1]=sample_Accu[0];


		if (freq_point[0]>1) freq_point[0]=1; else if (freq_point[0]<0) freq_point[0]=0;// just in case
		//freq_point[0]=0.50;
		freq_point[1]=1-freq_point[0];
		//freq_point[1]=(1-freq_point[0])*0.7;
		//filter_accus[1]=sample_Accu[1]-(filter_accus[5]*0.3);
		//filter_accus[1]=sample_Accu[1];
		filter_accus[1]=sample_Accu[1]+((filter_hold[0])*0.5);
		filter_accus[1]= filter_accus[1]*adsr_level[3]; // add adsr envelope
		filter_accus[2]=(filter_accus[1]*freq_point[0])+(filter_accus[2]*freq_point[1]);
		filter_accus[3]=(filter_accus[2]*freq_point[0])+(filter_accus[3]*freq_point[1]);
		filter_accus[4]=(filter_accus[3]*freq_point[0])+(filter_accus[4]*freq_point[1]);
		filter_accus[5]=(filter_accus[4]*freq_point[0])+(filter_accus[5]*freq_point[1]);
		filter_hold[0]=(filter_accus[5]+filter_accus[11])*0.5; //half sample , nice
		sample_Accu[0] =filter_accus[5]; // out
		filter_accus[11]=filter_accus[5]; //write back new value
		//sample_Accu[0] =sample_Accu[1];

		//filter 2
		sample_Accu[3]=(sample_Accu[2]>>4); // this one is louder than sine


				if (freq_point[2]>1) freq_point[2]=1;

				freq_point[3]=1-freq_point[2];
				filter_accus[6]=sample_Accu[3];
					filter_accus[6]= filter_accus[6]*adsr_level[3]; // add adsr envelope
				
				filter_accus[7]=(filter_accus[6]*freq_point[2])+(filter_accus[7]*freq_point[3]);
				filter_accus[8]=(filter_accus[7]*freq_point[2])+(filter_accus[8]*freq_point[3]);
				filter_accus[9]=(filter_accus[8]*freq_point[2])+(filter_accus[9]*freq_point[3]);
				filter_accus[10]=(filter_accus[9]*freq_point[2])+(filter_accus[10]*freq_point[3]);
				filter_hold[1]=(filter_accus[10]+filter_accus[12])*0.5; //half sample
				sample_Accu[2] =filter_accus[10]; //out
				filter_accus[12]=filter_accus[10]; //write back new value
				

filter_Accu=0;
filter_Accu=(sample_Accu[0]+sample_Accu[2])>>8; //filter + drum out


 if (one_shot!=199)   one_shot++;  //play one attack then stop

 if (filter_Accu>0xFFFF) filter_Accu=0xFFFF; else if (filter_Accu<-65535) filter_Accu=-65535;  // limiter to 16 bits


 play_sample[i_total]=(filter_Accu>>6)+1023;   // final output disable for now
	//play_sample[i_total]=(sample_Accu[4])+1023;

}
 //make sure it's finished
bank_write=0;
}




void lfo(void){ // save this for another page

	uint8_t n_count;
	uint8_t lfo_speed[5]; // lfo rate , pos exp value
	uint8_t lfo_gain[5]; // gain inc negative for phase on output , maybe allow some clipping as well at the end ,dunno
	uint8_t lfo_offset[5]; // different shapes , triangle for now
	uint8_t lfo_target[5]; // potSource target , various options with limits, maybe some feedback
	//uint16_t lfo_value[5]; // actual lfo value out
	uint8_t lfo_table[7]={0,0,0,0,0,0}; // lookup table for target
	//uint16_t lfo_output[5];

for (n_count=0;n_count<5;n_count++){			//grab variables from potSource , dont use i

lfo_speed[n_count]=potSource[130+n_count];
lfo_gain[n_count]	=potSource[135+n_count];
lfo_offset[n_count]=potSource[140+n_count];
lfo_target[n_count]=potSource[145+n_count];
	}
for (n_count=0;n_count<5;n_count++){

	 lfo_value[n_count]= (lfo_value[n_count]+adsr_lut[lfo_speed[n_count]]);


	if (lfo_value[n_count]>4095) lfo_value[n_count]=0; // limit to 12 bit
	//if (lfo_value[n_count]<-4095) lfo_value[n_count]=0; // limit to 12 bit
	if (lfo_value[n_count]>=2047) lfo_output[n_count]=lfo_value[n_count]-2047; else  lfo_output[n_count]=2047-(lfo_value[n_count]); // up down 0-2047
lfo_output[n_count]=((lfo_output[n_count]*lfo_gain[n_count])>>5)+(lfo_offset[n_count]*51);
if (lfo_output[n_count]>2047) lfo_output[n_count]=2047; else if (lfo_output[n_count]<0) lfo_output[n_count]=0;  // limit output 0-2047
}

potSource[28]=lfo_output[0]>>6;

}
void adsr(void){
	float note_attack;  //attack length 50/50 , just a default shape that is maybe interpolated , store values for both and position, time in isr 
	float note_sustain; //sustaing lenght and height 80/20 / 0-160 0-1-sustain-0  160 is 160 steps(10 notes) 80+80  0+(1/(attack/2))*(attack/2) 1-(1/(attack/2))*(attack/2)+sustain level 1/160*sustain at (1/sustain)*time
	//uint16_t adsr_countup[11];  //holds isr count on notes , 
	//float adsr_level[11]; //float for vol envelope  ,ps 20 21
	uint8_t ad;//counter    0-160-160-160 maybe change 1/10 dunno 
	float as_attack=potSource[20]*0.1; // for now
	float as_sustain=potSource[21];
	uint16_t as_temp; 
	
	
	
for (ad=0;ad<10;ad++){							// envelope generator,not controlled from elsewhere yet just potsource,needs to be faster 
	as_temp =adsr_countup[ad]; //grab counter
	
	if (note_channel[ad]) {as_temp=1;note_channel[ad]=0; } // reset on note & 1 isr length ,retrigger also clear not for later
	if (as_temp) {
	if (as_temp<(as_attack))    note_attack=(1/as_attack)*as_temp; //count up attack ok 
	if (as_temp>=(as_attack))    note_attack=1-((1/as_attack)*(as_temp-as_attack)); //count down attack
	if(note_attack<0) note_attack=0; // stop at 0
	note_sustain=as_sustain*0.00625;
	if ((as_temp>=(as_attack)) && (note_sustain>note_attack)) note_attack=note_sustain; // change over to sustain level
	
	if (as_temp>=(as_attack+(as_sustain*0.3))) {note_attack= 0;  as_temp=0; }else as_temp++; // no roll off for now just straight to 0 , shortened , also stops 
	
	adsr_level[ad]=note_attack;
	
	adsr_countup[ad]=as_temp; //write back new value
	} 

}	
}


/*
void adsr(void){   //might start using this for lfos or dump the lot
	adsr_retrigger[6]=1; //for filter
	adsr_retrigger[7]=1; //for filter 2
	//adsr_retrigger[5]=1;  //kick

	uint8_t mask_i;


	uint8_t note_plain=potValues[seq_pos & 7 ];

			// 16*64 max
		if (!(next_isr	& ((1<<potValues[97]))) ) {note_channel[16]=1023;adsr_toggle[6]=0; }  // filter 2, retrigger on every note  on 0 duration nothing else
		//if (!(next_isr	& 16 )) {note_channel[16]=1023;adsr_toggle[6]=0; }  // filter 2, retrigger on every note  on 0 duration nothing else

	///	if (!(next_isr	& adsr_lut[potSource[97]]) ) {note_channel[16]=1023;adsr_toggle[6]=0; }  // filter 2, retrigger on every note  on short duration



		if (!(next_isr	& ((1<<potValues[99]))))  {note_channel[17]=1023;adsr_toggle[7]=0; }  // filter 2, retrigger on every note  on short duration


			if (!(next_isr	& ((8<<potValues[105])-1)))  {note_channel[10]=1023;adsr_toggle[0]=2; }   // retrigger for notes , needs however next note ?!
				if (!(next_isr	& ((8<<potValues[106])-1)))  {note_channel[11]=1023;adsr_toggle[1]=2; } //might just skip the lot or just a very long retrigger ,doesnt change with notelenght
				if (!(next_isr	& ((8<<potValues[107])-1)))  {note_channel[12]=1023;adsr_toggle[2]=2; }
				if (!(next_isr	& ((8	<<potValues[108])-1)))  {note_channel[13]=1023;adsr_toggle[3]=2; }



	for (mask_i=0;mask_i<10;mask_i++){   // retrigger
		if ((adsr_toggle[mask_i]==2) && adsr_retrigger[mask_i])   {adsr_toggle[mask_i]=0;  adsr_retrigger[mask_i]=0;  }  // reset adsr
	}


	for (adsr_up=0;adsr_up<10;adsr_up++) {   //calc adsr  done per next_isr tick ie 16*note  , note_channel[10+ can never be 0 or problem


if  ((note_channel[10+adsr_up]<2047) && (adsr_toggle[0+adsr_up]==0))  note_channel[10+adsr_up]=note_channel[10+adsr_up] + adsr_lut[potSource[16+(adsr_up*2)]];  //attack
if  ((note_channel[10+adsr_up]>1023) && (adsr_toggle[0+adsr_up]==1))  note_channel[10+adsr_up]=note_channel[10+adsr_up] - adsr_lut[potSource[17+(adsr_up*2)]];	// decay
if  ((note_channel[10+adsr_up]>=2047) && (adsr_toggle[0+adsr_up]==0))	{adsr_toggle[0+adsr_up]=1;note_channel[10+adsr_up]=2047;}
if  ((note_channel[10+adsr_up]<=1023) && (adsr_toggle[0+adsr_up]==1))	{adsr_toggle[0+adsr_up]=2; note_channel[10+adsr_up]=1023;}

}

//	printf("%d",note_channel[17]);
//	printf("\n");

// for (mask_i=0;mask_i<5;mask_i++)				// calculate velocities from mask now: on/off , has to be fast  , might disable
			 {  mask_calc(potValues[(mask_i*2)+32], potValues[(mask_i*2)+33]); // this is ok
					//note_channel[20+mask_i]=mask_result; // store mask output
					note_vol[mask_i] =mask_result * (potSource[8 + mask_i]);
					note_channel[20+mask_i] =((note_channel[mask_i+10]-1023)*note_vol[mask_i])>>8;  // * note_vol[t_ac] )>>8;

			 }  // set vel per channel
*/

//

void mask_calc(uint8_t mask_select,uint8_t mask_speed){    //calculate mask output from lfos
uint8_t mask_temp;
uint8_t mask_tempB;

//(lfo_mask[mask_select]>>((seq_pos>>mask_speed) &7)& 1) ;
mask_temp=(seq_pos>>mask_speed) &7 ; // 0-8
mask_tempB=	lfo_mask[mask_select]>>mask_temp;


mask_result=mask_tempB &1;


}

void sine_count(void) {         // sine_out is the output 9  bit  , works

int32_t sine_tempA;
int32_t sine_tempB;
int8_t sine_frac;


sine_counter=(sine_counterB>>5);  // up countr controlled by counter
sine_frac=sine_counterB & 31;  // grab last 5 bits, actual position for linear interpol,fractional

	if (sine_counter>sine_length) sine_counter = sine_length;		// seems to be faster than using a for loop to calculate both values

	sine_out = sine_block[sine_counter];  // 0- 40000
	sine_tempA=sine_out; // grab first value , needs to be always plus
	sine_tempA=sine_tempA-20000; //convert to signed

	sine_counter++;
	if (sine_counter>=sine_length)  sine_counter=0; // set to sample length

			sine_out = sine_block[sine_counter];  // grab second value
		
		sine_tempB=sine_out; // grab first value
			sine_tempB=sine_tempB-20000;  // convert to signed and +256 to -256

			sine_tempB=	sine_tempB-sine_tempA;   // calculate fraction then add
			sine_tempB=sine_tempB>>5; // div 32 or 32 upsample

			sine_tempB=sine_tempB*sine_frac; // mult by steps , can overshoot !!

			sine_out=(sine_tempA+sine_tempB);   // add back to start value

			//sine_tempA=sine_tempA; //needs




			//sine_out=sine_tempA;


}


void print_pot(void){

	uint8_t n_t;
/*
	printf("%d",bsr_out);
printf(",");
printf("%d",spi_byte);
printf(",");
printf("%d",clk_pin);
printf(",");
printf("%d",i);
printf("\n");
*/
	//for (n_t=0;n_t<128;n_t+=1) {
//printf("%d",(potValues[n_t] +(potValues[n_t+1]<<4)));
	//	printf("%d",bsr_out);
//printf(",");
//printf("%d",spi_hold);
//printf("\n");
//printf("%d",potSource[n_t]);
//printf(",");
//	}// print all into 64bytes 4bit+4bit
	//printf("%d",bsr_out);
//	printf(",");
//	printf("%d",note_channel[11]);

}







/*
void analogInputloop(void) { // might make menuselect a master override for everything  if chnaged then halt writing new values until screen refreshed

		menuSelect = (7 - (adc_values[0] >>1))<<4;
		menuSelectX=15 - (adc_values[1] );
		tempValueA=adc_values[2];

		counterVarB = menuSelectX + menuSelect; // select mem page 10*8  and location pointer  77

	if (counterVarB != tempValue[120]) {
		modEnable = 1;
	//	lcd.setCursor((menuSelectX * 2), 1);
	} else
		modEnable = 0;   //compared to stored value
	tempValue[120] = counterVarB;


 tempValueA=(tempValueA*11) >> 4;
	if (tempValueA>10) tempValueA=10;

 tempValue[counterVarB] = tempValueA; // read and store pots 0-200 ,writemenu to single pos ,

	if ((modEnable) || (firstRound))
		tempValueB[counterVarB] = tempValue[counterVarB]; // needs replicate values after moving menupot to avoidwriting new values needs to move twice not good
	valueCurrent = tempValue[counterVarB]; // incoming value  10*8 blocks
	valueOriginal = tempValueB[counterVarB]; // read original stored value    might just make  all separate regardless

	if (valueCurrent != valueOriginal) {
		tempValueB[counterVarB] = valueCurrent;

		//  potValues[counterVarB] = 10 - (valueCurrent);  // potvalues 0- 63 and 180 up



	}    //perfect now   , change to 0-9 here


	potValues[counterVarB]=potValues[counterVarB]+

	firstRound = 0;


} // loop for reading analog inputs

*/








// if (counterVarB==n_lcd)lcd_blink = !lcd_blink;


/*

	if (n_lcd > 18) {
		row_toggle = !row_toggle;
		n_lcd = 1;
	}


	lcddata = spell[(n_lcd) + (row_toggle * 18)]; // when n=1 changes
	if (command_toggle == 4) {
		command_toggle = 0;
		lcd_blink = !lcd_blink;
	} // top row seems to be slower from potValues 8
	//if (n == 31)   lcddata = spell[(n >> 1)+ (row_toggle*17)] ; // needs to be here or it loops wrong, maybe not
	if (n_lcd > 16) {
		rs_toggle = 0;
		lcddata = command[command_toggle]; // 0-3 command set
		command_toggle++;
	} else
		rs_toggle = 1;  // sending commands after printing
*/









/*




	sine_counter++;
	sine_counter = sine_counter & 511;
		switch (sine_counter >> 7) {
		case 0:
			sine_out = 512 - sine_block[127 - sine_counter];
			break;     	//0-127
		case 1:
			sine_out = 512 - sine_block[sine_counter - 127];
			break;		// 128-255
		case 2:
			sine_out = sine_block[383 - sine_counter];
			break;			//256-383
		case 3:
			sine_out = sine_block[sine_counter - 384];
			break;    //384-511
		default:
			break;




 // future goal = interpolate instead of add
if ((note_holdA) && (adsr[1] ==128)) adsr[1]=1;  // dont use note for trigger ,works

if (adsr[1]==1) {

	adsr_time[1]=(isrCount+17) &1023; note_channel[5]=127; adsr[1]=2;} // set counter , clear trigger, works , 16 per note


if ((isrCount>=adsr_time[1]) && (adsr[1]==2))	 {adsr[1]=128;note_channel[5]=0; } // simple start finish, wwatch isrcount ,works

if (note_channel[0] && (adsr[0] ==128)) adsr[0]=1;  // dont use note for trigger ,works

if (adsr[0]==1) {

	adsr_time[0]=(isrCount+17) &1023; note_channel[4]=127; adsr[0]=2;} // set counter , clear trigger, works , 16 per note


if ((isrCount>=adsr_time[0]) && (adsr[0]==2))	 {adsr[0]=128;note_channel[4]=0; } // simple start finish, wwatch isrcount ,works


*/



/*			HAL_GPIO_WritePin(GPIOB, D4_Pin, ((lcddata) & 1));
HAL_GPIO_WritePin(GPIOB, D5_Pin, ((lcddata >> 1) & 1));
HAL_GPIO_WritePin(GPIOB, D6_Pin, ((lcddata >> 2) & 1));
HAL_GPIO_WritePin(GPIOB, D7_Pin, ((lcddata >> 3) & 1)); // true or 0 return , value not important , works
*/
/*

	 HAL_GPIO_WritePin(GPIOB, D4_Pin, ((lcddata >> 4) & 1));
		HAL_GPIO_WritePin(GPIOB, D5_Pin, ((lcddata >> 5) & 1));
			HAL_GPIO_WritePin(GPIOB, D6_Pin, ((lcddata >> 6) & 1));
			HAL_GPIO_WritePin(GPIOB, D7_Pin, ((lcddata >> 7) & 1)); // true or 0 return , value not important , works





tim example
 if (GPIOE->IDR & 0x01) TIM2->CNT = 0; // reset counter
 if (GPIOE->IDR & 0x02) TIM2->CR1 |= 0x01; // enable counter
 if (GPIOE->IDR & 0x04) TIM2->CR1 &= ~0x01; // disable counter






*/







/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

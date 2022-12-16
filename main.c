/*
	Author: NishikiChisato
	Date: 2022.12.15
	Instructions
		There are 4 keys in our electronic diagram, and we give the following definition
		K1: Switch the display mod. hour|minute and minute|secends
		K2: Select which bit to change, press once is hour, twice is minutes, three times is seconds, four times is return
		K3: In add and minus mode, it means add. In general mode, enter to Alarm mode after pressed and in Alarm mode, the way you can
			change the time is the same as K2
		K4: In add and minus mode, it means minus. In general mode, press it enter to Stopclock mode and you can press K3 to return
*/
#include <reg51.h>
#include <intrins.h>

#define uchar unsigned char
#define uint unsigned int

//Display Segment
uchar code discode1[] = {0xc0, 0xf9, 0xa4, 0xb0, 0x99, 0x92, 0x82, 0xf8, 0x80, 0x90, 0xff};

//Beep Segment
uchar code SONG_TONE[]={212, 212, 190, 0};
uchar code SONG_LONG[]={9, 3, 12, 0};

//Time Display Module
uchar hour, minute, second, display_mod;
uchar times;

//Alarm AlarmClock Module
uchar Alarm_h, Alarm_m, Alarm_s;//Alarm Time
uchar tmp_h, tmp_m, tmp_s;//save Current Time

//Keystorke Times Module
uchar keys = 0;

//Stopwatch Module
uchar Stopwatch_s, Stopwatch_ms, Stopwatch_times = 0;

//Bit Define
sbit keystroke = P3^3;
sbit up = P3^4;
sbit down = P3^5;
sbit Beep = P2^0;

void init();
void delay(uchar);
void Display();
void MusicPlay();
void ChangeTime();
void AlarmClock();
void Stopwatch();

void main()
{
	init();
	while(1)
	{
		Display();
		
		if((minute == 0 && second == 0) || (hour == Alarm_h && minute == Alarm_m && second == Alarm_s))
			MusicPlay();
		
		if(up == 0)
			AlarmClock();
		
		if(down == 0)
			Stopwatch();
		
		ChangeTime();		
	}
}

void init()
{
	TMOD = 0x11;
	EA = 1;
	ET0 = 1;
	ET1 = 1;
	EX0 = 1;
	EX1 = 1;
	PX0 = 1;
	PX1 = 1;
	
	IT0 = 1;
	IT1 = 1;
	
	TH0 = (65536 - 46483) / 256;
	TL0 = (65536 - 46483) % 256;
	TR0 = 1;
	
	times = 0;
	hour = 12, minute = 30, second = 0;

	display_mod = 1;	
}

void delay(uchar n)
{
	uint i;
	while(n--)
		for(i = 0; i < 114; i++);
}

//LED Display function
void Display()
{
	uchar i, j = 0x10;
	uchar choise;
	if(display_mod == 0)//Display hour and minute
	{
		for(i = 0; i < 4; i++)
		{
			switch(i)
			{
				case 3: choise = hour / 10; break;
				case 2: choise = hour % 10; break;
				case 1: choise = minute / 10; break;
				case 0: choise = minute % 10; break;
			}
			P0 = 0xff;
			if(i == 2) P0 = discode1[choise] + 0x80;
			else P0 = discode1[choise];
			P2 = ~j;
			j = _crol_(j, 1);
			delay(10);			
			if(j & 0x01 == 1) j = 0x10;
		}
	}
	else//Display minute and second
	{
		for(i = 0; i < 4; i++)
		{

			switch(i)
			{
				case 3: choise = minute / 10; break;
				case 2: choise = minute % 10; break;
				case 1: choise = second / 10; break;
				case 0: choise = second % 10; break;
			}
			P0 = 0xff;			
			if(i == 2) P0 = discode1[choise] + 0x80;
			else P0 = discode1[choise];
			P2 = ~j;
			j = _crol_(j, 1);
			delay(10);			
			if(j & 0x01 == 1) j = 0x10;
		}
	}
}

void MusicPlay()
{
	uint i = 0, j, k;
	while(SONG_LONG[i] != 0 || SONG_TONE[i] != 0)
	{
		for(j = 0; j < SONG_LONG[i] * 20; j++)
		{
			Beep = ~Beep;
			for(k = 0; k < SONG_TONE[i] / 3; k++);
		}
		delay(1);
		i++;
	}
}

//Change Time Function, maybe Current time or Alarm time
void ChangeTime()
{
	while(keys > 0)
	{

		TR0 = 0;	//Pause T0
		
		if(keys == 1)
		{
			P1 = 0xef;
			if(up == 0)
			{
				delay(100);
				if(up == 0)
					hour++;
				delay(1500);
			}
			if(down == 0)
			{
				delay(100);
				if(down == 0)
					hour--;
				delay(1500);
			}
		}
		if(keys == 2)
		{
			P1 = 0xdf;
			if(up == 0)
			{
				delay(100);
				if(up == 0)
					minute++;
				delay(1500);
			}
			if(down == 0)
			{
				delay(100);
				if(down == 0)
					minute--;
				delay(1500);
			}
		}
		if(keys == 3)
		{
			P1 = 0xbf;
			if(up == 0)
			{
				delay(100);
				if(up == 0)
					second++;
				delay(1500);
			}
			if(down == 0)
			{
				delay(100);
				if(down == 0)
					second--;
				delay(1500);
			}
		}
		if(keys == 4)
		{
			P1 = 0x7f;
			keys = 0;
			delay(1000);
			break;
		}
		Display();		
	}
	TR0 = 1;	//Open T0
	P1 = 0xff;
}

//Adjust AlarmClock Function
void AlarmClock()
{
	uchar i;
	for(i = 0; i < 5; i++)
	{
		P1 = 0xf0;
		delay(100);
		P1 = 0xff;
		delay(100);
	}
	tmp_h = hour, tmp_m = minute, tmp_s = second;//Save current time
	hour = Alarm_h, minute = Alarm_m, second = Alarm_s;//Display time which has been saved
	keys = 1;				
	ChangeTime();
	Alarm_h = hour, Alarm_m = minute, Alarm_s = second;//assign to AlarmClock Time
	hour = tmp_h, minute = tmp_m, second = tmp_s;//Return original time
	P1 = 0xff;
}

//Stopwatch Function
void Stopwatch()
{
	uchar i;
	uchar tmp_m, tmp_s, tmp_mod;
	tmp_m = minute, tmp_s = second, tmp_mod = display_mod;
	
	P1 = 0xf0;		
	TR0 = 0;
	TR1 = 1;
	
	TH1 = 0xee;
	TL1 = 0x00;
	
	Stopwatch_ms = 0, Stopwatch_s = 0;
	while(Stopwatch_s <= 60)
	{
		minute = Stopwatch_s, second = Stopwatch_ms, display_mod = 1;
		Display();
		if(up == 0)
		{
			for(i = 0; i < 10; i++)
			{
				Display();
				delay(500);
			}
			break;
		}
	}
	
	minute = tmp_m, second = tmp_s, display_mod = tmp_mod;
	
	P1 = 0xff;
	TR0 = 1;
	TR1 = 0;
}

void keystroke0() interrupt 0
{
	EX0 = 0;
	display_mod = (display_mod + 1) % 2;
	EX0 = 1;
}

void timedis() interrupt 1
{
	times++;
	if(times == 20)
	{
		times = 0;
		second++;
	}
	if(second >= 60)
	{
		second -= 60;
		minute++;
	}
	if(minute >= 60)
	{
		minute -= 60;
		hour++; 
	}
	if(hour >= 24)
	{
		hour = 0;
	}
	TH0 = (65536 - 46483) / 256;
	TL0 = (65536 - 46483) % 256;
}

void keystroke1() interrupt 2
{
	EX1 = 0;
	keys++;
	EX1 = 1;
}

void stopwatch1() interrupt 3
{
	Stopwatch_times++;
	if(Stopwatch_times == 2)
	{
		Stopwatch_times = 0;
		Stopwatch_ms++;
	}
	if(Stopwatch_ms == 100)
	{
		Stopwatch_ms = 0;
		Stopwatch_s++;
	}
	TH1 = 0xee;
	TL1 = 0x00;
}

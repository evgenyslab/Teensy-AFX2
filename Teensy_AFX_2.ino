/*********************

Example code for the Teensy 3.0 + Adafruit RGB Character LCD Shield and Library for Midi communication
with the Axe FX II.


**********************/

// include the library code:
#include <Wire.h>
#include <Adafruit_RGBLCDShield.h>
#include <MIDI.h>

// The shield uses the I2C SCL and SDA pins. On classic Arduinos
// this is Analog 4 and 5 so you can't use those for analogRead() anymore
// However, you can connect other I2C sensors to the I2C bus and share
// the I2C bus.
Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();

// These #defines make it easy to set the backlight color
#define RED 0x1
#define YELLOW 0x3
#define GREEN 0x2
#define TEAL 0x6
#define BLUE 0x4
#define VIOLET 0x5
#define WHITE 0x7

char * Notes[12] = {
  "A ","Bb","B ","C ","Db","D ","Eb","E ","F ","Gb","G ","Ab"};
  
byte RQSTNAME[6] = { 0x00, 0x01, 0x74, 0x03, 0x0F,0x09};
byte *RFN = RQSTNAME;

byte RQSTNUM[6] = {0x00, 0x01, 0x74, 0x03, 0x14,0x12};
byte *RFNU = RQSTNUM;
int Rl = 6;

char pname[32];

int preset = 0;
int tune = 1;
int initial = 0;


void setup() {
	// Debugging output
	Serial.begin(9600);
	MIDI.begin();
	// set up the LCD's number of columns and rows: 
	lcd.begin(16, 2);

	// Print a message to the LCD. We track how long it takes since
	// this library has been optimized a bit and we're proud of it :)
	int time = millis();
	lcd.print("Hello, world!");
	time = millis() - time;
	lcd.setBacklight(WHITE);

	// Run Checksum operations:
	//checksum(RFN,Rl);
	//checksum(RFNU,Rl);
}

int parseName(byte * sysex, int l){
	int out = 0;
	// Serial.println(l);
	if (sysex[5] == 0x0f){
		//lcd.clear();
		// lcd.setCursor(0,0);
		for(int i=0;i<31;++i){
			char p = sysex[i+6];
			pname[i] = sysex[i+6];
		}
		for(int i=31;i!=0;--i){ //backwards removal of space
			if (pname[i] == 0x20){
				pname[i] = 0x00;
			}
			else { break;}
		}
		// lcd.print(pname);
		Serial.println("");
		Serial.print(pname);

	}
	return out;
}

int parseNum(byte * sysex, int l){
	int out = 0;
	// Serial.println(l);
	if (sysex[5] == 0x14){
		if (sysex[6] == 0x00){
			out = int(sysex[7]);
		}
		else{
			out = int(sysex[7]) + 128*int(sysex[6]);
		}
		/*   lcd.clear();
lcd.setCursor(0,0);
for(int i=0;i<l;++i){
	char p = sysex[i];
Serial.print(sysex[i],HEX);
Serial.print(" ");
}
lcd.print(out);

Serial.println("");
Serial.print("Preset: ");
Serial.print(out);*/

	}
	return out;
}


void checksum(byte* msg, int l){
	byte cs;
	for (int i=0;i<l-1;++i){
		if (i==0){
			cs = 0xF0^msg[i];
		}
		else{
			cs = cs^msg[i];
		}
	}
	cs = cs & 0x7F;
	delay(2000);
	Serial.print(cs);
	Serial.println();
	for (int i=0;i<l;++i){
		char t = msg[i];
		Serial.print(t,HEX);
		Serial.print(" ");
	}
	Serial.println("");
}


int getPresetVal(){
	int t = millis();
	int sizear = 0;
	int exitf = 0;
	int name = 0;
	while (!exitf){
		MIDI.sendSysEx(6,RQSTNUM);
		if(MIDI.read()){
			if ((MIDI.getType() == SystemExclusive) && (MIDI.getData1()==10)){
				byte *sys = MIDI.getSysExArray();
				sizear = MIDI.getData1();
				preset = parseNum(sys,sizear);
				exitf = 1;
				name = 1;
				delay(250);
			}
		}
		if ((millis()-t)/1000 >1){
			exitf = 1;
			name = 0;
		}
	}
	if (name!=1){
		return 0;	//ERROR
	} 
	else{return 1;}	// OK!
}

int getPresetName(){
	int t = millis();
	int sizear = 0;
	int exitf = 0;
	int name = 0;
	while (!exitf){
		MIDI.sendSysEx(6,RQSTNAME);
		if(MIDI.read()){ // NEED TIMEOUT!!!!
			if ((MIDI.getType() == SystemExclusive) && (MIDI.getData1()==40)){
				byte *sys = MIDI.getSysExArray();
				sizear = MIDI.getData1();
				parseName(sys,sizear);
				exitf = 1;
				name = 1;
				delay(100);
			}
		}
		if ((millis()-t)/1000 >1){
			exitf = 1;
			name = 0;
		}
	}
	if (name!=1){
		return 0;	//ERROR
	} 
	else{return 1;}
}


int updateLCD(){
	if (getPresetVal() && getPresetName()){
        lcd.clear();
	lcd.setCursor(0,0);
	lcd.print(preset);
	lcd.print(" ");
	lcd.print(pname);
	return 1;}
	else{
        lcd.clear();
	lcd.setCursor(0,0);
	lcd.print("AXE FX TIMEOUT");
	lcd.setCursor(0,1);
	lcd.print("Check Connection");
	return 0;
	}
}

uint8_t i=0;
int con_err = 1;
void loop() {
	// set the cursor to column 0, line 1
	// (note: line 1 is the second row, since counting begins with 0):
	lcd.setCursor(0, 1);
	// print the number of seconds since reset:
	lcd.print(millis()/1000);
        int sizear = 0;
	uint8_t buttons = lcd.readButtons();
	if ((initial==0) || !con_err){
		// get current preset name & value:
		while(!updateLCD()){
		delay(750); // failed initialization, try again!
		}
		initial = 1;
	}

	if (buttons) {
		lcd.clear();
		lcd.setCursor(0,0);
		
		
		if (buttons & BUTTON_UP) {
			lcd.setBacklight(TEAL);
			preset++;
			if (preset>127){
				preset = 0;
			}
			MIDI.sendProgramChange(preset,1);
                        delay(50);
                        MIDI.sendControlChange(0,0,1);
			Serial.println(updateLCD());
		}
		if (buttons & BUTTON_DOWN) {
			lcd.setBacklight(TEAL);
			preset--;
			if (preset<0){ //ERROR
				preset = 127;
			}
			MIDI.sendProgramChange(preset,1);
                        delay(50);
                        MIDI.sendControlChange(0,0,1);
			Serial.println(updateLCD());
			
		}
		if (buttons & BUTTON_LEFT) {
			lcd.print("LEFT ");
			lcd.setBacklight(GREEN);
			
		}
		if (buttons & BUTTON_RIGHT) {
			lcd.clear();
                        lcd.print("Tuner");
			lcd.setBacklight(VIOLET);
                        int var;
                        int note = 0;
                        int note_last = -1;
			MIDI.sendControlChange(15,127,1);
                        delay(300);
			while (!lcd.readButtons()){
                        MIDI.read();
                        if ((MIDI.getType() == SystemExclusive)&& (MIDI.getData1()==10)){
                          byte *sys = MIDI.getSysExArray();
		          sizear = MIDI.getData1();
                          Serial.println(sizear);
                          Serial.print("Note: ");
                          Serial.print(Notes[sys[6]]);
                          Serial.println("");
                          Serial.print("Val: ");
                          Serial.print(sys[8]);
                          Serial.println("");
                          note = sys[6];
                          if( note!=note_last){
                          lcd.setCursor(0,1);
                          lcd.print(Notes[sys[6]]);
                          }
                          lcd.setCursor(4,1);
                          lcd.print("     ");
                          lcd.setCursor(4,1);
                            var = sys[8];  
                            if (var < 62){
                              var = 63-var;
                              lcd.print("-");
                              lcd.print(var);
                            }
                            else if (var > 64){
                              var = var - 63;
                              lcd.print("+");
                              lcd.print(var);
                            }
                            else if ((var>=62) && (var<=64)){
                              lcd.print("-*-");
                            }
                            delay(50);
                        } 
                        note_last = note;
                        }
                        buttons = 0x00;
		        MIDI.sendControlChange(15,0,1); 
			delay(300);
                        Serial.println(updateLCD());
		}
		if (buttons & BUTTON_SELECT) {
			lcd.setBacklight(TEAL);
			MIDI.sendProgramChange(0,1);
                        delay(50);
                        MIDI.sendControlChange(0,0,1);
			Serial.println(updateLCD());
		}
        delay(200);
	}
//Serial.println(updateLCD());
}

#include <ESP32Encoder.h>

ESP32Encoder encoder;

// timer and flag for example, not needed for encoders
unsigned long long last = 0;

void setup(){
	
	Serial.begin(115200);
	// Enable the weak pull down resistors

	//ESP32Encoder::useInternalWeakPullResistors=DOWN;
	// Enable the weak pull up resistors
	ESP32Encoder::useInternalWeakPullResistors=UP;

	// use pin 34 and 35 for the second encoder
	encoder.attachHalfQuad(34, 35);

	// clear the encoder's raw count and set the tracked count to zero
	encoder.clearCount();
}

void loop(){
	Serial.println(encoder.getCount() / 4172);
	delay(10);
}

void setup(){
	pinMode(10, OUTPUT);
	pinMode(11, OUTPUT);

	Serial.begin(9600);
}

	int white = 10;
	int black = 11;
	
void loop(){
	int i;
	while(i > 500){
		openGrip(white, black);
	}
	delay(1000);

	while(i > 500){
		openGrip(black, white);
	}
	delay(1000);

	
	// closeGrip(10, 11);
	// delay(1000);

	}

void closeGrip(int white, int black){
	digitalWrite(white, HIGH);
	digitalWrite(black, LOW);
}

void openGrip(int white, int black){
	digitalWrite(white, LOW);
	digitalWrite(black, HIGH);
}

void swap(int x, int y){
	int z; 
	z = x;
	x = y;
	y = z;
}
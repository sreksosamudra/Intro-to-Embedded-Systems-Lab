#define OP_DECODEMODE  8
#define OP_SCANLIMIT   10
#define OP_SHUTDOWN    11
#define OP_DISPLAYTEST 14
#define OP_INTENSITY   10

/*******************  Prototypes for Part 4 functions  ********************/
void spiTransfer(volatile byte row, volatile byte data);      // SPI command to LED matrix for given row
                                                              //data - bit representation of LEDs in a given row; 1 indicates ON, 0 indicates OFF

// change these pins as necessary
int DIN = 51;
int CS =  52;
int CLK = 53;

int mov_col = 0;
int mov_row = 0;

byte spidata[2]; //spi shift register uses 16 bits, 8 for ctrl and 8 for data


void setup() {

  // setup serial port to read A0 and A1 at 9600 baud
  Serial.begin(9600);
  pinMode(8, INPUT);
  digitalWrite(8, HIGH);

  // SPI for LED matrix setup
  pinMode(DIN, OUTPUT);
  pinMode(CS, OUTPUT);
  pinMode(CLK, OUTPUT);
  digitalWrite(CS, HIGH);
  spiTransfer(OP_DISPLAYTEST,0);
  spiTransfer(OP_SCANLIMIT,7);
  spiTransfer(OP_DECODEMODE,0);
  spiTransfer(OP_SHUTDOWN,1);

}

void loop() {

  // Get the column and row index
  convert(analogRead(A0), analogRead(A1));  // A0 for X-direction, A1 for Y-direction
  // Shift the row index value
  mov_row = 1 << mov_row;
  // Turn on the corresponding row and column LED
  spiTransfer(mov_col, mov_row);
  // Turn off the whole row
  spiTransfer(mov_col, 0b00000000);

}

void spiTransfer(volatile byte opcode, volatile byte data){
  int offset = 0; //only 1 device
  int maxbytes = 2; //16 bits per SPI command
  
  for(int i = 0; i < maxbytes; i++) { //zero out spi data
    spidata[i] = (byte)0;
  }
  //load in spi data
  spidata[offset+1] = opcode+1;
  spidata[offset] = data;
  digitalWrite(CS, LOW); //
  for(int i=maxbytes;i>0;i--)
    shiftOut(DIN,CLK,MSBFIRST,spidata[i-1]); //shift out 1 byte of data starting with leftmost bit
  digitalWrite(CS,HIGH);
}

void convert(int analogValue1, int analogValue2) {
  mov_col = analogValue1 * 7/1023;     // X-direction
  mov_row = analogValue2 * 7/1023;     // Y-direction
}

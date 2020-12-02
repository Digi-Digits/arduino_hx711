/*
NOTES:
messages a processed in process_serial
actions for each message are in the mainloop
*/

#define PIN2  (1<<2)
#define PIN3  (1<<3)
#define SCK_HIGH() PORTD |= PIN2  // OR with port d
#define SCK_LOW() PORTD &= ~PIN2  // AND NOT BYTE with port d
#define DT_READ() ((PIND & PIN3) == PIN3) // 0 or 1
#define BAUD_RATE 115200

////////// Global variables ////////////
//  Table 3 Input Channel and Gain Selection
//  gain  factor  range
//  1     128     20mV
//  3     64      40mV
//  2     32      80mV
const uint8_t hx711_gain = 1;
uint32_t force_value;      // Raw sensor value
uint8_t data[4] = { 0 };  // shift in buffer

uint32_t smoothed = 0;

////////// HX711 ////////////
void setup_hx711() {
  DDRD |= PIN2;   // Set pin D2 to output pin
  SCK_LOW();
  DDRD &= ~PIN3;  // Set pin D3 to input pin
  PORTD |= PIN3;  // Set pin D3 pull up resistor high
}

uint8_t read_hx711_byte() {
  // Each PD_SCK pulse shifts out one bit, starting with
  // the MSB bit first, until all 24 bits are shifted out.
  uint8_t force_value = 0;
  for (uint8_t i = 8; i > 0; i--) {
    SCK_HIGH();
    delayMicroseconds(1);
    force_value |= DT_READ() << i;
    SCK_LOW();
    delayMicroseconds(1);
  }
  return force_value;
}

bool read_hx711() {
  // DT goes LOW when data is ready
  if ( DT_READ() ) {
    return false;
  }
  force_value = 0;      // Clear
  cli();          // Stop interrupts
  data[2] = read_hx711_byte();
  data[1] = read_hx711_byte();
  data[0] = read_hx711_byte();
  // Set the channel and the gain factor for the next reading using the clock pin.
  for (uint8_t i = 0; i < hx711_gain; i++) {
    SCK_HIGH();
    delayMicroseconds(1);
    SCK_LOW();
    delayMicroseconds(1);
  }
  uint8_t padding = 0x00;
  if (data[2] & 0x80) {
    padding = 0xFF;
  }
  data[3] = padding;
  // cast the array as a pointer to long and dereference it
  force_value = *( (uint32_t *) data );
  sei();          // Allow interrupts
  return true;
}


///////// main ////////////
void setup() {
  Serial.begin(BAUD_RATE);   // For debugging
  setup_hx711();
}

float alpha = 0.02;

void loop() {
  // Check for new force reading
  if ( read_hx711() ) {
    smoothed = (1-alpha)*smoothed + alpha*force_value;
    Serial.print(force_value);
    Serial.print("\n");
  }
}

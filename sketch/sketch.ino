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
uint32_t raw_adc_value;    // Shift in buffer

////////// HX711 ////////////
void setup_hx711() {
  DDRD |= PIN2;   // Set pin D2 to output pin
  SCK_LOW();
  DDRD &= ~PIN3;  // Set pin D3 to input pin
  PORTD |= PIN3;  // Set pin D3 pull up resistor high
}

bool read_hx711() {
  // DT goes LOW when data is ready
  if ( DT_READ() ) {
    return false;
  }
  raw_adc_value = 0;      // Clear
  
  // Each PD_SCK pulse shifts out one bit, starting with
  // the MSB bit first, until all 24 bits are shifted out.
  for (uint8_t i = 0; i < 24; i++) {
    SCK_HIGH();
    delayMicroseconds(1);
    raw_adc_value = raw_adc_value << 1;
    raw_adc_value |= DT_READ();
    SCK_LOW();
    delayMicroseconds(1);
  }

  // Set the channel and the gain factor for the next reading using the clock pin.
  for (uint8_t i = 0; i < hx711_gain; i++) {
    SCK_HIGH();
    delayMicroseconds(1);
    SCK_LOW();
    delayMicroseconds(1);
  }

  // FIX THIS. Overflow handling.
  // Max 80 00 00
  // Min 7F FF FF
  if ( raw_adc_value & 0x00800000 ){
    raw_adc_value |= 0xFF000000;
  }
  
  force_value = raw_adc_value;
    
  return true;
}

///////// main ////////////
void setup() {
  Serial.begin(BAUD_RATE);
  setup_hx711();
}

void loop() {
  // Check for new force reading
  if ( read_hx711() ) {
    Serial.print(force_value);

//    Serial.print("\t");
//    uint8_t* structPtr = (uint8_t*) &raw_adc_value;
//    for (uint8_t i = 0; i < sizeof(raw_adc_value); i++) {
//      Serial.print(*structPtr++, HEX);
//      Serial.print(" ");
//    }

    Serial.println("");
  }
}

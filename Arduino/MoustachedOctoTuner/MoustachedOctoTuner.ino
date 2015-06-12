#include <math.h>

const int n PROGMEM = 256;

const byte rsin[256] PROGMEM = {
 -128,   131,   134,   137,   140,   143,   146,   149,   152,   155,   158,
 162,   165,   167,   170,   173,   176,   179,   182,   185,   188,   190,
 193,   196,   198,   201,   203,   206,   208,   211,   213,   215,   218,
 220,   222,   224,   226,   228,   230,   232,   234,   235,   237,   238,
 240,   241,   243,   244,   245,   246,   248,   249,   250,   250,   251,
 252,   253,   253,   254,   254,   254,   255,   255,   255,   255,   255,
 255,   255,   254,   254,   254,   253,   253,   252,   251,   250,   250,
 249,   248,   246,   245,   244,   243,   241,   240,   238,   237,   235,
 234,   232,   230,   228,   226,   224,   222,   220,   218,   215,   213,
 211,   208,   206,   203,   201,   198,   196,   193,   190,   188,   185,
 182,   179,   176,   173,   170,   167,   165,   162,   158,   155,   152,
 149,   146,   143,   140,   137,   134,   131,   128,   124,   121,   118,
 115,   112,   109,   106,   103,   100,   97,    93,    90,    88,    85, 
 82,    79,    76,    73,    70,    67,    65,    62,    59,    57,    54,
 52,    49,    47,    44,    42,    40,    37,    35,    33,    31,    29,
 27,    25,    23,    21,    20,    18,    17,    15,    14,    12,    11,
 10,    9,     7,     6,     5,     5,     4,     3,     2,     2,     1, 
 1,     1,     0,     0,     0,     0,     0,     0,     0,     1,     1,
 1,     2,     2,     3,     4,     5,     5,     6,     7,     9,     10,
 11,    12,    14,    15,    17,    18,    20,    21,    23,    25,    27,
 29,    31,    33,    35,    37,    40,    42,    44,    47,    49,    52,
 54,    57,    59,    62,    65,    67,    70,    73,    76,    79,    82,
 85,    88,    90,    93,    97,    100,   103,   106,   109,   112,   115,
 118,   121,   124}; 

const char half_tones[] PROGMEM  = {
"C2 \0C#2\0D2 \0D#2\0E2 \0F2 \0F#2\0G2 \0G#2\0A2 \0A#2\0H2 \0C3 \0C#3\0D3 \0D#3\0E3 \0F3 \0F#3\0G3 \0G#3\0A3 \0A#3\0H3 \0C4 \0C#4\0D4 \0D#4\0E4 \0F4 \0F#4\0G4 \0G#4\0A4 \0A#4\0H4 \0C5 \0C#5\0D5 \0D#5\0E5 \0"};

const byte notes[12] PROGMEM = 
{
  B00000000, //C
  B00000001, //C#
  B00000000, //D
  B00000001, //D#
  B00000000, //E 
  B00000000, //F
  B00000001, //F#
  B00000000, //G
  B00000001, //G#
  B00000000, //A
  B00000001, //A#
  B00000000  //B
};

const byte numbers[10] PROGMEM =
{
  B00000000, //0
  B00000000, //1
  B00000000, //2
  B00000000, //3
  B00000000, //4
  B00000000, //5
  B00000000, //6
  B00000000, //7
  B11111110, //8
  B00000000  //9
};

int sins[254];
int coss[254];
volatile int values[n];
volatile int q;
int v;
byte temp = 0, j = 0;
int sum = 0, i = 0, t = 0;

ISR(TIMER1_COMPA_vect)
{
  values[q] = analogRead(0) >> 2;
  
  q++;
  
  if(q == n)
  {
    q = 0;
    
    noInterrupts();
      TIMSK1 &= ~(1<<OCIE1A);
    interrupts();
  }
}

void setup()
{
  analogReference(DEFAULT);
  
  Serial.begin(9600);
  
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
  
  q = 0;
  
  for(int i = 0; i < n; i++)
    values[i] = 0;
  
  noInterrupts();
    TCCR1A = 0;
    TCCR1B = 0;
    TCNT1  = 0;
    
    OCR1A = 250; 
    TCCR1B |= (1 << WGM12);
    TCCR1B |= (1 << CS11);
    TCCR1B |= (1 << CS10);
  interrupts();
}

void loop()
{ 
  noInterrupts();
    TCNT1 = 0;
    TIMSK1 |= (1<<OCIE1A);
  interrupts();
  
  for(i = 0; i < 256; i++)
  {
    sins[i] = 0;
    coss[i] = 0;
  }

  for(i = 0; i < n; i++)
  {    
    v = values[i];
    
    for(j = 0; j < 254; j++)
    {
      temp = lowByte(i * (j + 1));
      
      t = (int)pgm_read_byte_near(rsin + temp) - 128;
      sins[j] += (v * t / 32);
      
      if(temp < 192)
      {
        temp += 64;
        t = (int)pgm_read_byte_near(rsin + temp) - 128;
      }
      else
      {
        temp -= 192;
        t = (int)pgm_read_byte_near(rsin + temp) - 128;
      }
      
      coss[j] += (v * t / 32);
    }

    digitalWrite(13, digitalRead(13) ^ 1);
  }
  
  Serial.println(F("End iterration"));
  
  int i_max = 0;
  int p_max = 0.0;
  
  Serial.println(F("Powers:"));
  
  for(i = 0; i < 127; i++)
  {
    int p = abs(sins[i] / 16) + abs(coss[i] / 16);
    
    if(p > p_max)
    {
      i_max = i;
      p_max = p;
    }
  }
  
  float k = (i_max + 1) * 0.06; // ~ * 3.9 / 65.406;
  
  float half_tone = log10(k) * 39.863; // ~ * 12 / log10(2);
  
  int half_tone_index = -1;
  
  if(half_tone > 0 && half_tone < 39.4)
    half_tone_index = (int)(round(half_tone));
  
  Serial.println();
  Serial.print(p_max);
  Serial.print(F(" f = "));
  Serial.println((i_max + 1) * 39);
  Serial.println();
  Serial.print(F(" n = "));
  if(half_tone_index == -1)
     Serial.println(F("So low..."));
  else
  {
    i = half_tone_index * 4;
    while(true)
    {
      temp = pgm_read_byte_near(half_tones + i);
      if(temp != '\0')
        Serial.print((char)temp);
      else
        break;
        
      i++;
    }
  }
  Serial.println();
  
  /*Serial.println();
  Serial.println(F("Cosins:"));
  for(i = 0; i < 256; i++)
    Serial.println(coss[i]);
  
  Serial.println();
  Serial.println(F("Sins:"));
  for(i = 0; i < 256; i++)
    Serial.println(sins[i]);
  
  Serial.println();
  for(i = 0; i < n; i++)
    Serial.println(values[i]);
  */
  
  while(true)
  {
    digitalWrite(13, HIGH);
    delay(100);
    digitalWrite(13, LOW);
    delay(500);
  }
}

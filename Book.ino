#define STATE_STROBE_FAST  0
#define STATE_STROBE_SLOW  1
#define STATE_FADE_FAST    2
#define STATE_FADE_SLOW    3
#define STATE_HEARTBEAT    4

#define STATE_ON           90
#define STATE_OFF          100

#define DIR_UP             0
#define DIR_DOWN           1

#define LED_COUNT          4

#define FASE_ALL_FADE_SLOW    0
#define FASE_ALL_FADE_FAST    1
#define FASE_ALL_STROBE_FAST  2
#define FASE_ALL_OFF          3
#define FASE_ALL_ON           4
#define FASE_ALL_HEARTBEAT    5
#define FASE_ALL_RANDOM       6

#define FASE_COUNT 7
//#define FASE_COUNT 1

// 11 rechts blauw/groen
// 9  rechts midden boven / oranje
// 6  rechts deur / ornaje
// 10 links

int ledPins[] = { 9, 6, 11, 10 };

struct LED {
  int pin;
  int state;
  int brightness;
  
  long millisLastChange;  
  int dir;
};

struct FASE {
  int type;
  int duration;
};

LED leds[ LED_COUNT ];
FASE fases[ FASE_COUNT ];

long previousMillis = 0;
long interval = 50;

int randomStates[] = { STATE_STROBE_FAST, STATE_STROBE_SLOW, STATE_FADE_FAST, STATE_FADE_SLOW, STATE_HEARTBEAT };
int randomStatesCount = 5;

int curFase = 0;
long lastFaseChange = 0;

// Setup all the led configurations, and insert the fases into the fase configuration
//
void setup()  { 
  Serial.begin(9600);
  
  fases[0].type     = FASE_ALL_HEARTBEAT;
  fases[0].duration = 8000;
  
  fases[1].type     = FASE_ALL_FADE_SLOW;
  fases[1].duration = 6000;  
  
  fases[2].type     = FASE_ALL_STROBE_FAST;
  fases[2].duration = 3000;  
  
  fases[3].type     = FASE_ALL_FADE_FAST;
  fases[3].duration = 4000;    
  
  fases[4].type     = FASE_ALL_OFF;
  fases[4].duration = 1000;  
  
  fases[5].type     = FASE_ALL_RANDOM;
  fases[5].duration = 5000;    

  fases[6].type     = FASE_ALL_OFF;
  fases[6].duration = 2000;    
  
  
  
//  fases[0].type     = FASE_ALL_ON;
//  fases[0].duration = 8000;
  
  
  int i;
  for (i = 0; i < LED_COUNT; i++ ) {
      
      LED *led = &leds[ i ];
      led->pin = ledPins[ i ];
      
      led->brightness = i * 60;
      led->dir = i % 2 == 0 ? DIR_DOWN : DIR_UP;
      led->state = STATE_OFF;
      led->millisLastChange = 0;
      
      pinMode( led->pin, OUTPUT);     
      
     Serial.println( led->pin );   
  }
} 

// Looperdeloop!
//
void loop()  { 
  unsigned long currentMillis = millis();
  
  // Is it time to change to the next fase? The current fase has a duration configured
  //
  if ( currentMillis - lastFaseChange > fases[ curFase ].duration ) {
    curFase++;
    
    // Wrap back to fase 0 when we are at the end
    //
    if ( curFase + 1 > FASE_COUNT ) {
      curFase = 0;
    }
    
    // Setup the next fase
    //
    FASE *fase = &fases[ curFase ];
    
    Serial.print( "Fase type " );
    Serial.println( fase->type );
       
    switch ( fase->type ) {
      case FASE_ALL_FADE_SLOW:
        for ( int i = 0; i < LED_COUNT; i++ ) {
          LED *led = &leds[ i ];
          led->pin = ledPins[ i ];
          
          led->brightness = i * 60;
          led->dir = i % 2 == 0 ? DIR_DOWN : DIR_UP;
          led->state = STATE_FADE_SLOW;
        }      
      break;
      
      case FASE_ALL_FADE_FAST:
        for ( int i = 0; i < LED_COUNT; i++ ) {
          LED *led = &leds[ i ];
          led->pin = ledPins[ i ];
          
          led->brightness = i * 17 * 3;
          led->dir = i % 2 == 0 ? DIR_DOWN : DIR_UP;
          led->state = STATE_FADE_FAST;
        }      
      break;     
      
      case FASE_ALL_RANDOM:
        for ( int i = 0; i < LED_COUNT; i++ ) {
          LED *led = &leds[ i ];
          led->pin = ledPins[ i ];
          
          led->brightness = i * 60;
          led->dir = i % 2 == 0 ? DIR_DOWN : DIR_UP;
          led->state = randomStates[ random( 0, randomStatesCount - 1 ) ];
        }      
      break;      
      
      case FASE_ALL_STROBE_FAST:
        for ( int i = 0; i < LED_COUNT; i++ ) {
          LED *led = &leds[ i ];
          led->pin = ledPins[ i ];
          
          led->brightness = 0;
          led->state = STATE_STROBE_FAST;
        }      
      break;      
      
      case FASE_ALL_OFF:
        for ( int i = 0; i < LED_COUNT; i++ ) {
          LED *led = &leds[ i ];
          led->pin = ledPins[ i ];
          
          led->brightness = 0;
          led->state = STATE_OFF;
        }      
      break;         
      
      case FASE_ALL_ON:
        for ( int i = 0; i < LED_COUNT; i++ ) {
          LED *led = &leds[ i ];
          led->pin = ledPins[ i ];
          
          led->brightness = 255;
          led->state = STATE_ON;
        }      
      break;         

      case FASE_ALL_HEARTBEAT:
        for ( int i = 0; i < LED_COUNT; i++ ) {
          LED *led = &leds[ i ];
          led->pin = ledPins[ i ];
          
          led->brightness = 0;
          led->state = STATE_HEARTBEAT;
          led->millisLastChange = 0;
        }      
      break;          
    }
    
    lastFaseChange = currentMillis;
  }
  
  // Is it time for the next 'animation frame'?
  //
  if ( currentMillis - previousMillis > interval ) {
    previousMillis = currentMillis;
    
    for ( int i = 0; i < LED_COUNT; i++ ) {
      
      LED *led = &leds[ i ];
      
      int cur = led->brightness;
      long diff = currentMillis - led->millisLastChange;
      
      switch ( led->state ) {
        case STATE_STROBE_FAST:
          if ( cur == 0 ) { cur = 255; } else { cur = 0; }
        break;
        
        case STATE_FADE_FAST:
        
          if ( led->dir == DIR_UP ) { cur += 17; } else { cur -= 17; }
          
          if ( cur >= 255 ) { led->dir = DIR_DOWN; } 
          else if ( cur <= 0 ) { led->dir = DIR_UP; }           
          
        break;        
        
        case STATE_FADE_SLOW:
        
          if ( led->dir == DIR_UP ) { cur += 5; } else { cur -= 5; }
          
          if ( cur >= 255 ) { led->dir = DIR_DOWN; } 
          else if ( cur <= 0 ) { led->dir = DIR_UP; }           
                    
        break;        
     
        case STATE_HEARTBEAT:
          
          if ( diff < 100 ) {
            cur = 255;
          } else if ( diff < 300 ) {
            cur = 0;
          } else if ( diff < 400 ) {
            cur = 255;
          } else if ( diff < 1800 ) {
            cur = 0;
          } else {
            led->millisLastChange = currentMillis;
          }          
        break;   
        
        case STATE_ON:
          if ( cur != 255 ) {
            cur = 255;
          }
        break;        
        
        case STATE_OFF:
          if ( cur != 0 ) {
            cur = 0;
          }
        break;                
      }
      
      led->brightness = cur;
      analogWrite( led->pin, cur );    
      
//      Serial.print( i );
//      Serial.print( " " );
//      Serial.print( led->pin );   
//      Serial.print( " " );
//      Serial.println( cur );         
    }
  }
}


#define ON_BOARD_LED 13
 
 int incomingByte = 0;

 void setup( void )
 {
   
   pinMode( ON_BOARD_LED, OUTPUT );
   digitalWrite( ON_BOARD_LED, LOW );
   
   Serial.begin( 115200 );

}


void loop( void )
{

  if ( Serial.available() > 0 )
  {
    String inputs = Serial.readStringUntil('\n');
    
    for(int i = 0; i < inputs.length() - 1; i++)
    {
      incomingByte = inputs.charAt(i);

      Serial.print( "Byte received: " );
      Serial.println( incomingByte );
      Serial.print( "Byte received: " );
      Serial.println( char( incomingByte ) );    
      Serial.print( "Byte received ASCII code decimal: " );
      Serial.println( incomingByte, DEC );
      Serial.print( "Byte received ASCII code hexadecimal: " );    
      Serial.println( incomingByte, HEX );
      
      // Toggle LED with a 2-second delay
      digitalWrite(ON_BOARD_LED, HIGH ); // Turn ON
      delay(2000);                      // Wait 2 seconds
      digitalWrite(ON_BOARD_LED, LOW);  // Turn OFF
      delay(2000);
    }
    Serial.print("\n");
  }

}

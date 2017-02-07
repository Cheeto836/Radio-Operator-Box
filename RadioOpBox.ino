#include <Adafruit_MCP23017.h>
#include <Wire.h>

Adafruit_MCP23017 mcp;

const int INT_PIN = 3;
const int DEBOUNCE_MS = 50;
const int TIMEOUT_MS = 100;

const int numButtonsPortA = 2;
const int portAButtons[] = {28, 23};
const int numButtonsPortB = 2;
const int portBButtons[] = {1, 4};

uint16_t value = 0;
bool valChanged = false;

void setup()
{
  //configure serial for comm with dashboard
  Serial.begin(9600);

  //configure the arduino interrupt pin
  attachInterrupt(digitalPinToInterrupt(INT_PIN), handleUpdate, RISING);

  //configure the MCP
  mcp.begin(); //default address, all ADR pins wired LOW
  mcp.setupInterrupts(false, false, HIGH); //interrupts not mirrored, floating,signaled with HIGH

  //configure port a as input with interrupt condition rising
  for (int i = 0; i < numButtonsPortA; i++)
  {
    mcp.pinMode(portAButtons[i], INPUT);
    mcp.pullUp(portAButtons[i], HIGH);
    mcp.setupInterruptPin(portAButtons[i], FALLING);
  }
  
  //configure port b as input with interrupt condition rising
  for (int i = 0; i < numButtonsPortB; i++)
  {
    mcp.pinMode(portBButtons[i], INPUT);
    mcp.pullUp(portBButtons[i], HIGH);
    mcp.setupInterruptPin(portBButtons[i], FALLING);
  }
}

void loop()
{
  //only process if interrupt declares we changed values
  if (valChanged)
  {
    //process command based on value
    Serial.print("value: ");
    Serial.println(value);
    valChanged = false;
  }
  
  Serial.print("value: ");
  Serial.println(mcp.readGPIOAB(), BIN);
}

void handleUpdate()
{
  //assume the value is zero (no buttons pressed)
  unsigned long refTime = millis();
  bool stable = false;
  bool timeout = false;
  bool prevVal = false;
  while (!stable && !timeout)
  {
    unsigned long curTime = millis();
    unsigned long elapsed = curTime - refTime;
    if (mcp.readGPIOAB() != 0)
    {
      //value true
      if (prevVal)
      {
        //previous value same, no bounce]
        if (elapsed >= DEBOUNCE_MS)
        {
          value = mcp.readGPIOAB();
          valChanged = true;
        }
      }
      else
      {
        //previous value different, bounce
        refTime = millis();
      }
      //update previous value
      prevVal = true;
    }
    else
    {
      //value false
      if (!prevVal)
      {
        //previous value same, no bounce
        timeout = (elapsed >= TIMEOUT_MS);
      }
      else
      {
        //previous value different, bounce
        refTime = millis();
      }
      //update previous value
      prevVal = false;
    }
  }
}

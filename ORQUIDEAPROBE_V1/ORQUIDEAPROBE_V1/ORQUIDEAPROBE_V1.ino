/**************************************************************************
 EJEMPLO TOMADO DE PULOWI
 
BLE SERVICIO

DF Robot Service: Id: 0000dfb0-0000-1000-8000-00805f9b34fb0000000000

DF Robot Characteristics: Name: Serial Port Id: 0000dfb1-0000-1000-8000-00805f9b34fb

Name: Command Id: 0000dfb2-0000-1000-8000-00805f9b34fb

DF Robot Descriptors: (same for both characteristics) Name: Read Name: WriteWithoutResponse Name: Write Name: Notify
 
 **************************************************************************/


#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
//#include <Adafruit_MPL115A2.h>
#include <DHT.h>       //OJO RE OJO ES EL  DHT11 HUMEDAD Y TEMPERATURA AMBIENTE
#define DHTPIN 2       // Define el pin al que está conectado el sensor DHT22
#define DHTTYPE DHT11  // Selecciona el tipo de sensor DHT (DHT11 o DHT22)
DHT dht(DHTPIN, DHTTYPE);

#include <OneWire.h>
#include <DallasTemperature.h>
#define ONE_WIRE_BUS 3
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);


#define ph_dir 99     //default I2C ID number for EZO pH Circuit.
#define ce_dir 100    //default I2C ID number for EZO EC Circuit.
#define od_dir 97     //default I2C ID number for EZO DO Circuit.
#define temp_dir 102  //default I2C ID number for EZO RTD Circuit.
#define orp_dir 98    //default I2C ID number for EZO ORP Circuit.
//Adafruit_MPL115A2 mpl115a2;

#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 32  // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library.
// On an arduino UNO:       A4(SDA), A5(SCL)
// On an arduino MEGA 2560: 20(SDA), 21(SCL)
// On an arduino LEONARDO:   2(SDA),  3(SCL), ...
#define OLED_RESET -1        // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C  ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32   3C PARA LAS I2C COMPRADAS POR AMAZON
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

char computerdata[20];            //we make a 20 byte character array to hold incoming data from a pc/mac/other.
char computerdata2[20];           //we make a 20 byte character array to hold incoming data from a pc/mac/other.
byte received_from_computer = 0;  //we need to know how many characters have been received.
byte serial_event = 0;            //a flag to signal when data has been received from the pc/mac/other.
byte code = 0;                    //used to hold the I2C response code.
char sensor_data[32];             //we make a 20 byte character array to hold incoming data from the pH circuit.
byte in_char = 0;                 //used as a 1 byte buffer to store inbound bytes from the pH Circuit.
byte i = 0;                       //counter used for ph_data array.
int time_ = 815;                  //used to change the delay needed depending on the command sent to the EZO Class pH Circuit.
//float ph_float;                  //float var used to hold the float value of the pH.
float HumedadP = 0, temperatureC = 0;
//float sensor_float=0;
//char *DO;
int estado = 0;
int valbat = 0;

void setup() {
  Serial.begin(115200);  //initial the Serial
  dht.begin();
  analogReference(INTERNAL);  //REF DE 1.1V RESISTENCIAS CON RELACION 5:1 5.1k Y 1k A GND. 3.2v EQUIVALE A 500adc 10bits. 4.2v EQUIVALE A 640adc 10bits. 3.4v 530.
  Wire.begin();               //enable I2C port.
  sensors.begin();
  pinMode(A3, OUTPUT);    ///Para reset de pantalalla NOOO PARA ENCENDER PANTALLA.
  pinMode(PD4, OUTPUT);   //para el pito
  digitalWrite(A3, LOW);  // sets the digital pin D2 on
  //digitalWrite(PD4, HIGH); // sets the digital pin D4 on  PITIDO
  delay(1000);             // waits for a second
  digitalWrite(A3, HIGH);  // sets the digital pin D2  ESTE ES EL RESET DE LA PANTALLA
  //digitalWrite(PD4, LOW);
  delay(1000);          // waits for a second
  pinMode(PD5, INPUT);  //BOTON PARA INICIAR RONDA DE MEDICION

  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;  // Don't proceed, loop forever
  }

  display.clearDisplay();
  display.setTextSize(2);       // Normal 1:1 pixel scale
  display.setTextColor(WHITE);  // Draw white text
  display.setCursor(0, 6);      // Start at top-left corner
  display.println("ORQUIDEA");
  display.display();
  display.startscrollleft(0x00, 0x05);
  delay(5000);
  display.clearDisplay();
  display.setTextSize(1);   // Normal 1:1 pixel scale
  display.setCursor(0, 8);  // Start at top-left corner

  display.println("Cargando....");
  display.println("Sonda");
  display.display();
  //display.startscrollright(0x00, 0x06);
  delay(5000);


  display.display();
  display.startscrollright(0x00, 0x05);
  /*
  if (! mpl115a2.begin()) {
    Serial.println("ERROR sensor 6");
    display.clearDisplay();
    display.setTextSize(2);      // Normal 1:1 pixel scale
    display.setTextColor(WHITE); // Draw white text
    display.setCursor(0, 6);     // Start at top-left corner
    display.println("ERROR_S6");
    display.display();
    display.startscrollright(0x00, 0x05);
    while (1);
  }
*/
  Serial.println(F("Right Start"));
  pito(200);
  delay(500);
  pito(200);
  display.clearDisplay();
  display.setTextSize(1);   // Normal 1:1 pixel scale
  display.setCursor(0, 4);  // Start at top-left corner

  display.println("Oprima el boton LEER");
  display.println("o use la APP para");
  display.println("Iniciar Medicion");
  display.display();
  display.startscrollright(0x00, 0x06);
}

void pito(int ciclos) {
  for (int i = 0; i < ciclos; i++) {
    digitalWrite(PD4, HIGH);  // sets the digital pin D4 on  PITIDO
    delay(1);
    digitalWrite(PD4, LOW);
    delay(1);
  }
}

int batt_info() {
  //delay(50);
  int sensorValue = analogRead(A0);  ////////RANGO ENTRE 530 3.4V Y 640 4.2V
  delay(50);
  sensorValue = analogRead(A0);  ////////RANGO ENTRE 530 3.4V Y 640 4.2V

  if (sensorValue < 530) {
    sensorValue = 0;
  } else if (sensorValue > 640) {
    sensorValue = 100;
  } else {
    sensorValue = sensorValue - 530;
    sensorValue = sensorValue * 0.91;
  }

  //sensorValue = sensorValue *0.45+60;     //ecuacion de 3.3V min 0 3.8 100%
  /*if(sensorValue > 100){
           sensorValue=100;
        }
        if(sensorValue < 0){
         sensorValue=0;
        }
        /*screen.setTextSize(1);
        screen.fillRect(80,310,40,10, COLOR_RGB565_BLACK);
        screen.setCursor(80, 310);
        screen.println(sensorValue);
        screen.setTextSize(2);
        serial_event = false;*/
  return sensorValue;
}

void serialEvent() {                                                     //this interrupt will trigger when the data coming from the serial monitor(pc/mac/other) is received.
  received_from_computer = Serial.readBytesUntil(13, computerdata, 20);  //we read the data sent from the serial monitor(pc/mac/other) until we see a <CR>. We also count how many characters have been received.
  computerdata[received_from_computer] = 0;                              //stop the buffer from transmitting leftovers or garbage.
  serial_event = true;
  //Serial.println(F("Serial EVENT"));
  //Serial.println(computerdata);
}
////////////////////////////////////////////////////////   LECTURA //////////////////////////////////
void lectura(int direccion) {

  //reset i, we will need it later
  //Serial.println(F(" Comando:"));
  //Serial.println(computerdata[1]);
  if (computerdata[1] == 'c' || computerdata[1] == 'r') time_ = 815;  //if a command has been sent to calibrate or take a reading we wait 815ms so that the circuit has time to take the reading.
  else time_ = 250;                                                   //if any other command has been sent we wait only 250ms.
  //computerdata = computerdata << 1;
  for (int k = 0; k < 20; k++) {

    computerdata2[k] = computerdata[k + 1];
  }
  Wire.beginTransmission(direccion);  //call the circuit by its ID number.
  Wire.write(computerdata2);          //transmit the command that was sent through the serial port.
  Wire.endTransmission();             //end the I2C data transmission.


  if (strcmp(computerdata2, "sleep") != 0) {  //if the command that has been sent is NOT the sleep command, wait the correct amount of time and request data.
                                              //if it is the sleep command, we do nothing. Issuing a sleep command and then requesting data will wake the circuit.

    delay(time_);  //wait the correct amount of time for the circuit to complete its instruction.

    if (computerdata[0] == '3') {
      Wire.requestFrom(direccion, 32, 1);
    } else {
      Wire.requestFrom(direccion, 20, 1);
    }
    //call the circuit and request 20 bytes (this may be more than we need)
    code = Wire.read();  //the first byte is the response code, we read this separately.

    switch (code) {  //switch case based on what the response code is.
      case 1:        //decimal 1.
        //Serial.println("Correcto");        //means the command was successful.
        pito(70);
        break;  //exits the switch case.

      case 2:  //decimal 2.
        //Serial.println("Error");         //means the command has failed.
        escribaD(16);
        break;  //exits the switch case.

      case 254:  //decimal 254.
        //Serial.println("Pendiente");        //means the command has not yet been finished calculating.
        break;  //exits the switch case.

      case 255:  //decimal 255.
        //Serial.println("No conectado");
        escribaD(16);  //means there is no further data to send.
        break;         //exits the switch case.
    }

    while (Wire.available()) {   //are there bytes to receive.
      in_char = Wire.read();     //receive a byte.
      sensor_data[i] = in_char;  //load this byte into our array.
      i += 1;                    //incur the counter for the array element.
      if (in_char == 0) {        //if we see that we have been sent a null command.
        i = 0;                   //reset the counter i to 0.
        break;                   //exit the while loop.
      }
    }
    if (code == 1) {
      //Serial.println(sensor_data);              //print the data.
    }
  }
  serial_event = false;  //reset the serial event flag.
                         //DO = strtok(sensor_data, ",");
                         //sensor_float=atof(sensor_data);
                         //return sensor_data;
}

void escribaD(int sensornumber) {
  display.clearDisplay();
  display.setTextSize(1);       // Normal 1:1 pixel scale
  display.setTextColor(WHITE);  // Draw white text
  display.setCursor(0, 2);      // Start at top-left corner
  switch (sensornumber) {
    case 1:
      display.print("pH: ");
      break;
    case 2:
      display.print("Dissolved oxygen     [%]: ");

      //display.println(sensor_data);
      break;
    case 3:
      display.setCursor(0, 0);  // Start at top-left corner
      display.println("Electrica            Conductividad[uS/cm]:");
      break;
    case 4:
      display.print("Agua Temperatura    [C]: ");
      //sensors.requestTemperatures();
      //float floatValue = sensors.getTempCByIndex(0);
      //snprintf(sensor_data, sizeof(sensor_data), "%f", floatValue);
      break;
    case 5:
      display.println("ORP [mV]: ");
      break;
    case 6:
      display.setCursor(0, 0);        // Start at top-left corner
      display.print("Humedad[%]: ");  // statements PresTemp
      display.println(HumedadP);
      display.println("AireTemperatura[C]: ");  // statements PresTemp
      display.print(temperatureC);
      display.println(" C");  // statements PresTemp
      display.print("Battery: ");
      display.print(valbat);
      display.println(" %");
      if (estado == 6) {
        display.display();
        display.startscrollright(0x00, 0x07);
        Serial.println(HumedadP);
        Serial.println(temperatureC);
        Serial.println(valbat);
        delay(5000);
      }
      pito(70);
      //memcpy(&sensor_data[0], "       ", 6); //sensor_data*="Error";sensor_data = "     ";
      break;
    case 7:
      display.println("pH suelo: ");
      display.print(" 8 ");
      break;
    case 8:
      display.println("HumSuelo: ");
      display.print(" 2000 ");
      break;
    case 9:
      display.println("Luminiosidad ");
      display.print(" 500 ");
      break;
    default:
      memcpy(&sensor_data[0], "ERROR", 5);  //sensor_data*="Error";
      pito(250);
      break;
  }

  display.setTextSize(2);
  display.println(sensor_data);
  display.display();
  display.startscrollright(0x00, 0x07);
  if (estado == 1) {
    Serial.println(sensor_data);
    delay(3000);
  }
}
////////////////////////////////////////////TODO///////////////////////////////////////////////////
void loop() {
  if (estado != 0) {
    Serial.println(F("INGRESO AL NO1"));

    display.clearDisplay();
    display.setTextSize(1);   // Normal 1:1 pixel scale
    display.setCursor(0, 4);  // Start at top-left corner

    display.println("Oprima el boton LEER");
    display.println("o use la APP para");
    display.println("Iniciar Medicion");
    //display.display();
    display.display();
    display.startscrollright(0x00, 0x06);
    estado = 0;
  }

  if (serial_event == true) {  //if a command was sent to the EZO device.

    Serial.println(F("INGRESO AL NO2"));
    for (i = 0; i <= received_from_computer; i++) {  //set all char to lower case, this is just so this exact sample code can recognize the "sleep" command.
      computerdata[i] = tolower(computerdata[i]);    //"Sleep" ≠ "sleep"
    }
    i = 0;
    estado = 1;
    switch (computerdata[0]) {
      case '1':
        // statements pH
        lectura(ph_dir);
        escribaD(1);
        break;
      case '2':
        // statements OD
        //Serial.println(F("OD"));
        lectura(od_dir);
        escribaD(2);
        break;
      case '3':
        // statements CE
        lectura(ce_dir);
        escribaD(3);
        break;
      case '4':
        // statements TEMP
        //lectura(temp_dir);
        escribaD(4);
        break;
      case '5':
        // statements ORP
        lectura(orp_dir);
        escribaD(5);
        break;
      case '6':
        estado = 6;
        // statements PresTemp
        HumedadP = dht.readHumidity();
        //Serial.print(pressureKPA, 2); Serial.println(" kPa");
        temperatureC = dht.readTemperature();
        //Serial.print(temperatureC, 2); Serial.println(" *C");
        valbat = batt_info();
        escribaD(6);
        serial_event = false;  //reset the serial event flag.
        break;

      case '7':
        // statements ORP
        //lectura(orp_dir);
        escribaD(7);
        break;
      case '8':
        // statements ORP
        //lectura(orp_dir);
        escribaD(8);
        break;
      case '9':
        // statements ORP
        //lectura(orp_dir);
        escribaD(9);
        break;
      default:
        escribaD(16);
        serial_event = false;
        // statements
        break;
    }
    //Uncomment this section if you want to take the pH value and convert it into floating point number.
    //ph_float=atof(ph_data);
  }
  if (digitalRead(PD5) == 1)  //si se oprime el boton
  {
    Serial.println(F("INGRESO AL NO3"));
    estado = 2;
    memcpy(&computerdata[0], "1r", 3);
    lectura(ph_dir);
    escribaD(1);
    delay(3000);
    memcpy(&computerdata[0], "2r", 3);
    lectura(od_dir);
    escribaD(2);
    delay(3000);
    memcpy(&computerdata[0], "3r", 3);
    lectura(ce_dir);
    escribaD(3);
    delay(3000);
    memcpy(&computerdata[0], "4r", 3);
    lectura(temp_dir);
    escribaD(4);
    delay(3000);
    memcpy(&computerdata[0], "5r", 3);
    lectura(orp_dir);
    escribaD(5);
    delay(3000);
    escribaD(7);
    delay(3000);
    escribaD(8);
    delay(3000);
    escribaD(9);
    delay(3000);
    //pressureKPA = mpl115a2.getPressure();
    //Serial.print(pressureKPA, 2); Serial.println(" kPa");
    //temperatureC = mpl115a2.getTemperature();
    //Serial.print(temperatureC, 2); Serial.println(" *C");
    valbat = batt_info();
    escribaD(6);
    delay(5000);
    serial_event = false;  //reset the serial event flag.
  }

  /*if (serial_event == true) {                                                     //if a command was sent to the EZO device.
    for (i = 0; i <= received_from_computer; i++) {                               //set all char to lower case, this is just so this exact sample code can recognize the "sleep" command.
      computerdata[i] = tolower(computerdata[i]);                                 //"Sleep" ≠ "sleep"
    }
  }
  estado=0;//Serial.println(batt_info());
  display.clearDisplay();
  display.setTextSize(1);      // Normal 1:1 pixel scale
  display.setCursor(0, 4);     // Start at top-left corner
  
  display.println("Oprima el boton");
  display.println("O Use la App para");
  display.println("Iniciar mediciones");
  display.display();
  display.startscrollright(0x00, 0x06);

while(estado==0)
{
   if (serial_event == true) {                                                     //if a command was sent to the EZO device.
    for (i = 0; i <= received_from_computer; i++) {                               //set all char to lower case, this is just so this exact sample code can recognize the "sleep" command.
      computerdata[i] = tolower(computerdata[i]);                                 //"Sleep" ≠ "sleep"
    }
    i=0;

    switch (computerdata[0]) {
    case '1':
    // statements pH
      lectura(ph_dir); 
      escribaD(1); 
    break;
    case '2':
    // statements OD
    //Serial.println(F("OD"));
    lectura(od_dir);
    escribaD(2); 
    break;
    case '3':
      // statements CE
    lectura(ce_dir); 
    escribaD(3); 
    break;
    case '4':
    // statements TEMP
    lectura(temp_dir); 
    escribaD(4); 
    break;
    case '5':
    // statements ORP
    lectura(orp_dir); 
    escribaD(5); 
    break;
    case '6':
    // statements PresTemp
    pressureKPA = mpl115a2.getPressure();
    //Serial.print(pressureKPA, 2); Serial.println(" kPa");
    temperatureC = mpl115a2.getTemperature(); 
    //Serial.print(temperatureC, 2); Serial.println(" *C");
    serial_event = false;                   //reset the serial event flag.
    escribaD(6); 
    break;
    default:
    escribaD(16); 
    serial_event = false;  
    // statements
    break;
    }
    //Uncomment this section if you want to take the pH value and convert it into floating point number.
    //ph_float=atof(ph_data);
    estado=1;
  }
 if(digitalRead(PD3)==1)  //si se oprime el boton
 {
      memcpy(&computerdata[0], "1r", 3);
      lectura(ph_dir); 
      escribaD(1); 
      delay(3000);
      memcpy(&computerdata[0], "2r", 3);    
      lectura(od_dir);
      escribaD(2); 
      delay(3000); 
      memcpy(&computerdata[0], "3r", 3);
      lectura(ce_dir); 
      escribaD(3);
      delay(3000); 
      memcpy(&computerdata[0], "4r", 3); 
      lectura(temp_dir); 
      escribaD(4);
      delay(3000); 
      memcpy(&computerdata[0], "5r", 3); 
      lectura(orp_dir); 
      escribaD(5); 
      delay(3000); 
      pressureKPA = mpl115a2.getPressure();
      //Serial.print(pressureKPA, 2); Serial.println(" kPa");
      temperatureC = mpl115a2.getTemperature(); 
      //Serial.print(temperatureC, 2); Serial.println(" *C");
      escribaD(6); 
      delay(5000);
      serial_event = false;                   //reset the serial event flag. 
      estado=1;
 }
 
}*/
}

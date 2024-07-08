#include <DHT.h>   ///RESISTENCIA DE PULL UP DE 4,5K A 10K
#include <OneWire.h>
#include <DallasTemperature.h>

#define DHTPIN 2
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

#define ONE_WIRE_BUS 3

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

void setup() {
  Serial.begin(9600);
  dht.begin();
  sensors.begin();
}

void loop() {
  // Espera 5 segundos entre lecturas
  delay(5000);

  // Lectura del sensor DHT11
  float temperaturaDHT11 = dht.readTemperature();
  float humedadDHT11 = dht.readHumidity();

  // Lectura del sensor DS18B20
  sensors.requestTemperatures();
  float temperaturaDS18B20 = sensors.getTempCByIndex(0);

  // Verifica si las lecturas fueron exitosas
  if (!isnan(temperaturaDHT11) && !isnan(humedadDHT11)) {
    Serial.print("DHT11 - Temperatura: ");
    Serial.print(temperaturaDHT11);
    Serial.print(" °C, Humedad: ");
    Serial.print(humedadDHT11);
    Serial.println(" %");
  } else {
    Serial.println("Error al leer el sensor DHT11");
  }

  if (temperaturaDS18B20 != DEVICE_DISCONNECTED_C) {
    Serial.print("DS18B20 - Temperatura: ");
    Serial.print(temperaturaDS18B20);
    Serial.println(" °C");
  } else {
    Serial.println("Error al leer el sensor DS18B20");
  }
}

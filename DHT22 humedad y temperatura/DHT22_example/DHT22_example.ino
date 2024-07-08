#include <DHT.h>  //OJO RE OJO ES EL  DHT11

#define DHTPIN 2      // Define el pin al que está conectado el sensor DHT22
#define DHTTYPE DHT11  // Selecciona el tipo de sensor DHT (DHT11 o DHT22)

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);
  dht.begin();
}

void loop() {
  // Espera 5 segundos entre lecturas
  delay(5000);

  // Lee la temperatura y la humedad
  float temperatura = dht.readTemperature();
  float humedad = dht.readHumidity();

  // Verifica si la lectura fue exitosa
  if (!isnan(temperatura) && !isnan(humedad)) {
    Serial.print("Temperatura: ");
    Serial.print(temperatura);
    Serial.print(" °C, Humedad: ");
    Serial.print(humedad);
    Serial.println(" %");
  } else {
    Serial.println("Error al leer el sensor DHT");
  }
}

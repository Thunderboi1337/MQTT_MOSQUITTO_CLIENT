/**
 * @file main.cpp
 * @author Thunderboi1337
 * @brief This is simple mqtt client, sending a light topic and temprature topic. I did this for simple practice using MQTT and getting a feeling for how it works. This Client was connected to a Mosquitto broker and sometimes it was used on HiveMQ.
 *
 * @copyright Copyright (c) 2024
 *
 */

#include <WiFi.h>
#include <Arduino.h>
#include <esp32-hal.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>

const char *ssid = "SSID_NAME";
const char *password = "SSID_PASSWORD";

const char *mqtt_server = "IP_ADDRESS";
const int mqtt_port = 1883;
const char *mqtt_username = ""; // if no username is set you can leave this string empty
const char *mqtt_password = ""; // if no password is set you can leave this string empty

const char *temperature_topic = "sensor/temperature";
const char *light_topic = "sensor/light";

const int analogTemperaturePin = 32; // Analog pin for temperature sensor
const int analogLightPin = 36;       // Analog pin for light sensor

WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi()
{
    delay(10);
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}

void callback(char *topic, byte *payload, unsigned int length) // This function is used to recevice topics from the broker
{
    Serial.print("Message received on topic: ");
    Serial.println(topic);

    Serial.print("Call back Message: ");
    for (int i = 0; i < length; i++)
    {
        Serial.print((char)payload[i]);
    }
    Serial.println();
}

void reconnect()
{
    while (!client.connected())
    {
        Serial.print("Attempting MQTT connection...");

        if (client.connect("ESP32Client", mqtt_username, mqtt_password))
        {
            Serial.println("connected");
            client.subscribe(temperature_topic);
            client.subscribe(light_topic);
        }
        else
        {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" try again in 5 seconds");
            delay(5000);
        }
    }
}

void setup()
{
    Serial.begin(9600);
    setup_wifi();
    client.setServer(mqtt_server, mqtt_port);
}

void loop()
{
    if (!client.connected())
    {
        reconnect();
    }
    client.loop();

    int temperatureSensorValue = analogRead(analogTemperaturePin);              // Read analog value from temperature sensor
    float temperature = (temperatureSensorValue * (5.0 / 1023.0) - 0.5) * 10.0; // Converts the anlog value to Celsius

    int lightSensorValue = analogRead(analogLightPin); // Read analog value from light sensor

    // Publish temperature to broker
    String temperatureMessage = String(temperature);
    client.publish(temperature_topic, temperatureMessage.c_str());
    Serial.println("Message published Temprature: " + temperatureMessage + " C");

    // Publish light to broker
    String lightMessage = String(lightSensorValue);
    client.publish(light_topic, lightMessage.c_str());
    Serial.println("Message published Light: " + lightMessage);

    client.setCallback(callback);

    delay(2000); // Adjust the delay as you wish
}
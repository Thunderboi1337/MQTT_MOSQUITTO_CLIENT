import paho.mqtt.client as mqtt
import json
from datetime import datetime

mqtt_broker = "IP_ADDRESS"  # Replace with your MQTT broker's IP address
mqtt_port = 1883
mqtt_username = ""
mqtt_password = ""
temperature_topic = "sensor/temperature"
light_topic = "sensor/light"
json_file_path = "Reciver/data/mqtt_data.json"


def on_connect(client, userdata, flags, rc):
    print(f"Connected with result code {rc}")
    client.subscribe(temperature_topic)
    client.subscribe(light_topic)


def on_message(client, userdata, msg):
    print(f"Received message on topic {msg.topic}: {msg.payload.decode()}")

    timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S")

    # Load existing data from the JSON file
    try:
        with open(json_file_path, 'r') as json_file:
            data = json.load(json_file)
    except FileNotFoundError:
        data = []

    # Append the new data to the list
    data.append({"timestamp": timestamp, "topic": msg.topic,
                f"{msg.topic}": msg.payload.decode()})

    # Write the updated data back to the JSON file
    with open(json_file_path, 'w') as json_file:
        json.dump(data, json_file, indent=2)

    print(f"Data added to {json_file_path}")


client = mqtt.Client()
client.username_pw_set(mqtt_username, mqtt_password)
client.on_connect = on_connect
client.on_message = on_message

client.connect(mqtt_broker, mqtt_port, 60)

try:
    client.loop_forever()
except KeyboardInterrupt:
    print("Exiting...")
    client.disconnect()

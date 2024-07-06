import time

import redis
from flask import Flask
from flask_mqtt import Mqtt

app = Flask(__name__)
cache = redis.Redis(host='redis', port=6379)

# Configure mqtt
app.config['MQTT_BROKER_URL']  = 'localhost' # Change to hostname of mosquitto, after changing mosquitto hostname
app.config['MQTT_BROKER_PORT'] = 1883
app.config['MQTT_USERNAME'] = ''
app.config['MQTT_PASSWD'] = ''
app.config['MQTT_KEEPALIVE'] = 5
app.config['MQTT_TLS_ENABLED'] = False   # Same machine as the mqtt broker

mqtt = Mqtt()

@mqtt.on_connect()
def handle_connect(client, userdata, flags, rc):
    mqtt.subscribe("pspot/#")

@mqtt.on_message()
def handle_mqtt_message(client, userdata, message):
    data = dict(
            topic = message.topic,
            payload = message.payload.decode()
            )

    split_topic = message.topic.split('/')
    if split_topic[2] is not None:
        parking_string = "pid"+split_topic[1]+"_pspot"+split_topic[2]
        park_value = cache.get(parking_string)
        if park_value is not None:
            cache.append(parking_string, -1 * int(park_value))
        else:
            cache.append(parking_string, 1)

# We could upgrade to display each parking lot by filtering their id. We only have one so pid == 1 always
@app.route('/')
def hello():
    return 'Hello'

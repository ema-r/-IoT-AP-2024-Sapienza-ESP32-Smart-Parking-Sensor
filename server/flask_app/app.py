import time

import redis
from flask import Flask
import paho.mqtt.client as mqtt

received = False

# CALLBACKS FOR PAHO MQTT
def on_message(client, userdata, message):
    data = dict(
            topic = message.topic,
            payload = message.payload.decode()
            )
    received = True
    print('aaa', flush = True)
#    print(data.get(payload))
#    split_topic = message.topic.split('/')
#    if split_topic[2] is not None:
#        parking_string = "pid"+split_topic[1]+"_pspot"+split_topic[2]
#        park_value = cache.get(parking_string)
#        if park_value is not None:
#            cache.append(parking_string, -1 * int(park_value))
#        else:
#            cache.append(parking_string, 1)

#from paho.mqtt.enums import CallbackAPIVersion
def on_connect(client, userdata, flags, reason_code, properties):
    if reason_code.is_failure:
        print(f"Failed to connect: {reason_code}. loop_forever() will retry connection")
    else:
        client.subscribe("test")


app = Flask(__name__)
cache = redis.Redis(host='redis', port=6379)

mqtt_broker_uri = "mosquitto"
mqtt_username = "user1"
mqtt_passwd = "test"


# We could upgrade to display each parking lot by filtering their id. We only have one so pid == 1 always
@app.route('/')
def hello():
    return 'Hello'

@app.route('/first_setup')
def stest():
    if received:
        return 'nice'
    else:
        return 'not nice'

with app.app_context():
    mqttc = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)
    mqttc.on_connect = on_connect
    mqttc.on_message = on_message

    mqttc.username_pw_set(mqtt_username, mqtt_passwd)
    mqttc.user_data_set([])
    mqttc.connect(mqtt_broker_uri, 1883, 60)
    mqttc.loop_start()


if __name__ == '__main__':
    app.run(host='127.0.0.1', port=5000)


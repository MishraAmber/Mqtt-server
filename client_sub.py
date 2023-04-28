import paho.mqtt.client as mqtt
import time

flag_connected = 0
flag_messagerecieved = 0 

def on_connect(client,userdata,flag,rc):
    if rc == 0:
        global flag_connected 
        flag_connected = 1
        print("Connected to mqtt server")
    else:
        print("Disconnected from mqtt server")

def on_message(client,userdata,message):
    print(str(message.topic)+" : " + str(message.payload.decode("utf8")))

def client_subscription(client):
    client.subscribe("esp32/#")
    client.subscribe("temperature")
    client.subscribe("humidity")
    client.subscribe("distance")
    client.subscribe("alcohol")
    client.subscribe("vooc")

client = mqtt.Client("Rpi_client1")
client.on_connect = on_connect
client.connect('127.0.0.1',1883)
client.message_callback_add('temperature',on_message)
client.message_callback_add('humidity',on_message)
client.message_callback_add('distance',on_message)
client.message_callback_add('alcohol',on_message)
client.message_callback_add('vooc',on_message)
client.loop_start()
client_subscription(client)
print("Setup Complete")
print("---------------------------------------------")

while True:
    time.sleep(4)
    if(flag_connected !=1):
        print("Trying to Connect to mqtt server..")

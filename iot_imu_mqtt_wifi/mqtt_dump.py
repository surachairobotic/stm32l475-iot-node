import paho.mqtt.subscribe as subscribe
import paho.mqtt.client as mqtt

fname = "test_move.txt"

# The callback for when the client receives a CONNACK response from the server.
def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))

    # Subscribing in on_connect() means that if we lose the connection and
    # reconnect then subscriptions will be renewed.
    client.subscribe(topic="@msg/sensor_data/1", qos=0)

# The callback for when a PUBLISH message is received from the server.
def on_message(client, userdata, msg):
    global fname
    print(msg.topic+" "+str(msg.payload))
    f = open(str(fname), "a")
    f.writelines(str(msg.payload.decode("utf-8"))+'\n')
    f.close()


def on_subscribe(client, userdata, mid, granted_qos):
    print("Subscribed "+str(granted_qos))

def main():
    global fname

    f = open(str(fname), "w")
    f.close()


    b_server = True

    if b_server:
      # aoff
      CLIENT_ID = 'f9cf386f-c6ab-4126-9eb7-96afa00c9095'
      NETPIE_TOKEN = 'YNUUUmtUZpRaNMYaeLRTuvxCXrzkg86a'
    else:
      # surachai
      CLIENT_ID = '662b41f4-27f8-431e-b4d7-4fae5f812097'
      NETPIE_TOKEN = 'pZEegSjycKvK4UAnDiQ3K4q1TsyS956h'

    client = mqtt.Client(client_id=CLIENT_ID)
    client.on_connect = on_connect
    client.on_message = on_message
    client.username_pw_set(NETPIE_TOKEN)
    client.connect("mqtt.netpie.io", 1883, 60)


    # Blocking call that processes network traffic, dispatches callbacks and
    # handles reconnecting.
    # Other loop*() functions are available that give a threaded interface and a
    # manual interface.

    client.loop_forever()
    
    
if __name__ == '__main__':
  main()

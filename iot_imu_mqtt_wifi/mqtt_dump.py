import paho.mqtt.subscribe as subscribe

# aoff
CLIENT_ID = 'f9cf386f-c6ab-4126-9eb7-96afa00c9095'
NETPIE_TOKEN = 'YNUUUmtUZpRaNMYaeLRTuvxCXrzkg86a'

# surachai
# CLIENT_ID = '662b41f4-27f8-431e-b4d7-4fae5f812097'
# NETPIE_TOKEN = 'pZEegSjycKvK4UAnDiQ3K4q1TsyS956h'

if __name__ == '__main__':
    while True:
        msg = subscribe.simple('@msg/sensor_data/bank', hostname='mqtt.netpie.io', port=1883, client_id=CLIENT_ID, auth={'username':NETPIE_TOKEN, 'password':None}, keepalive=10)
        print("%s %s" % (msg.topic, msg.payload))

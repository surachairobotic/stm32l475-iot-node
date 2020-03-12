import paho.mqtt.subscribe as subscribe

CLIENT_ID = 'f3c4b79d-3efc-4cde-bafb-0599d3772bb8'
NETPIE_TOKEN = 'RJ9AKph2eN2KWCFP6JFn9UFhjtejmqWt'

if __name__ == '__main__':
    while True:
        msg = subscribe.simple('@msg/taist2020/button/#', hostname='mqtt.netpie.io', port=1883, client_id=CLIENT_ID, auth={'username':NETPIE_TOKEN, 'password':None}, keepalive=10)
        print("%s %s" % (msg.topic, msg.payload))

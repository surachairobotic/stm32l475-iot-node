import asyncio
from time import sleep
from bleak import discover

async def scan(mac_addrs):
    while True:
        #print('Start scanning')
        #tstart = loop.time()
        devices = await discover()
        #print('Found %d devices'%(len(devices)))
        #for i in range(len(devices)):
        #    print("[%d] : %s" % (i, devices[i]))
        for dev in devices:
            dev_mac = str(dev).split(': ')[0]
            if dev_mac in mac_addrs:
                #print(dev_mac, 'detected at', dev.rssi, 'dBm')
                print(dev.rssi)
        #telapsed = loop.time() - tstart
        #print('Elapsed time: %.1f'%(telapsed))
        #await asyncio.sleep(6 - telapsed)

if __name__ == '__main__':
    mac_addrs = ("D6:07:04:2B:F7:B1") # surachai
    #mac_addrs = ("E0:75:58:38:05:A4") # inn
    
    loop = asyncio.get_event_loop()
    loop.create_task(scan(mac_addrs))
    try:
        loop.run_forever()
    except KeyboardInterrupt:
        loop.close()
        print('Program stopped')

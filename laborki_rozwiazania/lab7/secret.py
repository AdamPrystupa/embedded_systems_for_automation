import network, utime
def wifi_connect():
    sta = network.WLAN(network.STA_IF)
    if not sta.isconnected():
        sta.active(True)
        sta.connect("SWA","systemywbudowane")
        while not sta.isconnected():
            utime.sleep_ms(500)
    print('Connected:', sta.ifconfig())

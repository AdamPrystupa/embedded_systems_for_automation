from machine import Pin
import rp2, array 
NUM_LEDS = const(16) 
PIN_NUM = const(15) 
# dane pikseli po 32 bity (używane 24:GRB) na każdy LED 
ar = array.array("I", [0 for _ in range(NUM_LEDS)]) 
# PIO w wersji zanegowanej 
@rp2.asm_pio(sideset_init=rp2.PIO.OUT_HIGH, 
out_shiftdir=rp2.PIO.SHIFT_LEFT, autopull=True, pull_thresh=24) 
def ws2812(): 
    T1 = 2 
    T2 = 5 
    T3 = 3 
    wrap_target() 
    label("bitloop") 
    out(x, 1) .side(1) [T3 - 1] 
    jmp(not_x, "do_zero") .side(0) [T1 - 1] 
    jmp("bitloop") .side(0) [T2 - 1] 
    label("do_zero") 
    nop() .side(1) [T2 - 1] 
    wrap() 
# włączenie maszyny PIO 
sm = rp2.StateMachine(0, ws2812, freq=8_000_000, 
sideset_base=Pin(PIN_NUM)) 
sm.active(1) 
# pompowanie danych z podmianą RGB na GRB 
def ws_show(): 
    dimmer_ar = array.array("I", [0 for _ in range(NUM_LEDS)]) 
    for i,c in enumerate(ar): 
        r = int(((c >> 8) & 0xFF)) 
        g = int(((c >> 16) & 0xFF)) 
        b = int((c & 0xFF)) 
        dimmer_ar[i] = (g<<16) + (r<<8) + b 
    sm.put(dimmer_ar, 8)
# ustaw kolor jednego piksela 
def ws_set(i, color): 
    ar[i] = (color[1]<<16) + (color[0]<<8) + color[2] 
# ustaw kolor wszystkich pikseli 
def ws_fill(color): 
    for i in range(len(ar)): 
        ws_set(i, color)

# Przykład wywołania:
# Zapal wszystkie LEDy na biało (kolor RGB: 255,255,255)
ws_fill((255, 0, 0))
ws_show()



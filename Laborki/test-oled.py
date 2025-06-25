import ssd1306
from machine import I2C, Pin
import utime

i2c = I2C(0, sda=Pin(8), scl=Pin(9))
display = ssd1306.SSD1306_I2C(128, 32, i2c)
display.contrast(0)

while True:
    display.text('Beam Kit v.1', 0, 0, 1)
    display.text('J.M.nich', 0, 8, 1)
    display.text('K.J.U.rbanski', 0, 16, 1)
    display.text('################', 0, 24, 1)
    display.show()
    utime.sleep(1)

    display.fill(1)
    display.show()
    utime.sleep(1)

    display.fill(0)
    display.show()
    utime.sleep(1)


import micropython
import utime
from machine import I2C, Pin, Timer
from mpu9250 import MPU9250

micropython.alloc_emergency_exception_buf(100)

# I2C port on Measurement-Board
i2c = I2C(1,scl=Pin(7),sda=Pin(6),freq=400_000)
sensor = MPU9250(i2c)


while True:
    value = sensor.magnetic
    print(",".join(map(str, value)))
    utime.sleep_ms(500)
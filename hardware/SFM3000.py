from smbus2 import SMBus, i2c_msg
from time import sleep

class SFM3000:
    def __init__(self, address = 0x40):
        self.address = address
        self.bus = SMBus(1)
        self.offset = 32000
        self.scale = 140.0

    def soft_reset(self):
        msg = i2c_msg.write(self.address, [0x20, 0x00])
        self.bus.i2c_rdwr(msg)

    def get_flow(self) -> float:
        write = i2c_msg.write(self.address, [0x10, 0x00])
        self.bus.i2c_rdwr(write)
        sleep(0.03)
        read = i2c_msg.read(self.address, 3)
        self.bus.i2c_rdwr(read)
        [a, b, crc] = list(read)
        result = a << 8 | b
        flow = (float(result) - self.offset) / self.scale
        return flow

    def get_serial(self):
        write = i2c_msg.write(self.address, [0x31, 0xAE])
        self.bus.i2c_rdwr(msg)
        read = i2c_msg.read(self.address, 4)
        self.bus.i2c_rdwr(read)
        serial = list(read)
        return serial

    def close(self):
        self.bus.close()


if __name__ == '__main__':
    sfm = SFM3000()
    sfm.soft_reset()
    while True:
        flow = sfm.get_flow()
        print(flow)
    sfm.close()

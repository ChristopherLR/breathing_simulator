from smbus2 import SMBus
from time import sleep

class SFM3000:

    def __init__(self, address = 0x40, sda_pin = 2):
        self.address = address
        self.sda_pin = sda_pin
        self.bus = SMBus(1)
        self.commands = {
            "start_measure": 0x1000,
            "read_serial": 0x31AE,
            "soft_reset": 0x2000,
        }

    def soft_reset(self):
        b = self.bus.write_i2c_block_data(0x40, 0, [0x20, 0x00])
        sleep(1)
        b = self.bus.read_i2c_block_data(0x40, 0, 3)
        print(b)

    def read_flow(self):
        b = self.bus.write_i2c_block_data(0x40, 0, [0x10, 0x00])
        sleep(1)
        b = self.bus.read_i2c_block_data(0x40, 0, 3)
        print(b)

    def read_serial(self):
        self.bus.write_i2c_block_data(0x81, 0, [0x31, 0xAE])
        sleep(1)
        b = self.bus.read_i2c_block_data(0x40, 0, 4)
        print(b)

    def close(self):
        self.bus.close()


if __name__ == '__main__':
    sfm = SFM3000()
    sfm.read_flow()
    sfm.read_serial()
    sfm.read_flow()
    sfm.read_flow()
    sfm.read_flow()
    sfm.read_flow()
    sfm.read_flow()
    sfm.read_flow()
    sfm.read_flow()
    sfm.read_flow()
    sfm.read_flow()
    sfm.read_flow()
    sfm.read_flow()
    sfm.close()

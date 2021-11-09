from __future__ import annotations
import RPi.GPIO as IO
from enum import Enum, auto
import signal
import sys

class DriverState(Enum):
    initialised = auto()
    uninitialised = auto()

class Result(Enum):
    success = auto()
    driver_already_initialised = auto()

class L298N:
    def __init__(self, pos_pin: int, neg_pin: int, pwm_pin: int):
        self.pos_pin = pos_pin
        self.neg_pin = neg_pin
        self.pwm_pin = pwm_pin
        self.pwm = None
        self.state = DriverState.uninitialised
        signal.signal(signal.SIGUSR1, self.exit_handler)
    
    def exit_handler(self, signum, frame):
        print(f'Caught {signum} cleaning up IO')
        self.destroy()

    def setup(self) -> (L298N, Result):
        if self.state == DriverState.uninitialised:
            print(f'Initialising L298N Motor Driver: POS {self.pos_pin}, NEG {self.neg_pin}, PWM: {self.pwm_pin}')
            IO.setmode(IO.BCM)
            IO.setup(self.pos_pin, IO.OUT)
            IO.setup(self.neg_pin, IO.OUT)
            IO.setup(self.pwm_pin, IO.OUT)
            self.pwm = IO.PWM(self.pwm_pin, 1000)
            self.pwm.start(0)
            self.state = DriverState.initialised
            return (self, Result.success)
        elif self.state == DriverState.initialised:
            print('Motor Driver already initialised')
            return (self, Result.driver_already_initialised)
        else:
            print('Motor Driver in unknown state')
            return (self, Result.failure)

    def pos(self, power: float):
        if power > 100:
            self.destroy()
            raise AssertionError(f'Power: {power} needs to be less than 100')
        if self.state == DriverState.initialised:
            IO.output(self.pos_pin, IO.HIGH)
            IO.output(self.neg_pin, IO.LOW)
            self.pwm.ChangeDutyCycle(power)
        else:
            print(f'Motor Driver needs to be initialised: {self.state}')


    def destroy(self):
        if self.state == DriverState.initialised:
            self.pwm.stop()
            IO.output(self.pos_pin, IO.LOW)
            IO.output(self.neg_pin, IO.LOW)
            IO.cleanup()
            self.state = DriverState.uninitialised
            print('Cleaned up Motor Driver')
        else:
            print('Motor Driver already cleaned up')

if __name__ == '__main__':
    print('This should not be run as main')

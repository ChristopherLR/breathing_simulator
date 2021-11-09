from hardware.L298N import L298N
import time
import signal
import sys
import os

def exit_handler(signum, frame):
    os.kill(os.getpid(), signal.SIGUSR1)
    sys.exit(0)

def start():
    signal.signal(signal.SIGINT, exit_handler)
    (motor_driver, result) = L298N(pos_pin = 13, neg_pin = 6, pwm_pin = 19).setup()
    print(result)
    motor_driver.pos(100)
    time.sleep(20)
    motor_driver.destroy()

if __name__ == '__main__':
    start()

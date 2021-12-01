from hardware.L298N import L298N
from hardware.SFM3000 import SFM3000
import time
import signal
import sys
import json
import os

(motor_driver, result) = L298N(pos_pin = 13, neg_pin = 6, pwm_pin = 19).setup()
flow_meter = SFM3000()

def exit_handler(signum, frame):
    os.kill(os.getpid(), signal.SIGUSR1)
    motor_driver.destroy()
    flow_meter.close()
    sys.exit(0)

def start():
    data = []
    signal.signal(signal.SIGINT, exit_handler)
    flow = flow_meter.get_flow()
    ts = time.time()
    data.append({'ts': ts, 'flow': flow})
    t_end = time.time() + 4
    motor_driver.drive(100)
    while time.time() < t_end:
        flow = flow_meter.get_flow()
        ts = time.time()
        data.append({ 'ts': ts, 'flow': flow })

    t_end = time.time() + 4
    motor_driver.drive(0)
    while time.time() < t_end:
        flow = flow_meter.get_flow()
        ts = time.time()
        data.append({ 'ts': ts, 'flow': flow })

    with open('9HV_fan.json', 'w') as fp:
        json.dump(data, fp)

    motor_driver.destroy()

def run():
    signal.signal(signal.SIGINT, exit_handler)
    flow = flow_meter.get_flow()
    print(flow)
    motor_driver.drive(100)
    while True:
        flow = flow_meter.get_flow()
        print(flow)


if __name__ == '__main__':
    #start()
    run()

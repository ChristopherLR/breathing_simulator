import asyncio
from dotenv import dotenv_values
import signal
import functools
import time
import json

config = dotenv_values()

dead = False

async def read_messages(reader, writer, addr):
    print(f'{addr} connected')
    while True:
        data = await reader.read(100)
        if reader.at_eof():
            print('kill')
            global dead
            dead = True
            writer.close()
            return
        message = data.decode()
        print(f'recv: {message}')

async def send_heartbeat(writer):
    while dead == False:
        print(f'send heartbeat')
        writer.write(bytes(json.dumps({ 'msg': 'heartbeat', 'ts': time.time() }), encoding='utf-8'))
        await writer.drain()
        await asyncio.sleep(0.5)

async def handle_connection(reader, writer):
    loop = asyncio.get_event_loop()
    addr = writer.get_extra_info('peername')
    loop.create_task(read_messages(reader, writer, addr))
    loop.create_task(send_heartbeat(writer))

async def main():
    server = await asyncio.start_server(handle_connection, config['IP_ADDRESS'], int(config['PORT']))
    addrs = ', '.join(str(sock.getsockname()) for sock in server.sockets)
    print(f'Serving on {addrs}')

    async with server:
        await server.serve_forever()

def stopper(signame, loop):
    print(f'Received {signame}, stopping...')
    loop.stop()


def start():
    loop = asyncio.get_event_loop()
    for signame in ('SIGINT', 'SIGTERM'):
        loop.add_signal_handler(getattr(signal, signame), functools.partial(stopper, signame, loop))

    loop.create_task(main())
    loop.run_forever()

if __name__ == '__main__':
    start()
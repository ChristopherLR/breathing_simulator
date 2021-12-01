import asyncio
from dotenv import dotenv_values

config = dotenv_values()


async def handle_connection(reader, writer):
    data = await reader.read()
    message = data.decode()
    addr = writer.get_extra_info('peername')

    print(f'recv: {message} from {addr}')

async def main():
    server = await asyncio.start_server(handle_connection, config['IP_ADDRESS'], int(config['PORT']))
    addrs = ', '.join(str(sock.getsockname()) for sock in server.sockets)
    print(f'Serving on {addrs}')

    async with server:
        await server.serve_forever()

def start():
    asyncio.run(main())

if __name__ == '__main__':
    start()
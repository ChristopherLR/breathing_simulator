import asyncio
from dotenv import dotenv_values

config = dotenv_values()

async def tcp_echo_client(message):
    reader, writer = await asyncio.open_connection(config['IP_ADDRESS'], config['PORT'])

    print(f'Send: {message!r}')
    writer.write(message.encode())
    await writer.drain()


    data = await reader.read(100)
    print(f'Received: {data.decode()!r}')

    print('Close the connection')
    writer.close()


def main():
  asyncio.run(tcp_echo_client('Hello World!'))

def start():
  main()

if __name__ == '__main__':
  start()
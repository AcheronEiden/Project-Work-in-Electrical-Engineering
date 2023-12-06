import serial
import asyncio
import websockets

async def send_data(websocket, ser):
    try:
        while True:
            data = ser.readline().decode('utf-8').strip()
            await websocket.send(data)
    except websockets.ConnectionClosedError:
        print("WebSocket connection closed.")

ser = serial.Serial('COM6', 115200)
websocket_url = 'ws://localhost:8000'

async def main():
    async with websockets.connect(websocket_url) as websocket:
        await asyncio.gather(send_data(websocket, ser))

if __name__ == "__main__":
    asyncio.run(main())
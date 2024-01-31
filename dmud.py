import socketio
import redis

sio = socketio.Client()

@sio.event
def connect():
    print('connection established')

@sio.event
def my_message(data):
    print('message received with ', data)
#    sio.emit('my response', {'response': 'my response'})

@sio.event
def disconnect():
    reconnect()
    print('disconnected from server')
  
def reconnect():
    sio.connect('http://192.168.0.7:5000')
    sio.wait()

reconnect()

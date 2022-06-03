import io
import serial.tools.list_ports
import pynmea2
import serial
import sqlite3
import datetime

# find ports
ports = serial.tools.list_ports.comports()
available_ports = []
for p in ports:
    available_ports.append(p.device)

ser = serial.Serial(available_ports[0], 9600, timeout=5.0)
con = sqlite3.connect("/home/rpm/irap/gps.db")
cur = con.cursor()
while 1:
    try:
        line = ser.readline()
#        print(line.decode("utf-8"))
        msg = pynmea2.parse(line.decode("utf-8") )
        if msg.sentence_type == 'GGA':
            timestamp = datetime.datetime.now()
            lon = msg.lon + msg.lon_dir
            lat = msg.lat + msg.lat_dir
            altitude = msg.altitude
            cur.execute("update gps set lon=?, lat=?, altitude=?,timstamp=?",(lon, lat, altitude, timestamp))
            con.commit()
            print(repr(msg))
    except serial.SerialException as e:
        print('Device error: {}'.format(e))
        break
    except pynmea2.ParseError as e:
        print('Parse error: {}'.format(e))
        continue
~

from flask import Flask, render_template, Response
import io
import time
import sqlite3
import json

app = Flask(__name__)
@app.route('/')
def index():
    # rendering webpage
    return render_template('index.html')

@app.route('/get')
def get():
    con = sqlite3.connect('/home/rpm/irap/gps.db')
    cur = con.cursor()
    cur.execute("select * from gps")
    row = cur.fetchone()
    print(json.dumps(row))
    con.close()
    return Response(json.dumps(row),
                    mimetype='application/json')


if __name__ == '__main__':
    # defining server ip address and port
    app.run(host='0.0.0.0',port='5200', debug=False)

import flask
from flask import request, jsonify
from pymongo import MongoClient
import json
from influxdb import InfluxDBClient

client_mongodb = MongoClient('localhost', 27017)
database_mongodb = client_mongodb["selfbalancing"]
collection_mongodb = database_mongodb["selfbalancing"]

app = flask.Flask(__name__)
app.config["DEBUG"] = True

@app.route('/', methods=['GET'])
def home():
    return '''<h1>API v.0.01</h1>
    <p>A prototype API for reading self-balancing information</p>'''

@app.route('/api/v0.1/selfbalancing/mongodb/all', methods=['GET'])
def api_all():
    x = collection_mongodb.find({}, {'_id':False})
    docs = list(x)
    print(docs)
    return jsonify(docs)

@app.route('/api/v0.1/selfbalancing/mongodb/1000', methods=['GET'])
def api_1000():
    x = collection_mongodb.find({}, {'_id':False})
    docs = list(x)
    print(docs)
    return jsonify(docs[:1000])


@app.route('/api/v0.1/selfbalancing/influx/all', methods=['GET'])
def api_influx_all():
    client = InfluxDBClient(host='localhost', port=8086)
    client.switch_database('selfbalancing')
    query = 'SELECT * FROM selfbalancing'
    result = client.query(query)
    return jsonify(list(result))


app.run(host="0.0.0.0")
import flask
from flask import request, jsonify
from pymongo import MongoClient
import json

client_mongodb = MongoClient('localhost', 27017)
database_mongodb = client_mongodb["selfbalancing"]
collection_mongodb = database_mongodb["selfbalancing"]

app = flask.Flask(__name__)
app.config["DEBUG"] = True

@app.route('/', methods=['GET'])
def home():
    return '''<h1>API v.0.01</h1>
    <p>A prototype API for reading self-balancing information</p>'''

@app.route('/api/v0.01/selfbalancing/mongodb/all', methods=['GET'])
def api_all():
    x = collection_mongodb.find({})
    for y in x:
        json_final = {
            "gyroy": y["gyroy"],
            "kalangley": y["kalangley"],
            "pitch": y["pitch"],
            "res": y["res"],
            "kd": y["kd"],
            "ki": y["ki"],
            "kp": y["kp"]
        }
    print(json_final)
    return jsonify(json_final)

app.run()
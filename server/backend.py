import sys
import os
import json
import socket
import datetime
from flask import Flask, current_app, request, abort
from itsdangerous import BadData, TimedJSONWebSignatureSerializer as Serializer
import sql

pg = sql.PostgreSQL(database="SmartPillowDB",
                    user="postgres", password="jimpsql")
if "MSC" in sys.version:
    HOST = "127.0.0.1"
elif "GCC" in sys.version:
    HOST = "172.17.0.13"
else:
    HOST = "localhost"
SERVER_PORT = 12345

app = Flask(__name__)
app.config['SECRET_KEY'] = 'SmartPillowProject'


def createToken(tokenDict, expireTime=3600):
    """
    create token
    parameter:
        tokenDict: a dict
        expireTime: int
    return:
        token: str, ascii encoding
    """
    s = Serializer(current_app.config["SECRET_KEY"], expires_in=expireTime)
    token = s.dumps(tokenDict).decode("ascii")
    return token


def verifyToken(token):
    """
    verify and decode token
    parameter:
        token: str, ascii encoding
    return:
        tokenDict: a dict
    """
    s = Serializer(current_app.config["SECRET_KEY"])
    tokenDict = s.loads(token)
    # tuple (tokenDict["UserID"],)
    if (tokenDict["UserID"], tokenDict["Password"]) not in pg.select(("UserID", "Password"), "UserTable"):
        raise Exception("Invalid token")
    return tokenDict


@app.route("/login", methods=["POST"])
def login():
    """
    post jsonData to backend
    jsonData = {"UserName":str,"Password":str}
    return a json data
    if success return {"Token":token}
    else abort http 500
    """

    def check(data):
        for key in data.keys():
            if key not in ["UserName", "Password"]:
                raise Exception(
                    """key not in ["UserName", "Password"]""")
        return data

    try:
        data = request.get_json()
        data = check(data)
        UserName = data["UserName"].lower()
        Password = data["Password"]
        UserID, TruePassword = pg.select(("UserID", "Password"), "UserTable",
                                         f""" "UserName"='{UserName}' """)[0]
        if Password != TruePassword:
            raise Exception("Wrong password")

        tokenDict = {"UserID": UserID,
                     "UserName": UserName, "Password": Password}
        token = createToken(tokenDict)
        return {"Token": token}
    except:
        abort(401)


@app.route("/chart", methods=["POST"])
def chart():
    """
    post jsonData to backend
    jsonData = {"Action":"select","Token":token}
    or  {"Action":str, "Token":token, "Data":None}
    actually only Token is used
    return a json data
    if success return {"Data": list of {"date", "type", "value}}
    else abort http 500
    """

    def check(data):
        for key in data.keys():
            if key not in ["Token", "Data", "Action"]:
                raise Exception(
                    """key not in ["Token", "Data", "Action"]""")
        return data

    def selectTurn(UserID):
        dateNow = datetime.datetime.now()
        dateStart = (dateNow - datetime.timedelta(days=7)).isoformat().split('T')[0]
        dateEnd = dateNow.isoformat().split('T')[0]
        TurnInfo = pg.select(("Date", "TurnCount"), "TurnTable",
                             f""" "UserID"='{UserID}' AND "Date" BETWEEN '{dateStart}' AND {dateEnd} """)
        DataTurn = []
        for Turn in TurnInfo:
            DateTime, TurnCount = Turn
            resTurn = {"date": DateTime,
                       "type": "翻身次数",
                       "value": TurnCount
                       }
            DataTurn.append(resTurn)
        return DataTurn

    def selectSleepingTime(UserID):
        dateNow = datetime.datetime.now()
        dateStart = (dateNow - datetime.timedelta(days=7)).isoformat().split('T')[0]
        dateEnd = dateNow.isoformat().split('T')[0]
        SleepInfos = pg.select(("Date", "SleepTime"), "SleepingTable",
                               f""" "UserID"='{UserID}' AND "Date" BETWEEN '{dateStart}' AND '{dateEnd}' """)
        DataSleep = []
        for SleepInfo in SleepInfos:
            DateTime, SleepingTime = SleepInfo
            resTmp = {"date": DateTime,
                      "type": "睡眠时间",
                      "value": SleepingTime
                      }
            DataSleep.append(resTmp)
        return DataSleep

    try:
        data = request.get_json()
        data = check(data)
        Token = data["Token"]
        UserID = verifyToken(Token)["UserID"]
        #res = []
        #res.extend(selectTurn(UserID))
        #res.extend(selectSleepingTime(UserID))
        #return {"Data": res}
        return {"Data" : UserID}
    except:
        abort(401)


@app.route("/register", methods=["POST"])
def register():
    """
    post jsonData to backend
    jsonData = {"UserName":str,"Password":str}
    return a json data
    if success return {"Token":token}
    else abort http 500
    """

    def check(data):
        for key in data.keys():
            if key not in ["UserName", "Password"]:
                raise Exception(
                    """key not in ["UserName", "Password"]""")
        return data

    try:
        data = request.get_json()
        data = check(data)
        UserName = data["UserName"].lower()
        Password = data["Password"]
        exist = pg.select(("UserID"), "UserTable",
                          f""" "UserName"='{UserName}' """)
        if exist == [] or exist == [[]]:  # Account not exists
            pg.insert({"UserName": UserName, "Password": Password},
                      "UserTable")
            UserID = pg.select(("UserID"), "UserTable",
                               f""" "UserName"='{UserName}' """)[0][0]
        else:
            raise Exception("Account already exists")

        tokenDict = {"UserID": UserID,
                     "UserName": UserName, "Password": Password}
        token = createToken(tokenDict)
        return {"Token": token}
    except:
        abort(401)


@app.route("/setting", methods=["POST"])
def setting():
    """
    post json to backend
    jsonData = {"Action":str, "Token":token, "Data":list of dict}
    "Action" can be "select", "insert", "delete" or "update"
    if "select", "Data" can be "Data":None,
    if "insert", "TimePeriod" -> "TimeID" will be removed automatically
    "Data" = list of dict "TimePeriod"
    "TimePeriod" = {"TimeID":int, "SleepingTime":isoTime, "WakeupTime":isoTime}
    if success return {"Data":json}
    elif token expired abort http 401
    else abort http 500
    """

    def sendChange(UserID):
        DeviceID = pg.select(("DeviceID", "DeviceName"), "DeviceTable",
                             f""" "UserID"='{UserID}' """)[0][0]
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        # TCP client - connect to server
        s.connect((HOST, SERVER_PORT))
        x = {"DeviceID": DeviceID}
        mes = json.dumps(x)
        s.send(mes.encode())
        s.close()

    def check(data):
        for key in data.keys():
            if key not in ["Action", "Token", "Data"]:
                raise Exception(
                    """key not in ["Action", "Token", "Data"]""")
        if data["Action"] not in ["select", "insert", "delete", "update"]:
            raise Exception(
                '''"Action" can only be "select", "insert", "delete" or "update"''')
        return data

    def select(UserID):
        TimePeriods = pg.select(("TimeID", "SleepingTime", "WakeupTime"), "TimeTable",
                                f""" "UserID"='{UserID}' """)
        Data = []
        for i in range(len(TimePeriods)):
            TimeID, SleepingTime, WakeupTime = TimePeriods[i]
            TimePeriod = {"TimeID": TimeID,
                          "SleepingTime": SleepingTime.isoformat(),
                          "WakeupTime": WakeupTime.isoformat()}
            Data.append(TimePeriod)
        return Data

    def checkConflict(newTimePeriod, UserID):

        def inPeriod(time, TimePeriod):
            SleepingTime = TimePeriod["SleepingTime"]
            WakeupTime = TimePeriod["WakeupTime"]
            if SleepingTime > WakeupTime:
                if WakeupTime >= time or time >= SleepingTime:
                    return True
            else:
                if WakeupTime >= time and time >= SleepingTime:
                    return True
            return False

        TimePeriods = select(UserID)
        for TimePeriod in TimePeriods:
            if inPeriod(newTimePeriod["SleepingTime"], TimePeriod) or \
                    inPeriod(newTimePeriod["WakeupTime"], TimePeriod) or \
                    inPeriod(TimePeriod["SleepingTime"], newTimePeriod) or \
                    inPeriod(TimePeriod["WakeupTime"], newTimePeriod):
                raise Exception("Time Conflict")
        return

    try:
        jsonData = request.get_json()
        jsonData = check(jsonData)
        Action = jsonData["Action"]
        Token = jsonData["Token"]
        if "Data" in jsonData.keys():
            Data = jsonData["Data"]
        UserID = verifyToken(Token)["UserID"]

        if Action == "select":
            Data = select(UserID)

        elif Action == "insert":
            for TimePeriod in Data:
                if "TimeID" in TimePeriod.keys():
                    TimePeriod.pop("TimeID")
                checkConflict(TimePeriod, UserID)
                TimePeriod.update({"UserID": UserID})
                pg.insert(TimePeriod, "TimeTable")
            Data = select(UserID)
            sendChange(UserID)

        elif Action == "delete":
            for TimePeriod in Data:
                TimeID = TimePeriod["TimeID"]
                pg.delete("TimeTable", f""" "TimeID"='{TimeID}' """)
            Data = select(UserID)
            sendChange(UserID)

        elif Action == "update":
            for TimePeriod in Data:
                TimePeriod.update({"UserID": UserID})
                TimeID = TimePeriod["TimeID"]
                pg.update(TimePeriod, "TimeTable",
                          f""" "TimeID"='{TimeID}' """)
            Data = select(UserID)
            sendChange(UserID)

        return {"Data": Data}
    except BadData:
        abort(401)
    except:
        abort(500)


@app.route("/device", methods=["POST"])
def device():
    """
    post json to backend
    jsonData = {"Action":str, "Token":token, "Data":json}
    "Action" can be "select", "insert", "delete" or "update"
    "Data" = {"DeviceNumber":2, "Device0":Device, "Device1":Device}
    "Device" = {"DeviceID":int, "DeviceName":str}
    if success return {"Data":json}
    elif token expired abort http 401
    else abort http 500
    """

    def check(data):
        for key in data.keys():
            if key not in ["Action", "Token", "Data"]:
                raise Exception(
                    """key not in ["Action", "Token", "Data"]""")
        if data["Action"] not in ["select", "insert", "delete", "update"]:
            raise Exception(
                '''"Action" can only be "select", "insert", "delete" or "update"''')
        return data

    def select(UserID):
        DevicesInfo = pg.select(("DeviceID", "DeviceName"), "DeviceTable",
                                f""" "UserID"='{UserID}' """)
        Data = []
        for Device in DevicesInfo:
            DeviceID, DeviceName = Device
            Device = {"DeviceID": DeviceID,
                      "DeviceName": DeviceName}
            Data.append(Device)
        return Data

    def checkConflict(newDevice, UserID):
        Devices = select(UserID)
        if newDevice not in Devices:
            raise Exception("newDevice not exist")
        pass

    try:
        jsonData = request.get_json()
        jsonData = check(jsonData)
        Action = jsonData["Action"]
        Token = jsonData["Token"]
        if "Data" in jsonData.keys():
            Data = jsonData["Data"]
        UserID = verifyToken(Token)["UserID"]

        if Action == "select":
            Data = select(UserID)

        elif Action == "insert":
            for Device in Data:
                # UserID=0 is the administer, who controls all free devices
                checkConflict(Device, UserID=0)
                Device.update({"UserID": UserID})
                DeviceID = Device["DeviceID"]
                pg.update(Device, "DeviceTable",
                          f""" "DeviceID"='{DeviceID}' """)
            Data = select(UserID)

        elif Action == "delete":
            for Device in Data:
                Device.update({"UserID": 0})
                if "DeviceName" in Device.keys():
                    Device.pop("DeviceName")
                DeviceID = Device["DeviceID"]
                pg.update(Device, "DeviceTable",
                          f""" "DeviceID"='{DeviceID}' """)
            Data = select(UserID)

        elif Action == "update":
            for Device in Data:
                Device.update({"UserID": UserID})
                DeviceID = Device["DeviceID"]
                pg.update(Device, "DeviceTable",
                          f""" "DeviceID"='{DeviceID}' """)
            Data = select(UserID)

        return {"Data": Data}
    except BadData:
        abort(401)
    except:
        abort(500)


@app.route("/account", methods=["POST"])
def account():
    """
    post json to backend
    jsonData = {"Action":str, "Token":token, "Data":json}
    "Action" can be "changePassword"
    "Data" = {"Password":str}
    if success return {"Token":token}
    elif token expired abort http 401
    else abort http 500
    """

    def check(data):
        for key in data.keys():
            if key not in ["Action", "Token", "Data"]:
                raise Exception(
                    """key not in ["Action", "Token", "Data"]""")
        if data["Action"] not in ["changePassword"]:
            raise Exception(
                '''"Action" can only be "changePassword"''')
        return data

    try:
        print("OK")
        jsonData = request.get_json()
        jsonData = check(jsonData)
        Action = jsonData["Action"]
        Token = jsonData["Token"]
        Data = jsonData["Data"]
        tokenDict = verifyToken(Token)
        if list(Data.keys()) != ["Password"]:
            raise Exception("Invalid Data")

        if Action == "changePassword":
            tokenDict.update(Data)
            pg.update(tokenDict, "UserTable",
                      f""" "UserID"='{tokenDict['UserID']}' """)
        token = createToken(tokenDict)
        return {"Token": token}
    except BadData:
        abort(401)
    except:
        abort(500)


if __name__ == "__main__":
    app.run(host=HOST, port=SERVER_PORT, debug=True)

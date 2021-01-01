import sys
import os
import json
import socket
import time
import datetime
import tensorflow as tf
import numpy as np
import sql
import schedule


pg = sql.PostgreSQL(database="SmartPillowDB",
                    user="postgres", password="jimpsql")
if "MSC" in sys.version:
    HOST = "127.0.0.1"
elif "GCC" in sys.version:
    HOST = "172.17.0.13"
else:
    HOST = "localhost"
SERVER_PORT = 54321
MAX_CONNECTION = 99
BUFFER_SIZE = 8124
MODEL_NAME = "./data/" + "SmartPillowModel" + ".h5"


def buildModel():
    # build the deep learning model
    if os.path.exists(MODEL_NAME):
        model = tf.keras.models.load_model(MODEL_NAME)
    else:
        inputs = tf.keras.Input(shape=(3000, 1), name='inputs')
        x = tf.keras.layers.Dense(5, activation=None, name='dense_1')(inputs)
        x = tf.keras.layers.Dropout(0.01, name='dropout')(x)
        x = tf.keras.layers.GRU(5)(x)
        x = tf.keras.layers.Dense(5, activation=None, name='dense_2')(x)
        outputs = tf.keras.layers.Dense(
            2, activation='softmax', name='predictions')(x)

        model = tf.keras.Model(inputs=inputs, outputs=outputs)
        model.compile(optimizer=tf.keras.optimizers.Adam(),  # Optimizer
                      # Loss function to minimize
                      loss=tf.keras.losses.SparseCategoricalCrossentropy(),
                      # List of metrics to monitor
                      metrics=[tf.keras.metrics.SparseCategoricalAccuracy()])
        pg = sql.PostgreSQL(database="SmartPillowDB",
                            user="postgres", password="jimpsql")
        trainData = pg.select(("Pressure", "IsSleeping"),
                              "DataTable", """ "DeviceID"=0 """ +
                              """AND "IsSleeping" in (true, false)""")
        trainData = np.array(trainData)
        X = []
        y = []
        for i in range(len(trainData) - 3000):
            X.append(trainData[i:i + 3000, 0:1])
            y.append(trainData[i + 3000, 1])
        X = np.array(X)
        y = np.array(y)
        # model.fit(X, y, epochs=10)
        # model.save(MODEL_NAME)
    return model


def check(data):
    """
    check if the format of data is valid
    if valid return [data, UserID]
    else raise Exception
    """
    if "DataID" in data.keys():
        raise Exception(""""DataID" in data.keys()""")
    for key in data.keys():
        if key not in ["DeviceID", "Pressure", "Timedelta", "Volume", "DateTime"]:
            raise Exception("""data.keys() invalid""")

    if "DeviceID" not in data.keys():
        raise Exception(""""DeviceID" not in data.keys()""")
    if "DateTime" not in data.keys():
        data["DateTime"] = datetime.datetime.now()
    else:  # data.has_key("DateTime"):
        data["DateTime"] = datetime.datetime.fromisoformat(
            data["DateTime"])
    if "Timedelta" not in data.keys():
        data["Timedelta"] = datetime.timedelta(seconds=0.5)
    else:  # data.has_key("Timedelta"):
        data["Timedelta"] = datetime.timedelta(seconds=data["Timedelta"])

    UserID = pg.select("UserID", "DeviceTable",
                       f""" "DeviceID"='{data["DeviceID"]}' """)
    if UserID == []:  # NO User
        raise Exception("""user of device not exist""")
    else:  # extract
        UserID = UserID[0][0]
    return [data, UserID]


def reply(response, socketList, UserID):
    # should be send to all devices of the same user
    for [uID, cSocket] in socketList:
        if uID == UserID:
            try:
                cSocket.send(response.encode())
            except:
                # connectionSocket is already closed
                socketList.remove([uID, cSocket])


def handle(connectionSocket, socketList, SocketUserID):
    """
    Receive
    {
        "DeviceID":int, e.g. 1
        "Pressure":array of double/float,e.g.[0.1, 0.2,0.3]
        //optional
        "Timedelta":float, e.g. 0.5(optional, default=0.5)
        "Volume":array of double/float,e.g.[0.1, 0.2,0.3](optional, same length as Pressure) 
        "DateTime":isoformat (optional)
    }
    Send
    {
        "DeviceID":int,
        "IsSleeping":int, e.g. 0
        "DateTime":isoformat string,e.g. "2020-06-01T01:23:45"
        "Hash": int, e.g. 3346740174
        "PeriodNumber":3,
        "TimePeriod0":{"TimeID":int,"SleepingTime":isoTime, "WakeupTime":isoTime},
        "TimePeriod1":{"TimeID":int,"SleepingTime":isoTime, "WakeupTime":isoTime},
        "TimePeriod2":{"TimeID":int,"SleepingTime":isoTime, "WakeupTime":isoTime}
    }
    """
    try:
        # connectionSocket.setblocking(False)
        sentence = connectionSocket.recv(BUFFER_SIZE)
        data = json.loads(sentence.decode())
        print(
            f"recv from: {connectionSocket.getpeername()}, data = {data}", flush=True)
        # check data
        data, UserID = check(data)
        if SocketUserID != UserID:
            index = socketList.index([SocketUserID, connectionSocket])
            socketList[index][0] = UserID
    except BlockingIOError:
        # ignore when no new sentence
        return
    except Exception as e:
        socketList.remove([SocketUserID, connectionSocket])
        try:
            print(
                f"connection socket close: {connectionSocket.getpeername()}", flush=True)
            connectionSocket.send(str(e).encode())
        except:
            # connectionSocket is already closed
            pass
        connectionSocket.close()
        # skip this wrong data
        return

    # save and predict
    if "Pressure" in data.keys():
        # save data to database
        length = len(data["Pressure"])
        # predict
        DT = data["DateTime"] - datetime.timedelta(seconds=10800)
        data2predict = pg.select("Pressure", "DataTable",
                                 f""" "DeviceID"={data["DeviceID"]} AND """ +
                                 f""" "DateTime">'{DT.isoformat()}' """)[-3000:]
        data2predict = np.array([data2predict])
        if data2predict.shape[1] < 3000:
            data2predict = np.concatenate((
                np.zeros((1, 3000 - data2predict.shape[1], 1)),
                data2predict
            ), axis=1)
        IsSleeping = int(np.argmax(model.predict(data2predict)))
        # insert
        for i in range(length):
            data2insert = dict(DeviceID=data["DeviceID"],
                               Pressure=data["Pressure"][i],
                               DateTime=data["DateTime"] -
                               (length - 1 - i) * data["Timedelta"],
                               IsSleeping=(IsSleeping==1),
                               )
            pg.insert(data2insert, "DataTable")
    else:
        IsSleeping = 2

    # construct response
    PeriodsDict = {}
    Periods = pg.select(("SleepingTime", "WakeupTime"), "TimeTable",
                        f""" "UserID"='{UserID}' """)
    PeriodNumber = len(Periods)
    for i in range(PeriodNumber):
        # .isoformat().split(".")[0] mean remove microsecond
        PeriodsDict.update({f"SleepingTime{i}": Periods[i][0].isoformat().split(".")[0],
                            f"WakeupTime{i}": Periods[i][1].isoformat().split(".")[0]})
    response = {"DeviceID": data["DeviceID"],
                "DateTime": datetime.datetime.now().isoformat().split(".")[0],
                "IsSleeping": IsSleeping,
                "Hash": hash(str(PeriodsDict)) % pow(2, 32),
                "PeriodNumber": PeriodNumber}
    response.update(PeriodsDict)
    response = json.dumps(response)

    # send response to all device of one user
    reply(response, socketList, UserID)


def jobSleepTime():
    def time2sec(y):
        '''
        时间类型时分秒转换成秒
        '''
        h = y.hour  # 直接用datetime.time模块内置的方法，得到时、分、秒
        m = y.minute
        s = y.second
        return int(h) * 3600 + int(m) * 60 + int(s)  # int()函数转换成整数运算

    TimeInfos = pg.select(
        ("UserID", "WakeupTime", "SleepingTime"), "TimeTable")
    dictTurns = {}
    for TimeInfo in TimeInfos:
        userId, wakeupTime, sleepingTime = TimeInfo
        wakeupSecond = time2sec(wakeupTime)
        sleepingSecond = time2sec(sleepingTime)
        if wakeupSecond < sleepingSecond:
            timeDiff = wakeupSecond - sleepingSecond + 86400
        else:
            timeDiff = wakeupSecond - sleepingSecond
        dictTurns[userId] = dictTurns.get(userId, 0) + timeDiff
    for dictTurn in dictTurns:
        userId = dictTurn
        timeDiff = dictTurns[dictTurn]
        pg.insert({"UserID": userId, "Date": datetime.datetime.now().isoformat().split('T')[0],
                   "SleepTime": timeDiff}, "SleepingTable")


def jobTurn():
    enable = 1
    TurnInfos = pg.select(("DeviceID", "IsSleeping"), "DataTable",
                          f""" "enable"='{enable}' """)
    dictTurns = {}
    dictSleep = {}
    dictWake = {}
    for TurnInfo in TurnInfos:
        deviceId, isSleeping = TurnInfo
        userId = pg.select("UserID", "DeviceTable",
                           f''' "DeviceID" = '{deviceId}' ''')
        dictTurns[userId] = 0
        dictSleep[userId] = 0
        dictWake[userId] = 0

    for TurnInfo in TurnInfos:
        deviceId, isSleeping = TurnInfo
        userId = pg.select("UserID", "DeviceTable",
                           f''' "DeviceID" = '{deviceId}' ''')
        if isSleeping:
            dictSleep[userId] = dictSleep.get(userId, 0) + 1
        else:
            dictWake[userId] = dictWake.get(userId, 0) + 1

    for dictTurn in dictTurns:
        res = min(dictSleep[dictTurn], dictWake[dictTurn])
        pg.insert({"UserID": dictTurn, "Date": datetime.datetime.now().isoformat().split('T')[0],
                   "TurnCount": res}, "TurnTable")

    pg.update({"enable": 0}, "DataTable")


if __name__ == "__main__":
    serverSocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    serverSocket.bind((HOST, SERVER_PORT))
    serverSocket.listen(MAX_CONNECTION)
    serverSocket.setblocking(False)
    model = buildModel()

    # UserID connectionSocket list
    socketList = []

    while True:
        schedule.every().day.at("11:00").do(jobSleepTime)
        schedule.every().day.at("11:00").do(jobTurn)
        time.sleep(0.1)
        schedule.run_pending()
        try:
            # receive data
            connectionSocket, addr = serverSocket.accept()
            print(f"new connection from {addr}", flush=True)
            connectionSocket.setblocking(False)
            socketList.append([None, connectionSocket])
        except BlockingIOError:
            # ignore when no new connectionSocket
            pass
        for [UserID, connectionSocket] in socketList:
            handle(connectionSocket, socketList, UserID)

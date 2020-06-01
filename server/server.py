import sys
import os
import json
import socket
import datetime
import tensorflow as tf
import numpy as np
import sql


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
MODEL_NAME = PATH + "/data/" + "SmartPillowModel" + ".h5"


def buildModel():
    # build the deep learning model
    inputs = tf.keras.Input(shape=(1,), name='inputs')
    x = tf.keras.layers.Dense(5, activation=None, name='dense_1')(inputs)
    x = tf.keras.layers.Dropout(0.01, name='dropout')(x)
    x = tf.keras.layers.Dense(5, activation=None, name='dense_2')(x)
    outputs = tf.keras.layers.Dense(
        2, activation='softmax', name='predictions')(x)

    model = tf.keras.Model(inputs=inputs, outputs=outputs)
    model.compile(optimizer=tf.keras.optimizers.Adam(),  # Optimizer
                  # Loss function to minimize
                  loss=tf.keras.losses.SparseCategoricalCrossentropy(),
                  # List of metrics to monitor
                  metrics=[tf.keras.metrics.SparseCategoricalAccuracy()])

    if os.path.exists(MODEL_NAME):
        model.load_weights(MODEL_NAME)
    else:
        pg = sql.PostgreSQL(database="SmartPillowDB",
                            user="postgres", password="jimpsql")
        trainData = pg.select(("Pressure", "IsSleeping"),
                              "DataTable", """ "DeviceID"=0 """ +
                              """and "IsSleeping" in (true, false)""")
        trainData = np.array(trainData)
        trainInput = trainData.T[0]
        trainLabel = trainData.T[1]
        model.fit(trainInput, trainLabel, epochs=10)
        model.save_weights(MODEL_NAME)
    return model


def check(data):
    if "DataID" in data.keys():
        raise Exception(""""DataID" in data.keys()""")
    for key in data.keys():
        if key not in ["DeviceID", "Pressure", "Volume", "DateTime"]:
            raise Exception(
                """key not in ["DeviceID", "Pressure", "Volume", "DateTime"]""")
    if "DeviceID" not in data.keys():
        raise Exception(""""DeviceID" not in data.keys()""")

    if "DateTime" not in data.keys():
        data["DateTime"] = datetime.datetime.now()
    else:  # data.has_key("DateTime"):
        data["DateTime"] = datetime.datetime.fromisoformat(
            data["DateTime"])

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
                socketList.remove([uID, cSocket])


def handle(connectionSocket, socketList, SocketUserID):
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
            connectionSocket.send(str(e).encode())
            print(
                f"connection socket close: {connectionSocket.getpeername()}", flush=True)
        except:
            # connectionSocket is already closed
            pass
        connectionSocket.close()
        # skip this wrong data
        return
    if "Pressure" in data.keys():
        # save data to database
        pg.insert(data, "DataTable")
        # predict
        IsSleeping = int(np.argmax(model.predict([data["Pressure"]])[0]))
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
                "PeriodNumber": PeriodNumber}
    response.update(PeriodsDict)
    response = json.dumps(response)

    # send response to all device of one user
    reply(response, socketList, UserID)


if __name__ == "__main__":
    serverSocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    serverSocket.bind((HOST, SERVER_PORT))
    serverSocket.listen(MAX_CONNECTION)
    serverSocket.setblocking(False)
    model = buildModel()

    # UserID connectionSocket list
    socketList = []

    while True:
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

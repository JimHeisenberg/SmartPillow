import sys
import os
import json
import socket
import datetime
import numpy as np
import pandas as pd
import sql


TrainDataPath = "./data/train.csv"
pg = sql.PostgreSQL(database="SmartPillowDB",
                    user="postgres", password="jimpsql")
# pg.delete("DataTable")
# pg.delete("TimeTable")
# pg.delete("DeviceTable")
# pg.delete("UserTable")
# pg.select("*", "UserTable")
# pg.select("*", "TimeTable")
# pg.select("*", "DeviceTable")
# pg.select("*", "DataTable")

# create admin user and prototype device
# prototype device is for store train data
user = {"UserID": 0, "UserName": "admin", "Password": "admines"}
pg.insert(user, "UserTable")

user = {"UserName": "tom", "Password": "asd"}
pg.insert(user, "UserTable")

device = {"UserID": 0, "DeviceID": 0, "DeviceName": "prototype"}
pg.insert(device, "DeviceTable")

for number in range(1, 10):
    device = {"DeviceName": f"SmartPillow{number}", "UserID": 0}
    pg.insert(device, "DeviceTable")

for number in range(10, 20):
    device = {"DeviceName": f"Light{number}", "UserID": 0}
    pg.insert(device, "DeviceTable")

# insert data
df = pd.read_csv(TrainDataPath)
cmd = ""
for i in range(len(df)):
    cmd += f"""
    INSERT INTO public."DataTable"("DeviceID", "Pressure", "IsSleeping")
    VALUES (0, {df.Pressure[i]:.4f}, {bool(df.IsSleeping[i])});"""
pg.exe(cmd)

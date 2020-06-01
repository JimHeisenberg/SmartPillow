import scipy.stats as stats
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
# pg.select("*", "UserTable")
# pg.select("*", "TimeTable")
# pg.select("*", "DeviceTable")
# pg.select("*", "DataTable")

# create admin user and prototype device
# prototype device is for store train data
user = {"UserID": 0, "UserName": "admin", "Password": "admines"}
pg.insert(user, "UserTable")

device = {"UserID": 0, "DeviceID": 0, "DeviceName": "prototype"}
pg.insert(device, "DeviceTable")

for number in range(1, 10):
    device = {"DeviceName": f"SmartPillow{number}", "UserID": 0}
    pg.insert(device, "DeviceTable")


# create fake data as train data
mu, sigma = 0.7, 0.05
lower, upper = mu - 3 * sigma, mu + 3 * sigma  # 截断在[μ-3σ, μ+3σ]
truncNormGenT = stats.truncnorm(
    (lower - mu) / sigma, (upper - mu) / sigma, loc=mu, scale=sigma)
mu, sigma = 0.2, 0.05
lower, upper = mu - 3 * sigma, mu + 3 * sigma  # 截断在[μ-3σ, μ+3σ]
truncNormGenF = stats.truncnorm(
    (lower - mu) / sigma, (upper - mu) / sigma, loc=mu, scale=sigma)

for _ in range(500):
    dataT = {"DeviceID": 0, "IsSleeping": True,
             "Pressure": truncNormGenT.rvs()}
    pg.insert(dataT, "DataTable")
    dataF = {"DeviceID": 0, "IsSleeping": False,
             "Pressure": truncNormGenF.rvs()}
    pg.insert(dataF, "DataTable")

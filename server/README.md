# Smart Pillow Project
### Introduction
智能枕芯是一个通过物联网技术，与其它智能家居互联互通的嵌入式设备。旨在利用最小的成本，最大化地提升用户生活水平与生活便利程度。
### Server
server对应的是后端服务器，包括处理前端服务器发来的HTTP请求，与硬件设备通讯，并管理数据库。

## Installation
### Download
```
git clone https://github.com/JimHeisenberg/SmartPillow.git
# 目前未开源，不能直接下载，请使用以下指令
# git clone https://jimheisenberg:__PASSWORD__@github.com/JimHeisenberg/SmartPillow.git
# replace __PASSWORD__ with the real one
```
### Install PostgreSQL
```
# Ubuntu example
sudo apt install postgresql

# change password or modify the source code
sudo -u postgres psql
ALTER USER postgres WITH PASSWORD 'jimpsql';
\q
```
### create database
```
# SQL shell
sudo -u postgres psql
# then copy, paste and run all command in SmartPillowDB.sql
# ...
python initDB.py
```
### Install python requirements
```
python -m pip install -r requirements.txt
```

## Usage
```
cd ./SmartPillow/server
chmod +x run.sh
./run.sh
```
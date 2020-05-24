[TOC]
# API文档

所有提交均为HTTP POST  
ip:port = 212.64.54.38:12345  

## /login
POST  
```javascript
    {"UserName":str,"Password":str}
```
RETURN  
成功  
```javascript
200 ok 
{"Token":token}
```
失败  
```javascript
401 unauthorized
```

---------------

## /register
POST  
```javascript
    {"UserName":str,"Password":str}
```
RETURN  
成功  
```javascript
200 OK
{"Token":token}
```
失败  
```javascript
401 unauthorized
```

---------------

## /setting

无论任何操作，返回的都是获取所有闹钟的结果  

### 获取所有闹钟

POST  
```javascript
    {"Action":"select","Token":token}
or  {"Action":str, "Token":token, "Data":None}
//  Data will be removed automatically
```
if success return {"Data":json}  
elif token expired abort http 401  
else abort http 500  
```javascript
200 OK
{"Data":
    [
        {"TimeID":1,"SleepingTime":"23:30:00", "WakeupTime":"07:00:00"},
        {"TimeID":2,"SleepingTime":"13:30:00", "WakeupTime":"14:00:00"}
    ]
}
//TimeID 由后端自动分配，不能自行添加
```
### 添加闹钟

```javascript
{"Action":"insert","Token":token,"Data":list of dict}
//"Data" is a list of dict { "SleepingTime":isoTime, "WakeupTime":isoTime}
//  TimeID 由后端自动分配，不能自行添加
// "TimeID" in the list will be removed automatically
"Data":[
    {"TimeID":int, "SleepingTime":isoTime, "WakeupTime":isoTime},
{/*TimeID removed*/"SleepingTime":isoTime, "WakeupTime":isoTime},
    ......
]
example: "Data":[
    {"SleepingTime":"23:30:00", "WakeupTime":"07:00:00","TimeID":1},
    {"SleepingTime":"13:30:00", "WakeupTime":"14:00:00"}
]
```
### 修改
```javascript
{
"Action":"update","Token":token,"Data":[{"SleepingTime":isoTime, "WakeupTime":isoTime}]
}
"Data" is a list of dict {"TimeID":int, "SleepingTime":isoTime, "WakeupTime":isoTime}
example: "Data":[
    {"TimeID":1,"SleepingTime":"23:30:00", "WakeupTime":"07:00:00"},
    {"TimeID":2,"SleepingTime":"13:30:00", "WakeupTime":"14:00:00"}
]
```
### 删除
```javascript
{
"Action":"delete","Token":token,"Data":[{"SleepingTime":isoTime, "WakeupTime":isoTime}]
}
"Data" is a list of dict {"TimeID":int}
// Data 也可以是{"TimeID":int, "SleepingTime":isoTime, "WakeupTime":isoTime}
// 但是其中的SleepingTime与WakeupTime不会被考虑，也就是说被删除的数据仅仅取决于TimeID
example: "Data":[
    {"TimeID":1},
    {"TimeID":2},
or  {"TimeID":3,"SleepingTime":"13:30:00", "WakeupTime":"14:00:00"}
]
```

---------------

## /device 设备列表页面

无论任何操作，返回的都是获取所有设备的结果  

### 获取设备列表
POST  
```javascript
    {"Action":"select","Token":token}
or  {"Action":str, "Token":token, "Data":None}
//  Data will be removed automatically
```
if success return {"Data":json}  
elif token expired abort http 401  
else abort http 500  
```javascript
200 OK
{"Data":
    [
        {"DeviceID":1,"DeviceName":"asd"},
        {"DeviceID":2,"DeviceName":"qwe"}
    ]
}
//DeviceID 由后端自动分配，不能自行添加
```
### 添加设备

```javascript
{"Action":"insert","Token":token,"Data":list of dict}
//"Data" is a list of dict {"DeviceID":int,"DeviceName":str},
//  DeviceID 由后端自动分配，不能自行添加
// "DeviceID" is required
"Data":[
    {"DeviceID":1, "DeviceName":"asd"},
    {"DeviceID":2, "DeviceName":"qwe"},
    ......
]
```

### 修改
```javascript
{
"Action":"update","Token":token,"Data"::[{"TimeID":int,"DeviceName":str}]
}
"Data" is a list of dict {"DeviceID":int,"DeviceName":str},
example: "Data":[
    {"DeviceID":1,"DeviceName":"asd"},
    {"DeviceID":2,"DeviceName":"qwe"}
]
```
### 删除
```javascript
{
"Action":"delete","Token":token,"Data":[{"TimeID":int,"DeviceName":str}]
}
"Data" is a list of dict {"DeviceID":int}
// Data 也可以是{"DeviceID":int,"DeviceName":str},
// 被删除的数据仅仅取决于DeviceID
example: "Data":[
    {"TimeID":1},
    {"TimeID":2},
or  {"TimeID":3,"DeviceName":"asd"}
]
```
---------------
## /account

POST
```javascript
    {"Action":"changePassword","Token":token, "Data":{"Password":str}}
//这样是因为以后可能由拓展
```
返回  
if success return {"Token":token}
elif token expired abort http 401
else abort http 500
```javascript
200 ok
{"Token": token}
```







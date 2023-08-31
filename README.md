<<<<<<< HEAD
## 消息格式

> 每个标志位后面紧随四个字节的长度用于显示数据大小



1. 登录消息

   1. client

      `LOGIN`+`username`+`\t`+`password`

   2. server

      `LOGIN`+`SUCCESS`   

​			 `PASSWORD ERROR` 

​			`NEED REGISTER`



2. 注册消息

   1. client

      `REGISTER`+`username`+`\t`+`password`

   2. server

      `REGISTER SUCCESS`

      `USERNAME REPEAT`



3. 群发消息

   1. client

      `GROUP`+`|`+`聊天室名字`+`|`+`发送方用户名`+`|`+`mess`

      - 客户端如果打开了(创建)对应的聊天室直接将其添加到对应的聊天室界面
      - 客户端如果没有打开(未创建)对应的聊天室将其添加到`QVector<QMap<QString,QQueue<QString>>> groupMessCache`中等待聊天室新建并将消息添加进去

   2. server
      - 服务器根据当前在线用户进行洪泛转发



4. 单发消息

   1. client

      `SINGLE`+`|`+`接收方姓名`+`|`+`发送方姓名`+`|`+`mes`

      - 客户端如果打开了(创建)对应的聊天室直接将其添加到对应的聊天室界面
      - 客户端如果没有打开(未创建)对应私聊界面 将其添加到`QVector<QMap<QString,QQueue<QString>>> singleMessCache`中等待对应的私聊界面新建并将消息添加进去

   2. server
      - 提取接收方姓名,将信息格式变为:`SINGLE`+`|`+`发送方姓名`+`|`+`mes` 发送
      - 如果用户未上线驻留`QVector<QMap<QString,QQueue<QString>>> singleMessCache` 等到用户上线立即发送



5. 添加朋友
   1. client
      - `ADDFRIEND` +`|`+`被添加方`+`|` + `请求添加方`+`请求消息`
      - 被添加方 发送 `ADDFRIENDCONFIRM`+`|`+`被添加方`+`|`+`请求添加方`
   2. 服务端
      - 提取被添加方,发送: `ADDFRIEND` +`|`  `请求添加方`+`|`+`请求消息`
      - 提取请求添加方 向其发送 `ADDFRIENDCONFIRM`+`|`+`被添加方`



## 流程

1. 登录成功,服务端向客户端发送Json数据格式为 :`JSON`+`JsonData`   , 注意服务端要在发送完后才添加用户到当前在线用户列表

   `JsonData`格式为:

   ```json
   {
       type:newUser
       avater:序号,
   	signature:具体签名,
       sex:性别(男,女,未知(不予显示))
       friendList:[
           {
               userName:名称,
               signature:具体签名,
               sex:性别(男,女,未知(不予显示))
           },
   		{
               userName:名称,
               signature:具体签名,
               sex:性别(男,女,未知(不予显示))
           }
   		...
       ]
       chatRommNames:[
           "1","2","3" ...
       ]
   	onlineUserInfo:
   	[
           {
               userName:名称,
               loginTime:时间
           },
           ...
       ]
       offlineUserInfo:
   	[
           {
               userName:名称,
               loginTime:时间
           },
           ...
       ]
   }
   ```

   2. 客户端解析`JsonData`来初始化朋友列表,签名等信息
   3. 服务端要在发送完后`JsonData`添加用户到当前在线用户列表之前,向所有当前用户发送新连接信息
   4. 用户离线后向所有在线用户发送离线信息
      - 信息格式为 `CLOSE`+`用户信息`

   


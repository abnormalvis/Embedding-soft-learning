# 1. 引子

众所周知，当我们打开浏览器在输入框中输入www.baidu.com

![image](https://cdn-mineru.openxlab.org.cn/result/2026-03-21/e4ad0494-f5e0-4eab-b78b-3f730559e6c6/8e9fc6769b38f95e2ee37fc40630a249f0c7f11cc90517d346b888e12e716343.jpg)


![image](https://cdn-mineru.openxlab.org.cn/result/2026-03-21/e4ad0494-f5e0-4eab-b78b-3f730559e6c6/1137e9bd827d558343cf8282527f7405c23ea58d9bdb166538c7068daaaa22ba.jpg)


![image](https://cdn-mineru.openxlab.org.cn/result/2026-03-21/e4ad0494-f5e0-4eab-b78b-3f730559e6c6/accfaa455516840b782d6ac318aed2aff4d85e87e9d9c05783e4e76f389364c6.jpg)


![image](https://cdn-mineru.openxlab.org.cn/result/2026-03-21/e4ad0494-f5e0-4eab-b78b-3f730559e6c6/6344597d7499b568ebbfb5f614e8cca84174b81e73cf3a223bbbfec6eed65025.jpg)


www.baidu.com 

按下回车之后，正常可以看到打开了如下页面：

![image](https://cdn-mineru.openxlab.org.cn/result/2026-03-21/e4ad0494-f5e0-4eab-b78b-3f730559e6c6/0296d2d42cada5ae5a8cbf8a1af662a958c43df693b9b989dc2c12c4e6c492e9.jpg)


![image](https://cdn-mineru.openxlab.org.cn/result/2026-03-21/e4ad0494-f5e0-4eab-b78b-3f730559e6c6/3a9437f752024e43fc1e6623754d1cca2e60dcdf11c78c5bb26de3793aba0b2f.jpg)


百度一下

到底发生了什么？是如何实现页面的显示？

实际整个过程中，实现了一次http数据请求及http数据应答。

# 2. 关于 HTTP

在互联网实用四层模型中，应用层直接面向用户，所以拥有众中具体的应用协议

<table><tr><td>TCP/IP四层概念模型</td><td>对应网络协议</td></tr><tr><td>应用层</td><td>HTTP、FTP、TFTP、DHCP、NTP、POP3、IMAP4、Telnet、SNMP、SMTP、DNS、LDAP、SSH</td></tr><tr><td>传输层</td><td>TCP、UDP</td></tr><tr><td>网络层</td><td>IP、ICMP、ARP、RARP、OSPF、VRRP、IGMP、IS-IS、Ipsec、BGP</td></tr><tr><td rowspan="2">网络接口层</td><td>PPP、PPTP</td></tr><tr><td></td></tr></table>

其中，HTTP/HTTPS包含当中。

HTTP协议，即超文本传输协议（HyperText Transfer Protocol），是用于从web服务器传输超文本到本地浏览器的应用层传送协议。它可以使浏览器更加高效，使网络数据传输量减少。

HTTP 是一个应用层协议，由请求和响应构成，是一个标准的客户端服务器模型。默认 HTTP 的端口号为 80，HTTPS 的端口号为 443。

# 超文本

# HTTP 具有特点:

1. 无状态协议：对于事务处理没有记忆能力，即每次请求的网页之间没有关系

2. 支持客户-服务器模式：每次均需要客户请求，服务端作应答

3. 短连接通信：每次进行 HTTP 操作，就建立一次连接，任务结束就中断连接

4. 简单快速：客户向服务器请求服务时，只需传送请求方法和路径。

5. 通信灵活：HTTP 允许传输任意类型的数据对象

一些相关概念：

web( WWW (World Wide Web)) 

浏览器(Web browser)

URL（Uniform Resource Locator, 统一资源定位器）是 WWW 的统一资源定位标志，就是指网络地址

URI 统一资源标识符（Uniform Resource Identifier）

Web上可用的每种资源HTML文档、图像、视频片段、程序等由一个通用资源标识符（URI）进行定位。

HTML 是超文本标记语言英文的缩写（Hypertext Marked Language），不是编程语言，而是一种标记语言，通过标识符来标识网页中内容的显示方式，例如图片的显示尺寸、文字的大小、颜色、字体等。


协议版本：


<table><tr><td>版本</td><td>功能描述</td></tr><tr><td>HTTP/0.9</td><td>HTTP协议的最初版本，功能简陋，仅支持请求方式GET，并且仅能请求访问HTML格式的资源。</td></tr><tr><td>HTTP/1.0</td><td>短连接模式（请求-应答），每次通信只处理一个请求，处理完后断开连接。每次都要三次握手，四次断开，开销大</td></tr><tr><td>HTTP/1.1(目前版本)</td><td>1.1版的最大变化，就是引入了持久连接（persistent connection），即TCP 连接默认不关闭，可以被多个请求复用，不用声明Connection。简单地说，当一个网页打开完成后，客户端和服务器之间用于传输HTTP数据的TCP连接不会关闭，如果客户端再次访问这个服务器上的网页，会继续使用这一条已经建立好的连接。解决了1.0版本的keep-alive问题，1.1版本加入了持久连接，一个TCP连接可以允许多个HTTP请求；</td></tr><tr><td>HTTP/2.0</td><td>增加了双工模式，即不仅客户端能够同时发送多个请求，服务端也能同时处理多个请求，解决了队头堵塞的问题。 使用了多路复用的技术，做到同一个连接并发处理多个请求，而且并发请求</td></tr><tr><td></td><td>的数量比 HTTP1.1 大了好几个数量级。引入二进制数据帧和流的概念，其中帧对数据进行顺序标识，避免了 1.0 旧版本的重复繁琐的字段，并以压缩的方式传输，提高利用率。</td></tr></table>

# 3. HTTP 实现原理


web server 通信过程:


![image](https://cdn-mineru.openxlab.org.cn/result/2026-03-21/e4ad0494-f5e0-4eab-b78b-3f730559e6c6/0051364df6b4aa4e73c473cc3294e195bc2bf8508237a26768c3e14c69700721.jpg)



web server-brower 通信步骤：


![image](https://cdn-mineru.openxlab.org.cn/result/2026-03-21/e4ad0494-f5e0-4eab-b78b-3f730559e6c6/180872e1bfef195458cfa575b58f37b30111f3f33f1aa9aa409efadd6e8cd89f.jpg)



广州小蚁智控科技有限公司


# 更详细描述如下：

# 1. 建立TCP连接

在 HTTP 工作开始之前，Web 浏览器首先要通过网络与 Web 服务器建立连接，该连接是通过 TCP 来完成的。

# 2. 浏览器向Web服务器发送请求命令

一旦建立了TCP连接，Web浏览器就会向Web服务器发送请求命令。

例如：GET /sample/hello.jsp HTTP/1.1。

# 3. 浏览器发送请求头信息

浏览器发送其请求命令之后，还要以头信息的形式向Web服务器发送一些别的信息，之后浏览器发送了一空白行来通知服务器，它已经结束了该头信息的发送。

# 4. Web 服务器应答

客户端向服务器发出请求后，服务器会客户机回送应答，HTTP/1.1 200 OK，应答的第一部分是协议的版本号和应答状态码。

# 5. Web 服务器发送应答头信息

正如客户端会随同请求发送关于自身的信息一样，服务器也会随同应答向用户发送关于它自己的数据及被请求的文档。

# 6. Web 服务器向 Web 浏览器发送数据

Web 服务器向浏览器发送头信息后，它会发送一个空白行来表示头信息的发送到此为结束，接着，它就以 Content-Type 应答头信息所描述的格式发送用户所请求的实际数据

# 7. Web 服务器关闭 TCP 连接

一般情况下，一旦Web服务器向浏览器发送了请求数据，它就要关闭TCP连接。但如果浏览器或者服务器在其头信息加入了这行代码：

Connection:keep-alive 

TCP 连接在发送后将仍然保持打开状态，于是，浏览器可以继续通过相同的连接发送请求。保持连接节省了为每个请求建立新连接所需的时间，还节约了网络带宽。

# 4. HTTP 协议内容

http 协议主要是通过报文的形式定义了客户端和服务器之间交互的消息内容和操作步骤。

首先，客户端会向服务器发送请求消息请求消息中包含的内容是 "对什么" 和 "进行怎样的操作" 两个部分。其中 "对什么" 的部分就是 URI (Uniform Resource Identifier, 统一资源标识符), 一般就是网页或者文件或者程序等, 而 "进行怎样的操作" 的部分称为方法, 包括:

<table><tr><td rowspan="2">方法</td><td colspan="2">HTTP 版本</td><td rowspan="2">含义</td></tr><tr><td>1.0</td><td>1.1</td></tr><tr><td>GET</td><td>○</td><td>○</td><td>获取 URI指定的信息。如果 URI指定的是文件,则返回文件的内容;如果 URI指定的是 CGI程序,则返回该程序的输出数据</td></tr><tr><td>POST</td><td>○</td><td>○</td><td>从客户端向服务器发送数据。一般用于发送表单中填写的数据等情况下</td></tr><tr><td>HEAD</td><td>○</td><td>○</td><td>和GET基本相同。不过它只返回HTTP的消息头(message header),而并不返回数据的内容。用于获取文件最后更新时间等属性信息</td></tr><tr><td>OPTIONS</td><td></td><td>○</td><td>用于通知或查询通信选项</td></tr><tr><td>PUT</td><td>△</td><td>○</td><td>替换 URI指定的服务器上的文件。如果 URI指定的文件不存在,则创建该文件</td></tr><tr><td>DELETE</td><td>△</td><td>○</td><td>删除 URI指定的服务器上的文件</td></tr><tr><td>TRACE</td><td></td><td>○</td><td>将服务器收到的请求行和头部(header)直接返回给客户端。用于在使用代理的环境中检查改写请求的情况</td></tr><tr><td>CONNECT</td><td></td><td>○</td><td>使用代理传输加密消息时使用的方法</td></tr></table>

http 报文分为两种：请求报文和响应报文

报文由以下内容组成：

1. 请求行或状态码行

2. 头字段

3. 空行

4. 可选的报文主体数据

# 4.1 HTTP 请求报文

![image](https://cdn-mineru.openxlab.org.cn/result/2026-03-21/e4ad0494-f5e0-4eab-b78b-3f730559e6c6/e00c54ccffb8e7ad4756acd4fd5fef15ac26d542377c5c78b643637424ea7aa1.jpg)


# 示例：利用charles抓取的POST请求报文

```html
1 POST / HTTP1.1   
2 Host: www.wrox.com   
3 User-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; SV1; .NET CLR 2.0.50727; .NET CLR 3.0.04506.648; .NET CLR   
4 Content-Type:application/x-ww-form-xmlencoded   
5 Content-Length:40   
6 Connection: Keep-Alive   
7   
8 name=Professional%20Ajax&publisher-Wiley 
```

分析：

第一部分：请求行，第一行明了是post请求，以及http1.1版本。

第二部分：请求头部，第二行至第六行。

2. Host 请求的web服务器域名地址

3.User-Agent 浏览器类型的详细数据

4.Content-Type 服务器要响应文件的类型

5.Content-Length 服务器要响应文件的大小

6.Connection 表示是否需要持久连接

第三部分：空行，第七行的空行。

第四部分：请求数据，第八行

注意：

HTTP 的 GET、POST、PUT、DELETE 对应着对这个资源的查、改、增、删 4 个操作。

# 4.2 HTTP响应报文

HTTP响应也由四个部分组成，分别是：状态行、消息报头、空行和响应正文组成。

![image](https://cdn-mineru.openxlab.org.cn/result/2026-03-21/e4ad0494-f5e0-4eab-b78b-3f730559e6c6/b2d1c575d45cc6d43da9f6872e21541dd157d93bda25b75c6030f32b927d15e9.jpg)


示例：

```html
1 HTTP/1.1 200 OK  
2 Date: Fri, 22 May 2009 06:07:21 GMT  
3 Content-Type: text/html; charset=UTF-8  
4  
5 <html>  
6 <head></head>  
7 <body>  
8 <!--body goes here-->  
9 </body>  
10 </html> 
```

分析：

第一部分：状态行，由 HTTP 协议版本号，状态码，状态消息三部分组成。

HTTP/1.1 表明 HTTP 版本为 1.1 版本，状态码为 200，状态消息为（ok）

第二部分：消息报头，用来说明客户端要使用的一些附加信息

Date:生成响应的日期和时间；

Content-Type:指定了 MIME 类型, HTML(text/html),编码类型是 UTF-8

第三部分：空行，消息报头后面的空行是必须的

第四部分：响应正文，服务器返回给客户端的文本信息。

空行后面的html部分为响应正文。


HTTP状态码


<table><tr><td>状态码</td><td>含义</td></tr><tr><td>100~199</td><td>表示成功接收请求，要求客户端继续提交下一次请求才能完成整个处理过程</td></tr><tr><td>200~299</td><td>表示成功接收请求并已完成整个处理过程</td></tr><tr><td>300~399</td><td>为完成请求，客户需进一步细化请求。例如，请求的资源已经移动一个新地址</td></tr><tr><td>400~499</td><td>客户端的请求有错误</td></tr><tr><td>500~599</td><td>服务器端出现错误</td></tr></table>

# 常见的几个状态码

200 OK 表示成功

303 重定向,把你重定向到其他页面

304 资源并未修改,可以直接使用本地的缓存

404 找不到页面(页面被删除或其他)

500 服务端错误

# 5. HTTP 编程开发

流程设计：

![image](https://cdn-mineru.openxlab.org.cn/result/2026-03-21/e4ad0494-f5e0-4eab-b78b-3f730559e6c6/fd49bb02416009c94e443c21259c4fe864c67103d36d2b54778001ee3a547c32.jpg)


![image](https://cdn-mineru.openxlab.org.cn/result/2026-03-21/e4ad0494-f5e0-4eab-b78b-3f730559e6c6/9b202a56841d02dc437188cbc05357ffc0a686ad6198f2b4d8c685496c7d59d5.jpg)


# 开发示例：基于 HTTP 的 WEB 服务器开发

功能描述：

1. 通过浏览器可以显示出一个页面（html界面）

2. 界面上有 3 个按键，分别为文本按键，图片按键，视频按键

3. 点击文本按键，获取显示一个文件里面的字符串信息

4. 点击图片按键，获取显示一张图片并显示

5. 点击文本按键，获取显示一个视频并播放

# 开发拆解：

<table><tr><td colspan="2">开发要求</td></tr><tr><td>v1.0</td><td>通过浏览器能够连接上并获取显示字符串数据</td></tr><tr><td>v2.0</td><td>通过通过浏览器可以：
获取并显示一张图片：192.168.1.200:demo.jpg
获取并播放一个视频：192.168.1.200:demo.mp4
没有直接在页面中播放，可能：
a.应答包中没有包含让浏览器播的数据，是否可以在应答数据添加让它直接播放？
b.浏览器不支持当前这个视频的解码格式，是否可以给浏览器安装相关的解决插件？</td></tr><tr><td>v3.0</td><td>通过浏览器可以显示出一个页面（html 界面）
1.界面上有 3 个按键，分别为文本按键，图片按键，视频按键
2.点击文本按键，获取显示一个文件里面的字符串信息
3.点击图片按键，获取显示一张图片并显示</td></tr><tr><td></td><td>4.点击视频按键，获取显示一个视频并播放</td></tr><tr><td>拓展功能</td><td>浏览器上传，服务器接收</td></tr></table>
# spp
server work bench， 提供通用的后台服务处理，三大进程：controller统一管理系统进程，server采用多路I/O复用并发连接客户请求，worker可以同步/异步处理请求。

===== 2018-06-29 =====
完成spp基本框架：
control : 初始化启动server和worker进程组，通过监听心跳包，对server和worker进程组启动/关闭的管理，避免僵死多余进程或僵死进程的出现。
server : 采用多路I/O复用单进程并发连接客户请求，创建共享内存池发送请求到worker，以及接收响应并返回到客户端。
worker : 有同步和异步方式处理请求数据（尚未开发），目前是应用自己处理。
框架采用插件式应用开发方式，也即业务只需要填充系统预留的钩子函数，如handle_init(),handle_route(),handle_input(),handle_process()等，让业务可以更关注逻辑本身，而系统内的消息传递都是透明的。


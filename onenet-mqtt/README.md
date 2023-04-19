# 提示
mqtt的每次连接（即每个进程）都需要Client ID，所以onenet的mqtt网关设备只能在同一个进程进行发布订阅。

# 编译
./set.sh

# 说明
```
0、3rdparty：三方库路径，编译好的paho.mqtt.c-1.3.10；cJSON-1.7.15源码；去掉打印信息编译好的onenet_studio_sdk库。

1、demo：基于 paho.mqtt.c 对onenet网关设备进行发布订阅。

2、demo_onenet_studio_sdk：基于onenet_studio_sdk的demo：onenet_studio_sdk主要支持子设备主题。

3、gateway_sub-device：初步实现 epoll服务端 + onenet_studio_sdk 的网关，epoll客户端的子设备demo：网关通过加载设备列表配置（json）初始化连接onenet，支持子设备post信息到onenet，支持在onenet中的设备调试-应用模拟的属性设置下发到子设备。
实现：当子设备连接网关时，发送"add_sub_devices:子设备名"到服务器，网关保存文件描述符对应的子设备名，然后进行子设备"批量上报属性和事件"(topic)。当在onenet平台调试，对子设备应用进行"子设备属性设置"(topic)，网关会接收到此订阅topic的消息，发送此消息到指定的子设备。注意：这简单的demo，只是为了集成测试。
提示：可通过xlsx2json，将xlsx转换为json文件。
子设备post信息到onenet，在子设备程序界面输入：
sub_devices_post:{"test_value":{"value":325}, "test_string":{"value":"hello, test_subdev_01-kk"}}

4、mqtt-class（不完善）：基于 paho.mqtt.c 的类，但不适用于onenet，因为mqtt的每次连接（即每个进程）都需要Client ID，所以onenet的mqtt网关设备只能在同一个进程进行发布订阅。

5、OneNET-Token-no-openssl：基于c++实现的onenet token计算。

6、OneNET-Token-openssl：基于openssl实现的onenet token计算。

7、onejson：简单参照 onenet_studio_sdk 的 tm_data.h

8、modify_lib：简单修改onenet_studio_sdk，修改如：onenet_studio_sdk的问题的第2点

9、resource：一些资源

```
## onenet_studio_sdk的问题：
```
1、当绑定子设备过多（绑了21个），调用 tm_subdev_topo_get 接口会导致 tm_step 出问题。

2、tm_subdev_add、tm_subdev_delete、tm_subdev_login、tm_subdev_logout，四个接口的timeout不生效。
若要生效需要在 tm_data_parse 的 tm_post_reply(payload, payload_len);
前的else if中添加条件：
|| (0 == strncmp(action, "/sub/login/reply", osl_strlen("/sub/login/reply")))
|| (0 == strncmp(action, "/sub/logout/reply", osl_strlen("/sub/logout/reply")))
|| (0 == strncmp(action, "/sub/topo/add/reply", osl_strlen("/sub/topo/add/reply")))
|| (0 == strncmp(action, "/sub/topo/delete/reply", osl_strlen("/sub/topo/delete/reply")))

3、测试一秒钟发送100+次处理，只会有56次响应，响应的是前56次的处理。sdk在在响应了56次后, tm_step 会错误(返回值不等于0)，
报错：Mqtt client send packet failed!

4、sdk实现的的子设备的四条回调函数会立即响应（reply），当对应的实体子设备可能立即响应对应的数据，建议按照 subdev_data_callback
实现自己的 my_subdev_data_callback 函数。

5、sdk只能一个进程接一个网关，需要接多网关便需要多进程.....

```
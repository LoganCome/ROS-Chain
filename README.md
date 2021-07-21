# README

```
master
|
|
|---Inc:开发板底层驱动所需头文件（c语言）
|
|---Src:开发板底层驱动相关实现类
|     |
|     |---itheima:接口
|
|---zxcar_ws6:Ros节点
|           |
|           |---src:package
|                 |
|                 |---include:没啥用      
|                 |
|                 |---zxcar:上位机驱动
|                 |
|                 |---zxcar_driver:上位机驱动    
|           
|
|---udp_receiver:udp数据接收端
|
|---udp_sender:udp数据发送端
|
|---examples:python同以太坊交互的示例代码（编译、部署、调用）编译部分暂时不可用，后续视情况更新
|
|---本项目的智能合约以及其编译文件abi、字节码
```

## 2021.7.12 New:see_no_evil:

Python和以太坊交互笔记，持续施工中...:heart_eyes:

`2021.7.13`更新了以太坊状态机、普通交易、裸交易等笔记

------------------------------------------------------------------分割线-------------------------------------------------------------

## 2021.7.19​ :innocent:

python以太坊交互笔记施工完毕，请放心食用~:grin:

更新了ROS环境和外部环境通信的相关部分（简单的端口监听）

------------------------------------------------------------------分割线-------------------------------------------------------------

## 2021.7.21 :seedling:

更新了以太坊交互的方式

修正了调用智能合约方法返回乱码的bug

附上web3.js开发手册

http://cw.hubwiz.com/card/c/web3.js-1.0/1/4/9/

# ROS-Chain

By Shenhui Zhang, Bernie Liu, Ming Tang

Hainan University, RobAI-Lab:robot: **&** HKUST, Hong Kong.

## :question:What is ROS-Chain​​

ROS-Chain serves as a kind of interaction scheme for ROS and Ethereum in a secure and convenient way. ROS-Chain is built on SM algorithm family which includes SM2, SM3 and SM4, UDP transport protocol, Ethereum and, of course, ROS. As a user, what you need to do to get ROS-Chain works is just to input some key information(Like names of topics, your comma etc.).

ROS-Chain presents a user-friendly framework that blocks the complicated implementation process of detailed interaction scheme along with a **unified Ethereum Blockchain network** reserving for interaction. Our goal is to present a kind of **Information storage and sharing media with security and flexibility** to facilitate the information communication in all ROS systems within the world. Our experiments focus on the security performance, encryption and decryption performance and stability of SM algorithm family.

Thanks for the key-exchange scheme based on SM algorithm family, ROS-Chain is equipped with a complete secure encryption mechanism in the cost of a slight loss in performance. Every user has a unique identity and related interactive permissions like authority granting, information check and messages storage in the Ethereum network. Through this, every user having registered in the network can not only do the job of **persistent, tamper proof storage of critical information**, but also can conduct trusted information exchange communication with the authorization of either party of information communication, out of the intervention of third party.

## :hammer_and_wrench: How to use

#### Step :one:: Set up ROS and related environments

ROS-Chain is used for robots build upon `ROS Melodic`, so please set up `ROS Melodic` in `Ubuntu18.04` and put the `listen_node` in the repo to your local catkin workspace. Also you need to prepare the environment of `gcc` ,  `g++`  & `python 3.6+` well and run `catkin_make` to compile your workspace. If you have no idea about that, this [blog](https://blog.csdn.net/weixin_42108484/article/details/83021957) is a good start and for the python, we suggest you use the [anaconda3](https://www.anaconda.com/).

#### Step :two:: Run it!  

Run the `console.py` in the `listen_node` in the repo. For the first use, you need to upload your **SM4 key** which is used to encrypt your ROS messages. And please choose one of the **SM2 public key** in the `keys.txt` file which is used to encrypt your SM4 key when the program need you to input.

```python
python console.py
```

After that, there will be an interaction to define nodes whose information you want to push to  Blockchain.

#### Step :three:: Check the topic

You can run the command below and check the working status of the nodes on your PC.

```
rostopic list
```

## :bookmark_tabs:Release Plan

ROS-Chain is still in the process of testing and revisions of codes. In mid-Dec, we will have official version released in **mid-Dec**.

## :email:Contact Us​

If you have any concerns here, please post as Github issues, or send an e-mail to Shenhui Zhang by freak01716@163.com; o0freak0o01716@gmail.com.

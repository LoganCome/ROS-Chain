# ROS-Chain（Easy_use）

By Shenhui Zhang, Bernie Liu, Ming Tang

Hainan University, RobAI-Lab:robot: **&** HKUST, Hong Kong.

## NOTIFICATION

**This is a brief version of ROS chain, which is easy to use and test the basic functions of ROS chain. At present, this version only supports the related functions / operations of odometry type messages.**

## :question:What is ROS-Chain​​

ROS-Chain serves as a kind of interaction scheme for ROS and Ethereum in a secure and convenient way. ROS-Chain is built on SM algorithm family which includes SM2, SM3 and SM4, UDP transport protocol, Ethereum and, of course, ROS. As a user, what you need to do to get ROS-Chain works is just to input some key information(Like names of topics, your comma etc.).

ROS-Chain presents a user-friendly framework that blocks the complicated implementation process of detailed interaction scheme along with a **unified Ethereum Blockchain network** reserving for interaction. Our goal is to present a kind of **Information storage and sharing media with security and flexibility** to facilitate the information communication in all ROS systems within the world. Our experiments focus on the security performance, encryption and decryption performance and stability of SM algorithm family.

Thanks for the key-exchange scheme based on SM algorithm family, ROS-Chain is equipped with a complete secure encryption mechanism in the cost of a slight loss in performance. Every user has a unique identity and related interactive permissions like authority granting, information check and messages storage in the Ethereum network. Through this, every user having registered in the network can not only do the job of **persistent, tamper proof storage of critical information**, but also can conduct trusted information exchange communication with the authorization of either party of information communication, out of the intervention of third party.

## :hammer_and_wrench: How to use

#### Step :one::  Prepare ROS and run envs

ROS-Chain is used for robots build upon `ROS Melodic` and related codes are run in `Anaconda3` envs along with `redis`. To prepare envs, you should prepare two virtual python envs, one in `pthon2.7` and the other in `python3.8`. And for the redis setup, please refer to [this](https://redis.io/download).

```shell
conda activate -n py2 python=2.7
conda activate -n py3 python=3.8
```

Then, you should set up `redis` module in the two virtual envs.

```shell
conda activate py2
conda install redis redis-py
conda activate py3
conda install redis redis-py
```

#### Step :two::  compile your workspace

Cloning the codes and move the file `listen_node` to your `src` of your workspace and run the following commands:

```shell
catkin_make
```

And export the path of your workspace to the `~/.bashrc`:

```shell
sudo vim ~/.bashrc
:i
export CATKIN_WS=/...
:wq
source ~/.bashrc
```

#### Step :three::  Register your identity

To register the identity, please run the following codes ($catkin_ws is the location of your workspace):

```shell
cd $catkin_ws/src/listen_node/scripts/v2/register
conda activate py3
python easy_register.py
```

And follow the instructions.

#### Step :four::  Assign the topics and start monitoring

Run the following codes to initialize the variables:

```shell
cd $catkin_ws/src/listen_node/scripts/v2
conda activate py3
python easy_console
```

When you enter the topics and message types, you can run the following codes to start monitoring. But, before you start monitoring, please make sure that **the `redis` module runs normally**.

```shell
cd $catkin_ws/src/listen_node/scripts/v2
conda activate py2
python Monitor.py
```

#### Step :five::  Assign the topics and start monitoring

Finally, after the capturing of the wanted messages, you can switch the **terminal of step 4** to begin the interaction with Ethereum launched on the server by enter **`1`** and check the info on the chain by enter **`2`**.

## :bookmark_tabs:Release Plan

ROS-Chain is still in the process of testing and revisions of codes. In mid-Feb, 2022, we will have final version released.

## :email:Contact Us​

If you have any concerns here, please post as Github issues, or send an e-mail to Shenhui Zhang by freak01716@163.com; o0freak0o01716@gmail.com.

# ROS-Ethereum

By Shenhui Zhang, Bernie Liu

HNU, RobAI-Lab🤖 & HKUST, Hong Kong.

## NOTIFICATION

**Now ROS-Ethereum only support the messages of 'Odometry' type. ROS-Ethereum will support more types of messages, such as pictures. **

## ❓What is ROS-Ethereum

ROS-Ethereum serves as a kind of interaction scheme for ROS  and Ethereum in a secure and convenient way. ROS-Ethereum is built on SM algorithm family which includes SM2, SM3 and SM4, TCP/IP transport  protocol, Ethereum and, of course, ROS. As a user, what you need to do  to get ROS-Ethereum works is just to input some key information(Like names and types of topics etc.).

ROS-Ethereum presents a user-friendly framework that blocks  the complicated implementation process of detailed interaction scheme along with a **unified Ethereum Blockchain network** reserving for interaction. Our goal is to present a kind of **Critical Information storage and sharing media with security and flexibility** to facilitate the information communication in **all ROS systems** within  the world.

Thanks for the key-exchange scheme based on SM algorithm  family, ROS-Ethereum is equipped with a complete secure encryption mechanism in the cost of a slight loss in time-consuming performance. Every user has a  unique identity and related interactive permissions like authority  granting, information check and messages storage in the Ethereum  network. Through this, every user having registered in the network can not only do the job of **persistent, tamper proof storage of critical information**, but also can conduct trusted information exchange communication with the other authorized party, out of  the intervention of third party.

## 🛠️ How to use

#### Step 1️⃣:  Prepare ROS and run envs

ROS-Ethereum is used for robots build upon `ROS Melodic` and related codes are run in `Anaconda3` envs along with `redis`. To prepare envs, you should prepare two virtual python envs, one in `pthon2.7` and the other in `python3.8`. And for the redis setup, please refer to [this](https://redis.io/download).

```
conda activate -n py2 python=2.7
conda activate -n py3 python=3.8
```

Then, you should set up `redis` module in the two virtual envs.

```
conda activate py2
conda install redis redis-py
conda activate py3
conda install redis redis-py
```

#### Step 2️⃣:  compile your workspace

Cloning the codes and move the folder`listen_node` to your `src` of your workspace and run the following commands:

```
catkin_make
```

And export the path of your workspace to the `~/.bashrc`:

```
sudo vim ~/.bashrc
:i
export CATKIN_WS=/...
:wq
source ~/.bashrc
```

#### Step 3️⃣:  Registration

To register the identity, please run the following codes ($catkin_ws is the location of your workspace):

```
cd $catkin_ws/src/listen_node/scripts/Ultimate
conda activate py3
python register.py
```

And follow the instructions.

#### Step 4️⃣:  Assign the topics and start monitoring

Run the following codes to initialize the variables:

```
cd $catkin_ws/src/listen_node/scripts/Utlimate
conda activate py2
python console
```

When you enter the topics and message types, you can run  the following codes to start monitoring. But, before you start  monitoring, please make sure that **the `redis` module runs normally**.

```
cd $catkin_ws/src/listen_node/scripts/v2/Utlimate
conda activate py2
python Monitor.py
```

## 📑Release Plan

ROS-Ethereum will support more message types in the future and will be equipment with the decryption module to facilitate the decryption of messages obtained from the Ethereum.

## 📧Contact Us

If you have any concerns here, please post as Github issues, or send an e-mail to Shenhui Zhang by [freak01716@163.com](mailto:freak01716@163.com).
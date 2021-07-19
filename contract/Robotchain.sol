pragma solidity ^0.4.21;

contract RobotChainContract {
    //机器人数量
    int public RobotCount;

    //机器人对象
    //isRegistered标识机器人是否注册
    //将地址和enode绑定
    struct Robot{
        address  robotaddress;
        bool isRegistered;
        int enode;
    }

    //机器人信息对象
    //由二维坐标(_x,_y)、线速度 vel 组成
    struct Robot_data{
        int _x;
        int _y;
        int vel;
    }

    mapping(address => Robot) public robot_storage;
    mapping(int => Robot_data) public robot_data;

    constructor(
        int robotCount
        )public {
        RobotCount = robotCount;
    }

    //机器人对象信息初始化
    function registerRobot(int T_enode) public {
    if (!robot_storage[msg.sender].isRegistered) {
        robot_storage[msg.sender].robotaddress = msg.sender;
        robot_storage[msg.sender].isRegistered = true;
        robot_storage[msg.sender].enode = T_enode;
        RobotCount += 1;
    }
  }

    //节点状态初始化
    // function init(address carPeerAddress) public {
    //     x[carPeerAddress] = 0;
    //     y[carPeerAddress] = 0;
    //     vel[carPeerAddress] = 0;
    // }

    //机器人位置上传
    function setPosition( int _x, int _y) public {
        robot_data[robot_storage[msg.sender].enode]._x = _x;
        robot_data[robot_storage[msg.sender].enode]._y = _y;
    }

    //获取当前机器人位置信息
     function getOwnPosition() public view returns(int, int) {
        return (robot_data[robot_storage[msg.sender].enode]._x,robot_data[robot_storage[msg.sender].enode]._y);
    }


    //位置读取
    // function getPosition(address carPeerAddress) public view returns(int, int) {
    //     return (x[carPeerAddress], y[carPeerAddress]);
    // }

    //速度上传
    function setVel( int _vel) public {
        robot_data[robot_storage[msg.sender].enode].vel = _vel;
    }

    //速度读取
    function getVel() public view returns(int) {
        return robot_data[robot_storage[msg.sender].enode].vel;
    }

    function Calucate(int x,int y,int tar_x,int tar_y) internal pure returns(int dis){
        dis = ((x-tar_x)*(x-tar_x))+((y-tar_y)*(y-tar_y));
        return dis;
    }

    //决定哪个去
    function decision() public view returns (int en){
        int dicision = 0;
        int count = 1;
        int MAX = 8000000;
        int tmp_dis;
        while(count<RobotCount){
            tmp_dis = Calucate(robot_data[count]._x,robot_data[count]._y,50,50);
            if(tmp_dis < MAX){
                dicision = count;
                MAX = tmp_dis;
                count += 1;
            }else{
                count += 1;
            }
        }
        return count;
    }
}
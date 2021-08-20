pragma solidity ^0.4.21;
// pragma experimental ABIEncoderV2;
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
        bool isonjob;
    }

    struct Tranc{
        int x;
        int y;
        bool isdone;
    }

    mapping(address => Robot) public robot_storage;
    mapping(int => Robot_data) public robot_data;
    mapping(int => Tranc) public Tranc_info;

    constructor(
        int robotCount
        )public {
        RobotCount = robotCount;
    }

    function getRobot() public view returns(address r){
        return robot_storage[msg.sender].robotaddress;
    }
    //机器人对象信息初始化
    function registerRobot(int T_enode) public{

        robot_storage[msg.sender].robotaddress = msg.sender;
        robot_storage[msg.sender].isRegistered = true;
        robot_storage[msg.sender].enode = T_enode;
        robot_data[robot_storage[msg.sender].enode].isonjob = false;
        RobotCount += 1;
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

    function printHello() public pure returns(string memory){
        return 'hello!';
    }

    function Tranc_stor(int eno,int x,int y) public {
        Tranc_info[eno].x = x;
        Tranc_info[eno].y = y;
        Tranc_info[eno].isdone = false;
    }

    function Tranc_status(int i) public {
        robot_data[i].isonjob = false;
    }

    //决定哪个去
    function decision() public returns(int en){
        int dicision = 1;
        int count = 1;
        int Tranc_count = 1;
        int MAX = 8000000;
        int tmp_dis;
        while(count<RobotCount){
            if(Tranc_info[Tranc_count].isdone = false){
                if(robot_data[count].isonjob = false){
                tmp_dis = Calucate(robot_data[count]._x,robot_data[count]._y,Tranc_info[Tranc_count].x,Tranc_info[Tranc_count].y);
                if(tmp_dis < MAX){
                    dicision = count;
                    MAX = tmp_dis;
                    count += 1;
                }else if(count < RobotCount){
                    count += 1;
                }else{
                    Tranc_info[Tranc_count].isdone = true;
                    robot_data[dicision].isonjob = true;
                    return dicision;
                }
           }else{
               count += 1;
                }
            }else{
                Tranc_count += 1;
            }
        }
    }
}
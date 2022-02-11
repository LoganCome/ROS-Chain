pragma solidity ^0.4.21;
// pragma experimental ABIEncoderV2;
contract RobotChainContract {
    //机器人数量
    int public RobotCount;
    int public Tranc_Count;

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
        int rel_x;
        int rel_y;
        int vel_x;
        int vel_y;
        string time;
        string key;
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
        int robotCount,
        int TrancCount
        )public {
        RobotCount = robotCount;
        Tranc_Count = TrancCount;
    }

    function getRobot() public view returns(address r){
        return robot_storage[msg.sender].robotaddress;
    }

    function getenode() public view returns(int){
        return robot_storage[msg.sender].enode;
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
    // function setPosition( int _x, int _y) public {
    //     robot_data[robot_storage[msg.sender].enode]._x = _x;
    //     robot_data[robot_storage[msg.sender].enode]._y = _y;
    // }
    function setKeyMessage(int _x, int _y,int vel_x,int vel_y,string time) public {
        robot_data[robot_storage[msg.sender].enode].rel_x = _x;
        robot_data[robot_storage[msg.sender].enode].rel_y = _y;
        robot_data[robot_storage[msg.sender].enode].vel_x = vel_x;
        robot_data[robot_storage[msg.sender].enode].vel_y = vel_y;
        robot_data[robot_storage[msg.sender].enode].time = time;
    }

    //获取当前机器人位置信息
     function getOwnPosition() public view returns(int, int) {
        return (robot_data[robot_storage[msg.sender].enode].rel_x,robot_data[robot_storage[msg.sender].enode].rel_y);
    }

    function Info_Check() public view returns(int, int, int, int, string){
        return(robot_data[robot_storage[msg.sender].enode].rel_x,robot_data[robot_storage[msg.sender].enode].rel_y,robot_data[robot_storage[msg.sender].enode].vel_x,robot_data[robot_storage[msg.sender].enode].vel_y,robot_data[robot_storage[msg.sender].enode].time);

    }

    //位置读取
    // function getPosition(address carPeerAddress) public view returns(int, int) {
    //     return (x[carPeerAddress], y[carPeerAddress]);
    // }

    //速度上传
    // function setVel( int vel_x,int vel_y) public {
    //     robot_data[robot_storage[msg.sender].enode].vel_x = _vel_x;
    // }

    //速度读取
    function getVel() public view returns(int,int) {
        return (robot_data[robot_storage[msg.sender].enode].vel_x,robot_data[robot_storage[msg.sender].enode].vel_y);
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
        Tranc_Count += 1;
    }

    function Tranc_status(int i) public {
        robot_data[i].isonjob = false;
    }

    function Get_Tranc() public view returns(int, int,int){
        return (Tranc_info[Tranc_Count].x,Tranc_info[Tranc_Count].y,Tranc_Count);

    }
    function Get_num() public view returns(int){
        return RobotCount;
    }
    function change_robot_status(int i) public{
        robot_data[i].isonjob = true;
    }
    function change_tranc_status(int i)public {
         Tranc_info[i].isdone = true;
    }


    //决定哪个去
    function decision() public payable  returns(int,int,int,int){
        int dicision = 1;
        int count = 1;
        int Tranc_count = 1;
        int MAX = 800000000000000;
        int tmp_dis = 0;
        while(count<=RobotCount){
            tmp_dis = Calucate(robot_data[count].rel_x,robot_data[count].rel_y,Tranc_info[Tranc_count].x,Tranc_info[Tranc_count].y);
            if((tmp_dis < MAX) && (Tranc_info[Tranc_count].isdone == false) && (robot_data[count].isonjob == false)){
                dicision = count;
                MAX = tmp_dis;
                count = count+1;
            }else if(count >= RobotCount){
                    change_tranc_status(Tranc_count);
                    change_robot_status(count);

            }else{
                count = count+1;
            }
    }
    return (dicision,Tranc_count,Tranc_info[Tranc_count].x,Tranc_info[Tranc_count].y);
}

}
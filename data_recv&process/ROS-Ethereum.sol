//SPDX-License-Identifier: <SPDX-License>
pragma solidity >0.5.0;


contract RobotChainContract {
    
    uint256 public ROSCount;
    
    address payable Administrator;
    
    struct ROS_Entity{
        address payable rosaddress;
        bool isRegistered;
        uint256 enode;
        bytes token;
    }
    
    bytes[][200] public upload_time;
    bytes[][200] public ciphertext;
    bytes[][200] public token_List;
    bytes[][200] public t_ciphertext;
    address payable[] public User_addresses;
    
    mapping(address => ROS_Entity) public Users;
    
   
    constructor() public {
        ROSCount = 0;
        Administrator = msg.sender;
        
        Users[Administrator].isRegistered = true;
        Users[Administrator].enode = 0;
        // Users[Administrator].token = token;
    }
    
    function register(bytes memory token) public returns (uint){
        require(
            msg.sender != Administrator,
            "The Administrator don't need to register!");
        require(
            Users[msg.sender].isRegistered != true,
            "You have registered!");
        require(ROSCount < 200,"Maximum limit exceeded!");
        
        if(ROSCount < 200){    
            ROSCount += 1;
            Users[msg.sender].isRegistered = true;
            Users[msg.sender].enode = ROSCount;
            Users[msg.sender].token = token;
            token_List[ROSCount].push(token);
            User_addresses.push(msg.sender);
            return 1;
        }else{
            return 2;
           
        }
    }
    
    function utilCompareInternal(bytes memory a, bytes memory b) internal pure returns (bool) {
        
        if (a.length != b.length) {
            return false;
        }
        
        for (uint i = 0; i < a.length; i ++) {
            if(a[i] != b[i]) {
                return false;
            }
        }
        
        return true;
    }
    
    function upload_data(bytes memory ctext,bytes memory token,bytes memory utime) public{
        
        require(Users[msg.sender].isRegistered == true,
        "U can't upload data cause u have't register! If u want to use the function of ROS2Chain, please register firstly");
        
        require(utilCompareInternal(Users[msg.sender].token,token) == true,
        "Wrong token!");
        
        uint256 index = uint256(Users[msg.sender].enode);
        // bytes memory text = bytes(ctext);
        // bytes memory time = bytes(utime);
        ciphertext[index].push(ctext);
        upload_time[index].push(utime);
    }
    
    function grant_authority(address to) public{
        require(Users[msg.sender].isRegistered == true,
        "U can't grant cause u have't register! If u want to use the function of ROS2Chain, please register firstly");
        require(Users[to].isRegistered == true,
        "The user u want to grant have't registered!");
        
        bytes memory t = Users[msg.sender].token;

        uint256 index = uint256(Users[to].enode);
        token_List[index].push(t);
        
        
    }
    
    function eth_apply_for() public returns (bool){
        require(Users[msg.sender].isRegistered == true,
        "U can't apply for ether cause u have't register! If u want to use the function of ROS2Chain, please register firstly");
        // address address_apply_for_eth = msg.sender;
        // require(address_apply_for_eth.balance < 10,
        // "U have some eth, reject to didisrstribute!");
       
        msg.sender.transfer(10);
        return true;
    }
    
    function isExistToken(address from,bytes memory token) public view returns (bool){
       
        bytes memory temp_token = token;
        
        for(uint i = 0; i < User_addresses.length; i++ ){
            if(from == User_addresses[i]){
                for(uint j = 0; j < token_List[i+1].length; j++){
                    if(token_List[i+1][j].length == temp_token.length){
                        for(uint k = 0; k < token_List[i+1][j].length+1; k++){
                            if(k == temp_token.length){
                                return true;
                            }
                            if(token_List[i+1][j][k] == temp_token[k]){
                                continue;
                            }else{
                                break;
                            }
                        }
                    }else{
                        continue;
                    }
                }
            }else{
                continue;
            }
        }
        return false;
    }
    
    function data_query_by_time_stamp(bytes memory token,bytes memory utime) public view returns (bytes memory){
        require(Users[msg.sender].isRegistered == true,
        "U can't query data cause u have't register! If u want to use the function of ROS2Chain, please register firstly");
        require(isExistToken(msg.sender,token) == true,
        "Token dosen't exist!");
        
        bytes memory temp_time = utime;
        bytes memory result;
        result = bytes('0x00');
        
        for(uint i = 0; i < User_addresses.length; i++){
            if (utilCompareInternal(Users[User_addresses[i]].token,token) == true){
                for(uint j = 0; j < upload_time[i+1].length; j++){
                    if(upload_time[i+1][j].length == temp_time.length){
                        for(uint k =0; k < temp_time.length+1; k++){
                            if(k == temp_time.length){
                                // return (ciphertext[i+1][j]);
                                result = ciphertext[i+1][j];
                            }
                            if(upload_time[i+1][j][k] == temp_time[k]){
                                continue;
                            }else{
                                break;
                            }
                            
                        }
                    }else{
                        continue;
                    }
                }
            }else{
                    continue;
                }
        }
    return result;
    }
    
}
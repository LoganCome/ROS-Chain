pragma solidity ^0.5.0;
pragma experimental ABIEncoderV2;

library Dataset {
    struct NodeData {           //链表节点数据结构
        string someData;
    }

    function getInitNodeData()
        internal
        pure
        returns(NodeData memory)
    {
        return NodeData("");
    }
}

library SinglyLinkedList {
    struct HeadNode {
        uint256 listLength;
        uint256 spacePointer;   //备用链指针，指针如果为0，则需要使用push方法扩展节点的长度
        uint256 next;           //指向第一个节点的数组下标
    }

    struct Node {
        Dataset.NodeData data;
        uint256 next;
    }

    struct List {
        HeadNode head;          //头结点
        Node[] nodes;
    }

    modifier validIndex(List storage self, uint256 index) {
        uint256 listLength = getListLength(self);
        require(index > 0 && index <= listLength, 'invalid index.');
        _;
    }

    function getListLength(List storage self)
        internal
        view
        returns(uint256)
    {
        return self.head.listLength;
    }

    function initList(List storage self)
        internal
        returns(bool)
    {
        require(0 == self.nodes.length, 'no need to initialize');
        //填充数组下标为0的元素，防止获取备用链数组下标时错判
        self.nodes.push(Node(Dataset.getInitNodeData(), 0));
        return true;
    }

    //获取节点的指针（数组下标）
    function getNodePointer(List storage self, uint256 index)
        validIndex(self, index)
        internal
        view
        returns(uint256)
    {
        uint256 firstNodePointer = self.head.next;
        if (1 == index) {
            return firstNodePointer;
        }

        Node storage node = self.nodes[firstNodePointer];
        uint256 j = 1;
        while (j < index - 1) {
            node = self.nodes[node.next];   //此节点的前驱节点
            j++;
        }
        return node.next;
    }

    function getNode(List storage self, uint256 index)
        validIndex(self, index)
        internal
        view
        returns(Node storage)
    {
        uint256 nodePointer = getNodePointer(self, index);
        return self.nodes[nodePointer];
    }

    function getNodeData(List storage self, uint256 index)
        validIndex(self, index)
        internal
        view
        returns(Dataset.NodeData storage)
    {
        Node storage node = getNode(self, index);
        return node.data;
    }

    function popSpacePointer(List storage self)
        private
        returns(uint256)
    {
        uint256 spacePointer = self.head.spacePointer;  //找到空闲节点的下标
        if (spacePointer > 0) {
            self.head.spacePointer = self.nodes[spacePointer].next;  //已经拿出一个备用空闲下标，将其的下一个备用空闲下标拿出来做备用
        }
        return spacePointer;
    }

    function insert(List storage self, uint256 index, Dataset.NodeData memory data)
        internal
        returns(bool)
    {
        uint256 listLength = getListLength(self);
        require(index > 0 && index <= listLength + 1, 'invalid index.');    //最多只能往链表的末尾加一个节点

        //如果是一个空的list则需要先初始化它
        if (0 == self.nodes.length) {
            initList(self);
        }

        uint256 newNodePointer = popSpacePointer(self);  //获得空闲节点的下标
        if (0 == newNodePointer) {    //备用节点下标为0，需要扩展list的节点数组长度
            Node memory newInitNode = Node(data, 0);
            newNodePointer = self.nodes.push(newInitNode) - 1;    //返回新节点的数组下标
        } else {
            self.nodes[newNodePointer].data = data;
        }

        if (1 == index) {   //如果index == 1，则需要更改头指针指向新节点的数组下标
            self.nodes[newNodePointer].next = self.head.next;
            self.head.next = newNodePointer;
        } else {            //如果index大于1则更改前驱节点的指针和新节点的指针
            Node storage preNode = getNode(self, index - 1);
            self.nodes[newNodePointer].next = preNode.next;
            preNode.next = newNodePointer;
        }

        self.head.listLength++;     //list长度加1
        return true;
    }

    function del(List storage self, uint256 index)
        validIndex(self, index)
        internal
        returns(bool)
    {
        uint256 deleteNodePointer = getNodePointer(self, index);
        Node storage deleteNode = self.nodes[deleteNodePointer];
        uint256 listLength = getListLength(self);

        if (1 == listLength) {      //最后一个节点被删除的时候，将头节点next指针重置为0
            self.head.next = 0;
        } else {
            if (1 == index) {       //第一个节点被删除的时候，需要更改头节点的next指针
                self.head.next = deleteNode.next;
            } else {
                Node storage preNode = getNode(self, index - 1);
                preNode.next = deleteNode.next;
            }
        }

        //将待删除节点移到备用节点的最前端，并且将备用节点链接到待删除节点之后
        deleteNode.next = self.head.spacePointer;
        self.head.spacePointer = deleteNodePointer;

        self.head.listLength--;     //list长度减1
        return true;
    }
}

contract TestLinkedList {
    using SinglyLinkedList for SinglyLinkedList.List;
    SinglyLinkedList.List singlyLinkedList;

    constructor() public {
        Dataset.NodeData memory firtNodeData = Dataset.NodeData("a");
        singlyLinkedList.insert(1, firtNodeData);
    }

    function insert(string memory data, uint256 index) public returns(bool) {
        Dataset.NodeData memory firtNodeData = Dataset.NodeData(data);
        return singlyLinkedList.insert(index, firtNodeData);
    }

    function del(uint256 index) public returns(bool) {
        return singlyLinkedList.del(index);
    }

    //链表长度
    function getListLength() public view returns(uint256) {
        return singlyLinkedList.head.listLength;
    }

    //链表节点数组的长度
    function getListNodesArrLength() public view returns(uint256) {
        return singlyLinkedList.nodes.length;
    }

    //获取链表节点数据组成的数组
    function getLinkedListData() public view returns(Dataset.NodeData[] memory) {
        uint256 listLength = getListLength();
        Dataset.NodeData[] memory listData = new Dataset.NodeData[](listLength + 1);
        for(uint256 i=1;i <= listLength;i++) {
            Dataset.NodeData memory nodeData = singlyLinkedList.getNodeData(i);
            listData[i] = nodeData;
        }
        return listData;
    }

    //获取链表节点的数组下标
    function getLinkedListPointer() public view returns(uint256[] memory) {
        uint256 listLength = getListLength();
        uint256[] memory listPointers = new uint256[](listLength + 1);
        for(uint256 i=1;i <= listLength;i++) {
            listPointers[i] = singlyLinkedList.getNodePointer(i);
        }
        return listPointers;
    }

    //获取空闲节点的数组下标
    function getSpaceLinkedList() public view returns(uint256[] memory) {
        uint256 listNodesArrLength = getListNodesArrLength();
        uint256[] memory spaceListPointers = new uint256[](listNodesArrLength + 1);
        uint256 firstSpacePointer = singlyLinkedList.head.spacePointer;
        SinglyLinkedList.Node storage spaceNode = singlyLinkedList.nodes[firstSpacePointer];
        spaceListPointers[1] = firstSpacePointer;
        uint256 i = 2;
        while(spaceNode.next > 0) {
            spaceListPointers[i] = spaceNode.next;
            spaceNode = singlyLinkedList.nodes[spaceNode.next];
            i++;
        }
        return spaceListPointers;
    }
}
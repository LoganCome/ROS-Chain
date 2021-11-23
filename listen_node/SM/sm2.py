#! /usr/bin/env python
# coding: utf-8
"""
SM2 国密非对称加密算法,属于椭圆曲线密码体制（ECC）
Author:John
基于椭圆曲线的离散对数难题，目前 SM2 256 bit 加密算法是相当安全的，相当于 RSA 2048 bit 及以上的安全性
有公钥、私钥之分，公钥给别人，可以在一定范围内公开，私钥留给自己，必须保密。由私钥可以计算公钥；由公钥计算私钥，是相当困难的，现阶段是不可能

加密过程：
    设需要发送的消息为比特串 M ，klen 为 M 的比特长度。
    为了对明文 M 进行加密，作为加密者的用户 A 应实现以下运算步骤：
    A1：用随机数发生器产生随机数k∈[1,n-1]；
    A2：计算椭圆曲线点 C1=[k]G=(x1,y1)，（[k]G 表示 k*G ）将C1的数据类型转换为比特串；
    A3：计算椭圆曲线点 S=[h]PB，若S是无穷远点，则报错并退出；
    A4：计算椭圆曲线点 [k]PB=(x2,y2)，将坐标 x2、y2 的数据类型转换为比特串；
    A5：计算t=KDF(x2 ∥ y2, klen)，若 t 为全0比特串，则返回 A1；
    A6：计算C2 = M ⊕ t；
    A7：计算C3 = Hash(x2 ∥ M ∥ y2)；
    A8：输出密文C = C1 ∥ C2 ∥ C3。
解密过程：
    设klen为密文中C2的比特长度。
    为了对密文C=C1 ∥ C2 ∥ C3 进行解密，作为解密者的用户 B 应实现以下运算步骤：
    B1：从C中取出比特串C1，将C1的数据类型转换为椭圆曲线上的点，验证C1是否满足椭圆曲线方程，若不满足则报错并退出；
    B2：计算椭圆曲线点 S=[h]C1，若S是无穷远点，则报错并退出；
    B3：计算[dB]C1=(x2,y2)，将坐标x2、y2的数据类型转换为比特串；
    B4：计算t=KDF(x2 ∥ y2, klen)，若t为全0比特串，则报错并退出；
    B5：从C中取出比特串C2，计算M′ = C2 ⊕ t；
    B6：计算u = Hash(x2 ∥ M′ ∥ y2)，从C中取出比特串C3，若u != C3，则报错并退出；
    B7：输出明文M′。
原理：
     用户 A 持有公钥PB=[dB]G（仅有PB值），用户 B 持有私钥 dB
     加密：C1=k*G  C2=M⊕(k*PB)      解密：M′=C2 ⊕ (dB*C1)     # 这里只叙述基本原理，便于理解
     证明：dB*C1=dB*k*G=k*(dB*G)=k*PB  因此，M′=C2 ⊕ (dB*C1)=M⊕(k*PB)⊕(k*PB)=M  得证

注：此实现算法所研究的椭圆曲线是基于域 Fp 上的椭圆曲线

安全参数设置：
     随机数 k 和私钥 dB 最好大点，2**50 以上比较安全

     successfully!!!  和官方例子加密结果测试一样
"""
import time
import math
import binascii as ba
from hashlib import sha256
import random
class SM2:
    # 固定 椭圆曲线 参数
    a=0x787968B4FA32C3FD2417842E73BBFEFF2F3C848B6831D7E0EC65228B3937E498
    b=0x63E4C6D3B23B0C849CF84241484BFE48F61D59A5B16BA06E6E12D1DA27C5249A
    v=256   # 哈希函数结果所占比特位
    p= 0x8542D69E4C044F18E8B92435BF6FF7DE457283915C45517D722EDB8B08F1DFC3
    Gx=0x421DEBD61B62EAB6746434EBC3CC315E32220B3BADD50BDC4C4E6C147FEDD43D
    Gy=0x0680512BCBB42C07D47349D2153B70C4E5D7FDFCBFA36EA1A85841B9E46E09A2
    n=0x8542D69E4C044F18E8B92435BF6FF7DD297720630485628D5AE74EE7C32E79B7

    # 可以自己调控  2048 最佳配置
    group=2048   # 明文分组大小  64 的倍数都可以

    def encrypt(self,plain,publickey):
        """
        加密主函数
        :param plain: 明文字符串
        :param publickey: 公钥 {x,y}
        :return: 16 进制密文
        """
        mstrb=ba.b2a_hex(plain.encode())
        mstr16=str(mstrb,'utf-8')
        grouplen=self.group//4

        mlist=[]
        i=-1           # 防止明文过短，而出现错误
        # 分组  2048 bit 一组 不需要 填充
        for i in range(len(mstr16)//grouplen):
            mlist.append(int(mstr16[grouplen*i:grouplen*(i+1)],16))

        if math.ceil(len(mstr16)/grouplen)!=len(mstr16)//grouplen:
            mlist.append(int(mstr16[grouplen*(i+1):],16))

        # 加密
        cipher=""
        for m in mlist:
            cipher+=self.encrypt_base(m,publickey)

        return cipher

    def decrypt(self,cipher,privatekey):
        """
        解密主函数
        :param cipher: 密文 16 进制串
        :param privatekey: 私钥 dB
        :return: 明文字符串
        """
        grouplen=(self.group+256+520)//4
        clist = []
        i = -1  # 防止密文过短，而出现错误
        # 分组  2824 bit 一组 不需要 填充
        for i in range(len(cipher) // grouplen):
            clist.append(cipher[grouplen * i:grouplen * (i + 1)])

        if math.ceil(len(cipher)/grouplen)!=len(cipher)//grouplen:
            clist.append(cipher[grouplen * (i + 1):])

        # 解密
        plain16=""
        for c in clist:
            plain16+=self.decrypt_base(c,privatekey)

        plain=ba.a2b_hex(plain16).decode()

        return plain

    def KDF(self,xy,klen):
        """
        秘钥扩展函数
        :param xy:经过转换后的秘钥 2 进制串
        :param klen:明文二进制长度
        :return: 扩展后的密钥 2 进制串
        """
        ct=1  # 计数器
        K=""  # 16 进制串
        for i in range(klen//self.v):
            K+=self.hash(xy+bin(ct)[2:].zfill(32))
            ct+=1

        # 取剩余部分
        flengh=klen/self.v
        if math.ceil(flengh)!=int(flengh):
            K+=self.hash(xy+bin(ct)[2:].zfill(32))[:(klen-self.v*int(flengh))//4]

        # 返回 2 进制
        binK=bin(int(K,16))[2:].zfill(klen)
        return binK

    def hash(self,str1):
        """
        哈希函数  国密 SM2 原版 hash 函数 是 SM3 256 bit，这里只是测试方便
        :return: 16 进制串 256 bit
        """
        sha=sha256()
        sha.update(str1.encode())
        return sha.hexdigest()

    def find_inverse_element(self,n,p):
        '''
        辗转相除法求 n 的逆元，不需要 n<p，但需要 n p 都为正整数
        注：辗转相除法求 s,t（as+bt=(a,b) 这里默认 a,b 为正整数）
            如果 a>b 且不大于 2**32，执行循环的次数最多为 logn（以 2 为底），故定义一维数组的 a[32],b[32] 的长度为 32
            这条信息在 C 语言等需要定义数组长度的情况下有用
        :param n: int 型
        :param p: 模数
        :return: n mod p 的逆元
        '''
        a = [0] * 1000  # 注意 1000 可能不够长，适时调整即可
        b = [0] * 1000
        a[0] = p
        b[0] = n

        i = 0
        while (a[i] % b[i]):
            a[i + 1] = b[i]
            b[i + 1] = a[i] % b[i]
            i += 1

        i -= 1
        shang_a = 1
        shang_b = -(a[i] // b[i])
        while (i != -1):
            if (i >= 1):
                tmp = shang_a
                shang_a = shang_b
                shang_b = tmp - a[i - 1] // b[i - 1] * shang_b

            i -= 1

        return shang_b % a[0]

    def oval_same_add(self,G):
        """
        椭圆曲线上的相同坐标的两个点相加
        :param G: 生成元，基点
        :return: 相加之后的点
        """
        x1,y1=G

        # 计算 斜率 k ，k 已不具备明确的几何意义
        tmp1=3*x1*x1+self.a
        tmp2=self.find_inverse_element(2*y1,self.p)
        k=tmp1*tmp2%self.p

        # 求 x3
        x3=(k*k-x1-x1)%self.p

        # 求 y3
        y3=(k*(x1-x3)-y1)%self.p

        return x3,y3

    def oval_diff_add(self,G1,G2):
        """
        椭圆曲线上的不同坐标的两个点相加
        :param G1，G2: 两个点坐标
        :return: 相加之后的点
        """
        x1,y1=G1
        x2,y2=G2

        # 计算 斜率 k
        tmp1=y2-y1
        tmp2=self.find_inverse_element((x2-x1)%self.p,self.p)
        k=tmp1*tmp2%self.p

        # 求 x3
        x3=(k*k-x1-x2)%self.p

        # 求 y3
        y3=(k*(x1-x3)-y1)%self.p

        return x3,y3

    def oval_diff_add_near(self,point,pointBase):
        """
        相邻的两个点相加 ，pointBase 为基点，如：23P 点 + 24P 点
        :return: 新的点
        """
        return self.oval_diff_add(pointBase,self.oval_same_add(point))

    def oval_multiply(self,k,G):
        """
        椭圆曲线上的点乘以常数 k
        :param k: int 型 k*G 中的 k
        :param G: 生成元，基点
        :return: 相乘之后的点
        """
        # if k==1:
        #     return G   # 只有 k 取 1 时，才有这种可能

        if k==2:
            return self.oval_same_add(G)

        if k==3:
            return self.oval_diff_add(G,self.oval_same_add(G))

        if k%2==0:
            return self.oval_same_add(self.oval_multiply(k//2,G))  # return 里只能有一个 oval_multiply 函数，两个及以上很有可能出错，进入无限循环

        if k%2==1:
            return self.oval_diff_add_near(self.oval_multiply(k//2,G),G)

    def encrypt_base(self,m,publickey):
        """
        加密基函数
        :param m: int 型明文
        :return: 密文 16 进制字符串
        """
        # 明文 转 二进制
        binm=bin(m)[2:] #  明文长度

        # 给明文 2 进制补全
        binm='0'*(-len(binm)%8)+binm

        # 计算 明文 比特串 长度
        mlen = len(binm)

        # 提取公钥
        x,y=publickey

        while(True):
            # 生成随机数 k
            k=random.randint(1<<50,1<<100)  # 包括两端

            # C1 = k*G  and C1 转 比特串
            C1=self.oval_multiply(k,(self.Gx,self.Gy))
            binC1=self.point_to_bit(C1)

            # Cb = k*PB and Cb 转 比特串 ，其中，PB 为公钥
            Cb=self.oval_multiply(k,(x,y))
            binCb=self.point_to_bit(Cb,False)

            # t=KDF(x2||y2,Mlen)  KDF 是密钥派生函数
            t=self.KDF(binCb[0],mlen)     # binCb[0]=x2||y2

            if t.count('0')!=mlen:
                break

        # C2 = M ⊕ t
        binC2=self.bin_xor(binm,t)

        # C3=Hash(x2||M||y2)
        C3=self.hash(binCb[1]+binm+binCb[2])

        # 16 进制 C1 C2
        hexC1=hex(int(binC1,2))[2:].zfill(130)
        hexC2=hex(int(binC2,2))[2:].zfill(mlen//4)

        return hexC1 + hexC2 + C3

    def decrypt_base(self,cipher,privatekey):
        """
        解密基函数
        :param cipher: 16 进制串
        :param privatekey: 私钥 dB
        :return: 16 进制串
        """
        # 取私钥
        dB=privatekey

        # 取 密文 各 部分
        clen=len(cipher)
        hexC1=cipher[:130]
        hexC2=cipher[130:clen-self.v//4]
        hexC3=cipher[-self.v//4:]

        # 将密文 C1 转换成 椭圆上的 点坐标
        pointC1=self.hex_to_point(hexC1)

        # 检测 密文 点坐标 是否是 椭圆上 的点坐标
        if not self.discover_cipher_true_or_false(pointC1):
            exit("密文错误，请重新更换正确的密文")

        # 计算 dB*C1=(x2,y2) and 转 比特串
        dB_C1=self.oval_multiply(dB,pointC1)
        binC1=self.point_to_bit(dB_C1,False)

        # 密文 C2 转 二进制
        binlenC2=len(hexC2)*4
        binC2=bin(int(hexC2,16))[2:].zfill(binlenC2)

        # 进入密钥扩展
        t=self.KDF(binC1[0],binlenC2)

        # 解密后的明文二进制 M′
        binm=self.bin_xor(binC2,t)

        # 计算 u = Hash(x2 ∥ M′ ∥ y2)
        u=self.hash(binC1[1]+binm+binC1[2])

        # 判断 解密 后 的明文 是否是 以前的明文
        if u!=hexC3:
            exit("解密明文非原明文，程序出现错误")

        # 明文 2 进制 转 16 进制
        hexplain=hex(int(binm,2))[2:].zfill(len(binm)//4)

        return hexplain

    def key_produce(self):
        """
        生成公钥 私钥函数
        :return: 公钥：PB {x,y} 私钥：dB
        """
        dB=random.randint(1<<50,1<<100)
        PB=self.oval_multiply(dB,(self.Gx,self.Gy))

        #  以下参数为官方文档例子参数
        # dB=0x1649AB77A00637BD5E2EFE283FBF353534AA7F7CB89463F208DDBC2920BB0DA0
        # PB=0x435B39CCA8F3B508C1488AFC67BE491A0F7BA07E581A0E4849A5CF70628A7E0A,\
        #    0x75DDBA78F15FEECB4C7895E2C1CDF5FE01DEBB2CDBADF45399CCF77BBA076A42
        return PB,dB

    def hex_to_point(self, hexstr, C1_sign=True):
        """
        16 进制串转点坐标
        :param hexstr: 16 进制串
        :param C1_sign: True 为 C1 16 进制串，false 为其他 16 进制串
        :return: 点坐标
        """
        x=int(hexstr[-128:-64], 16)
        y=int(hexstr[-64:], 16)

        if C1_sign==True:
            if int(hexstr[:2], 16)==4:
                return x,y
            else:
                return False
        else:
            return x,y

    def point_to_bit(self,point,C1_sign=True):
        """
        点坐标转比特串
        :param C1_sign: true 为 转换来自 C1 的点，false 为 转换来自 其他点
        :param point: 点坐标
        :return: 2 进制串
        """
        x=point[0]
        y=point[1]
        pc='00000100'  # 单一字节 04
        binx=bin(x)[2:].zfill(256)
        biny=bin(y)[2:].zfill(256)

        if C1_sign:
            return pc+binx+biny
        else:
            return binx+biny,binx,biny

    def bin_xor(self,bina,binb):
        """
        比特串异或,需要长度一样
        :param bina: 比特串 a
        :param binb: 比特串 b
        :return: 异或后的比特串
        """
        binc = ''
        lengh=len(bina)
        for i in range(lengh):
            binc += str(int(bina[i]) ^ int(binb[i]))
        return binc

    def discover_cipher_true_or_false(self,cipher):
        """
        检测密文 点坐标 是否正确
        :param cipher: 待检测的密文 点坐标
        :return: True 检测合格 False 检测不合格
        """
        x,y=cipher

        # y*y=x*x*x+a*x+b 检测方程
        tmp1=y*y%self.p
        tmp2=(x*x*x+self.a*x+self.b)%self.p

        if tmp1==tmp2:
            return True
        else:
            return False

#########################################################################################################################
# start=time.time()
# my=SM2()
#
# # 生成公、私钥
# # key=my.key_produce()
# # PB,dB=key
#
# # plain="are you ok!!! i am ok"
# # plain='123456'
#
# # cipher=my.encrypt(plain,PB)
# # print("密文：",cipher)
# plain = 'ROSChain'
# dB = 1224746328650657713959770508238
# cipher = '0461096e236fec39c9e477ba55880834166a4d7dd3e4e286a47bcdb7c1f02be2e266ab92d535d5ba5c555044fc79f81875ea1efe64328ce28a204d6a714e298825143a5d22fc9005b606ff5b375fc18b113ccd895cf2351537d79e75960e4cd4ee42ed9ffd8620021d'
# p=my.decrypt(cipher,dB)
# print p
#
#
# if plain==p:
#     print("加解密成功")
# else:
#     print("失败")
#
# print('time',time.time()-start)
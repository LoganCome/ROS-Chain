# Python And Ethereum

JSONRPC手册：http://cw.hubwiz.com/card/c/ethereum-json-rpc-api/1/1/1/

## ganache的使用

ganache虽然不是一个真正的以太坊节点软件， 但它完整实现了以太坊的JSON RPC接口，非常适合以太坊智能合约与去中心化应用开发的 学习与快速验证

安装：http://blog.hubwiz.com/2018/04/16/ganache-cli-manual/

启动：

```
~$ ganache-cli
```

默认情况下，ganache会随机创建10个账户，每个账户中都有100ETH的余额。你可以在 命令行中指定一些参数来调整这一默认行为。

```
~$ ganache-cli -a 20
```

## 使用curl获取节点版本信息

![web3 client version protocol](http://xc.hubwiz.com/class/5b40462cc02e6b6a59171de4/img/client-version-rpc.png)

这是一个典型的请求/应答模型，请求包和响应包都是标准的JSON格式。其中，`jsonrpc`字段用来 标识协议版本，`id`则用来帮助建立响应包与请求包的对应关系。

在请求包中，使用`method`字段来声明接口方法，例如`web3_clientVersion`，使用`params` 字段来声明接口方法的参数数组。 在响应包中，`result`字段中保存了命令执行的返回结果。

以太坊JSON RPC并没有规定传输层的实现，不过大部分节点都会实现HTTP和IPC的访问。因此 我们可以使用命令行工具`curl`来测试这个接口：

```
~$ curl -X POST http://localhost:8545  -d '{
> "jsonrpc": "2.0",
> "method": "web3_clientVersion",
> "params": [],
> "id": 123
> }' | jq    //jq是级联命令，是一个json解析工具
```

返回值：

```
{
  "id": 123,
  "jsonrpc": "web3_clientVersion",
  "result": "EthereumJS TestRPC..."
}
```

So,我们只需要利用python向代码发送http请求包就可以了。你可以使用任何一个你喜欢的http库例如requests等， 甚至直接使用socket来调用以太坊的JSON RPC API。

例如：

```python
import requests, time
#dumps用于将python数据转换成json字符串
data = json.dumps({
  'jsonrpc':'2.0',
  'method': 'web3_clientVersion',
  'params': [],
  'id': int(time.time()*1000)
})
rsp = requests.post('http://localhost:8545',data=data)
print(rsp.text)
```

## 使用现成的轮子

`web3.py`是以太坊官方维护的Python版rpc接口封装库，因此我们优先选择它。下面是使用web3.py获取节点版本信息的代码：

```python
from web3 import Web3, HTTPProvider

provider = Web3.HTTPProvider('http://localhost:8545') 
w3 = Web3(provider)
```

`Web3`是`web3.py`的入口类，我们与以太坊的交互，基本上是通过这个入口来完成的，实例化`Web3`需要指定一个通信服务提供器，即明确采用何种通信 方式与哪一个节点进行交互。

`web3.py`目前实现了IPC、HTTP和Websocket这三种网络服务提供器。 `HTTPProvider`则声明了与以太坊节点旳连接采用HTTP协议，只需要指定 节点旳URL即可。由于我们使用本机的ganche-cli仿真器，因此使用`http://localhost:8545` 这个URL。

`Web3`实例的`manager`属性是一个`RequestManager`实例，它负责利用指定的通信服务提供器来进行jsonrpc交互，因此它是通信传输无关的（`provider agnostic`）。`request_blocking()`返回的结果不是完整的jsonrpc响应，而仅仅是包含其`result`字段。 调用其`request_blocking()`方法来提交请求：

```python
version = w3.manager.request_blocking('web3_clientVersion',[])
```

## web3.py的命名规范

`web3.py`根据不同的RPC接口类别，封装了不同的实现类。 例如对于`eth_*`这一族的接口，其调用实现就封装在`Eth`类中：

![rpc class pair](http://xc.hubwiz.com/class/5b40462cc02e6b6a59171de4/img/rpc-class-pair.png)

这些RPC封装类的构造函数都需要传入一个`Web3`实例，以便与明确的节点交互。 例如，下面的代码创建一个`Eth`对象，并读取某个账户的余额：

```python
from web3 import Web3, HTTPProvider
from web3.eth import Eth

w3 = Web3(HTTPProvider('http://localhost:8545'))
eth = Eth(w3)

balance = eth.getBalance('0x123458...')
```

在具体的方法命名上，`web3.py`基本上采用了对应的RPC接口名称。例如 `eth_getBalance`调用对应于`Eth`类的`getBalance()`方法 —— 下划线之前的 对应于类名，之后的对应于方法名。

**不过还是有一些例外。**

对于那些不需要参数的只读RPC调用，例如`eth_accounts`，`web3.py`将其声明为 属性。例如读取当前节点旳账户清单：

```python
console.log(eth.accounts)
```

另一种情况是，`web3.py`有时会把多个RPC调用合并到一个方法里。例如`Eth` 类的`getBlock()`方法同时对应于`eth_getBlockByHash`和`eth_getBlockByNumber` 这两个调用 —— 它通过对参数类型的判别来区分应该调用哪个RPC接口：

```python
block = eth.getBlock(12) # 读取第12块的信息，对应eth_getBlockByNumber调用
block = eth.getBlock('0x12345890...') # 读取具有指定哈希的块，对应eth_getBlockByHash调用
```

## Web3入口类

`Web3`是一个入口类，通过它即可访问其他RPC封装类的实例而无须单独创建：

![web3 umbrella](http://xc.hubwiz.com/class/5b40462cc02e6b6a59171de4/img/web3-umbrella.png)

例如，下面的代码利用入口类实例的`eth`对象调用`eth_accounts`接口获取 节点账户列表，然后调用`eth_getBalance`接口获取第一个账户的余额：

```python
from web3 import Web3
w3 = Web3(Web3.HTTPProvider('http://localhost:8545'))
accounts = w3.eth.accounts
balance = w3.eth.getBalance(accounts[0])
print('balance: %d' % balance)
```

## 公钥私钥与地址

![keys and address](http://xc.hubwiz.com/class/5b40462cc02e6b6a59171de4/img/keypairs-address.png)

从私钥可以得到公钥，然后进一步得到账户地址，而反之则无效。 显然，以太坊不需要一个中心化的账户管理系统，我们可以根据以太坊约定 的算法自由地生成账户。

在Python中，可以使用`eth_keys`包的`KeyAPI`类来生成密钥对和账户地址。 例如，下面的代码首先创建一个随机私钥，然后生成公钥，最后后利用公钥生成账户地址：

```python
from eth_keys import keys
from eth_utils.curried import keccak
import os

key_bytes = keccak(os.urandom(32) + b'add some entropy') # 32字节私钥
privateKey = keys.PrivateKey(key_bytes)    #创建私钥对象
publicKey = privateKey.public_key          #私钥对象中包含对应的公钥
address = publicKey.to_checksum_address()  #公钥转换为地址
```

![keys and address class](http://xc.hubwiz.com/class/5b40462cc02e6b6a59171de4/img/keys-address.png)

## 创建账户

在`eth_accounts`包中，使用账户类来保存一个以太坊账户的私钥、公钥和地址信息：

![account](http://xc.hubwiz.com/class/5b40462cc02e6b6a59171de4/img/account-class.png)

Account类同时提供了一个静态方法`create()`来创建新的账户，该方法 返回一个`LocalAccount`实例。例如，下面的代码首先创建一个账户， 然后提取账户私钥、公钥和地址：

```python
from eth_account import Account
account = Account.create()
privateKey = account._key_obj
publicKey = privateKey.public_key
address = publicKey.to_checksum_address()
```

## 导入私钥

我们已经知道，只有私钥是最关键的，公钥和账户都可以从私钥一步步 推导出来。

假如你之前已经通过其他方式有了一个账户，例如使用Metamask创建 的钱包，那么可以把该账户导入Python应用，重新生成公钥和账户地址：

![account](http://xc.hubwiz.com/class/5b40462cc02e6b6a59171de4/img/account-2.png)

```python
from eth_account import Account

account = Account.privateKeyToAccount('0x08422574....')
privateKey = account._key_obj
publicKey = privateKey.public_key
address = publicKey.to_checksum_address()
```

## keystore钱包文件

鉴于私钥的重要性，我们需要以一种安全地方式保存它，而不是简单地 存到一个文件里。

keystore允许你用加密的方式存储密钥。这是安全性（一个攻击者需要 keystore 文件和你的钱包口令才能盗取你的资金）和可用性（你只需要keystore 文件和钱包口令就能用你的钱了）两者之间完美的权衡。

下图是一个keystore文件的内容：

![keystore](http://xc.hubwiz.com/class/5b40462cc02e6b6a59171de4/img/keystore.png)

从图中可以看出，keystore的生成使用了两重算法：首先使用你指定的钱包口令 采用kpf参数约定的算法生成一个用于AES算法的密钥，然后使用该密钥 结合ASE算法参数iv对要保护的私钥进行加密。

由于采用对称加密算法，当我们需要从keystore中恢复私钥时，只需要使用生成该钱包的密码，并结合keystore文件中的算法参数，即可进行 解密出你的私钥。

`Account`类提供了两个静态方法`encrypt()`和`decrypt()`，用于将私钥转换为 keystore格式的json对象以及反之，这两个方法是基于`eth_keyfile包的相应 函数实现的。

例如，下面的代码使用口令`123`将账户私钥存入`./keystore`目录，并返回钱包文件名：

```python
wallet = Account.encrypt(account.privateKey,'123')
wfn = './keystore/{0}.json'.format(account.address)
with open(wfn,'w') as f: 
json.dump(wallet,f)
```

下面的代码使用口令`123`从钱包文件恢复出私钥并重建账户对象：

```python
with open(wfn,'r') as f: 
wallet = json.load(f)
priv_key = Account.decrypt(wallet,'123')
account = Account.privateKeyToAccount(priv_key)
```

## 增加以太坊支付功能

在应用中生成密钥对和账户有很多用处，例如，用户可以用以太币 在我们的网站上购买商品或服务 —— 为每一笔订单生成一个新的以太坊地址，让用户支付到该地址，然后我们检查该地址余额即可了解订单的支付情况，进而执行后续的流程。

为什么不让用户直接支付到我们的主账户？

稍微思考一下你就明白，创建一个新地址的目的是为了将支付与订单关联起来。如果让用户支付到主账户，那么除非用户支付时在交易数据 里留下对应的订单号，否则你无法简单的确定收到的交易与订单之间的关系，而不是所有的钱包软件—— 例如coinbase —— 都支持将留言包含 在交易里发送到链上。

![pay by ether](http://xc.hubwiz.com/class/5b40462cc02e6b6a59171de4/img/pay-by-ether.png)

当用户选择使用以太币支付一个订单时，web服务器将根据该订单的订单号 提取或生成对应的以太坊地址，然后在支付页面中展示该收款地址。为了 方便使用手机钱包的用户，可以同时在支付页面中展示该收款地址的二维码。

用户使用自己的以太坊钱包向该收款地址支付以太币。由于网站的支付处理 进程在周期性地检查该收款地址的余额，一旦收到足额款项，支付处理进程 就可以根据收款地址将对应的订单结束，并为用户开通对应的服务。

## 以太坊状态机

以太坊将世界抽象为数量巨大的状态节点，而交易则是导致这些状态节点 的值发生变化的激励。

例如，某个账户的余额就是一个状态节点，它的值在某个时刻是确定的， 如果其他人向该账户发起转账交易，那么在下一个时刻，这个状态节点 旳值就会更新为新的值：

![block transaction and state](http://xc.hubwiz.com/class/5b40462cc02e6b6a59171de4/img/block-tx-state.png)

以太坊采用一种树形数据结构来保存所有的状态节点，该状态树的根节点则 和交易记录一起保存在区块中。在每一个区块都对应着一棵状态树，它代表 着世界在那个时刻的确定状态 —— 一个快照。

容易理解，状态可以从交易推演出来。例如，要得到一个账户的余额，只需要 把该账户所有的转账交易汇总在一起就可以得出。但以太坊通过使用状态树 来表征交易的结果，使得这一查询可以迅速完成。

## 获取账户余额

以太坊定义了[eth_getBalance](http://cw.hubwiz.com/card/c/ethereum-json-rpc-api/1/3/8/) 接口用来获取账户余额，在`web3.py`中，对应`Eth`类的`getBalance()`方法。

例如，下面的代码读取节点管理的第一个账户的余额：

```Python
from web3 import Web3

w3 = Web3(Web3.HTTPProvider('http://localhost:8545'))
accounts = w3.eth.accounts
balance = w3.eth.getBalance(accounts[0],'latest')
print('balance@latest => {0}'.format(balance))
```

注意`eth_getBalance`接口的第二个参数，使用它来指定一个特定的块。 `'latest'`这个字符串表示使用链上的最后一个块，也就意味着它使用最后一个块的状态树 中记录的账户余额，即当前账户余额：

![block balance](http://xc.hubwiz.com/class/5b40462cc02e6b6a59171de4/img/block-balance.png)

由于每个块都有对应着块生成那一时刻的状态树，因此你指定一个不同的块，就意味着 将账户余额这个状态回溯到那个块生成的特定时刻。例如，查看这个账户 最初的余额：

```python
balance = w3.eth.getBalance(accounts[0],'earliest')
print('balance@earliest => {0}'.format(balance))
```

也可以使用编号来指定块，例如，查看第12块时的账户余额：

```python
balance = w3.eth.getBalance(accounts[0],12)
print('balance@block#12 => {0}'.format(balance))
```

## 以太坊货币单位

`eth_getBalance`调用返回的数字，其单位为`wei`，是以太币众多面值单位中 最小的一只，而我们常说的1个以太币，则对应单位`ether`，这两者之间差着 18个0：

```
1 ether = 10^18 wei
```

以太坊定义了10个等级的面值单位，连续两个单位之间总是差3个0，也就是1000倍 的关系。下表列出了常用的单位：

| 单位               | 单位价值 | 换算为wei                 |
| :----------------- | :------- | :------------------------ |
| wei                | 1 wei    | 1                         |
| Kwei(babbage)      | 1e3 wei  | 1,000                     |
| Mwei(lovelace)     | 1e6 wei  | 1,000,000                 |
| Gwei(shannon)      | 1e9 wei  | 1,000,000,000             |
| microether(szabo)  | 1e12 wei | 1,000,000,000,000         |
| milliether(finney) | 1e15 wei | 1,000,000,000,000,000     |
| ether              | 1e18 wei | 1,000,000,000,000,000,000 |

## 货币单位表示与换算

由于在应用逻辑中通常使用wei作为计量单位，`Web3`类提供了两个静态 方法用于将其他单位换算到wei，或者从wei换算到其他单位：

![convert](http://xc.hubwiz.com/class/5b40462cc02e6b6a59171de4/img/uml-convert.png)

例如，下面的代码将1个ether转换为wei计量的值：

```python
one_ether = Web3.toWei(1,'ether')
```

又如，下面的代码将1 wei转换为kwei计量的值：

```python
one_wei = Web3.fromWei(1,'kwei')
```

如果你需要进行转换的两个单位中不包含wei，那么可以结合`fromWei()` 和`toWei()`这两个函数，先转化到wei，再转化到目标单位。

## 交易类型

在以太坊中，约定了两种交易：普通交易（`Transaction`）和裸交易（`RawTransaction`）。

这两种交易的区别在于：普通交易由节点负责签名，然后发送到网络中进行确认；

![transaction process](http://xc.hubwiz.com/class/5b40462cc02e6b6a59171de4/img/tx-proc.png)

而裸交易则由外部应用进行签名，节点不再额外处理，而只是负责发送到网络中进行确认 —— 这也是裸交易名称的由来 —— 未经节点加工的原始交易：

![raw transaction process](http://xc.hubwiz.com/class/5b40462cc02e6b6a59171de4/img/raw-tx-proc.png)

以太坊约定了两种交易不同的提交接口：普通交易使用`eth_sendTransaction` 调用提交，而裸交易则应当使用`eth_sendRawTransaction`调用提交。事实上， 在公共节点中，通常会拒绝普通交易的提交，而要求外部应用必须进行离线签名。

## 提交普通交易

普通交易由节点负责进行签名。在`web3.py`中，提交一个普通交易，需要使用 `Eth`类的`sendTransaction()`方法发送一个请求包，该方法对应 [eth_sendTransaction](http://cw.hubwiz.com/card/c/ethereum-json-rpc-api/1/3/17/) 这个RPC接口。

应用首先应当准备一个请求包，其中可以包含以下内容：

- from: 发送交易的源地址
- to: 交易的目标地址
- gas: 交易执行gas用量上限
- gasPrice: gas价格
- value: 交易发送的金额，单位：wei
- data: 额外携带的数据
- nonce: 一次性序号，用来对抗重放攻击

不过只有发送方和接收方的信息是必须的，其他不需要的信息都可以不填。例如， 下面的代码从节点第1个账户向第2个账户转100wei的资金，我们只需要设置`from`、 `to`和`value`字段：

```python
accounts = w3.eth.accounts
payload = {
  'from': accounts[0],
  'to': accounts[1],
  'value': 100
}
tx_hash = w3.eth.sendTransaction(payload)
```

向节点成功发送交易请求后，节点将返回该交易的哈希值。不过由于ganache是实时完成交易的，所以我们可以 马上检查第1个账户的余额：

```python
balance = w3.eth.getBalance(accounts[0],'latest')
```

## 获取交易数据

由于以太坊的交易需要提交到网络中进行共识处理，因此我们提交的交易不会 马上生效。要查询交易是否生效，需要使用`eth_sendTransaction`调用返回 的交易哈希读取交易收据。

根据以太坊的约定，应用需要调用[eth_getTransactionReceipt](http://cw.hubwiz.com/card/c/ethereum-json-rpc-api/1/3/26/) 接口来检索具有指定哈希交易的收据。

例如，下面的代码读取具有指定哈希的交易的收据：

```python
receipt = w3.eth.getTransactionReceipt(tx_hash)
```

交易收据是一个`AttributeDict`对象，因此你可以直接使用`.`来访问以下属性：

- transactionHash: 交易哈希
- transactionIndex: 交易在块内的索引序号
- blockHash: 交易所在块的哈希
- blockNumber: 交易所在块的编号
- cumulativeGasUsed: 交易所在块消耗的gas总量
- gasUsed: 本次交易消耗的gas用量
- contractAddress: 对于合约创建交易，该值为新创建的合约地址，否则为null
- logs: 本次交易生成的日志对象数组

例如，查看这个交易消耗的`gas`：

```python
print('gas used => {0}'.format(receipt.gasUsed))
```

按照以太坊的出块速度，大约最快需要15秒交易才可能得到确认，因此我们需要 周期性地检查交易收据。例如，下面的代码每隔2秒钟检查一次交易收据， 直到60秒超时或取得有效收据：

```python
def waitForReceipt(tx_hash,timeout=60,interval=2):
  t0 = time.time()
  while True:
    receipt = w3.eth.getTransactionReceipt(txhash)
    if receipt is not None:
      break
    delta = time.time() - t0
    if delta > timeout :
      break
    time.sleep(interval)

  return receipt

receipt = waitForReceipt(tx_hash)  
print(receipt)
```

`web3.py`的`Eth`类也提供了`waitForTransactionReceipt()`方法来等待 交易收据，它的实现逻辑基本和上面的代码一致，区别在于它在一个单独的 线程里等待收据。例如，下面的代码设置超时时长为60s:

```python
receipt = w3.eth.waitForTransactionReceipt(tx_hash,60)
```

## gas价格与用量

在我们之前创建交易对象时，有意忽略了gas相关的参数，让节点自己决定。 因为，gas是以太坊中最令人迷惑的概念之一。

**Gas**：Gas对应于一个交易(Transaction)中以太坊虚拟机(EVM)的实际运算步数。 越简单的交易，例如单纯的以太币转帐交易，需要的运算步数越少， Gas亦会需要的少一点。 反之，如果要计算一些复杂运算，Gas的消耗量就会大。

**Gas Price**：Gas Price就是你愿意为一个单位的Gas出多少Eth，一般用Gwei作单位。 所以Gas Price 越高， 就表示交易中每运算一步，会支付更多的Eth。

因此，以太坊的交易手续费计算公式很简单：

```
交易手续费(Tx Fee) = 实际运行步数(Actual Gas Used) * 单步价格(Gas Price)
```

例如你的交易需要以太坊执行50步完成运算，假设你设定的Gas Price是2 Gwei ，那么整个 交易的手续费 就是50 * 2 = 100 Gwei 了。

**Gas Limit**：Gas Limit就是一次交易中Gas的可用上限，也就是你的交易中最多会执行多少步运算。 由于交易复杂程度各有不同， 确切的Gas消耗量是在完成交易后才会知道，因此在你提交交易之前，需要为交易设定一个Gas用量的上限。

如果说你提交的交易尚未完成，消耗的Gas就已经超过你设定的Gas Limit，那么这次交易就会被取消，而已经消耗的手续费同样被扣取 —— 因为要奖励已经付出劳动的矿工。 而如果交易已经完成，消耗的Gas未达到Gas Limit， 那么只会按实际消耗的Gas 收取交易服务费。 换句话说，一个交易可能被收取的最高服务费就是Gas Limit * Gas Price 了。

最后值得一提的是Gas Price 越高，你提交的交易会越快被矿工接纳。 但通常人们都不愿多支付手续费， 那么究竟应该将Gas Price设置为多少，才可以在正常时间(eg 10 mins)内，确保交易被确认到区域链上呢？ [这个网站](http://ethgasstation.info/)可以帮到你。

## 估算交易的gas用量

以太坊提供了一个接口[eth_estimateGas](http://cw.hubwiz.com/card/c/ethereum-json-rpc-api/1/3/20/) 用来估算一个交易的gas用量。在`web3.py`中，对应于`Eth`类的`estimateGas()`方法。

例如，下面的代码估算向链上写入数据用量：

```python
payload = {
  'from': accounts[0],
  'to': accounts[1],
  'value': 1000
}
estimation = w3.eth.estimateGas(payload)
```

`eth_estimateGas`调用的实现原理是，仅仅在节点自己的EVM中执行这个交易并返回交易的gas消耗量。与实际提交交易不同，这个调用不会把交易扩散到其他 节点。

要写入的字节越多，所需要消耗的gas越多。例如，在下面的代码中，我们估算将整本《孙子兵法》写入链上的gas用量：

```python
with open('./the-art-of-war.txt') as f:
  txt = f.read()

payload = {
  'from': accounts[0],
  'to': accounts[0],
  'data': to_hex(str.encode(txt))
}
estimation = w3.eth.estimateGas(payload)
```

## 使用裸交易

与普通交易由节点负责签名不同，裸交易需要外部应用进行离线签名。 因此在使用裸交易之前，需要首先载入账户对象 —— 要用到其中 保存的私钥进行签名：

![raw transaction steps](http://xc.hubwiz.com/class/5b40462cc02e6b6a59171de4/img/raw-tx-steps.png)

可以使用钱包来实例化一个账户凭证对象，例如，下面的代码使用 密码`123`解密指定的钱包文件并重建账户对象：

```python
with open('./keystore/...','r') as f:
  wallet = json.load(f)
priv_key = Account.decrypt(wallet,'123')  
account = Account.privateKeyToAccount(priv_key)
```

接下来创建裸交易对象。由于发送方将对裸交易签名，因此在裸交易对象中不需要重复指定发送方账户。例如，下面的代码将构造一个从钱包账户向节点第2个账户转账的裸交易对象：

```python
nonce = w3.eth.getTransactionCount(account.address)
payload = {
  'to': accounts[1],
  'value': 100,
  'gas': 200000,
  'gasPrice': Web3.toWei(20,'gwei'),
  'nonce': nonce,
  'chainId': 1
}
```

`nonce`的作用是对抗重放攻击，在不同的交易中它应当是不重复的，除非你需要覆盖之前 的交易。在以太坊中应当将这个值设置为发送方账户已经发送的交易数量 —— 使用 [eth_getTransactionCount](http://cw.hubwiz.com/card/c/ethereum-json-rpc-api/1/3/10/) 调用来获取这个值，对应于`web3.py`中的`getTransactionCount()`方法。

一旦创建了裸交易对象，就可以使用发送方账户对其进行签名，签名的结果是一个 字符串：

```python
signed = account.signTransaction(payload)
print('signed payload => {0}'.format(to_hex(signed.rawTransaction)))
```

`Account`对象的`signTransaction()`方法返回一个`AttributeDict`，其中的`rawTransaction` 属性包含了我们需要提交给节点旳签名码流。看起来是这样：

```python
{
  'hash': HexBytes('0x6893a6ee8df79b0f5d64a180cd1ef35d030f3e296a5361cf04d02ce720d32ec5'),
  'r': 4487286261793418179817841024889747115779324305375823110249149479905075174044,
  'rawTransaction': HexBytes('0xf86a8086d55698372431831e848.......216a6f3ee2c051fea6a0428'), 
  's': 30785525769477805655994251009256770582792548537338581640010273753578382951464,
  'v': 37
}
```

一切就绪，调用`eth_sendRawTransaction`接口来发送裸交易请求，这对应于`web3.py` 的`sendRawTransaction()`方法：

```python
tx_hash = w3.eth.sendRawTransaction(signed.rawTransaction)
receipt = w3.eth.waitForTransactionReceipt(tx_hash)
```

## ERC20代币规范

目前几乎所有用于ICO筹集资金的代币，都是基于同样的技术：以太坊ERC-20标准，这些代币实际上就是实现了ERC20标准的智能合约。

一个ERC20代币合约应当实现如下标准的接口，当然你也可以根据自己的实际需要补充额外的接口：

```python
contract ERC20 {
   function totalSupply() constant returns (uint theTotalSupply);
   function balanceOf(address _owner) constant returns (uint balance);
   function transfer(address _to, uint _value) returns (bool success);
   function transferFrom(address _from, address _to, uint _value) returns (bool success);
   function approve(address _spender, uint _value) returns (bool success);
   function allowance(address _owner, address _spender) constant returns (uint remaining);
   event Transfer(address indexed _from, address indexed _to, uint _value);
   event Approval(address indexed _owner, address indexed _spender, uint _value);
}
```

**totalSupply()** 

该函数应当返回流通中的代币供给总量。比如你准备为自己的网站发行100万个代币。

**balanceOf()**

该函数应当返回指定账户地址的代币余额。

**approve()**

使用该函数进行授权，被授权的账户可以调用账户的名义进行转账。

**transfer()**

该函数让调用账户进行转账操作，将指定数量的代币发送到另一个账户。

**transferFrom()**

该函数允许第三方进行转账操作，转出账户必须之前已经调用**approve()**方法授权给调用账户。

**Transfer事件**

每次转账成功后都必须触发该事件，参数为：转出账户、转入账户和转账代币数量。

**Approval事件**

每次进行授权，都必须触发该事件。参数为：授权人、被授权人和授权额度。

除了以上必须实现的接口，ERC20还约定了几个可选的状态，以便钱包或 其他应用可以更好的标识代币：

- **name** ： 代币名称。例如：`HAPPY COIN`。
- **symbol** ： 代币符号。例如：`HAPY` ，在钱包或交易所展示这个名字。
- **decimals** ：小数位数。默认值为18。钱包应用会使用这个参数。例如 假设你的代币小数位数设置为2，那么1002个代币在钱包里就显示为10.02了。

## 代币合约状态设计

智能合约的设计核心是状态的设计，然后再围绕状态设计相应的操作。代币合约也不例外。

首先我们需要有一个状态来记录代币发行总量，通常这个总量在合约部署的 时候就固定下来了，不过你也可以定义额外的非标接口来操作这个状态， 例如增发（`Secondary Coin Offering`）：

![erc20 supply state](http://xc.hubwiz.com/class/5b40462cc02e6b6a59171de4/img/erc20-state-supply.png)

在solidity中，我们可以使用一个uint256类型的变量来记录发行总量：

```
uint256 totalSupply;
```

接下来我们还需要有一个状态来保存所有账户的代币余额：

![erc20 balance state](http://xc.hubwiz.com/class/5b40462cc02e6b6a59171de4/img/erc20-state-balance.png)

在solidity中，可以使用一个从账户地址到整数（表示余额）的映射表来表示这个状态：

```
mapping(address => uint256) balances;
```

最后一个重要的状态是授权关系，我们需要记录三个信息：授权账户、被授权账户和授权额度：

![erc20 allow state](http://xc.hubwiz.com/class/5b40462cc02e6b6a59171de4/img/erc20-state-allow.png)

```
mapping (address => mapping (address => uint256)) public allowed;
```

## 代币合约方法实现

```
constructor(
    uint256 _initialAmount,
    string _tokenName,
    uint8 _decimalUnits,
    string _tokenSymbol
) public {
    balances[msg.sender] = _initialAmount;               
    totalSupply = _initialAmount;                        
    name = _tokenName;                                   
    decimals = _decimalUnits;                            
    symbol = _tokenSymbol;                               
}
```

构造函数保存了传入四个参数：初识发行总量、代币名称、小数点位数和代币符号。 在上面的实现中，**部署合约的账户在开始时将持有所有的代币**。

**transfer(to,value)**

容易理解，`transfer()`函数操作的状态就是balances。实现逻辑很直白，代码如下：

```
function transfer(address _to, uint256 _value) public returns (bool success) {
    require(balances[msg.sender] >= _value);
    balances[msg.sender] -= _value;
    balances[_to] += _value;
    emit Transfer(msg.sender, _to, _value); 
    return true;
}
```

由于`transfer()`是从调用账户转出代币，因此首先需要检查调用账户的代币余额是否足够。 接下来就可以分别调整双方的账户余额，然后触发`Transfer`事件即可。

**approve(spender,value)**

`approve()`函数操作的状态是allowed。实现逻辑同样直白，上代码：

```
function approve(address _spender, uint256 _value) public returns (bool success) {
    allowed[msg.sender][_spender] = _value;
    emit Approval(msg.sender, _spender, _value); 
    return true;
}
```

修改allowed映射表之后，触发`Approval`事件即可。

**transferFrom(from,to,value)**

`transferFrom()`方法的逻辑相对复杂一点，它需要同时操作balances状态和allowed状态：

```
function transferFrom(address _from, address _to, uint256 _value) public returns (bool success) {
    uint256 allowance = allowed[_from][msg.sender];
    require(balances[_from] >= _value && allowance >= _value);
    balances[_to] += _value;
    balances[_from] -= _value;
    if (allowance < MAX_UINT256) {
        allowed[_from][msg.sender] -= _value;
    }
    emit Transfer(_from, _to, _value); //solhint-disable-line indent, no-unused-vars
    return true;
}
```

代码首先查看allowed状态来确定调用账户是否得到转出账户的授权以及授权额度是否足够 本次转账。然后还要检查转出账户的余额是否足够本次转账。这些条件满足以后，直接调整 balances状态中转出账户和转入账户的余额，同时调整allowed状态中响应的授权额度。最后 触发`Transfer`事件即可。

**balanceOf(owner)**

`balanceOf()`方法只是查询balances状态，因此它是一个不消耗gas的`view`函数：

```
function balanceOf(address _owner) public view returns (uint256 balance) {
    return balances[_owner];
}
```

**allowance(owner,spender)**

`allowance()`方法查询账户对的授权额度，显然，它也是一个不修改状态的`view`函数：

```
function allowance(address _owner, address _spender) public view returns (uint256 remaining) {
    return allowed[_owner][_spender];
}
```

## 编译代币合约

为了在Python代码中与合约交互，我们需要先编译solidity编写的合约，以便得到 EVM字节码和二进制应用接口（`ABI`）。

字节码是最终运行在以太坊虚拟机中的代码，看起来就是这样：

```
608060405234801561001057600080fd5b5060405...
```

每两个字符表示1个字节，就像PC里的机器码，以太坊的字节码对应着以太坊虚拟机的操作码 —— 字节码就是最终在以太坊的EVM上运行的合约代码，我们在部署合约的时候需要用到它。

而ABI则是描述合约接口的一个JSON对象，用来在其他开发语言中调用合约。ABI 描述了合约中的每个方法、状态与事件的语言特性。例如，对于代币合约中的 `transfer()`方法，在ABI中描述如下：

![abi](http://xc.hubwiz.com/class/5b40462cc02e6b6a59171de4/img/abi.png)

ABI提供的丰富信息是实现其他语言绑定的关键资料。任何时候与合约进行交互， 都需要持有合约的ABI信息。

solidity的官方编译器`solc`是一个命令行程序，根据运行选项的不同会有 不一样的行为。例如，下面的使用`--bin`和`--abi`选项要求solc编译器同时生成字节码文件和ABI文件，并输出到`build`目录：

```
~$ mkdir -p contract/build
~$ solc contract/EzToken.sol --bin --abi \
                   --optimize --overwrite \
                   -o contract/build/
```

编译成功后，将在build目录中得到两个文件，这是我们下一步工作的基础：

```
~$ ls contract/build
EzToken.abi EzToken.bin
```

## 使用Python编译合约

如果你追求纯粹，希望整个开发流水线都使用Python，而不借助于bash脚本， 那么也可以借助于`py-solc`这个包在Python脚本中编译合约。

例如，下面的代码载入`EzToken.sol`源代码，然后编译：

```python
import solc

with open('./contract/EzToken.sol','r') as f:
  source = f.read()

compiled = solc.compile_source(source)
```

编译结果是一个`Dict`，我们感兴趣的abi和字节码在`<stdin:EzToken>`下， 为了后续部署和调用合约，我们这两部分内容保存下来：

```python
root = compiled['<stdin>:EzToken']

with open('./contract/build/EzToken.abi','w') as f:
  json.dump(root['abi'],f)

with open('./contract/build/EzToken.bin','w') as f:
  f.write(root['bin'])
```

## 合约部署原理

在以太坊中，合约的部署也是一个交易，只是与普通的交易相比，部署交易需要把合约的字节码和编码后构造函数参数放在payload的`data`字段里，然后 通过`eth_sendTransaction`或`eth_sendRawTransaction`调用提交给节点：

![deploy tx data](http://xc.hubwiz.com/class/5b40462cc02e6b6a59171de4/img/deploy-tx-data.png)

构造函数的参数需要首先经过abi编码才可以附加在字节码之后提交。例如， 对于EzToken合约，如果我们希望发行10亿枚HAPY币，其编码后的数据布局示意如下：

![deploy tx abi](http://xc.hubwiz.com/class/5b40462cc02e6b6a59171de4/img/deploy-tx-abi.png)

编码后的数据按32字节对齐，在上图中每一行都是32个字节。

第一个参数`_initialAmount`是静态的`uint256`类型，因此直接补齐为32字节， 占据第一行；第二个参数`_tokenName`是动态的`string`类型，因此第二行将 保存该参数值的实际位置，即上图中指向的`0x0080`位置；第三个参数`_decimalUnit` 是静态的`uint8`类型，因此也直接补齐为32字节，占据第三行；第四个参数`_tokenSymbol` 同样是动态的`string`类型，因此第四行保存的是该参数值的实际保存位置，即 `0x00c0`位置。

一个`string`类型的数据，在编码时首先使用32字节保存其长度，例如上图中 `0x0080`位置，然后在接下来按32字节对齐填写实际的内容，即`HAPPY COIN`。

这看起来很麻烦，好在`eth_abi`包提供了现成的方法可以使用，我们只需要理解 上述编码原理就可以了。

我们可以使用`eth_abi`包的`encode_abi()`方法来对构造函数的参数进行编码，例如

```python
encoded_params = encode_abi(['uint256','string','uint8','string'],[1000000000,'HAPPY COIN',0,'HAPY']))
```

然后拼接到合约字节码的后面，并将结果转换为16进制字符串：

```python
with open('./contract/build/EzToken.bin') as f:
  bin = f.read()
tx_data = to_hex(HexBytes(bin) + encoded_params)
```

现在可以提交部署交易了，例如使用第一个节点账户提交：

```python
payload = {
  'from': accounts[0],
  'data': tx_data
}
tx_hash = w3.eth.sendTransaction(payload)
```

需要指出的是，==**合约部署交易载荷的`to`字段必须留空**==。

如果交易成功，在部署交易的收据里，将包含合约的部署地址：

```python
receipt = w3.eth.waitForTransactionReceipt(tx_hash)
print(receipt.contractAddress)
```

## 使用合约类部署代币合约

有了合约的字节码和ABI，更简单的办法是使用`Eth`类的`contract()`方法来部署合约到链上。

首先载入之前保存的字节码和ABI：

```python
with open('./contract/build/EzToken.abi','r') as f:
  abi = json.load(f)

with open('./contract/build/Eztoken.bin','r') as f:
  bin = f.read()
```

然后调用`contract()`方法创建一个合约工厂：

```python
factory = w3.eth.contract(abi=abi,bytecode=bin)
```

然后调用合约工厂的`constructor()`方法来执行对合约构造函数及调用参数的编码：

```python
wrapper = factory.constructor(1000000,'HAPPY COIN',0,'HAPY')
```

传入的4个参数对应于合约的构造函数的参数列表，因此上面的代码意味着我们 希望发行1000000枚HAPY币。在`wrapper`的`data_in_transaction`中保存有编码后的数据，我们可以在payload的`data`字段使用这个数据。例如：

```python
payload = {
  'from': accounts[0],
  'data': wrapper.data_intransaction
}
txhash = w3.eth.sendTransaction(payload)
```

不过`wrapper`也提供了封装好的交易发送方法`transact()`，我们直接调用即可。 例如，下面的代码使用节点第一个账户来部署合约：

```python
tx_hash = wrapper.transact({'from': accounts[0]})
```

由于部署账户是节点旳第1个账户，该账户将持有初始发行的全部代币。

接下来要等待交易收据，因为在收据中有合约部署的具体地址：

```python
receipt = w3.eth.waitForTransactionReceipt(tx_hash)
print('deployed address => {0}'.format(receipt.contractAddress))
```

在合约部署后，最重要的一件事，是把地址抄下来，或者记录到一个文件中，否则接下来没法访问合约了。

```python
with open('./contract/build/EzToken.addr','w') as f:
  f.write(receipt.contractAddress)
```

## 合约方法执行原理

当合约部署上链后，我们就可以通过以下调用来执行合约方法了：

- eth_sendTransaction - 执行合约中修改状态的方法，节点负责签名
- eth_sendRawTransaction - 执行合约中修改状态的方法，应用负责签名
- eth_call - 执行合约中的状态只读方法

类似于合约的部署交易，我们首先需要将函数选择符及参数值列表组合为16进制字符串：

![call data](http://xc.hubwiz.com/class/5b40462cc02e6b6a59171de4/img/call-data.png)

函数选择符就是函数名及参数列表的Keccak哈希的头4个字节，例如， 下面的代码计算一个`balanceOf()`调用的选择符：

```python
func_hash = keccak('balanceOf(address)')
selector = func_hash[:4]
```

同样，我们使用`eth_abi`包的`encode_abi()`方法来编码函数调用参数：

```python
encode_params = encode_abi(['address'],[accounts[0]])
```

然后拼接函数选择符和编码参数，并转化为16进制字符串：

```python
data = to_hex(HexBytes(selector) + encoded_params)
```

由于合约`balanceOf()`方法并不修改合约的状态，因此我们使用`eth_call`调用：

```python
payload = {
  'from': accounts[0],
  'to': contract_address,
  'data': data
}
ret = w3.eth.call(payload)
```

返回值是abi编码的，因此我们需要使用`eth_abi`包的`decode_single()`来反过来解码：

```python
balance = decode_single('uint256',ret)
```

前一节我们从原理上理解了如何调用合约的方法。不过`web3.py`提供了封装好 的接口，我们不必这么麻烦。

首先还是老样子，要访问一个已经部署在链上的合约，先载入它的ABI和部署地址。

```python
with open('./contract/build/EzToken.abi') as f:
  abi = json.load(f)
with open('./contract/build/EzToken.addr') as f:
  addr = f.read()
```

然后构建合约对象，设置其部署地址和ABI：

```python
token = w3.eth.contract(address=addr,abi=abi)
```

接下来就可以调用合约的方法了。这分两种情况。

如果是那些修改合约状态的交易函数，比如`transfer()`， 使用方法包装对象的`transact()`方法，这将映射到`eth_sendTransaction` 调用。例如，向第2个节点账户转一些代币：

```python
wrapper = token.functions.transfer(accounts[1],10)
txhash = wrapper.transact({'from':accounts[0]})
w3.eth.waitForTransactionReceipt(txhash)
```

交易函数返回的总是交易哈希，我们总是应该等待交易收据。

注意，因为我们使用使用第1个节点账户部署的合约，因此现在它持有全部的代币。

如果是要调用合约中的只读函数，例如`balanceOf()`，那么使用方法包装对象的`call()`方法，这将映射到`eth_call`调用。例如，获取第1个节点账户的代币余额：

```python
wrapper = token.functions.balanceOf(accounts[0])
balance = wrapper.call()
```

## 通知机制

通知机制对任何应用开发都很重要，因为它提供了另外一个方向的调用能力。 以太坊也不例外，它的通知机制增强了智能合约与外部应用之间的沟通能力。

以太坊的通知机制是建立在日志基础之上。例如，如果智能合约触发了一个事件，那么该事件将写入以太坊日志；如果外部应用订阅了这个事件，那么应用就可以在日志中出现该事件后及时检测到：

![notification](http://xc.hubwiz.com/class/5b40462cc02e6b6a59171de4/img/notification.png)

需要指出的是，以太坊的通知机制不是推（`Push`）模式，而是需要外部应用周期性轮询的拉（`Pull`）模式。外部应用通过在节点中创建过滤器来订阅感兴趣 的日志，之后则通过检测该过滤器的变化获得最新的日志。

在这一部分的课程中，我们将学习以下内容：

- 使用块过滤器监听新块生成事件和新交易事件
- 使用待定交易过滤器监听待定交易事件
- 使用主题过滤器监听合约事件
- 解析合约事件产生的日志

## 监听新块事件

首先使用`latest`参数调用`Eth`类的`filter()`方法获取一个块过滤器对象， 该方法在内部执行[eth_newBlockFilter](http://cw.hubwiz.com/card/c/ethereum-json-rpc-api/1/3/34/)调用 来创建一个新块过滤器：

```python
filter = w3.eth.filter('latest')
```

周期调用其`get_new_entries()`方法即可获取新的日志。该方法在内部则执行 [eth_getFilterChanges](http://cw.hubwiz.com/card/c/ethereum-json-rpc-api/1/3/37/)调用：

```python
while True:
  for bk_hash in filter.get_new_entries():
    print(bk_hash)
  time.sleep(2)
```

对于块过滤器，`eth_getFilterChanges`调用将返回块哈希值的数组。 如果你希望获取块的详细信息，可以使用`Eth`类的`getBlock()`方法来获取块的 详细信息。例如，下面的代码获取并显示块的时间戳：

```python
block = w3.eth.getBlock(blk_hash)
print('timestamp => {0}'.format(block.timestamp))
```

`getBlock()`方法在参数是一个块哈希时，将通过 [eth_getBlockByHash](http://cw.hubwiz.com/card/c/ethereum-json-rpc-api/1/3/21/) 调用来获取块的详细信息。该方法返回一个`AttributeDict`对象，因此可以使用`block.timestamp` 或`block['timestamp']`来获取属性值。

## 监听新交易事件

要监听新的确认交易，也是使用块过滤器。其过程如下：

![tx filter](http://xc.hubwiz.com/class/5b40462cc02e6b6a59171de4/img/tx-filter.png)

实际上它的实现机制，就是在捕捉到新块事件后，进一步获取新块的详细信息。

因此和监听新块事件一样，首先使用`latest`参数调用`filter()`创建一个 块过滤器：

```python
filter = w3.eth.filter('latest')
```

然后调用周期性调用过滤器的`get_new_entries()`方法进行检查。 对于该调用返回的每一个块哈希，进一步调用`getBlock()`方法获取块内交易信息：

```python
while True:
  for bk_hash in filter.get_new_entries():
    block = w3.eth.getBlock(bk_hash,True)
    for tx in block.transactions:
      print('tx data => {0}'.format(tx.input))
  time.sleep(2)
```

`getBlock()`方法的第二个参数用来声明是否需要返回完整的交易对象， 如果设置为false将仅返回交易的哈希。

## 监听待交易事件

待定交易指那些提交给节点但还未被网络确认的交易，因此它不会包含在区块中。 使用待定交易过滤器来监听新待定交易事件。其过程如下：

![pending tx filter](http://xc.hubwiz.com/class/5b40462cc02e6b6a59171de4/img/pending-tx-filter.png)

使用参数`pending`来调用`filter()`方法，即可创建一个待定交易过滤器。该方法 内部执行[eth_newPendingTransactionFilter](http://cw.hubwiz.com/card/c/ethereum-json-rpc-api/1/3/35/) 调用：

```
filter = w3.eth.filter('pending')
```

然后同样是通过周期性地调用`get_new_entries()`方法来获取最新的待定交易信息。 例如，下面的代码将打印新出现的待定交易哈希：

```
while True:
  for tx_hash in filter.get_new_entries():
    print('pending tx hash => {0}'.format(tx_hash))

  time.sleep(2)
```

对于待定交易过滤器，`get_new_entries()`方法返回的结果是自上次 调用之后新产生的一组待定交易的哈希。可以使用`getTransaction()`方法来获取交易的 详细信息，该方法内部执行[eth_getTransactionByHash](http://cw.hubwiz.com/card/c/ethereum-json-rpc-api/1/3/23/) 调用：

```
tx = w3.eth.getTransaction(tx_hash)
print('tx data => {0}'.format(tx.input))
```

## 监听合约事件

合约事件的监听是通过主题过滤器实现的，其过程如下：

![event filter](http://xc.hubwiz.com/class/5b40462cc02e6b6a59171de4/img/event-filter.png)

首先使用一个`Dict`对象作为参数调用`filter()`方法来创建一个主题过滤器， 该方法在内部执行[eth_newFilter](http://cw.hubwiz.com/card/c/ethereum-json-rpc-api/1/3/33/)调用 ，并返回LogFilter对象：

```
filter = w3.eth.filter({})
```

`eth_newFilter`调用可以接收一个`Dict`类型的选项参数，来过滤监听的日志类型。该选项 可以指定一些监听过滤条件，例如要监听的合约地址等。 不过在上面的代码中，我们使用一个空的字典，没有设置这些参数，这意味着我们将监听全部日志。

在创建主题过滤器之后，同样使用过滤器的`get_new_entries()`方法 进行周期性的检查，看是否有新的日志产生。该调用将返回自上次调用之后的所有新日志的数组：

```
while True:
  for log in filter.get_new_entries():
    pprint(log)

  time.sleep(2)
```

每一个日志在`web3.py`中被映射到一个`AttributeDict`对象，其中的`topics`和`data`中 包含了我们感兴趣的数据。但显然，捕捉到的日志还需要进一步解码才可以得到事件的参数：

![log data](http://xc.hubwiz.com/class/5b40462cc02e6b6a59171de4/img/log-data.png)

## 使用主题过滤日志

当我们创建主题过滤器时，以及查看日志数据时，都接触到了一个概念：主题。 以太坊利用主题来区别不同的事件。

主题实际上就是事件的Keccak哈希签名。例如，对于代币合约的`Transfer` 事件，它的签名计算如下：

![topic](http://xc.hubwiz.com/class/5b40462cc02e6b6a59171de4/img/topic.png)

例如，下面的代码计算`Transfer`事件的签名：

```
topic = to_hex(keccak('Transfer(address,address,uint256)'))
```

在创建主题过滤器时，就可以用这个主题来限定监听行为了：

```
opts = {
  'topics': [topic]
}
filter = w3.eth.filter(opts)
```

现在，只有`Transfer`事件才会触发日志了。

## 解码日志数据

日志中的`topics`和`data`字段，包含了我们感兴趣的数据。

`topics`的第一个成员，总是相应事件的签名。而其他的成员则有序对应着事件 参数中的有索引（`indexed`）参数：

![log decode](http://xc.hubwiz.com/class/5b40462cc02e6b6a59171de4/img/log-decode.png)

因此我们首先依次解码参数即可：

```
from eth_abi import decode_single

from_account = decode_single('address',bytes(log['topics'][1]))
to_account = decode_single('address',bytes(log['topics'][2]))
value = decode_single('uint256',to_bytes(hexstr=log['data']))
```

由于`Transfer`事件的value参数不是有索引参数，因此它的值在日志的`data`字段中。 如果存在多个无索引参数时，则`data`字段是这些参数按32字节顺序拼接的结果：

通过前面两节的学习，我们已经理解了如何使用主题来监听特定的事件，也理解 了如何解码日志中的事件参数。不过实际上`web3.py`已经做了很好的封装，在 实际使用时我们不必如此麻烦。

当我们创建`Contract`对象时，会自动根据合约的abi生成一组`ContractEvent`类 的实例保存在合约对象的`events`属性中。例如，对于`Transfer`事件，我们 可以通过合约实例的`events.Transfer`访问这个合约事件对象。

合约事件对象封装了abi编解码的繁琐环节，我们可以直接利用它的`createFilter()` 方法来创建一个`LogFilter`对象：

```
filter = token.events.Transfer.createFilter(fromBlock=0)
```

同样，定期使用这个主题过滤器的`get_new_entries()`方法来读取新的合约日志：

```
while True:
  for event in filter.get_new_entries():
    print(event.event,event.args)

  time.sleep(2)
```

从上面的代码容易看到，`LogFilter`已经自动帮我们进行了事件主题过滤和 参数解码工作，`get_new_entries()`方法返回的不再是原始的日志，而是解析 过的事件，其`event`属性表示事件名，`args`属性则包含了事件触发时的参数值。

> 由于ganache-cli一个已知的bug，它会自动将地址转化为小写字符，因此使用一个 具有混合大小写字符的校验和地址建立的过滤器将无法正常触发。为此我们修改了 `web3.utils.valitation`模块中的`validation()`方法，以便让它可以接受小写形式 的地址 —— 这不是web3.py的问题，是ganache-cli的bug。


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
~$ curl -X POST http://localhost:845  -d '{
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
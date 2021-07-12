# -*- coding = utf-8 -*-
# @Time:2021-07-12 17:55
# @Author:Freak
# @File:build-contract.py
# @SoftWare:PyCharm
from solc import compile_source
import json


def build_artifacts(contract):
    fn_src = './contract/{0}.sol'.format(contract)
    fn_abi = './contract/build/{0}.abi'.format(contract)
    fn_bin = './contract/build/{0}.bin'.format(contract)

    with open(fn_src, 'r') as f:
        source = f.read()

    compiled = compile_source(source)
    root = compiled['<stdin>:{0}'.format(contract)]

    with open(fn_abi, 'w') as f:
        json.dump(root['abi'], f)

    with open(fn_bin, 'w') as f:
        f.write(root['bin'])

    print('done.')


build_artifacts('EzToken')

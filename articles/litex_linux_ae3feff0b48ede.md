---
title: "LiteXではじめる自作SoC(3) Linuxの動くSoCを自作する"
type: "tech" # tech: 技術記事 / idea: アイデア
topics: ["FPGA", "Linux"]
published: false
---

https://zenn.dev/tanakmura/articles/litex2_d64c6905903aff 第2回

# Linux の動く SoC を自作する
とりあえずサクっとLinuxが動く環境を作ろう。

LiteXは何か色々便利なものが集まっていて、コマンドを実行するだけで色々な環境が作れるが、

https://github.com/litex-hub/linux-on-litex-vexriscv

Linuxが動くSoCを作るようのスクリプトがあって、これを実行するだけで作れるようになっている。(すごい)

とりあえず動くというレベルではなくて、Arty の場合は、MMUが付いたSMP(マルチコア)のRISC-Vが100MHzで動いて、DRAMを使えて、Ethernet経由のTCP/IPで通信できて、QSPI Flash データが保存できる、ちゃんとしたLinuxが動く。Pmod を付ければ SD Card も使える。


手元で試した限りだと、SymbiFlowで作るとJTAGが使えないのと、なんかUARTが動かない気がしたので今回はVivadoでやっていく。

Vivadoを使う場合は、vivadoコマンドにパスが通っていればいい。通常Vivadoを使う時と同様、

    $ source /opt/Xilinx/Vivado/20xx.x/settings64.sh
    
settings64.sh を実行しておく。


今のmake.pyは実行すると失敗するので https://github.com/litex-hub/linux-on-litex-vexriscv/pull/242/files この変更を入れておく。


あとはコマンドを実行するだけだ。

    (base) $ conda activate xc7
    (xc7) $ git clone https://github.com/litex-hub/linux-on-litex-vexriscv
    (xc7) $ cd linux-on-litex-vexriscv
    (xc7) $ ./make.py --board=arty --cpu-count=2 --build

`--cpu-count` を変えると、コア数を変えられる。

`--build` を付けると、vivadoを起動して合成してbitstreamを書き出すところまでやってくれる。


(あ、今見たらVexRISCVを作るためにsbtがいるっぽい。sbtもインストールする必要がある。私は詳しくないのでうまいことなんとかやっておいて。あとdtcも必要だった。前回のところに追記しておいた)


https://github.com/litex-hub/linux-on-litex-vexriscv/issues/164

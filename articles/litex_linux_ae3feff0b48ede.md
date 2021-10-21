---
title: "LiteXではじめる自作SoC(3) Linuxの動くSoCを自作する"
type: "tech" # tech: 技術記事 / idea: アイデア
topics: ["FPGA", "Linux"]
published: true
---

https://zenn.dev/tanakmura/articles/litex2_d64c6905903aff 第2回

# Linux の動く SoC を自作する
とりあえずサクっとLinuxが動く環境を作ろう。

LiteXは何か色々便利なものが集まっていて、コマンドを実行するだけで色々な環境が作れるが、

https://github.com/litex-hub/linux-on-litex-vexriscv

Linuxが動くSoCを作るようのスクリプトがあって、これを実行するだけで作れるようになっている。スクリプトを動かすだけでLinuxが動くSoCが作れるのだ(大事なことなので2回書いた)

とりあえず動くというレベルではなくて、Arty の場合は、MMUが付いたSMP(マルチコア)のRISC-Vが100MHzで動いて、DRAMを使えて、Ethernet経由のTCP/IPで通信できて、QSPI Flash にデータが保存できる、ちゃんとしたLinuxが動く。Pmod を付ければ SD Card も使える(私は確認してないが)。


手元で試した限りだと、そのままではSymbiFlowで作るといくつか機能が使えなくて合成に失敗するのと、なんか100MHzが動かない気がしたので今回はVivadoでやっていく。

(SymbiFlow で試したい人は、make.py に https://github.com/litex-hub/litex-boards/issues/276#issuecomment-948667406 この diff を入れればできる気がする)



Vivadoを使う場合は、vivadoコマンドにパスが通っていればいい。通常Vivadoを使う時と同様、

    $ source /opt/Xilinx/Vivado/20xx.x/settings64.sh

settings64.sh を実行しておく。


あとはコマンドを実行するだけだ。

    (base) $ conda activate xc7
    (xc7) $ git clone https://github.com/litex-hub/linux-on-litex-vexriscv
    (xc7) $ cd linux-on-litex-vexriscv
    (xc7) $ ./make.py --board=arty --build

`--build` を付けると、vivadoを起動して合成してbitstreamを書き出すところまでやってくれる。SymbiFlowで合成する場合は、`--toolchain=symbiflow`を付ける。


(あ、今見たらVexRISCVを作るためにsbtがいるっぽい。sbtもインストールする必要がある。私は詳しくないのでうまいことなんとかやっておいて。あとdtcも必要だった。前回のところに追記しておいた)


しばらく待てば、FPGAに入れるbitstreamが`build/arty/gateware/arty.bit`に作られる。

このbitstreamには、RISC-V 用の初期化プログラム(litexではbiosと呼んでいるので、以後biosと呼ぶ)が含まれていて、このbitstreamをFPGAに書けば、ブートに必要な初期化プログラムがRISC-V上で動くようになっている。

bios は、UART に出力を出すので、とりあえず bitstream を書いて、UART から調子を見てみよう。

bitstream を書くのは、OpenOCD を使うが、make.py は OpenOCD を起動してbitstreamを書くところまでやってくれる。さきほどは、`--build` を指定したが、これを `--load` に変えるだけだ。(OpenOCDのコマンドを思い出す必要がない)

    (xc7) $ ./make.py --board=arty --load

うまくいっていれば、Arty が起動して、LEDが煌めく様子が確認できるだろう。


`--load`は電源が切れるまでの一時的なものになる。`--flash`すれば、bitstreamがSPI FLASHに書き込まれて、次回からは電源を入れるだけで起動するようになる。(注意:ArtyではSPIに書くためには多分ジャンパ14をはずす必要がある)

SPI FLASHの中身が消えてよければ、`--flash`でbitstreamを書いておこう。


    (xc7) $ ./make.py --board=arty --flash


何も変更しないで、Arty用にbitstreamを作った場合は、UARTは1e6Hz (1MHz) で起動する。これを見てみよう。

    $ sudo minicom -D /dev/ttyUSB1 -b 1000000

`/dev/ttyUSB1` のところは、環境によって変更する必要があるかもしれない。動いていなければ、/dev/ttyUSB0, /dev/ttyUSB2, ... と試してみてほしい。

接続できれば、Enterキーを押せば、litex の bios のプロンプトが表示される。

    litex> 

ここに help と入力すれば、bios で使えるコマンド一覧が表示される。

```
litex> help


LiteX BIOS, available commands:

help                     - Print this help
ident                    - Identifier of the system
crc                      - Compute CRC32 of a part of the address space
flush_cpu_dcache         - Flush CPU data cache
leds                     - Set Leds value

boot                     - Boot from Memory
reboot                   - Reboot
serialboot               - Boot from Serial (SFL)
netboot                  - Boot via Ethernet (TFTP)
sdcardboot               - Boot from SDCard

mem_list                 - List available memory regions
mem_read                 - Read address space
mem_write                - Write address space
mem_copy                 - Copy address space
mem_test                 - Test memory access
mem_speed                - Test memory speed

sdram_init               - Initialize SDRAM (Init + Calibration)
sdram_cal                - Calibrate SDRAM
sdram_test               - Test SDRAM
sdram_force_rdphase      - Force read phase
sdram_force_wrphase      - Force write phase
sdram_mr_write           - Write SDRAM Mode Register

mdio_write               - Write MDIO register
mdio_read                - Read MDIO register
mdio_dump                - Dump MDIO registers

sdcard_detect            - Detect SDCard
sdcard_init              - Initialize SDCard
sdcard_freq              - Set SDCard clock freq
sdcard_read              - Read SDCard block
sdcard_write             - Write SDCard block
```

とりあえずrebootしてみよう。litex の bios がDRAMやEthernetを初期化している様子が確認できる。


litex on Arty のbiosは、

- SDカード
- UART
- TFTP

からLinuxのカーネルを取得してブートする機能を持っている。UARTは遅いので、SDカードやTFTPからブートするほうがよいのだが、ハードウェアのセットアップが必要なので、ここはUARTからカーネルを起動しよう。

litex には、bios からのUART出力を監視して、適切なプロトコルでデータを送信する `litex_term` というツールがある。`minicom`のかわりにこの`litex_term`を使って、UARTを操作すれば、ブート途中で Linux kernel を送信し、そこからブートすることができる(`litex_term` を終了するときは、Ctrl-C を2回押す)。litexを正しくインストールしていれば、$HOME/.local/bin にインストールされている。

ロードするファイルは、`litex_term --images=<path>` で指定する。path には、ロードするファイルと、アドレスを記述したjsonを指定する。このjsonは、litexのbitstreamビルド時に、images 以下に出力されている。

```json
{
	"Image":       "0x40000000",
	"rv32.dtb":    "0x40ef0000",
	"rootfs.cpio": "0x41000000",
	"opensbi.bin": "0x40f00000"
}
```

rv32.dtb は、boot.json と同じディレクトリに出力されているものを使おう。

Imageは、Linux カーネル、rootfs.cpio はLinuxのinitrd、opensbi.binは…すいません、よくわからないです。

これらも、ソースからビルドしてもいいが、手順が長くなるので、prebuilt されたものを使うことにする。

https://github.com/litex-hub/linux-on-litex-vexriscv/issues/164

ここの、Linux と OpenSBI を取得すれば、Image, rootfs.cpio, opensbi.bin が手に入る。

これら5個のファイルを(boot.json, Image, rv32.dtb, rootfs.cpio, opensbi.bin)を、同じのディレクトリに置いて、次のようにlitex_termを起動して、litex のコンソールからrebootする。

```
 $ litex_term --images=boot.json --speed=1e6 /dev/ttyUSB1
 <ここでEnterを一回押してlitexのコンソールを改行する>
 litex> reboot

```

うまくいっていれば、reboot途中で、litex_term がブート時にシリアルから出てくるマジックシーケンス(`sL5DdSMmkekro`)を見つけて、ブート処理に割り込み、biosにboot.jsonで指定されたファイルをbiosに送信する。

1. bios がマジックシーケンスを出す
2. litex_term がそれを受け取ると、`--images` で指定されたjsonに書かれたファイルの中身をbiosに送る
3. biosは送られたファイルをDRAMに書きこむ
4. litex_term は送信が終わったらbootコマンドをbiosに送る
5. bios は送られたブートコマンドに従ってブートする


UARTは、1Mhzで動いていて遅いので、Linux + rootfs をロードするのはそれなりに時間がかかるので数分必要だが、うまくいけば、しばらくして、Linuxが起動するはずだ。


ただし、この状態だと、エラーが表示され続けて使いづらいかもしれない。SDカードが差さっていない場合、リトライし続けてしまうようだ。必要なければ、build/arty/arty.dtsを編集して、mmcのエントリを消したあと、dtcでarty.dtsをビルドして、SDカードを無効化したrv32.dtbを使おう。

また、make.py が生成する dts とビルド済みカーネルに含まれるイーサネットドライバが見るdtsでバージョンのずれがあって、そのままではイーサネットが使えないようだ。

arty.dtsに含まれる、`litex,rx-slots`、 `litex,tx-slots` を、`rx-fifo-depth`、`tx-fifo-depth`に書きかえておこう。これでイーサネットも使えるようになるはずだ。


以上の手順をまとめたスクリプトは以下になる。

https://github.com/tanakamura/litex-jisakusoc/blob/main/src/setup_linux_root/setup.sh

    $ ./setup.sh /usr/src/linux-linux-on-litex-vexriscv /dev/ttyUSB1
    $ # /usr/src/linux-linux-on-litex-vexriscv は make.py を実行したディレクトリ

のようにすれば、上の手順を実行した状態で、litex_termが起動する。このlitex_termのコンソールから、rebootコマンドなどを使って再起動すればLinuxが起動する。


Linux が起動したらあとは好きなように使おう。ここで使ったrootfsには最低限のものしか入っていないので、buildrootが使える人は、自分でrootfsを作ってみるのもよいかもしれない。


さて、簡単ではあったが、Litexを使ってLinuxが動くSoCを作る手順を見ていった。

ここで作ったSoCに含まれる部品は、全てソースが公開されていて、その気になれば起こった現象を自分で完全に解析することが可能なわけで、ブラックボックスがほとんど存在しないシステムが作れたと言えるだろう。


とは言っても、`make.py` を実行しただけでは、何が起こってるのかはあまりわからないかもしれない。

次回からは、自分でLitexを使ったスクリプトを作って、一個一個丹精込めてSoCを作っていってみよう。まず次はCPUからだ。(続く)

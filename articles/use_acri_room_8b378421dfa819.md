---
title: "ACRiルームのArty A7を使う"
emoji: "🐕"
type: "tech"
topics: ["FPGA"]
published: true
---
# ACRiルームのArty A7を使う

Artyを使ってみたいけど、FPGAのボードを買うほどではないかな、という人は、

https://gw.acri.c.titech.ac.jp/wp/

ACRiルームという施設の環境を使ってみるのもよいと思う。

ACRiルームの紹介はリンク先を読んでもらうのがいいと思うが、https://www.acri.c.titech.ac.jp/wp/ アダプティブコンピューティング研究推進体（ACRi） という団体が、

> 共通の FPGA 環境を利用することで開発成果もトラブルも簡単に共有できる環境の提供を目指しています。

ということを目的にして開設している施設、らしい。(なんで無料なのかはよくわからないが、学生さん達にはよい環境だと思う。社会人は自分でボードを買いましょう)

いまのところ、いつ見てもかなり空いているようなので、日中は必要なときに借りることができると思う(深夜はやっていない。多分火事の心配とかではないかな…)。


基本的な使いかたは、

まず、アカウントを申請する。

https://gw.acri.c.titech.ac.jp/wp/manual/apply-for-account

私の場合は、申請してから2日でアカウント作成のメールが来た。(9/5に申請して、9/7にメールが来た)

アカウントはWeb予約サービス用のアカウントと、Linuxログイン用の二つが作られる。IDは一緒だが、パスワードは異なるので注意。

アカウントができたら、

https://gw.acri.c.titech.ac.jp/wp/wp-login.php

予約システムにログインして、空いてる時間を予約する。これは予約サービス用のパスワードでログインする。
00:00-9:00 以外ならいつでも予約できる。Arty は16台存在していて、予約している人は多くて一人なので、予約取れないということはないと思う(2021/10/24に見たところ)

Arty ではなくて、もっとでかいAlveoもかなり空いていて、見ている限りだといつでも予約できる(！？)。

予約したら、ゲートウェイサーバー(gw.acri.c.titech.ac.jp)にssh接続する。これはLinuxログイン用のパスワードでログインする。

    $ # 手元のマシンから
    $ ssh gw.acri.c.titech.ac.jp

ゲートウェイサーバーにログインできたら、そこから vs001, vs002 などにログインする。これもLinuxログイン用のパスワードでログインする。

    $ # gw.acri.c.titech.ac.jp にログインしてから
    $ ssh vs002

これで Arty の繋がったPCが使える。Vivadoは、/tools/Xilinx/Vivado にインストールされていて、

    $ source /tools/Xilinx/Vivado/2021.1/settings64.sh
    $ vivado -mode tcl
    
    ****** Vivado v2021.1 (64-bit)
      **** SW Build 3247384 on Thu Jun 10 19:36:07 MDT 2021
      **** IP Build 3246043 on Fri Jun 11 00:30:35 MDT 2021
        ** Copyright 1986-2021 Xilinx, Inc. All Rights Reserved.
    
    Vivado% exit
    exit
    INFO: [Common 17-206] Exiting Vivado at Sun Oct 24 09:52:15 2021...

こんな感じで使える。

# make.py で作ったビットストリームをFPGAに入れる

https://zenn.dev/tanakmura/articles/litex_linux_ae3feff0b48ede.md

これで作ったbitstreamを使ってみよう。リンク先の解説では、`--board=arty` で作っていたが、Arty A7向けに作るときは`--board=arty_a7` にする。

    $ ./make.py --board=arty_a7 --build

(なお、いまのところ違いはないようなので、気にしなくていいかもしれない)


合成が終わると、`build/arty/gateware/arty.bit` に bitstream ができるのでなんとかしてACRiルームのサーバーにコピーする。

https://gw.acri.c.titech.ac.jp/wp/manual/how-to-reserve#toc6

この手順を参考にしてもよいし、sftp とかを使ってもよいと思う。(わからない初心者の人は恥ずかしがらないでコメントに書いてね)


このあと、正規の手順では、リモートデスクトップ接続してVivadoを起動するのだけど、めんどいので`xsdb` で書く。


```
 $ cat > program.tcl
connect
fpga "~/arty.bit"
 $ xsdb program.tcl
rlwrap: warning: your $TERM is 'screen.xterm-256color' but rlwrap couldn't find it in the terminfo database. Expect some problems.
attempting to launch hw_server
p
****** Xilinx hw_server v2021.1.0
  **** Build date : Jun  5 2021 at 17:46:51
    ** Copyright 1986-2021 Xilinx, Inc. All Rights Reserved.

INFO: hw_server application started
INFO: Use Ctrl-C to exit hw_server application

INFO: To connect to this hw_server instance use url: TCP:127.0.0.1:3121

called Tcl_FindHashEntry on deleted table
/tools/Xilinx/Vitis/2021.1/bin/loader: line 312: 26368 Aborted                 (core dumped) "$RDI_PROG" "$@"
```

`xsdb`を起動して、`connect`、`fpga "bitstream"`とすれば、指定したファイルをFPGAに書きこめる。


(最後にcore dumpしているが、まあXilinxのツールなので…)


成功していれば、UARTにlitexのbiosのコンソールが出るはずだ。ACRiルームのサーバーには`miniterm`が入っているのでこれを使ってuartを観察しよう。

```
$ miniterm --raw /dev/ttyUSB1 1000000
```
などすれば、FPGAのUARTに1MHzで接続できる。エスケープシーケンスが`miniterm`に取られないようにするために、`--raw`を付けておく。


```
$ miniterm --raw /dev/ttyUSB1 1000000
--- Miniterm on /dev/ttyUSB1  1000000,8,N,1 ---
--- Quit: Ctrl+] | Menu: Ctrl+T | Help: Ctrl+T followed by Ctrl+H ---
 BIOS CRC passed (2f3c13ed)

 Migen git sha1: 7507a2b
 LiteX git sha1: 8fa4de5e

--=============== SoC ==================--
CPU:            VexRiscv SMP-LINUX @ 80MHz
BUS:            WISHBONE 32-bit @ 4GiB
CSR:            32-bit data
ROM:            64KiB
SRAM:           8KiB
L2:             0KiB
FLASH:          16384KiB
SDRAM:          262144KiB 16-bit @ 640MT/s (CL-7 CWL-5)
...
```

`miniterm`を終了するには、`Ctrl + ]`を押す。


https://gw.acri.c.titech.ac.jp/wp/manual/gallery

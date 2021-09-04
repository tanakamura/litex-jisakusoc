---
title: "LiteXではじめる自作SoC(1)"
type: "tech"
topics: ["FPGA"]
published: true
---
# LiteX ではじめる自作SoC
最近は、RISC-V の誕生により、CPUコアは個人の趣味で作るのが難しくない時代になってきた。





さて、コンピュータというのは、CPUコアだけで動くものではなく、周辺のストレージ、ネットワーク、DRAM、USB、その他入出力装置が揃ってはじめて動くものだ。

CPUコアが個人で自由に作れるようになったら、その次のステップとして、周辺装置も含めた物体、つまり、SoC(System On Chip)を自分で作れるようになりたいと思うのは自然なことだろう。


__LiteX__ は、そういうSoCを自分で作ってみたいという人向けのツール、RTLなどが集まったプロジェクトだ。

公式のWikiによると、
https://github.com/enjoy-digital/litex/wiki/LiteX-for-Hardware-Engineers#what-is-litex

> LiteX is a Python "front-end" that generates Verilog netlists, and drives proprietary build "back-ends", such as Vivado or ISE, to create bitstreams ("gateware") for FPGAs.

などと書いてある。

初見だとなんかいまいちよくわからないとは思うが、自分で好きなようにパーツを組み立ててSoCを作れるようにするためのPythonライブラリみたいなものだ。何ができるかについては、これから(私に暇があれば)書いていく予定。(途中で飽きて書かないかもしれない)


このLiteXの良いところがもう一点あって、最初から SymbiFlow がサポートされている点だ。


最近では、オープンソースのツールだけでFPGAのビットストリームを作れるようにしようという活動があって、

- Yosys (色々サポートしてるSynthesizer)
- Project X-Ray (XilinxのFPGAビットストリームを解析するプロジェクト)
- VTR (PNR するツール。これは昔からある)

これらを組み合わせてかなり確実にXilinxのFPGAでもビットストリームを作れるようになってきている。(Quicklogic というところが作っているFPGAではこれらのオープンソースのツールが公式ツールとして採用されている)


SymbiFlow は、これらのツールを組み合わせてソフトウェアのプログラマでも使えるようにするための、"FPGA向けコンパイラのフロントエンド"みたいなツールだ。(実際、公式サイトでは、"GCC of FPGAs" というような表現をしている https://symbiflow.readthedocs.io/en/latest/introduction.html)


SymbiFlow を使えば、

https://github.com/SymbiFlow/symbiflow-examples/blob/master/common/Makefile#L52

このようなMakefileを書けば、ソフトウェアの開発ツールと同じような手応えでバリバリRTLを書いていけるようになる。(合成 + 配置 + 配線の時間はコンパイル時間より大分かかるが)

もうビバヨのボタンをマウスでポチポチしなくてええにゃ…


このフローは、ソフトウェアのプログラマにとって馴染み深く、git用のディレクトリ構成作るのに苦労しなくて(なんでこんな苦労しないといけないんですかねぇ…)使いやすというのはもちろんあるが、何よりも重要なのは、_一番下のRTLからそれを合成するツール、OS(Linux)、コンパイラ(GCC)の一番上のソフトウェアまで、全てがソース公開されている_という部分だろう。

これらを使ってコンピュータを作れば、いよいよコンピュータからブラックボックスを完全に無くしてしまうことが可能だ。(まあIOとか電源とかのアナログ部分はブラックボックス残るが)


今は、個人のPCでブラックボックスのないコンピュータシステムを作るための環境がそろった歴史上はじめてのタイミングなんではないだろうか。

色々といじって、コンピュータの全てを自分で操作してみよう。


次回インストール編




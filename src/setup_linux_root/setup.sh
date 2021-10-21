set -e -o pipefail

cd images

function extract() {
    local url=$1
    local contents=$2
    local basename=`basename $url`

    if ! test -r "$basename"
    then
        wget "$url"
    fi

    if ! test -r "$contents"
    then
        unzip "$basename"
    fi
}

if test "$1" == "" -o "$2" == ""
then
    echo 'usage: setup.sh <litex path> <ttyUSB path>'
    echo '  ex) setup.sh /usr/src/linux-linux-on-litex-vexriscv /dev/ttyUSB1'
    exit 1
fi

LITEX=$1
TTYUSB=$2
cp $LITEX/images/boot.json .


# arty.dts を編集して、SDカードがなくてもエラーにならない rv32.dtb を作る
# - sdcardを消す
# - 古いカーネルでも動くように ethernet のエントリを編集する
sed 's|compatible = "litex,mmc";||; s|litex,rx-slots|rx-fifo-depth|; s|litex,tx-slots|tx-fifo-depth|;' < $LITEX/build/arty/arty.dts > ./arty.dts

dtc arty.dts -o rv32.dtb

# カーネル + rootfs
LINUX_URL=https://github.com/litex-hub/linux-on-litex-vexriscv/files/6220823/linux_2021_03_29.zip

extract $LINUX_URL Image

# OpenSBI
OPENSBI_URL=https://github.com/litex-hub/linux-on-litex-vexriscv/files/5702162/opensbi_2020_12_15.zip

extract $OPENSBI_URL opensbi.bin

#echo "start tftp server"
#sudo in.tftpd -l -L  .

export PATH=$PATH:$HOME/.local/bin

# baudrate = 1MHz で litex_term を起動
litex_term --images=boot.json --speed=1e6 $TTYUSB

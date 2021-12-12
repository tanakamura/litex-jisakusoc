import argparse
from litex_boards.platforms import arty
from litex.soc.integration.builder import Builder, builder_args, builder_argdict
from litex.soc.integration.soc_core import SoCCore, soc_core_args, soc_core_argdict
from litex.soc.integration.soc import SoC,LiteXSoC
from litex.soc.integration.export import get_csr_csv, get_csr_json, get_csr_header
from litex.build.xilinx.vivado import vivado_build_args, vivado_build_argdict
from litex.soc.cores import uart

def main():
    parser = argparse.ArgumentParser(description="gentest")

    platform = arty.Platform(variant="a7-35", toolchain="symbiflow")
    x = SoC(platform,  sys_clk_freq=25e6)
    x.add_rom("boot_rom", 0, 4096, contents=[0,1,2,3])

    # uart
    x.submodules.uart_phy = uart.UARTPHY(x.platform.request('serial'), clk_freq=1e6, baudrate=115200)
    x.submodules.uart = uart.UART(phy=x.uart_phy)

    x.add_controller("ctrl")
    x.mem_map['rom'] = 0
    x.add_cpu('picorv32')

    x.build(run=False, build_name="mysoc")
    header = get_csr_json(x.csr_regions, x.constants)
    print(header)


if __name__ == "__main__":
    main()

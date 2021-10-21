from litex.soc.integration.soc import SoC
from litex.build.xilinx import XilinxPlatform

io = [
    ("clk100",    0, Pins("E3"), IOStandard("LVCMOS33")),
    ("cpu_reset", 0, Pins("C2"), IOStandard("LVCMOS33"))
    ]

class Platform(XilinxPlatform):
    def __init__(self, toolchain="symbiflow"):
        XilinxPlatform.__init__(self, "xc7a35ticsg324-1L", io, [], toolchain=toolchain)
    def do_finalize(self, fragment):
        XilinxPlatform.do_finalize(self, fragment)
        self.add_period_constraint(self.lookup_request("clk100", loose=True), 1e9/100e6)


def main():
    platform = arty.Platform(variant="a7-35", toolchain="symbiflow")
    x = SoC(platform,  sys_clk_freq=1e6)
    x.irq.enable()

    x.add_rom("boot_rom", 0, 4096, contents=[0,1,2,3])

    # uart
    x.submodules.uart_phy = uart.UARTPHY(x.platform.request('serial'), clk_freq=1e6, baudrate=115200)
    x.submodules.uart = uart.UART(phy=x.uart_phy)

    x.add_controller("ctrl")
    x.mem_map['rom'] = 0
    x.add_cpu('serv')

    x.irq.add("uart")

    from litex.soc.cores.timer import Timer
    x.submodules.timer0 = Timer()
    x.irq.add('timer0')

    # led
    # x.submodules.uartbone_phy = uart.UARTPHY(x.platform.request('serial'), 1e6, 115200)
    # x.submodules.uartbone = uart.UARTBone(phy=x.uartbone_phy, clk_freq=1e6)
    # x.bus.add_master(name="uartbone", master=x.uartbone.wishbone)


    #x.do_finalize()

    x.build(run=False, build_name="mysoc")
    json = get_csr_json(x.csr_regions, x.constants)
    print(json)


if __name__ == "__main__":
    main()

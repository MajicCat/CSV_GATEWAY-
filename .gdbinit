set remotetimeout 20
shell ./tools/common/OSX/dash -c "trap \\"\\" 2;"./tools/OpenOCD/OSX/openocd-all-brcm-libftdi" -f ./tools/OpenOCD/BCM9WCD1EVAL1.cfg -f ./tools/OpenOCD/stm32f4x.cfg -f ./tools/OpenOCD/stm32f4x_gdb_jtag.cfg -l build/openocd_log.txt &"

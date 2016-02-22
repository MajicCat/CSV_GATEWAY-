WICED_SDK_MAKEFILES           += ./WICED/platform/MCU/STM32F4xx/peripherals/libraries/libraries.mk ./libraries/daemons/DHCP_server/DHCP_server.mk ./libraries/utilities/crc/crc.mk ./libraries/utilities/ring_buffer/ring_buffer.mk ./WICED/platform/MCU/STM32F4xx/peripherals/peripherals.mk ./WICED/platform/GCC/GCC.mk ././WICED/network/NetX_Duo/WICED/WICED.mk ././WICED/network/NetX_Duo/WWD/WWD.mk ./libraries/crypto/micro-ecc/micro-ecc.mk ./WICED/security/BESL/crypto_open/crypto_open.mk ./libraries/utilities/base64/base64.mk ./libraries/utilities/TLV/TLV.mk ././libraries/drivers/bluetooth_le/BTE/BTE.mk ./libraries/daemons/DNS_redirect/DNS_redirect.mk ./libraries/daemons/simple_HTTP_server/simple_HTTP_server.mk ./libraries/utilities/linked_list/linked_list.mk ././WICED/platform/MCU/STM32F4xx/STM32F4xx.mk ./libraries/filesystems/wicedfs/wicedfs.mk ./WICED/network/NetX_Duo/NetX_Duo.mk ./libraries/protocols/DNS/DNS.mk ././WICED/security/BESL/BESL.mk ././WICED/WWD/WWD.mk ././WICED/RTOS/ThreadX/WICED/WICED.mk ././WICED/RTOS/ThreadX/WWD/WWD.mk ./libraries/inputs/gpio_button/gpio_button.mk ./libraries/drivers/spi_flash/spi_flash.mk ./libraries/daemons/bt_smartbridge/bt_smartbridge.mk ./libraries/drivers/bluetooth_le/bluetooth_le.mk ./libraries/daemons/Gedday/Gedday.mk ./libraries/protocols/SNTP/SNTP.mk ./libraries/daemons/device_configuration/device_configuration.mk ./libraries/daemons/HTTP_server/HTTP_server.mk ././WICED/WICED.mk ./WICED/RTOS/ThreadX/ThreadX.mk ./platforms/ISM43341_M4G_L44/ISM43341_M4G_L44.mk ./apps/demo/bt_smartbridge/bt_smartbridge.mk
TOOLCHAIN_NAME            := GCC
WICED_SDK_LDFLAGS             += -Wl,--gc-sections -Wl,-Os -Wl,--cref -mthumb -mcpu=cortex-m4 -Wl,-A,thumb -mlittle-endian -nostartfiles -Wl,--defsym,__STACKSIZE__=800 -L ./WICED/platform/MCU/STM32F4xx/GCC -L ./WICED/platform/MCU/STM32F4xx/GCC/STM32F415
RESOURCE_CFLAGS           += -mthumb -mcpu=cortex-m4 -mlittle-endian
WICED_SDK_LINK_SCRIPT         += ././WICED/platform/MCU/STM32F4xx/GCC/app_with_bootloader.ld
WICED_SDK_LINK_SCRIPT_CMD     += -Wl,-T ././WICED/platform/MCU/STM32F4xx/GCC/app_with_bootloader.ld
WICED_SDK_PREBUILT_LIBRARIES  += ./WICED/RTOS/ThreadX/ThreadX.ARM_CM4.release.a ./libraries/daemons/Gedday/Gedday.ThreadX.NetX_Duo.ARM_CM4.release.a ././WICED/security/BESL/BESL.ARM_CM4.release.a ./WICED/network/NetX_Duo/NetX_Duo.ThreadX.ARM_CM4.release.a ././libraries/drivers/bluetooth_le/BTE/BTE_LE.ThreadX.NetX_Duo.ARM_CM4.release.a
WICED_SDK_CERTIFICATES        += 
WICED_SDK_PRE_APP_BUILDS      += bootloader
WICED_SDK_DCT_LINK_SCRIPT     += ././WICED/platform/MCU/STM32F4xx/GCC/STM32F415/dct.ld
WICED_SDK_DCT_LINK_CMD        += -Wl,-T ././WICED/platform/MCU/STM32F4xx/GCC/STM32F415/dct.ld
WICED_SDK_APPLICATION_DCT     += ./apps/demo/bt_smartbridge/bt_smartbridge_dct.c
WICED_SDK_WIFI_CONFIG_DCT_H   += ./include/default_wifi_config_dct.h
WICED_SDK_BT_CONFIG_DCT_H     += ./include/default_bt_config_dct.h
WICED_SDK_LINK_FILES          +=                                        $(OUTPUT_DIR)/Modules/./WICED/platform/MCU/STM32F4xx/../../ARM_CM4/crt0_GCC.o $(OUTPUT_DIR)/Modules/./WICED/platform/MCU/STM32F4xx/../../ARM_CM4/hardfault_handler.o $(OUTPUT_DIR)/Modules/./WICED/platform/MCU/STM32F4xx/platform_vector_table.o                      $(OUTPUT_DIR)/Modules/WICED/platform/GCC/mem_newlib.o $(OUTPUT_DIR)/Modules/WICED/platform/GCC/stdio_newlib.o          
WICED_SDK_INCLUDES            +=                                                                             -I./WICED/platform/MCU/STM32F4xx/peripherals/libraries/. -I./WICED/platform/MCU/STM32F4xx/peripherals/libraries/inc -I./WICED/platform/MCU/STM32F4xx/peripherals/libraries/../../../ARM_CM4/CMSIS -I./libraries/daemons/DHCP_server/. -I./libraries/utilities/crc/. -I./libraries/utilities/ring_buffer/. -I./WICED/platform/MCU/STM32F4xx/peripherals/. -I./WICED/platform/GCC/. -I././WICED/network/NetX_Duo/WICED/. -I././WICED/network/NetX_Duo/WWD/. -I./libraries/crypto/micro-ecc/. -I./WICED/security/BESL/crypto_open/. -I./WICED/security/BESL/crypto_open/srp -I./libraries/utilities/base64/. -I./libraries/utilities/TLV/. -I././libraries/drivers/bluetooth_le/BTE/. -I././libraries/drivers/bluetooth_le/BTE/../include -I././libraries/drivers/bluetooth_le/BTE/WICED -I././libraries/drivers/bluetooth_le/BTE/Components/stack/include -I././libraries/drivers/bluetooth_le/BTE/Projects/bte/main -I./libraries/daemons/DNS_redirect/. -I./libraries/daemons/simple_HTTP_server/. -I./libraries/utilities/linked_list/. -I././WICED/platform/MCU/STM32F4xx/. -I././WICED/platform/MCU/STM32F4xx/.. -I././WICED/platform/MCU/STM32F4xx/../.. -I././WICED/platform/MCU/STM32F4xx/../../include -I././WICED/platform/MCU/STM32F4xx/../../ARM_CM4 -I././WICED/platform/MCU/STM32F4xx/../../ARM_CM4/CMSIS -I././WICED/platform/MCU/STM32F4xx/peripherals -I././WICED/platform/MCU/STM32F4xx/WAF -I././WICED/platform/MCU/STM32F4xx/../../../../../apps/waf/bootloader/ -I./libraries/filesystems/wicedfs/src -I./WICED/network/NetX_Duo/ver5.7_sp2 -I./WICED/network/NetX_Duo/WICED -I./libraries/protocols/DNS/. -I././WICED/security/BESL/host/WICED -I././WICED/security/BESL/TLS -I././WICED/security/BESL/crypto -I././WICED/security/BESL/WPS -I././WICED/security/BESL/include -I././WICED/security/BESL/P2P -I././WICED/security/BESL/crypto/homekit_srp -I././WICED/security/BESL/crypto/ed25519 -I././WICED/security/BESL/supplicant -I././WICED/security/BESL/DTLS -I././WICED/WWD/. -I././WICED/WWD/include -I././WICED/WWD/include/network -I././WICED/WWD/include/RTOS -I././WICED/WWD/internal/bus_protocols/SDIO -I././WICED/WWD/internal/chips/4334x -I././WICED/RTOS/ThreadX/WICED/. -I././WICED/RTOS/ThreadX/WWD/. -I././WICED/RTOS/ThreadX/WWD/CM3_CM4 -I./libraries/inputs/gpio_button/. -I./libraries/drivers/spi_flash/. -I./libraries/daemons/bt_smartbridge/. -I./libraries/daemons/bt_smartbridge/./include -I./libraries/daemons/bt_smartbridge/./internal -I./libraries/drivers/bluetooth_le/include -I./libraries/daemons/Gedday/. -I./libraries/protocols/SNTP/. -I./libraries/daemons/HTTP_server/. -I././WICED/. -I././WICED/platform/include -I./WICED/RTOS/ThreadX/ver5.6 -I./WICED/RTOS/ThreadX/ver5.6/Cortex_M3_M4/GCC -I./WICED/RTOS/ThreadX/WWD/CM3_CM4 -I./platforms/ISM43341_M4G_L44/. -I./platforms/ISM43341_M4G_L44//libraries/inputs/gpio_button -I./WICED/WWD/internal/chips/4334x -I./libraries -I./include
WICED_SDK_DEFINES             +=                                                                    -DSFLASH_APPS_HEADER_LOC=0x0000 -DBUILDCFG -DBLUETOOTH_BTE -DUSE_STDPERIPH_DRIVER -D_STM3x_ -D_STM32x_ -DSTM32F40_41xxx -DMAX_WATCHDOG_TIMEOUT_SECONDS=22 -DUSING_WICEDFS -DNETWORK_NetX_Duo=1 -DNetX_Duo_VERSION=\"v5.7_sp2\" -DNX_INCLUDE_USER_DEFINE_FILE -D__fd_set_defined -DSYS_TIME_H_AVAILABLE -DADD_LWIP_EAPOL_SUPPORT -DNXD_EXTENDED_BSD_SOCKET_SUPPORT -DOPENSSL -DSTDC_HEADERS -DBT_TRACE_PROTOCOL=FALSE -DBT_USE_TRACES=FALSE -DADD_NETX_EAPOL_SUPPORT -DWWD_STARTUP_DELAY=10 -DBOOTLOADER_MAGIC_NUMBER=0x4d435242 -DRTOS_ThreadX=1 -DThreadX_VERSION=\"v5.6\" -DTX_INCLUDE_USER_DEFINE_FILE -DWWD_DIRECT_RESOURCES -DHSE_VALUE=26000000 -DCRLF_STDIO_REPLACEMENT -DUSE_SELF_SIGNED_TLS_CERT -DWICED_SDK_WIFI_CONFIG_DCT_H=\"./include/default_wifi_config_dct.h\" -DWICED_SDK_BT_CONFIG_DCT_H=\"./include/default_bt_config_dct.h\"
COMPONENTS                := App_BT_SmartBridge Platform_ISM43341 ThreadX WICED Lib_HTTP_Server Lib_device_configuration Lib_SNTP Lib_Gedday Lib_WICED_Bluetooth_Low_Energy_Driver_for_BCM43341B0 Lib_Bluetooth_SmartBridge Lib_SPI_Flash_Library_ISM43341_M4G_L44 Lib_GPIO_button WWD_ThreadX_Interface WICED_ThreadX_Interface WWD_for_SDIO_ThreadX Supplicant_BESL Lib_DNS NetX_Duo Lib_Wiced_RO_FS STM32F4xx Lib_Linked_List Lib_Simple_HTTP_Server Lib_DNS_Redirect_Daemon Lib_Bluetooth_Low_Energy_Only_Embedded_Stack_for_WICED Lib_TLV Lib_base64 Lib_crypto_open Lib_micro_ecc WWD_NetX_Duo_Interface WICED_NetX_Duo_Interface common_GCC STM32F4xx_Peripheral_Drivers Lib_Ring_Buffer Lib_crc Lib_DHCP_Server STM32F4xx_Peripheral_Libraries
BUS                       := SDIO
IMAGE_TYPE                := ram
NETWORK_FULL              := 
RTOS_FULL                 := 
PLATFORM_DIRECTORY             := ISM43341_M4G_L44
APP_FULL                  := demo/bt_smartbridge
NETWORK                   := NetX_Duo
RTOS                      := ThreadX
PLATFORM                  := ISM43341_M4G_L44
USB                       := 
APP                       := bt_smartbridge
HOST_OPENOCD                := stm32f4x
HOST_ARCH                 := ARM_CM4
WICED_SDK_CERTIFICATE         := ./resources/certificates/brcm_demo_server_cert.cer
WICED_SDK_PRIVATE_KEY         :=  ./resources/certificates/brcm_demo_server_cert_key.key
NO_BUILD_BOOTLOADER           := 
NO_BOOTLOADER_REQUIRED         := 
App_BT_SmartBridge_LOCATION         := ./apps/demo/bt_smartbridge/
Platform_ISM43341_LOCATION         := ./platforms/ISM43341_M4G_L44/
ThreadX_LOCATION         := ./WICED/RTOS/ThreadX/
WICED_LOCATION         := ././WICED/
Lib_HTTP_Server_LOCATION         := ./libraries/daemons/HTTP_server/
Lib_device_configuration_LOCATION         := ./libraries/daemons/device_configuration/
Lib_SNTP_LOCATION         := ./libraries/protocols/SNTP/
Lib_Gedday_LOCATION         := ./libraries/daemons/Gedday/
Lib_WICED_Bluetooth_Low_Energy_Driver_for_BCM43341B0_LOCATION         := ./libraries/drivers/bluetooth_le/
Lib_Bluetooth_SmartBridge_LOCATION         := ./libraries/daemons/bt_smartbridge/
Lib_SPI_Flash_Library_ISM43341_M4G_L44_LOCATION         := ./libraries/drivers/spi_flash/
Lib_GPIO_button_LOCATION         := ./libraries/inputs/gpio_button/
WWD_ThreadX_Interface_LOCATION         := ././WICED/RTOS/ThreadX/WWD/
WICED_ThreadX_Interface_LOCATION         := ././WICED/RTOS/ThreadX/WICED/
WWD_for_SDIO_ThreadX_LOCATION         := ././WICED/WWD/
Supplicant_BESL_LOCATION         := ././WICED/security/BESL/
Lib_DNS_LOCATION         := ./libraries/protocols/DNS/
NetX_Duo_LOCATION         := ./WICED/network/NetX_Duo/
Lib_Wiced_RO_FS_LOCATION         := ./libraries/filesystems/wicedfs/
STM32F4xx_LOCATION         := ././WICED/platform/MCU/STM32F4xx/
Lib_Linked_List_LOCATION         := ./libraries/utilities/linked_list/
Lib_Simple_HTTP_Server_LOCATION         := ./libraries/daemons/simple_HTTP_server/
Lib_DNS_Redirect_Daemon_LOCATION         := ./libraries/daemons/DNS_redirect/
Lib_Bluetooth_Low_Energy_Only_Embedded_Stack_for_WICED_LOCATION         := ././libraries/drivers/bluetooth_le/BTE/
Lib_TLV_LOCATION         := ./libraries/utilities/TLV/
Lib_base64_LOCATION         := ./libraries/utilities/base64/
Lib_crypto_open_LOCATION         := ./WICED/security/BESL/crypto_open/
Lib_micro_ecc_LOCATION         := ./libraries/crypto/micro-ecc/
WWD_NetX_Duo_Interface_LOCATION         := ././WICED/network/NetX_Duo/WWD/
WICED_NetX_Duo_Interface_LOCATION         := ././WICED/network/NetX_Duo/WICED/
common_GCC_LOCATION         := ./WICED/platform/GCC/
STM32F4xx_Peripheral_Drivers_LOCATION         := ./WICED/platform/MCU/STM32F4xx/peripherals/
Lib_Ring_Buffer_LOCATION         := ./libraries/utilities/ring_buffer/
Lib_crc_LOCATION         := ./libraries/utilities/crc/
Lib_DHCP_Server_LOCATION         := ./libraries/daemons/DHCP_server/
STM32F4xx_Peripheral_Libraries_LOCATION         := ./WICED/platform/MCU/STM32F4xx/peripherals/libraries/
App_BT_SmartBridge_SOURCES          += bt_smartbridge.c default_renderer.c tcp_client.c wiced_sense_renderer.c
Platform_ISM43341_SOURCES          += platform.c 
ThreadX_SOURCES          += 
WICED_SOURCES          += internal/wiced_core.c internal/time.c internal/system_monitor.c internal/wiced_lib.c internal/wiced_crypto.c internal/waf.c internal/wifi.c internal/wiced_cooee.c internal/wiced_easy_setup.c internal/wiced_filesystem.c
Lib_HTTP_Server_SOURCES          += http_server.c
Lib_device_configuration_SOURCES          += device_configuration.c device_configuration_http_content.c
Lib_SNTP_SOURCES          += sntp.c
Lib_Gedday_SOURCES          += 
Lib_WICED_Bluetooth_Low_Energy_Driver_for_BCM43341B0_SOURCES          += ../bluetooth/firmware/43341B0/bt_firmware_image.c
Lib_Bluetooth_SmartBridge_SOURCES          += wiced_bt_smartbridge.c wiced_bt_management.c wiced_bt_smartbridge_gatt.c wiced_bt_smartbridge_cfg.c internal/bt_smartbridge_stack_interface.c internal/bt_smartbridge_helper.c internal/bt_smartbridge_socket_manager.c internal/bt_smartbridge_att_cache_manager.c internal/bt_smart_attribute.c
Lib_SPI_Flash_Library_ISM43341_M4G_L44_SOURCES          += spi_flash.c spi_flash_wiced.c
Lib_GPIO_button_SOURCES          += gpio_button.c
WWD_ThreadX_Interface_SOURCES          += wwd_rtos.c CM3_CM4/low_level_init.c
WICED_ThreadX_Interface_SOURCES          += wiced_rtos.c ../../wiced_rtos_common.c
WWD_for_SDIO_ThreadX_SOURCES          += internal/wwd_thread.c internal/wwd_thread_internal.c internal/wwd_sdpcm.c internal/wwd_internal.c internal/wwd_management.c internal/wwd_wifi.c internal/wwd_logging.c internal/wwd_eapol.c internal/bus_protocols/wwd_bus_common.c internal/bus_protocols/SDIO/wwd_bus_protocol.c ../internal/wiced_crypto.c  internal/chips/4334x/wwd_ap.c internal/chips/4334x/wwd_chip_specific_functions.c
Supplicant_BESL_SOURCES          += host/WICED/besl_host.c host/WICED/wiced_tls.c host/WICED/wiced_wps.c host/WICED/wiced_p2p.c host/WICED/cipher_suites.c host/WICED/tls_cipher_suites.c host/WICED/dtls_cipher_suites.c host/WICED/p2p_internal.c host/WICED/wiced_supplicant.c P2P/p2p_events.c P2P/p2p_frame_writer.c host/WICED/wiced_dtls.c
Lib_DNS_SOURCES          += dns.c
NetX_Duo_SOURCES          += ver5.7_sp2/nxd_external_functions.c
Lib_Wiced_RO_FS_SOURCES          += src/wicedfs.c wicedfs_drivers.c
STM32F4xx_SOURCES          += ../../ARM_CM4/crt0_GCC.c ../../ARM_CM4/hardfault_handler.c ../../ARM_CM4/host_cm4.c ../platform_resource.c ../platform_stdio.c ../wiced_platform_common.c ../wwd_platform_separate_mcu.c ../wwd_resources.c ../wiced_apps_common.c ../wiced_waf_common.c ../wiced_dct_internal_common.c ../platform_nsclock.c platform_vector_table.c platform_init.c platform_unhandled_isr.c platform_filesystem.c WAF/waf_platform.c  ../platform_button.c WWD/wwd_platform.c WWD/wwd_SDIO.c
Lib_Linked_List_SOURCES          += linked_list.c
Lib_Simple_HTTP_Server_SOURCES          += simple_http_server.c
Lib_DNS_Redirect_Daemon_SOURCES          += dns_redirect.c
Lib_Bluetooth_Low_Energy_Only_Embedded_Stack_for_WICED_SOURCES          += 
Lib_TLV_SOURCES          += tlv.c
Lib_base64_SOURCES          += bsd-base64.c
Lib_crypto_open_SOURCES          += aes.c arc4.c bignum.c camellia.c certs.c chacha_reference.c curve25519.c des.c ed25519/ed25519.c md4.c md5.c poly1305.c seed.c sha1.c sha2.c sha4.c x509parse.c
Lib_micro_ecc_SOURCES          += uECC.c
WWD_NetX_Duo_Interface_SOURCES          += wwd_buffer.c wwd_network.c
WICED_NetX_Duo_Interface_SOURCES          += wiced_network.c tcpip.c ../../wiced_network_common.c ../../wiced_tcpip_common.c
common_GCC_SOURCES          += mem_newlib.c math_newlib.c cxx_funcs.c stdio_newlib.c
STM32F4xx_Peripheral_Drivers_SOURCES          += platform_adc.c platform_gpio.c platform_i2c.c platform_mcu_powersave.c platform_pwm.c platform_rtc.c platform_spi.c platform_uart.c platform_watchdog.c platform_i2s.c platform_ext_memory.c platform_audio_timer.c
Lib_Ring_Buffer_SOURCES          += ring_buffer.c
Lib_crc_SOURCES          += crc.c
Lib_DHCP_Server_SOURCES          += dhcp_server.c
STM32F4xx_Peripheral_Libraries_SOURCES          += src/misc.c src/stm32f4xx_adc.c src/stm32f4xx_can.c src/stm32f4xx_crc.c src/stm32f4xx_dac.c src/stm32f4xx_dbgmcu.c src/stm32f4xx_dma.c src/stm32f4xx_exti.c src/stm32f4xx_flash.c src/stm32f4xx_gpio.c src/stm32f4xx_rng.c src/stm32f4xx_i2c.c src/stm32f4xx_iwdg.c src/stm32f4xx_pwr.c src/stm32f4xx_rcc.c src/stm32f4xx_rtc.c src/stm32f4xx_sdio.c src/stm32f4xx_spi.c src/stm32f4xx_syscfg.c src/stm32f4xx_tim.c src/stm32f4xx_usart.c src/stm32f4xx_wwdg.c src/stm32f4xx_fsmc.c
App_BT_SmartBridge_CHECK_HEADERS    += 
Platform_ISM43341_CHECK_HEADERS    += 
ThreadX_CHECK_HEADERS    += 
WICED_CHECK_HEADERS    += internal/wiced_internal_api.h ../include/default_wifi_config_dct.h ../include/resource.h ../include/wiced.h ../include/wiced_defaults.h ../include/wiced_easy_setup.h ../include/wiced_framework.h ../include/wiced_management.h ../include/wiced_platform.h ../include/wiced_rtos.h ../include/wiced_security.h ../include/wiced_tcpip.h ../include/wiced_time.h ../include/wiced_utilities.h ../include/wiced_crypto.h ../include/wiced_wifi.h
Lib_HTTP_Server_CHECK_HEADERS    += 
Lib_device_configuration_CHECK_HEADERS    += 
Lib_SNTP_CHECK_HEADERS    += 
Lib_Gedday_CHECK_HEADERS    += 
Lib_WICED_Bluetooth_Low_Energy_Driver_for_BCM43341B0_CHECK_HEADERS    += 
Lib_Bluetooth_SmartBridge_CHECK_HEADERS    += 
Lib_SPI_Flash_Library_ISM43341_M4G_L44_CHECK_HEADERS    += 
Lib_GPIO_button_CHECK_HEADERS    += 
WWD_ThreadX_Interface_CHECK_HEADERS    += wwd_rtos.h
WICED_ThreadX_Interface_CHECK_HEADERS    += rtos.h
WWD_for_SDIO_ThreadX_CHECK_HEADERS    += internal/wwd_ap.h internal/wwd_bcmendian.h internal/wwd_internal.h internal/wwd_logging.h internal/wwd_sdpcm.h internal/wwd_thread.h internal/wwd_thread_internal.h internal/bus_protocols/wwd_bus_protocol_interface.h internal/bus_protocols/SDIO/wwd_bus_protocol.h internal/chips/4334x/chip_constants.h include/wwd_assert.h include/wwd_constants.h include/wwd_debug.h include/wwd_events.h include/wwd_management.h include/wwd_poll.h include/wwd_structures.h include/wwd_wifi.h include/wwd_wlioctl.h include/Network/wwd_buffer_interface.h include/Network/wwd_network_constants.h include/Network/wwd_network_interface.h include/platform/wwd_bus_interface.h include/platform/wwd_platform_interface.h include/platform/wwd_resource_interface.h include/platform/wwd_sdio_interface.h include/platform/wwd_spi_interface.h include/RTOS/wwd_rtos_interface.h
Supplicant_BESL_CHECK_HEADERS    += 
Lib_DNS_CHECK_HEADERS    += 
NetX_Duo_CHECK_HEADERS    += 
Lib_Wiced_RO_FS_CHECK_HEADERS    += 
STM32F4xx_CHECK_HEADERS    += 
Lib_Linked_List_CHECK_HEADERS    += 
Lib_Simple_HTTP_Server_CHECK_HEADERS    += 
Lib_DNS_Redirect_Daemon_CHECK_HEADERS    += 
Lib_Bluetooth_Low_Energy_Only_Embedded_Stack_for_WICED_CHECK_HEADERS    += 
Lib_TLV_CHECK_HEADERS    += 
Lib_base64_CHECK_HEADERS    += 
Lib_crypto_open_CHECK_HEADERS    += 
Lib_micro_ecc_CHECK_HEADERS    += 
WWD_NetX_Duo_Interface_CHECK_HEADERS    += wwd_buffer.h wwd_network.h
WICED_NetX_Duo_Interface_CHECK_HEADERS    += wiced_network.h
common_GCC_CHECK_HEADERS    += 
STM32F4xx_Peripheral_Drivers_CHECK_HEADERS    += 
Lib_Ring_Buffer_CHECK_HEADERS    += 
Lib_crc_CHECK_HEADERS    += 
Lib_DHCP_Server_CHECK_HEADERS    += 
STM32F4xx_Peripheral_Libraries_CHECK_HEADERS    += 
App_BT_SmartBridge_INCLUDES         := 
Platform_ISM43341_INCLUDES         := 
ThreadX_INCLUDES         := 
WICED_INCLUDES         := -I././WICED/security/BESL/crypto -I././WICED/security/BESL/include -I././WICED/security/BESL/host/WICED -I././WICED/security/BESL/WPS
Lib_HTTP_Server_INCLUDES         := 
Lib_device_configuration_INCLUDES         := 
Lib_SNTP_INCLUDES         := 
Lib_Gedday_INCLUDES         := 
Lib_WICED_Bluetooth_Low_Energy_Driver_for_BCM43341B0_INCLUDES         := 
Lib_Bluetooth_SmartBridge_INCLUDES         := 
Lib_SPI_Flash_Library_ISM43341_M4G_L44_INCLUDES         := 
Lib_GPIO_button_INCLUDES         := 
WWD_ThreadX_Interface_INCLUDES         := 
WICED_ThreadX_Interface_INCLUDES         := 
WWD_for_SDIO_ThreadX_INCLUDES         := 
Supplicant_BESL_INCLUDES         := 
Lib_DNS_INCLUDES         := 
NetX_Duo_INCLUDES         := 
Lib_Wiced_RO_FS_INCLUDES         := 
STM32F4xx_INCLUDES         := 
Lib_Linked_List_INCLUDES         := 
Lib_Simple_HTTP_Server_INCLUDES         := 
Lib_DNS_Redirect_Daemon_INCLUDES         := 
Lib_Bluetooth_Low_Energy_Only_Embedded_Stack_for_WICED_INCLUDES         := 
Lib_TLV_INCLUDES         := 
Lib_base64_INCLUDES         := 
Lib_crypto_open_INCLUDES         := 
Lib_micro_ecc_INCLUDES         := 
WWD_NetX_Duo_Interface_INCLUDES         := 
WICED_NetX_Duo_Interface_INCLUDES         := 
common_GCC_INCLUDES         := 
STM32F4xx_Peripheral_Drivers_INCLUDES         := 
Lib_Ring_Buffer_INCLUDES         := 
Lib_crc_INCLUDES         := 
Lib_DHCP_Server_INCLUDES         := 
STM32F4xx_Peripheral_Libraries_INCLUDES         := 
App_BT_SmartBridge_DEFINES          := 
Platform_ISM43341_DEFINES          := 
ThreadX_DEFINES          := 
WICED_DEFINES          := 
Lib_HTTP_Server_DEFINES          := 
Lib_device_configuration_DEFINES          := 
Lib_SNTP_DEFINES          := 
Lib_Gedday_DEFINES          := 
Lib_WICED_Bluetooth_Low_Energy_Driver_for_BCM43341B0_DEFINES          := 
Lib_Bluetooth_SmartBridge_DEFINES          := 
Lib_SPI_Flash_Library_ISM43341_M4G_L44_DEFINES          := -DSFLASH_SUPPORT_SST_PARTS -DSFLASH_SUPPORT_MACRONIX_PARTS -DSFLASH_SUPPORT_EON_PARTS -DSFLASH_SUPPORT_MICRON_PARTS
Lib_GPIO_button_DEFINES          := 
WWD_ThreadX_Interface_DEFINES          := 
WICED_ThreadX_Interface_DEFINES          := 
WWD_for_SDIO_ThreadX_DEFINES          := 
Supplicant_BESL_DEFINES          := 
Lib_DNS_DEFINES          := 
NetX_Duo_DEFINES          := 
Lib_Wiced_RO_FS_DEFINES          := 
STM32F4xx_DEFINES          := 
Lib_Linked_List_DEFINES          := 
Lib_Simple_HTTP_Server_DEFINES          := 
Lib_DNS_Redirect_Daemon_DEFINES          := 
Lib_Bluetooth_Low_Energy_Only_Embedded_Stack_for_WICED_DEFINES          := 
Lib_TLV_DEFINES          := 
Lib_base64_DEFINES          := 
Lib_crypto_open_DEFINES          := -DTROPICSSL_DES_C -DTROPICSSL_AES_ROM_TABLES -DTROPICSSL_HAVE_LONGLONG -DSTDC_HEADERS -DUSE_SRP_SHA_512 -DOPENSSL -DED25519_FORCE_32BIT -DOPENSSL_SMALL_FOOTPRINT
Lib_micro_ecc_DEFINES          := 
WWD_NetX_Duo_Interface_DEFINES          := 
WICED_NetX_Duo_Interface_DEFINES          := 
common_GCC_DEFINES          := 
STM32F4xx_Peripheral_Drivers_DEFINES          := 
Lib_Ring_Buffer_DEFINES          := 
Lib_crc_DEFINES          := 
Lib_DHCP_Server_DEFINES          := 
STM32F4xx_Peripheral_Libraries_DEFINES          := 
App_BT_SmartBridge_CFLAGS           :=                 -mthumb -mcpu=cortex-m4    -mlittle-endian                    -DNDEBUG -ggdb -Os -Wall -fsigned-char -ffunction-sections -fdata-sections -fno-common -std=gnu11                 -DWICED_VERSION=\"3.5.2\" -DBUS=\"SDIO\" -Ibuild/demo.bt_smartbridge-ISM43341_M4G_L44/resources/ -DPLATFORM=\"ISM43341_M4G_L44\" 
Platform_ISM43341_CFLAGS           :=                 -mthumb -mcpu=cortex-m4    -mlittle-endian                    -DNDEBUG -ggdb -Os -Wall -fsigned-char -ffunction-sections -fdata-sections -fno-common -std=gnu11                 -DWICED_VERSION=\"3.5.2\" -DBUS=\"SDIO\" -Ibuild/demo.bt_smartbridge-ISM43341_M4G_L44/resources/ -DPLATFORM=\"ISM43341_M4G_L44\" 
ThreadX_CFLAGS           :=                 -mthumb -mcpu=cortex-m4    -mlittle-endian                    -DNDEBUG -ggdb -Os -Wall -fsigned-char -ffunction-sections -fdata-sections -fno-common -std=gnu11                 -DWICED_VERSION=\"3.5.2\" -DBUS=\"SDIO\" -Ibuild/demo.bt_smartbridge-ISM43341_M4G_L44/resources/ -DPLATFORM=\"ISM43341_M4G_L44\" 
WICED_CFLAGS           :=                 -mthumb -mcpu=cortex-m4    -mlittle-endian                    -DNDEBUG -ggdb -Os -Wall -fsigned-char -ffunction-sections -fdata-sections -fno-common -std=gnu11                 -DWICED_VERSION=\"3.5.2\" -DBUS=\"SDIO\" -Ibuild/demo.bt_smartbridge-ISM43341_M4G_L44/resources/ -DPLATFORM=\"ISM43341_M4G_L44\" -Werror -Wstrict-prototypes  -W -Wshadow  -Wwrite-strings -pedantic -std=c99 -U__STRICT_ANSI__ -Wconversion -Wextra -Wdeclaration-after-statement -Wconversion -Waddress -Wlogical-op -Wstrict-prototypes -Wold-style-definition -Wmissing-prototypes -Wmissing-declarations -Wmissing-field-initializers -Wdouble-promotion -Wswitch-enum -Wswitch-default -Wuninitialized -Wunknown-pragmas -Wfloat-equal  -Wundef  -Wshadow 
Lib_HTTP_Server_CFLAGS           :=                 -mthumb -mcpu=cortex-m4    -mlittle-endian                    -DNDEBUG -ggdb -Os -Wall -fsigned-char -ffunction-sections -fdata-sections -fno-common -std=gnu11                 -DWICED_VERSION=\"3.5.2\" -DBUS=\"SDIO\" -Ibuild/demo.bt_smartbridge-ISM43341_M4G_L44/resources/ -DPLATFORM=\"ISM43341_M4G_L44\" -Werror -Wstrict-prototypes  -W -Wshadow  -Wwrite-strings -pedantic -std=c99 -U__STRICT_ANSI__ -Wconversion -Wextra -Wdeclaration-after-statement -Wconversion -Waddress -Wlogical-op -Wstrict-prototypes -Wold-style-definition -Wmissing-prototypes -Wmissing-declarations -Wmissing-field-initializers -Wdouble-promotion -Wswitch-enum -Wswitch-default -Wuninitialized -Wunknown-pragmas -Wfloat-equal  -Wundef  -Wshadow 
Lib_device_configuration_CFLAGS           :=                 -mthumb -mcpu=cortex-m4    -mlittle-endian                    -DNDEBUG -ggdb -Os -Wall -fsigned-char -ffunction-sections -fdata-sections -fno-common -std=gnu11                 -DWICED_VERSION=\"3.5.2\" -DBUS=\"SDIO\" -Ibuild/demo.bt_smartbridge-ISM43341_M4G_L44/resources/ -DPLATFORM=\"ISM43341_M4G_L44\" 
Lib_SNTP_CFLAGS           :=                 -mthumb -mcpu=cortex-m4    -mlittle-endian                    -DNDEBUG -ggdb -Os -Wall -fsigned-char -ffunction-sections -fdata-sections -fno-common -std=gnu11                 -DWICED_VERSION=\"3.5.2\" -DBUS=\"SDIO\" -Ibuild/demo.bt_smartbridge-ISM43341_M4G_L44/resources/ -DPLATFORM=\"ISM43341_M4G_L44\" -Werror -Wstrict-prototypes  -W -Wshadow  -Wwrite-strings -pedantic -std=c99 -U__STRICT_ANSI__ -Wconversion -Wextra -Wdeclaration-after-statement -Wconversion -Waddress -Wlogical-op -Wstrict-prototypes -Wold-style-definition -Wmissing-prototypes -Wmissing-declarations -Wmissing-field-initializers -Wdouble-promotion -Wswitch-enum -Wswitch-default -Wuninitialized -Wunknown-pragmas -Wfloat-equal  -Wundef  -Wshadow 
Lib_Gedday_CFLAGS           :=                 -mthumb -mcpu=cortex-m4    -mlittle-endian                    -DNDEBUG -ggdb -Os -Wall -fsigned-char -ffunction-sections -fdata-sections -fno-common -std=gnu11                 -DWICED_VERSION=\"3.5.2\" -DBUS=\"SDIO\" -Ibuild/demo.bt_smartbridge-ISM43341_M4G_L44/resources/ -DPLATFORM=\"ISM43341_M4G_L44\" -Werror -Wstrict-prototypes  -W -Wshadow  -Wwrite-strings -pedantic -std=c99 -U__STRICT_ANSI__ -Wconversion -Wextra -Wdeclaration-after-statement -Wconversion -Waddress -Wlogical-op -Wstrict-prototypes -Wold-style-definition -Wmissing-prototypes -Wmissing-declarations -Wmissing-field-initializers -Wdouble-promotion -Wswitch-enum -Wswitch-default -Wuninitialized -Wunknown-pragmas -Wfloat-equal  -Wundef  -Wshadow 
Lib_WICED_Bluetooth_Low_Energy_Driver_for_BCM43341B0_CFLAGS           :=                 -mthumb -mcpu=cortex-m4    -mlittle-endian                    -DNDEBUG -ggdb -Os -Wall -fsigned-char -ffunction-sections -fdata-sections -fno-common -std=gnu11                 -DWICED_VERSION=\"3.5.2\" -DBUS=\"SDIO\" -Ibuild/demo.bt_smartbridge-ISM43341_M4G_L44/resources/ -DPLATFORM=\"ISM43341_M4G_L44\" 
Lib_Bluetooth_SmartBridge_CFLAGS           :=                 -mthumb -mcpu=cortex-m4    -mlittle-endian                    -DNDEBUG -ggdb -Os -Wall -fsigned-char -ffunction-sections -fdata-sections -fno-common -std=gnu11                 -DWICED_VERSION=\"3.5.2\" -DBUS=\"SDIO\" -Ibuild/demo.bt_smartbridge-ISM43341_M4G_L44/resources/ -DPLATFORM=\"ISM43341_M4G_L44\" 
Lib_SPI_Flash_Library_ISM43341_M4G_L44_CFLAGS           :=                 -mthumb -mcpu=cortex-m4    -mlittle-endian                    -DNDEBUG -ggdb -Os -Wall -fsigned-char -ffunction-sections -fdata-sections -fno-common -std=gnu11                 -DWICED_VERSION=\"3.5.2\" -DBUS=\"SDIO\" -Ibuild/demo.bt_smartbridge-ISM43341_M4G_L44/resources/ -DPLATFORM=\"ISM43341_M4G_L44\" -Werror -Wstrict-prototypes  -W -Wshadow  -Wwrite-strings -pedantic -std=c99 -U__STRICT_ANSI__ -Wconversion -Wextra -Wdeclaration-after-statement -Wconversion -Waddress -Wlogical-op -Wstrict-prototypes -Wold-style-definition -Wmissing-prototypes -Wmissing-declarations -Wmissing-field-initializers -Wdouble-promotion -Wswitch-enum -Wswitch-default -Wuninitialized -Wunknown-pragmas -Wfloat-equal  -Wundef  -Wshadow 
Lib_GPIO_button_CFLAGS           :=                 -mthumb -mcpu=cortex-m4    -mlittle-endian                    -DNDEBUG -ggdb -Os -Wall -fsigned-char -ffunction-sections -fdata-sections -fno-common -std=gnu11                 -DWICED_VERSION=\"3.5.2\" -DBUS=\"SDIO\" -Ibuild/demo.bt_smartbridge-ISM43341_M4G_L44/resources/ -DPLATFORM=\"ISM43341_M4G_L44\" -Werror -Wstrict-prototypes  -W -Wshadow  -Wwrite-strings -pedantic -std=c99 -U__STRICT_ANSI__ -Wconversion -Wextra -Wdeclaration-after-statement -Wconversion -Waddress -Wlogical-op -Wstrict-prototypes -Wold-style-definition -Wmissing-prototypes -Wmissing-declarations -Wmissing-field-initializers -Wdouble-promotion -Wswitch-enum -Wswitch-default -Wuninitialized -Wunknown-pragmas -Wfloat-equal  -Wundef  -Wshadow 
WWD_ThreadX_Interface_CFLAGS           :=                 -mthumb -mcpu=cortex-m4    -mlittle-endian                    -DNDEBUG -ggdb -Os -Wall -fsigned-char -ffunction-sections -fdata-sections -fno-common -std=gnu11                 -DWICED_VERSION=\"3.5.2\" -DBUS=\"SDIO\" -Ibuild/demo.bt_smartbridge-ISM43341_M4G_L44/resources/ -DPLATFORM=\"ISM43341_M4G_L44\" -Werror -Wstrict-prototypes  -W -Wshadow  -Wwrite-strings -pedantic -std=c99 -U__STRICT_ANSI__ -Wconversion -Wextra -Wdeclaration-after-statement -Wconversion -Waddress -Wlogical-op -Wstrict-prototypes -Wold-style-definition -Wmissing-prototypes -Wmissing-declarations -Wmissing-field-initializers -Wdouble-promotion -Wswitch-enum -Wswitch-default -Wuninitialized -Wunknown-pragmas -Wfloat-equal  -Wundef  -Wshadow 
WICED_ThreadX_Interface_CFLAGS           :=                 -mthumb -mcpu=cortex-m4    -mlittle-endian                    -DNDEBUG -ggdb -Os -Wall -fsigned-char -ffunction-sections -fdata-sections -fno-common -std=gnu11                 -DWICED_VERSION=\"3.5.2\" -DBUS=\"SDIO\" -Ibuild/demo.bt_smartbridge-ISM43341_M4G_L44/resources/ -DPLATFORM=\"ISM43341_M4G_L44\" -Werror -Wstrict-prototypes  -W -Wshadow  -Wwrite-strings -pedantic -std=c99 -U__STRICT_ANSI__ -Wconversion -Wextra -Wdeclaration-after-statement -Wconversion -Waddress -Wlogical-op -Wstrict-prototypes -Wold-style-definition -Wmissing-prototypes -Wmissing-declarations -Wmissing-field-initializers -Wdouble-promotion -Wswitch-enum -Wswitch-default -Wuninitialized -Wunknown-pragmas -Wfloat-equal  -Wundef  -Wshadow 
WWD_for_SDIO_ThreadX_CFLAGS           :=                 -mthumb -mcpu=cortex-m4    -mlittle-endian                    -DNDEBUG -ggdb -Os -Wall -fsigned-char -ffunction-sections -fdata-sections -fno-common -std=gnu11                 -DWICED_VERSION=\"3.5.2\" -DBUS=\"SDIO\" -Ibuild/demo.bt_smartbridge-ISM43341_M4G_L44/resources/ -DPLATFORM=\"ISM43341_M4G_L44\" -Werror -Wstrict-prototypes  -W -Wshadow  -Wwrite-strings -pedantic -std=c99 -U__STRICT_ANSI__ -Wconversion -Wextra -Wdeclaration-after-statement -Wconversion -Waddress -Wlogical-op -Wstrict-prototypes -Wold-style-definition -Wmissing-prototypes -Wmissing-declarations -Wmissing-field-initializers -Wdouble-promotion -Wswitch-enum -Wswitch-default -Wuninitialized -Wunknown-pragmas -Wfloat-equal  -Wundef  -Wshadow 
Supplicant_BESL_CFLAGS           :=                 -mthumb -mcpu=cortex-m4    -mlittle-endian                    -DNDEBUG -ggdb -Os -Wall -fsigned-char -ffunction-sections -fdata-sections -fno-common -std=gnu11                 -DWICED_VERSION=\"3.5.2\" -DBUS=\"SDIO\" -Ibuild/demo.bt_smartbridge-ISM43341_M4G_L44/resources/ -DPLATFORM=\"ISM43341_M4G_L44\" -fno-strict-aliasing
Lib_DNS_CFLAGS           :=                 -mthumb -mcpu=cortex-m4    -mlittle-endian                    -DNDEBUG -ggdb -Os -Wall -fsigned-char -ffunction-sections -fdata-sections -fno-common -std=gnu11                 -DWICED_VERSION=\"3.5.2\" -DBUS=\"SDIO\" -Ibuild/demo.bt_smartbridge-ISM43341_M4G_L44/resources/ -DPLATFORM=\"ISM43341_M4G_L44\" -Werror -Wstrict-prototypes  -W -Wshadow  -Wwrite-strings -pedantic -std=c99 -U__STRICT_ANSI__ -Wconversion -Wextra -Wdeclaration-after-statement -Wconversion -Waddress -Wlogical-op -Wstrict-prototypes -Wold-style-definition -Wmissing-prototypes -Wmissing-declarations -Wmissing-field-initializers -Wdouble-promotion -Wswitch-enum -Wswitch-default -Wuninitialized -Wunknown-pragmas -Wfloat-equal  -Wundef  -Wshadow 
NetX_Duo_CFLAGS           :=                 -mthumb -mcpu=cortex-m4    -mlittle-endian                    -DNDEBUG -ggdb -Os -Wall -fsigned-char -ffunction-sections -fdata-sections -fno-common -std=gnu11                 -DWICED_VERSION=\"3.5.2\" -DBUS=\"SDIO\" -Ibuild/demo.bt_smartbridge-ISM43341_M4G_L44/resources/ -DPLATFORM=\"ISM43341_M4G_L44\" 
Lib_Wiced_RO_FS_CFLAGS           :=                 -mthumb -mcpu=cortex-m4    -mlittle-endian                    -DNDEBUG -ggdb -Os -Wall -fsigned-char -ffunction-sections -fdata-sections -fno-common -std=gnu11                 -DWICED_VERSION=\"3.5.2\" -DBUS=\"SDIO\" -Ibuild/demo.bt_smartbridge-ISM43341_M4G_L44/resources/ -DPLATFORM=\"ISM43341_M4G_L44\" -Werror -Wstrict-prototypes  -W -Wshadow  -Wwrite-strings -pedantic -std=c99 -U__STRICT_ANSI__ -Wconversion -Wextra -Wdeclaration-after-statement -Wconversion -Waddress -Wlogical-op -Wstrict-prototypes -Wold-style-definition -Wmissing-prototypes -Wmissing-declarations -Wmissing-field-initializers -Wdouble-promotion -Wswitch-enum -Wswitch-default -Wuninitialized -Wunknown-pragmas -Wfloat-equal  -Wundef  -Wshadow 
STM32F4xx_CFLAGS           :=                 -mthumb -mcpu=cortex-m4    -mlittle-endian                    -DNDEBUG -ggdb -Os -Wall -fsigned-char -ffunction-sections -fdata-sections -fno-common -std=gnu11                 -DWICED_VERSION=\"3.5.2\" -DBUS=\"SDIO\" -Ibuild/demo.bt_smartbridge-ISM43341_M4G_L44/resources/ -DPLATFORM=\"ISM43341_M4G_L44\" -Werror -Wstrict-prototypes  -W -Wshadow  -Wwrite-strings -pedantic -std=c99 -U__STRICT_ANSI__ -Wconversion -Wextra -Wdeclaration-after-statement -Wconversion -Waddress -Wlogical-op -Wstrict-prototypes -Wold-style-definition -Wmissing-prototypes -Wmissing-declarations -Wmissing-field-initializers -Wdouble-promotion -Wswitch-enum -Wswitch-default -Wuninitialized -Wunknown-pragmas -Wfloat-equal  -Wundef  -Wshadow 
Lib_Linked_List_CFLAGS           :=                 -mthumb -mcpu=cortex-m4    -mlittle-endian                    -DNDEBUG -ggdb -Os -Wall -fsigned-char -ffunction-sections -fdata-sections -fno-common -std=gnu11                 -DWICED_VERSION=\"3.5.2\" -DBUS=\"SDIO\" -Ibuild/demo.bt_smartbridge-ISM43341_M4G_L44/resources/ -DPLATFORM=\"ISM43341_M4G_L44\" 
Lib_Simple_HTTP_Server_CFLAGS           :=                 -mthumb -mcpu=cortex-m4    -mlittle-endian                    -DNDEBUG -ggdb -Os -Wall -fsigned-char -ffunction-sections -fdata-sections -fno-common -std=gnu11                 -DWICED_VERSION=\"3.5.2\" -DBUS=\"SDIO\" -Ibuild/demo.bt_smartbridge-ISM43341_M4G_L44/resources/ -DPLATFORM=\"ISM43341_M4G_L44\" -Werror -Wstrict-prototypes  -W -Wshadow  -Wwrite-strings -pedantic -std=c99 -U__STRICT_ANSI__ -Wconversion -Wextra -Wdeclaration-after-statement -Wconversion -Waddress -Wlogical-op -Wstrict-prototypes -Wold-style-definition -Wmissing-prototypes -Wmissing-declarations -Wmissing-field-initializers -Wdouble-promotion -Wswitch-enum -Wswitch-default -Wuninitialized -Wunknown-pragmas -Wfloat-equal  -Wundef  -Wshadow 
Lib_DNS_Redirect_Daemon_CFLAGS           :=                 -mthumb -mcpu=cortex-m4    -mlittle-endian                    -DNDEBUG -ggdb -Os -Wall -fsigned-char -ffunction-sections -fdata-sections -fno-common -std=gnu11                 -DWICED_VERSION=\"3.5.2\" -DBUS=\"SDIO\" -Ibuild/demo.bt_smartbridge-ISM43341_M4G_L44/resources/ -DPLATFORM=\"ISM43341_M4G_L44\" -Werror -Wstrict-prototypes  -W -Wshadow  -Wwrite-strings -pedantic -std=c99 -U__STRICT_ANSI__ -Wconversion -Wextra -Wdeclaration-after-statement -Wconversion -Waddress -Wlogical-op -Wstrict-prototypes -Wold-style-definition -Wmissing-prototypes -Wmissing-declarations -Wmissing-field-initializers -Wdouble-promotion -Wswitch-enum -Wswitch-default -Wuninitialized -Wunknown-pragmas -Wfloat-equal  -Wundef  -Wshadow 
Lib_Bluetooth_Low_Energy_Only_Embedded_Stack_for_WICED_CFLAGS           :=                 -mthumb -mcpu=cortex-m4    -mlittle-endian                    -DNDEBUG -ggdb -Os -Wall -fsigned-char -ffunction-sections -fdata-sections -fno-common -std=gnu11                 -DWICED_VERSION=\"3.5.2\" -DBUS=\"SDIO\" -Ibuild/demo.bt_smartbridge-ISM43341_M4G_L44/resources/ -DPLATFORM=\"ISM43341_M4G_L44\" 
Lib_TLV_CFLAGS           :=                 -mthumb -mcpu=cortex-m4    -mlittle-endian                    -DNDEBUG -ggdb -Os -Wall -fsigned-char -ffunction-sections -fdata-sections -fno-common -std=gnu11                 -DWICED_VERSION=\"3.5.2\" -DBUS=\"SDIO\" -Ibuild/demo.bt_smartbridge-ISM43341_M4G_L44/resources/ -DPLATFORM=\"ISM43341_M4G_L44\" 
Lib_base64_CFLAGS           :=                 -mthumb -mcpu=cortex-m4    -mlittle-endian                    -DNDEBUG -ggdb -Os -Wall -fsigned-char -ffunction-sections -fdata-sections -fno-common -std=gnu11                 -DWICED_VERSION=\"3.5.2\" -DBUS=\"SDIO\" -Ibuild/demo.bt_smartbridge-ISM43341_M4G_L44/resources/ -DPLATFORM=\"ISM43341_M4G_L44\" 
Lib_crypto_open_CFLAGS           :=                 -mthumb -mcpu=cortex-m4    -mlittle-endian                    -DNDEBUG -ggdb -Os -Wall -fsigned-char -ffunction-sections -fdata-sections -fno-common -std=gnu11                 -DWICED_VERSION=\"3.5.2\" -DBUS=\"SDIO\" -Ibuild/demo.bt_smartbridge-ISM43341_M4G_L44/resources/ -DPLATFORM=\"ISM43341_M4G_L44\" 
Lib_micro_ecc_CFLAGS           :=                 -mthumb -mcpu=cortex-m4    -mlittle-endian                    -DNDEBUG -ggdb -Os -Wall -fsigned-char -ffunction-sections -fdata-sections -fno-common -std=gnu11                 -DWICED_VERSION=\"3.5.2\" -DBUS=\"SDIO\" -Ibuild/demo.bt_smartbridge-ISM43341_M4G_L44/resources/ -DPLATFORM=\"ISM43341_M4G_L44\" 
WWD_NetX_Duo_Interface_CFLAGS           :=                 -mthumb -mcpu=cortex-m4    -mlittle-endian                    -DNDEBUG -ggdb -Os -Wall -fsigned-char -ffunction-sections -fdata-sections -fno-common -std=gnu11                 -DWICED_VERSION=\"3.5.2\" -DBUS=\"SDIO\" -Ibuild/demo.bt_smartbridge-ISM43341_M4G_L44/resources/ -DPLATFORM=\"ISM43341_M4G_L44\" -Werror -Wstrict-prototypes  -W -Wshadow  -Wwrite-strings -pedantic -std=c99 -U__STRICT_ANSI__ -Wconversion -Wextra -Wdeclaration-after-statement -Wconversion -Waddress -Wlogical-op -Wstrict-prototypes -Wold-style-definition -Wmissing-prototypes -Wmissing-declarations -Wmissing-field-initializers -Wdouble-promotion -Wswitch-enum -Wswitch-default -Wuninitialized -Wunknown-pragmas -Wfloat-equal  -Wundef  -Wshadow 
WICED_NetX_Duo_Interface_CFLAGS           :=                 -mthumb -mcpu=cortex-m4    -mlittle-endian                    -DNDEBUG -ggdb -Os -Wall -fsigned-char -ffunction-sections -fdata-sections -fno-common -std=gnu11                 -DWICED_VERSION=\"3.5.2\" -DBUS=\"SDIO\" -Ibuild/demo.bt_smartbridge-ISM43341_M4G_L44/resources/ -DPLATFORM=\"ISM43341_M4G_L44\" -Werror -Wstrict-prototypes  -W -Wshadow  -Wwrite-strings -pedantic -std=c99 -U__STRICT_ANSI__ -Wconversion -Wextra -Wdeclaration-after-statement -Wconversion -Waddress -Wlogical-op -Wstrict-prototypes -Wold-style-definition -Wmissing-prototypes -Wmissing-declarations -Wmissing-field-initializers -Wdouble-promotion -Wswitch-enum -Wswitch-default -Wuninitialized -Wunknown-pragmas -Wfloat-equal  -Wundef  -Wshadow 
common_GCC_CFLAGS           :=                 -mthumb -mcpu=cortex-m4    -mlittle-endian                    -DNDEBUG -ggdb -Os -Wall -fsigned-char -ffunction-sections -fdata-sections -fno-common -std=gnu11                 -DWICED_VERSION=\"3.5.2\" -DBUS=\"SDIO\" -Ibuild/demo.bt_smartbridge-ISM43341_M4G_L44/resources/ -DPLATFORM=\"ISM43341_M4G_L44\" 
STM32F4xx_Peripheral_Drivers_CFLAGS           :=                 -mthumb -mcpu=cortex-m4    -mlittle-endian                    -DNDEBUG -ggdb -Os -Wall -fsigned-char -ffunction-sections -fdata-sections -fno-common -std=gnu11                 -DWICED_VERSION=\"3.5.2\" -DBUS=\"SDIO\" -Ibuild/demo.bt_smartbridge-ISM43341_M4G_L44/resources/ -DPLATFORM=\"ISM43341_M4G_L44\" 
Lib_Ring_Buffer_CFLAGS           :=                 -mthumb -mcpu=cortex-m4    -mlittle-endian                    -DNDEBUG -ggdb -Os -Wall -fsigned-char -ffunction-sections -fdata-sections -fno-common -std=gnu11                 -DWICED_VERSION=\"3.5.2\" -DBUS=\"SDIO\" -Ibuild/demo.bt_smartbridge-ISM43341_M4G_L44/resources/ -DPLATFORM=\"ISM43341_M4G_L44\" 
Lib_crc_CFLAGS           :=                 -mthumb -mcpu=cortex-m4    -mlittle-endian                    -DNDEBUG -ggdb -Os -Wall -fsigned-char -ffunction-sections -fdata-sections -fno-common -std=gnu11                 -DWICED_VERSION=\"3.5.2\" -DBUS=\"SDIO\" -Ibuild/demo.bt_smartbridge-ISM43341_M4G_L44/resources/ -DPLATFORM=\"ISM43341_M4G_L44\" 
Lib_DHCP_Server_CFLAGS           :=                 -mthumb -mcpu=cortex-m4    -mlittle-endian                    -DNDEBUG -ggdb -Os -Wall -fsigned-char -ffunction-sections -fdata-sections -fno-common -std=gnu11                 -DWICED_VERSION=\"3.5.2\" -DBUS=\"SDIO\" -Ibuild/demo.bt_smartbridge-ISM43341_M4G_L44/resources/ -DPLATFORM=\"ISM43341_M4G_L44\" -Werror -Wstrict-prototypes  -W -Wshadow  -Wwrite-strings -pedantic -std=c99 -U__STRICT_ANSI__ -Wconversion -Wextra -Wdeclaration-after-statement -Wconversion -Waddress -Wlogical-op -Wstrict-prototypes -Wold-style-definition -Wmissing-prototypes -Wmissing-declarations -Wmissing-field-initializers -Wdouble-promotion -Wswitch-enum -Wswitch-default -Wuninitialized -Wunknown-pragmas -Wfloat-equal  -Wundef  -Wshadow 
STM32F4xx_Peripheral_Libraries_CFLAGS           :=                 -mthumb -mcpu=cortex-m4    -mlittle-endian                    -DNDEBUG -ggdb -Os -Wall -fsigned-char -ffunction-sections -fdata-sections -fno-common -std=gnu11                 -DWICED_VERSION=\"3.5.2\" -DBUS=\"SDIO\" -Ibuild/demo.bt_smartbridge-ISM43341_M4G_L44/resources/ -DPLATFORM=\"ISM43341_M4G_L44\" 
App_BT_SmartBridge_CXXFLAGS         :=                 -mthumb -mcpu=cortex-m4  -mlittle-endian                    -DNDEBUG -ggdb -Os -Wall -fsigned-char -ffunction-sections -fdata-sections -fno-common -fno-rtti -fno-exceptions  -DWICED_VERSION=\"3.5.2\" -DBUS=\"SDIO\" -Ibuild/demo.bt_smartbridge-ISM43341_M4G_L44/resources/ -DPLATFORM=\"ISM43341_M4G_L44\" 
Platform_ISM43341_CXXFLAGS         :=                 -mthumb -mcpu=cortex-m4  -mlittle-endian                    -DNDEBUG -ggdb -Os -Wall -fsigned-char -ffunction-sections -fdata-sections -fno-common -fno-rtti -fno-exceptions  -DWICED_VERSION=\"3.5.2\" -DBUS=\"SDIO\" -Ibuild/demo.bt_smartbridge-ISM43341_M4G_L44/resources/ -DPLATFORM=\"ISM43341_M4G_L44\" 
ThreadX_CXXFLAGS         :=                 -mthumb -mcpu=cortex-m4  -mlittle-endian                    -DNDEBUG -ggdb -Os -Wall -fsigned-char -ffunction-sections -fdata-sections -fno-common -fno-rtti -fno-exceptions  -DWICED_VERSION=\"3.5.2\" -DBUS=\"SDIO\" -Ibuild/demo.bt_smartbridge-ISM43341_M4G_L44/resources/ -DPLATFORM=\"ISM43341_M4G_L44\" 
WICED_CXXFLAGS         :=                 -mthumb -mcpu=cortex-m4  -mlittle-endian                    -DNDEBUG -ggdb -Os -Wall -fsigned-char -ffunction-sections -fdata-sections -fno-common -fno-rtti -fno-exceptions  -DWICED_VERSION=\"3.5.2\" -DBUS=\"SDIO\" -Ibuild/demo.bt_smartbridge-ISM43341_M4G_L44/resources/ -DPLATFORM=\"ISM43341_M4G_L44\" 
Lib_HTTP_Server_CXXFLAGS         :=                 -mthumb -mcpu=cortex-m4  -mlittle-endian                    -DNDEBUG -ggdb -Os -Wall -fsigned-char -ffunction-sections -fdata-sections -fno-common -fno-rtti -fno-exceptions  -DWICED_VERSION=\"3.5.2\" -DBUS=\"SDIO\" -Ibuild/demo.bt_smartbridge-ISM43341_M4G_L44/resources/ -DPLATFORM=\"ISM43341_M4G_L44\" 
Lib_device_configuration_CXXFLAGS         :=                 -mthumb -mcpu=cortex-m4  -mlittle-endian                    -DNDEBUG -ggdb -Os -Wall -fsigned-char -ffunction-sections -fdata-sections -fno-common -fno-rtti -fno-exceptions  -DWICED_VERSION=\"3.5.2\" -DBUS=\"SDIO\" -Ibuild/demo.bt_smartbridge-ISM43341_M4G_L44/resources/ -DPLATFORM=\"ISM43341_M4G_L44\" 
Lib_SNTP_CXXFLAGS         :=                 -mthumb -mcpu=cortex-m4  -mlittle-endian                    -DNDEBUG -ggdb -Os -Wall -fsigned-char -ffunction-sections -fdata-sections -fno-common -fno-rtti -fno-exceptions  -DWICED_VERSION=\"3.5.2\" -DBUS=\"SDIO\" -Ibuild/demo.bt_smartbridge-ISM43341_M4G_L44/resources/ -DPLATFORM=\"ISM43341_M4G_L44\" 
Lib_Gedday_CXXFLAGS         :=                 -mthumb -mcpu=cortex-m4  -mlittle-endian                    -DNDEBUG -ggdb -Os -Wall -fsigned-char -ffunction-sections -fdata-sections -fno-common -fno-rtti -fno-exceptions  -DWICED_VERSION=\"3.5.2\" -DBUS=\"SDIO\" -Ibuild/demo.bt_smartbridge-ISM43341_M4G_L44/resources/ -DPLATFORM=\"ISM43341_M4G_L44\" 
Lib_WICED_Bluetooth_Low_Energy_Driver_for_BCM43341B0_CXXFLAGS         :=                 -mthumb -mcpu=cortex-m4  -mlittle-endian                    -DNDEBUG -ggdb -Os -Wall -fsigned-char -ffunction-sections -fdata-sections -fno-common -fno-rtti -fno-exceptions  -DWICED_VERSION=\"3.5.2\" -DBUS=\"SDIO\" -Ibuild/demo.bt_smartbridge-ISM43341_M4G_L44/resources/ -DPLATFORM=\"ISM43341_M4G_L44\" 
Lib_Bluetooth_SmartBridge_CXXFLAGS         :=                 -mthumb -mcpu=cortex-m4  -mlittle-endian                    -DNDEBUG -ggdb -Os -Wall -fsigned-char -ffunction-sections -fdata-sections -fno-common -fno-rtti -fno-exceptions  -DWICED_VERSION=\"3.5.2\" -DBUS=\"SDIO\" -Ibuild/demo.bt_smartbridge-ISM43341_M4G_L44/resources/ -DPLATFORM=\"ISM43341_M4G_L44\" 
Lib_SPI_Flash_Library_ISM43341_M4G_L44_CXXFLAGS         :=                 -mthumb -mcpu=cortex-m4  -mlittle-endian                    -DNDEBUG -ggdb -Os -Wall -fsigned-char -ffunction-sections -fdata-sections -fno-common -fno-rtti -fno-exceptions  -DWICED_VERSION=\"3.5.2\" -DBUS=\"SDIO\" -Ibuild/demo.bt_smartbridge-ISM43341_M4G_L44/resources/ -DPLATFORM=\"ISM43341_M4G_L44\" 
Lib_GPIO_button_CXXFLAGS         :=                 -mthumb -mcpu=cortex-m4  -mlittle-endian                    -DNDEBUG -ggdb -Os -Wall -fsigned-char -ffunction-sections -fdata-sections -fno-common -fno-rtti -fno-exceptions  -DWICED_VERSION=\"3.5.2\" -DBUS=\"SDIO\" -Ibuild/demo.bt_smartbridge-ISM43341_M4G_L44/resources/ -DPLATFORM=\"ISM43341_M4G_L44\" 
WWD_ThreadX_Interface_CXXFLAGS         :=                 -mthumb -mcpu=cortex-m4  -mlittle-endian                    -DNDEBUG -ggdb -Os -Wall -fsigned-char -ffunction-sections -fdata-sections -fno-common -fno-rtti -fno-exceptions  -DWICED_VERSION=\"3.5.2\" -DBUS=\"SDIO\" -Ibuild/demo.bt_smartbridge-ISM43341_M4G_L44/resources/ -DPLATFORM=\"ISM43341_M4G_L44\" 
WICED_ThreadX_Interface_CXXFLAGS         :=                 -mthumb -mcpu=cortex-m4  -mlittle-endian                    -DNDEBUG -ggdb -Os -Wall -fsigned-char -ffunction-sections -fdata-sections -fno-common -fno-rtti -fno-exceptions  -DWICED_VERSION=\"3.5.2\" -DBUS=\"SDIO\" -Ibuild/demo.bt_smartbridge-ISM43341_M4G_L44/resources/ -DPLATFORM=\"ISM43341_M4G_L44\" 
WWD_for_SDIO_ThreadX_CXXFLAGS         :=                 -mthumb -mcpu=cortex-m4  -mlittle-endian                    -DNDEBUG -ggdb -Os -Wall -fsigned-char -ffunction-sections -fdata-sections -fno-common -fno-rtti -fno-exceptions  -DWICED_VERSION=\"3.5.2\" -DBUS=\"SDIO\" -Ibuild/demo.bt_smartbridge-ISM43341_M4G_L44/resources/ -DPLATFORM=\"ISM43341_M4G_L44\" 
Supplicant_BESL_CXXFLAGS         :=                 -mthumb -mcpu=cortex-m4  -mlittle-endian                    -DNDEBUG -ggdb -Os -Wall -fsigned-char -ffunction-sections -fdata-sections -fno-common -fno-rtti -fno-exceptions  -DWICED_VERSION=\"3.5.2\" -DBUS=\"SDIO\" -Ibuild/demo.bt_smartbridge-ISM43341_M4G_L44/resources/ -DPLATFORM=\"ISM43341_M4G_L44\" 
Lib_DNS_CXXFLAGS         :=                 -mthumb -mcpu=cortex-m4  -mlittle-endian                    -DNDEBUG -ggdb -Os -Wall -fsigned-char -ffunction-sections -fdata-sections -fno-common -fno-rtti -fno-exceptions  -DWICED_VERSION=\"3.5.2\" -DBUS=\"SDIO\" -Ibuild/demo.bt_smartbridge-ISM43341_M4G_L44/resources/ -DPLATFORM=\"ISM43341_M4G_L44\" 
NetX_Duo_CXXFLAGS         :=                 -mthumb -mcpu=cortex-m4  -mlittle-endian                    -DNDEBUG -ggdb -Os -Wall -fsigned-char -ffunction-sections -fdata-sections -fno-common -fno-rtti -fno-exceptions  -DWICED_VERSION=\"3.5.2\" -DBUS=\"SDIO\" -Ibuild/demo.bt_smartbridge-ISM43341_M4G_L44/resources/ -DPLATFORM=\"ISM43341_M4G_L44\" 
Lib_Wiced_RO_FS_CXXFLAGS         :=                 -mthumb -mcpu=cortex-m4  -mlittle-endian                    -DNDEBUG -ggdb -Os -Wall -fsigned-char -ffunction-sections -fdata-sections -fno-common -fno-rtti -fno-exceptions  -DWICED_VERSION=\"3.5.2\" -DBUS=\"SDIO\" -Ibuild/demo.bt_smartbridge-ISM43341_M4G_L44/resources/ -DPLATFORM=\"ISM43341_M4G_L44\" 
STM32F4xx_CXXFLAGS         :=                 -mthumb -mcpu=cortex-m4  -mlittle-endian                    -DNDEBUG -ggdb -Os -Wall -fsigned-char -ffunction-sections -fdata-sections -fno-common -fno-rtti -fno-exceptions  -DWICED_VERSION=\"3.5.2\" -DBUS=\"SDIO\" -Ibuild/demo.bt_smartbridge-ISM43341_M4G_L44/resources/ -DPLATFORM=\"ISM43341_M4G_L44\" 
Lib_Linked_List_CXXFLAGS         :=                 -mthumb -mcpu=cortex-m4  -mlittle-endian                    -DNDEBUG -ggdb -Os -Wall -fsigned-char -ffunction-sections -fdata-sections -fno-common -fno-rtti -fno-exceptions  -DWICED_VERSION=\"3.5.2\" -DBUS=\"SDIO\" -Ibuild/demo.bt_smartbridge-ISM43341_M4G_L44/resources/ -DPLATFORM=\"ISM43341_M4G_L44\" 
Lib_Simple_HTTP_Server_CXXFLAGS         :=                 -mthumb -mcpu=cortex-m4  -mlittle-endian                    -DNDEBUG -ggdb -Os -Wall -fsigned-char -ffunction-sections -fdata-sections -fno-common -fno-rtti -fno-exceptions  -DWICED_VERSION=\"3.5.2\" -DBUS=\"SDIO\" -Ibuild/demo.bt_smartbridge-ISM43341_M4G_L44/resources/ -DPLATFORM=\"ISM43341_M4G_L44\" 
Lib_DNS_Redirect_Daemon_CXXFLAGS         :=                 -mthumb -mcpu=cortex-m4  -mlittle-endian                    -DNDEBUG -ggdb -Os -Wall -fsigned-char -ffunction-sections -fdata-sections -fno-common -fno-rtti -fno-exceptions  -DWICED_VERSION=\"3.5.2\" -DBUS=\"SDIO\" -Ibuild/demo.bt_smartbridge-ISM43341_M4G_L44/resources/ -DPLATFORM=\"ISM43341_M4G_L44\" 
Lib_Bluetooth_Low_Energy_Only_Embedded_Stack_for_WICED_CXXFLAGS         :=                 -mthumb -mcpu=cortex-m4  -mlittle-endian                    -DNDEBUG -ggdb -Os -Wall -fsigned-char -ffunction-sections -fdata-sections -fno-common -fno-rtti -fno-exceptions  -DWICED_VERSION=\"3.5.2\" -DBUS=\"SDIO\" -Ibuild/demo.bt_smartbridge-ISM43341_M4G_L44/resources/ -DPLATFORM=\"ISM43341_M4G_L44\" 
Lib_TLV_CXXFLAGS         :=                 -mthumb -mcpu=cortex-m4  -mlittle-endian                    -DNDEBUG -ggdb -Os -Wall -fsigned-char -ffunction-sections -fdata-sections -fno-common -fno-rtti -fno-exceptions  -DWICED_VERSION=\"3.5.2\" -DBUS=\"SDIO\" -Ibuild/demo.bt_smartbridge-ISM43341_M4G_L44/resources/ -DPLATFORM=\"ISM43341_M4G_L44\" 
Lib_base64_CXXFLAGS         :=                 -mthumb -mcpu=cortex-m4  -mlittle-endian                    -DNDEBUG -ggdb -Os -Wall -fsigned-char -ffunction-sections -fdata-sections -fno-common -fno-rtti -fno-exceptions  -DWICED_VERSION=\"3.5.2\" -DBUS=\"SDIO\" -Ibuild/demo.bt_smartbridge-ISM43341_M4G_L44/resources/ -DPLATFORM=\"ISM43341_M4G_L44\" 
Lib_crypto_open_CXXFLAGS         :=                 -mthumb -mcpu=cortex-m4  -mlittle-endian                    -DNDEBUG -ggdb -Os -Wall -fsigned-char -ffunction-sections -fdata-sections -fno-common -fno-rtti -fno-exceptions  -DWICED_VERSION=\"3.5.2\" -DBUS=\"SDIO\" -Ibuild/demo.bt_smartbridge-ISM43341_M4G_L44/resources/ -DPLATFORM=\"ISM43341_M4G_L44\" 
Lib_micro_ecc_CXXFLAGS         :=                 -mthumb -mcpu=cortex-m4  -mlittle-endian                    -DNDEBUG -ggdb -Os -Wall -fsigned-char -ffunction-sections -fdata-sections -fno-common -fno-rtti -fno-exceptions  -DWICED_VERSION=\"3.5.2\" -DBUS=\"SDIO\" -Ibuild/demo.bt_smartbridge-ISM43341_M4G_L44/resources/ -DPLATFORM=\"ISM43341_M4G_L44\" 
WWD_NetX_Duo_Interface_CXXFLAGS         :=                 -mthumb -mcpu=cortex-m4  -mlittle-endian                    -DNDEBUG -ggdb -Os -Wall -fsigned-char -ffunction-sections -fdata-sections -fno-common -fno-rtti -fno-exceptions  -DWICED_VERSION=\"3.5.2\" -DBUS=\"SDIO\" -Ibuild/demo.bt_smartbridge-ISM43341_M4G_L44/resources/ -DPLATFORM=\"ISM43341_M4G_L44\" 
WICED_NetX_Duo_Interface_CXXFLAGS         :=                 -mthumb -mcpu=cortex-m4  -mlittle-endian                    -DNDEBUG -ggdb -Os -Wall -fsigned-char -ffunction-sections -fdata-sections -fno-common -fno-rtti -fno-exceptions  -DWICED_VERSION=\"3.5.2\" -DBUS=\"SDIO\" -Ibuild/demo.bt_smartbridge-ISM43341_M4G_L44/resources/ -DPLATFORM=\"ISM43341_M4G_L44\" 
common_GCC_CXXFLAGS         :=                 -mthumb -mcpu=cortex-m4  -mlittle-endian                    -DNDEBUG -ggdb -Os -Wall -fsigned-char -ffunction-sections -fdata-sections -fno-common -fno-rtti -fno-exceptions  -DWICED_VERSION=\"3.5.2\" -DBUS=\"SDIO\" -Ibuild/demo.bt_smartbridge-ISM43341_M4G_L44/resources/ -DPLATFORM=\"ISM43341_M4G_L44\" 
STM32F4xx_Peripheral_Drivers_CXXFLAGS         :=                 -mthumb -mcpu=cortex-m4  -mlittle-endian                    -DNDEBUG -ggdb -Os -Wall -fsigned-char -ffunction-sections -fdata-sections -fno-common -fno-rtti -fno-exceptions  -DWICED_VERSION=\"3.5.2\" -DBUS=\"SDIO\" -Ibuild/demo.bt_smartbridge-ISM43341_M4G_L44/resources/ -DPLATFORM=\"ISM43341_M4G_L44\" 
Lib_Ring_Buffer_CXXFLAGS         :=                 -mthumb -mcpu=cortex-m4  -mlittle-endian                    -DNDEBUG -ggdb -Os -Wall -fsigned-char -ffunction-sections -fdata-sections -fno-common -fno-rtti -fno-exceptions  -DWICED_VERSION=\"3.5.2\" -DBUS=\"SDIO\" -Ibuild/demo.bt_smartbridge-ISM43341_M4G_L44/resources/ -DPLATFORM=\"ISM43341_M4G_L44\" 
Lib_crc_CXXFLAGS         :=                 -mthumb -mcpu=cortex-m4  -mlittle-endian                    -DNDEBUG -ggdb -Os -Wall -fsigned-char -ffunction-sections -fdata-sections -fno-common -fno-rtti -fno-exceptions  -DWICED_VERSION=\"3.5.2\" -DBUS=\"SDIO\" -Ibuild/demo.bt_smartbridge-ISM43341_M4G_L44/resources/ -DPLATFORM=\"ISM43341_M4G_L44\" 
Lib_DHCP_Server_CXXFLAGS         :=                 -mthumb -mcpu=cortex-m4  -mlittle-endian                    -DNDEBUG -ggdb -Os -Wall -fsigned-char -ffunction-sections -fdata-sections -fno-common -fno-rtti -fno-exceptions  -DWICED_VERSION=\"3.5.2\" -DBUS=\"SDIO\" -Ibuild/demo.bt_smartbridge-ISM43341_M4G_L44/resources/ -DPLATFORM=\"ISM43341_M4G_L44\" 
STM32F4xx_Peripheral_Libraries_CXXFLAGS         :=                 -mthumb -mcpu=cortex-m4  -mlittle-endian                    -DNDEBUG -ggdb -Os -Wall -fsigned-char -ffunction-sections -fdata-sections -fno-common -fno-rtti -fno-exceptions  -DWICED_VERSION=\"3.5.2\" -DBUS=\"SDIO\" -Ibuild/demo.bt_smartbridge-ISM43341_M4G_L44/resources/ -DPLATFORM=\"ISM43341_M4G_L44\" 
App_BT_SmartBridge_ASMFLAGS         :=                 -mcpu=cortex-m4 -mfpu=softvfp                      -ggdb
Platform_ISM43341_ASMFLAGS         :=                 -mcpu=cortex-m4 -mfpu=softvfp                      -ggdb
ThreadX_ASMFLAGS         :=                 -mcpu=cortex-m4 -mfpu=softvfp                      -ggdb
WICED_ASMFLAGS         :=                 -mcpu=cortex-m4 -mfpu=softvfp                      -ggdb
Lib_HTTP_Server_ASMFLAGS         :=                 -mcpu=cortex-m4 -mfpu=softvfp                      -ggdb
Lib_device_configuration_ASMFLAGS         :=                 -mcpu=cortex-m4 -mfpu=softvfp                      -ggdb
Lib_SNTP_ASMFLAGS         :=                 -mcpu=cortex-m4 -mfpu=softvfp                      -ggdb
Lib_Gedday_ASMFLAGS         :=                 -mcpu=cortex-m4 -mfpu=softvfp                      -ggdb
Lib_WICED_Bluetooth_Low_Energy_Driver_for_BCM43341B0_ASMFLAGS         :=                 -mcpu=cortex-m4 -mfpu=softvfp                      -ggdb
Lib_Bluetooth_SmartBridge_ASMFLAGS         :=                 -mcpu=cortex-m4 -mfpu=softvfp                      -ggdb
Lib_SPI_Flash_Library_ISM43341_M4G_L44_ASMFLAGS         :=                 -mcpu=cortex-m4 -mfpu=softvfp                      -ggdb
Lib_GPIO_button_ASMFLAGS         :=                 -mcpu=cortex-m4 -mfpu=softvfp                      -ggdb
WWD_ThreadX_Interface_ASMFLAGS         :=                 -mcpu=cortex-m4 -mfpu=softvfp                      -ggdb
WICED_ThreadX_Interface_ASMFLAGS         :=                 -mcpu=cortex-m4 -mfpu=softvfp                      -ggdb
WWD_for_SDIO_ThreadX_ASMFLAGS         :=                 -mcpu=cortex-m4 -mfpu=softvfp                      -ggdb
Supplicant_BESL_ASMFLAGS         :=                 -mcpu=cortex-m4 -mfpu=softvfp                      -ggdb
Lib_DNS_ASMFLAGS         :=                 -mcpu=cortex-m4 -mfpu=softvfp                      -ggdb
NetX_Duo_ASMFLAGS         :=                 -mcpu=cortex-m4 -mfpu=softvfp                      -ggdb
Lib_Wiced_RO_FS_ASMFLAGS         :=                 -mcpu=cortex-m4 -mfpu=softvfp                      -ggdb
STM32F4xx_ASMFLAGS         :=                 -mcpu=cortex-m4 -mfpu=softvfp                      -ggdb
Lib_Linked_List_ASMFLAGS         :=                 -mcpu=cortex-m4 -mfpu=softvfp                      -ggdb
Lib_Simple_HTTP_Server_ASMFLAGS         :=                 -mcpu=cortex-m4 -mfpu=softvfp                      -ggdb
Lib_DNS_Redirect_Daemon_ASMFLAGS         :=                 -mcpu=cortex-m4 -mfpu=softvfp                      -ggdb
Lib_Bluetooth_Low_Energy_Only_Embedded_Stack_for_WICED_ASMFLAGS         :=                 -mcpu=cortex-m4 -mfpu=softvfp                      -ggdb
Lib_TLV_ASMFLAGS         :=                 -mcpu=cortex-m4 -mfpu=softvfp                      -ggdb
Lib_base64_ASMFLAGS         :=                 -mcpu=cortex-m4 -mfpu=softvfp                      -ggdb
Lib_crypto_open_ASMFLAGS         :=                 -mcpu=cortex-m4 -mfpu=softvfp                      -ggdb
Lib_micro_ecc_ASMFLAGS         :=                 -mcpu=cortex-m4 -mfpu=softvfp                      -ggdb
WWD_NetX_Duo_Interface_ASMFLAGS         :=                 -mcpu=cortex-m4 -mfpu=softvfp                      -ggdb
WICED_NetX_Duo_Interface_ASMFLAGS         :=                 -mcpu=cortex-m4 -mfpu=softvfp                      -ggdb
common_GCC_ASMFLAGS         :=                 -mcpu=cortex-m4 -mfpu=softvfp                      -ggdb
STM32F4xx_Peripheral_Drivers_ASMFLAGS         :=                 -mcpu=cortex-m4 -mfpu=softvfp                      -ggdb
Lib_Ring_Buffer_ASMFLAGS         :=                 -mcpu=cortex-m4 -mfpu=softvfp                      -ggdb
Lib_crc_ASMFLAGS         :=                 -mcpu=cortex-m4 -mfpu=softvfp                      -ggdb
Lib_DHCP_Server_ASMFLAGS         :=                 -mcpu=cortex-m4 -mfpu=softvfp                      -ggdb
STM32F4xx_Peripheral_Libraries_ASMFLAGS         :=                 -mcpu=cortex-m4 -mfpu=softvfp                      -ggdb
App_BT_SmartBridge_RESOURCES        := ./resources/apps/bt_smartbridge/smartbridge_report.html ./resources/apps/bt_smartbridge/data.html ./resources/images/brcmlogo.png ./resources/images/brcmlogo_line.png ./resources/images/favicon.ico ./resources/styles/buttons.css ./resources/scripts/general_ajax_script.js ./resources/scripts/wpad.dat
Platform_ISM43341_RESOURCES        := 
ThreadX_RESOURCES        := 
WICED_RESOURCES        := 
Lib_HTTP_Server_RESOURCES        := 
Lib_device_configuration_RESOURCES        := ./resources/images/brcmlogo.png ./resources/images/brcmlogo_line.png ./resources/images/favicon.ico ./resources/images/scan_icon.png ./resources/images/wps_icon.png ./resources/images/64_0bars.png ./resources/images/64_1bars.png ./resources/images/64_2bars.png ./resources/images/64_3bars.png ./resources/images/64_4bars.png ./resources/images/64_5bars.png ./resources/images/tick.png ./resources/images/cross.png ./resources/images/lock.png ./resources/images/progress.gif ./resources/scripts/general_ajax_script.js ./resources/scripts/wpad.dat ./resources/config/device_settings.html ./resources/config/scan_page_outer.html ./resources/config/redirect.html ./resources/styles/buttons.css 
Lib_SNTP_RESOURCES        := 
Lib_Gedday_RESOURCES        := 
Lib_WICED_Bluetooth_Low_Energy_Driver_for_BCM43341B0_RESOURCES        := 
Lib_Bluetooth_SmartBridge_RESOURCES        := 
Lib_SPI_Flash_Library_ISM43341_M4G_L44_RESOURCES        := 
Lib_GPIO_button_RESOURCES        := 
WWD_ThreadX_Interface_RESOURCES        := 
WICED_ThreadX_Interface_RESOURCES        := 
WWD_for_SDIO_ThreadX_RESOURCES        := ./resources/firmware/43341/43341B0.bin
Supplicant_BESL_RESOURCES        := 
Lib_DNS_RESOURCES        := 
NetX_Duo_RESOURCES        := 
Lib_Wiced_RO_FS_RESOURCES        := 
STM32F4xx_RESOURCES        := 
Lib_Linked_List_RESOURCES        := 
Lib_Simple_HTTP_Server_RESOURCES        := 
Lib_DNS_Redirect_Daemon_RESOURCES        := 
Lib_Bluetooth_Low_Energy_Only_Embedded_Stack_for_WICED_RESOURCES        := 
Lib_TLV_RESOURCES        := 
Lib_base64_RESOURCES        := 
Lib_crypto_open_RESOURCES        := 
Lib_micro_ecc_RESOURCES        := 
WWD_NetX_Duo_Interface_RESOURCES        := 
WICED_NetX_Duo_Interface_RESOURCES        := 
common_GCC_RESOURCES        := 
STM32F4xx_Peripheral_Drivers_RESOURCES        := 
Lib_Ring_Buffer_RESOURCES        := 
Lib_crc_RESOURCES        := 
Lib_DHCP_Server_RESOURCES        := 
STM32F4xx_Peripheral_Libraries_RESOURCES        := 
App_BT_SmartBridge_MAKEFILE         := ./apps/demo/bt_smartbridge/bt_smartbridge.mk
Platform_ISM43341_MAKEFILE         := ./platforms/ISM43341_M4G_L44/ISM43341_M4G_L44.mk
ThreadX_MAKEFILE         := ./WICED/RTOS/ThreadX/ThreadX.mk
WICED_MAKEFILE         := ././WICED/WICED.mk
Lib_HTTP_Server_MAKEFILE         := ./libraries/daemons/HTTP_server/HTTP_server.mk
Lib_device_configuration_MAKEFILE         := ./libraries/daemons/device_configuration/device_configuration.mk
Lib_SNTP_MAKEFILE         := ./libraries/protocols/SNTP/SNTP.mk
Lib_Gedday_MAKEFILE         := ./libraries/daemons/Gedday/Gedday.mk
Lib_WICED_Bluetooth_Low_Energy_Driver_for_BCM43341B0_MAKEFILE         := ./libraries/drivers/bluetooth_le/bluetooth_le.mk
Lib_Bluetooth_SmartBridge_MAKEFILE         := ./libraries/daemons/bt_smartbridge/bt_smartbridge.mk
Lib_SPI_Flash_Library_ISM43341_M4G_L44_MAKEFILE         := ./libraries/drivers/spi_flash/spi_flash.mk
Lib_GPIO_button_MAKEFILE         := ./libraries/inputs/gpio_button/gpio_button.mk
WWD_ThreadX_Interface_MAKEFILE         := ././WICED/RTOS/ThreadX/WWD/WWD.mk
WICED_ThreadX_Interface_MAKEFILE         := ././WICED/RTOS/ThreadX/WICED/WICED.mk
WWD_for_SDIO_ThreadX_MAKEFILE         := ././WICED/WWD/WWD.mk
Supplicant_BESL_MAKEFILE         := ././WICED/security/BESL/BESL.mk
Lib_DNS_MAKEFILE         := ./libraries/protocols/DNS/DNS.mk
NetX_Duo_MAKEFILE         := ./WICED/network/NetX_Duo/NetX_Duo.mk
Lib_Wiced_RO_FS_MAKEFILE         := ./libraries/filesystems/wicedfs/wicedfs.mk
STM32F4xx_MAKEFILE         := ././WICED/platform/MCU/STM32F4xx/STM32F4xx.mk
Lib_Linked_List_MAKEFILE         := ./libraries/utilities/linked_list/linked_list.mk
Lib_Simple_HTTP_Server_MAKEFILE         := ./libraries/daemons/simple_HTTP_server/simple_HTTP_server.mk
Lib_DNS_Redirect_Daemon_MAKEFILE         := ./libraries/daemons/DNS_redirect/DNS_redirect.mk
Lib_Bluetooth_Low_Energy_Only_Embedded_Stack_for_WICED_MAKEFILE         := ././libraries/drivers/bluetooth_le/BTE/BTE.mk
Lib_TLV_MAKEFILE         := ./libraries/utilities/TLV/TLV.mk
Lib_base64_MAKEFILE         := ./libraries/utilities/base64/base64.mk
Lib_crypto_open_MAKEFILE         := ./WICED/security/BESL/crypto_open/crypto_open.mk
Lib_micro_ecc_MAKEFILE         := ./libraries/crypto/micro-ecc/micro-ecc.mk
WWD_NetX_Duo_Interface_MAKEFILE         := ././WICED/network/NetX_Duo/WWD/WWD.mk
WICED_NetX_Duo_Interface_MAKEFILE         := ././WICED/network/NetX_Duo/WICED/WICED.mk
common_GCC_MAKEFILE         := ./WICED/platform/GCC/GCC.mk
STM32F4xx_Peripheral_Drivers_MAKEFILE         := ./WICED/platform/MCU/STM32F4xx/peripherals/peripherals.mk
Lib_Ring_Buffer_MAKEFILE         := ./libraries/utilities/ring_buffer/ring_buffer.mk
Lib_crc_MAKEFILE         := ./libraries/utilities/crc/crc.mk
Lib_DHCP_Server_MAKEFILE         := ./libraries/daemons/DHCP_server/DHCP_server.mk
STM32F4xx_Peripheral_Libraries_MAKEFILE         := ./WICED/platform/MCU/STM32F4xx/peripherals/libraries/libraries.mk
App_BT_SmartBridge_PRE_BUILD_TARGETS:= 
Platform_ISM43341_PRE_BUILD_TARGETS:= 
ThreadX_PRE_BUILD_TARGETS:= 
WICED_PRE_BUILD_TARGETS:= 
Lib_HTTP_Server_PRE_BUILD_TARGETS:= 
Lib_device_configuration_PRE_BUILD_TARGETS:= 
Lib_SNTP_PRE_BUILD_TARGETS:= 
Lib_Gedday_PRE_BUILD_TARGETS:= 
Lib_WICED_Bluetooth_Low_Energy_Driver_for_BCM43341B0_PRE_BUILD_TARGETS:= 
Lib_Bluetooth_SmartBridge_PRE_BUILD_TARGETS:= 
Lib_SPI_Flash_Library_ISM43341_M4G_L44_PRE_BUILD_TARGETS:= 
Lib_GPIO_button_PRE_BUILD_TARGETS:= 
WWD_ThreadX_Interface_PRE_BUILD_TARGETS:= 
WICED_ThreadX_Interface_PRE_BUILD_TARGETS:= 
WWD_for_SDIO_ThreadX_PRE_BUILD_TARGETS:= 
Supplicant_BESL_PRE_BUILD_TARGETS:= 
Lib_DNS_PRE_BUILD_TARGETS:= 
NetX_Duo_PRE_BUILD_TARGETS:= 
Lib_Wiced_RO_FS_PRE_BUILD_TARGETS:= 
STM32F4xx_PRE_BUILD_TARGETS:= 
Lib_Linked_List_PRE_BUILD_TARGETS:= 
Lib_Simple_HTTP_Server_PRE_BUILD_TARGETS:= 
Lib_DNS_Redirect_Daemon_PRE_BUILD_TARGETS:= 
Lib_Bluetooth_Low_Energy_Only_Embedded_Stack_for_WICED_PRE_BUILD_TARGETS:= 
Lib_TLV_PRE_BUILD_TARGETS:= 
Lib_base64_PRE_BUILD_TARGETS:= 
Lib_crypto_open_PRE_BUILD_TARGETS:= 
Lib_micro_ecc_PRE_BUILD_TARGETS:= 
WWD_NetX_Duo_Interface_PRE_BUILD_TARGETS:= 
WICED_NetX_Duo_Interface_PRE_BUILD_TARGETS:= 
common_GCC_PRE_BUILD_TARGETS:= 
STM32F4xx_Peripheral_Drivers_PRE_BUILD_TARGETS:= 
Lib_Ring_Buffer_PRE_BUILD_TARGETS:= 
Lib_crc_PRE_BUILD_TARGETS:= 
Lib_DHCP_Server_PRE_BUILD_TARGETS:= 
STM32F4xx_Peripheral_Libraries_PRE_BUILD_TARGETS:= 
App_BT_SmartBridge_PREBUILT_LIBRARY := 
Platform_ISM43341_PREBUILT_LIBRARY := 
ThreadX_PREBUILT_LIBRARY := ./WICED/RTOS/ThreadX/ThreadX.ARM_CM4.release.a
WICED_PREBUILT_LIBRARY := 
Lib_HTTP_Server_PREBUILT_LIBRARY := 
Lib_device_configuration_PREBUILT_LIBRARY := 
Lib_SNTP_PREBUILT_LIBRARY := 
Lib_Gedday_PREBUILT_LIBRARY := ./libraries/daemons/Gedday/Gedday.ThreadX.NetX_Duo.ARM_CM4.release.a
Lib_WICED_Bluetooth_Low_Energy_Driver_for_BCM43341B0_PREBUILT_LIBRARY := 
Lib_Bluetooth_SmartBridge_PREBUILT_LIBRARY := 
Lib_SPI_Flash_Library_ISM43341_M4G_L44_PREBUILT_LIBRARY := 
Lib_GPIO_button_PREBUILT_LIBRARY := 
WWD_ThreadX_Interface_PREBUILT_LIBRARY := 
WICED_ThreadX_Interface_PREBUILT_LIBRARY := 
WWD_for_SDIO_ThreadX_PREBUILT_LIBRARY := 
Supplicant_BESL_PREBUILT_LIBRARY := ././WICED/security/BESL/BESL.ARM_CM4.release.a
Lib_DNS_PREBUILT_LIBRARY := 
NetX_Duo_PREBUILT_LIBRARY := ./WICED/network/NetX_Duo/NetX_Duo.ThreadX.ARM_CM4.release.a
Lib_Wiced_RO_FS_PREBUILT_LIBRARY := 
STM32F4xx_PREBUILT_LIBRARY := 
Lib_Linked_List_PREBUILT_LIBRARY := 
Lib_Simple_HTTP_Server_PREBUILT_LIBRARY := 
Lib_DNS_Redirect_Daemon_PREBUILT_LIBRARY := 
Lib_Bluetooth_Low_Energy_Only_Embedded_Stack_for_WICED_PREBUILT_LIBRARY := ././libraries/drivers/bluetooth_le/BTE/BTE_LE.ThreadX.NetX_Duo.ARM_CM4.release.a
Lib_TLV_PREBUILT_LIBRARY := 
Lib_base64_PREBUILT_LIBRARY := 
Lib_crypto_open_PREBUILT_LIBRARY := 
Lib_micro_ecc_PREBUILT_LIBRARY := 
WWD_NetX_Duo_Interface_PREBUILT_LIBRARY := 
WICED_NetX_Duo_Interface_PREBUILT_LIBRARY := 
common_GCC_PREBUILT_LIBRARY := 
STM32F4xx_Peripheral_Drivers_PREBUILT_LIBRARY := 
Lib_Ring_Buffer_PREBUILT_LIBRARY := 
Lib_crc_PREBUILT_LIBRARY := 
Lib_DHCP_Server_PREBUILT_LIBRARY := 
STM32F4xx_Peripheral_Libraries_PREBUILT_LIBRARY := 
WICED_SDK_UNIT_TEST_SOURCES   :=                                      ./libraries/filesystems/wicedfs/src/unit/wicedfs_unit_images.c ./libraries/filesystems/wicedfs/src/unit/wicedfs_unit.cpp                ./WICED/security/BESL/crypto_open/unit/crypto_unit.cpp ./WICED/security/BESL/crypto_open/unit/chacha_test_vectors.c ./WICED/security/BESL/crypto_open/unit/chacha_test.c ./WICED/security/BESL/crypto_open/unit/test-curve25519.c ./WICED/security/BESL/crypto_open/ed25519/test.c                ./libraries/daemons/DHCP_server/unit/dhcp_server_unit.cpp ./libraries/daemons/DHCP_server/unit/dhcp_server_test_content.c  
APP_WWD_ONLY              := 
USES_BOOTLOADER_OTA       := 1
NODCT                     := 
ALL_RESOURCES             :=                         ./resources/apps/bt_smartbridge/data.html ./resources/apps/bt_smartbridge/smartbridge_report.html ./resources/config/device_settings.html ./resources/config/redirect.html ./resources/config/scan_page_outer.html ./resources/firmware/43341/43341B0.bin ./resources/images/64_0bars.png ./resources/images/64_1bars.png ./resources/images/64_2bars.png ./resources/images/64_3bars.png ./resources/images/64_4bars.png ./resources/images/64_5bars.png ./resources/images/brcmlogo.png ./resources/images/brcmlogo_line.png ./resources/images/cross.png ./resources/images/favicon.ico ./resources/images/lock.png ./resources/images/progress.gif ./resources/images/scan_icon.png ./resources/images/tick.png ./resources/images/wps_icon.png ./resources/scripts/general_ajax_script.js ./resources/scripts/wpad.dat ./resources/styles/buttons.css
INTERNAL_MEMORY_RESOURCES :=                         ./resources/apps/bt_smartbridge/data.html ./resources/apps/bt_smartbridge/smartbridge_report.html ./resources/config/device_settings.html ./resources/config/redirect.html ./resources/config/scan_page_outer.html ./resources/firmware/43341/43341B0.bin ./resources/images/64_0bars.png ./resources/images/64_1bars.png ./resources/images/64_2bars.png ./resources/images/64_3bars.png ./resources/images/64_4bars.png ./resources/images/64_5bars.png ./resources/images/brcmlogo.png ./resources/images/brcmlogo_line.png ./resources/images/cross.png ./resources/images/favicon.ico ./resources/images/lock.png ./resources/images/progress.gif ./resources/images/scan_icon.png ./resources/images/tick.png ./resources/images/wps_icon.png ./resources/scripts/general_ajax_script.js ./resources/scripts/wpad.dat ./resources/styles/buttons.css
EXTRA_TARGET_MAKEFILES :=   ./tools/makefiles/standard_platform_targets.mk
APPS_LUT_HEADER_LOC := 0x0000
APPS_START_SECTOR := 1 
FR_APP := 
OTA_APP := 
DCT_IMAGE := 
FILESYSTEM_IMAGE :=  
WIFI_FIRMWARE :=  
APP0 :=  
APP1 :=  
APP2 :=  
FR_APP_SECURE := 
OTA_APP_SECURE := 
WICED_ROM_SYMBOL_LIST_FILE := 
WICED_SDK_CHIP_SPECIFIC_SCRIPT :=                                    
WICED_SDK_CONVERTER_OUTPUT_FILE :=                                    
WICED_SDK_FINAL_OUTPUT_FILE :=                                    
WICED_RAM_STUB_LIST_FILE := 
DCT_IMAGE_SECURE := 
FILESYSTEM_IMAGE_SECURE := 
WIFI_FIRMWARE_SECURE := 
APP0_SECURE := 
APP1_SECURE := 
APP2_SECURE := 
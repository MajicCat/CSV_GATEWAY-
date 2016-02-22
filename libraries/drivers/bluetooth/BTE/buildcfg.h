/*
 * Copyright 2015, Broadcom Corporation
 * All Rights Reserved.
 *
 * This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom Corporation;
 * the contents of this file may not be disclosed to third parties, copied
 * or duplicated in any form, in whole or in part, without the prior
 * written permission of Broadcom Corporation.
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define BTEWICED                        TRUE
#ifndef WICED_BLUETOOTH_PLATFORM
#define WICED_BLUETOOTH_PLATFORM        TRUE
#endif
#ifndef SBC_GKI_BUFFERBASED
#define SBC_GKI_BUFFERBASED TRUE
#endif
#define SBC_IS_64_MULT_IN_QUANTIZER   FALSE
#define SBC_OPTIMIZATION        TRUE

#define BTM_INCLUDED                    TRUE                # Makefile only
#define BTM_CMD_POOL_ID                 GKI_POOL_ID_1
#define BTM_CLB_INCLUDED                FALSE
#define BTM_CLB_RX_INCLUDED             FALSE
#define BTM_TBFC_INCLUDED               FALSE
#define BTM_SCO_INCLUDED                TRUE
#define BTM_SCO_HCI_INCLUDED            TRUE
#define BTM_INQ_DB_INCLUDED             FALSE
#define BTM_BUSY_LEVEL_CHANGE_INCLUDED  FALSE
#define BTM_ALLOW_CONN_IF_NONDISCOVER   TRUE
#define BTM_MAX_REM_BD_NAME_LEN         10
#define BTM_DUMO_ADDR_CENTRAL_ENABLED   FALSE
#define BTM_APP_DEV_INIT                bte_post_reset
/* By default LINKKEY management will be handled by the application and not by BTE */
#define BTM_INTERNAL_LINKKEY_STORAGE_INCLUDED FALSE
#define BTM_BLE_PRIVACY_SPT             TRUE
#define BTM_USE_CONTROLLER_PRIVATE_ADDRESS  FALSE

#define BTU_INCLUDED                    TRUE                # Makefile only
#define BTUTHIN_INCLUDED                FALSE               # Makefile only
#define BTU_BTC_SNK_INCLUDED            FALSE
#define BTU_STACK_LITE_ENABLED          FALSE
#define BTU_DYNAMIC_CB_INCLUDED         TRUE

#define L2CAP_INCLUDED                  TRUE
#define L2CAP_CMD_POOL_ID               GKI_POOL_ID_1
#define L2CAP_FCR_INCLUDED              TRUE
#define L2CAP_UCD_INCLUDED              FALSE
#define L2CAP_WAKE_PARKED_LINK          FALSE
#define L2CAP_NON_FLUSHABLE_PB_INCLUDED FALSE
#define L2CAP_ROUND_ROBIN_CHANNEL_SERVICE   FALSE
#define L2CAP_MTU_SIZE                  ((UINT16)(HCI_ACL_POOL_BUF_SIZE - BT_HDR_SIZE - 8))     /* ACL bufsize minus BT_HDR, and l2cap/hci packet headers */
#define L2CAP_LE_COC_INCLUDED           TRUE
#define L2CAP_EXTFEA_SUPPORTED_MASK     (L2CAP_EXTFEA_ENH_RETRANS | \
                                         L2CAP_EXTFEA_STREAM_MODE | \
                                         L2CAP_EXTFEA_FIXED_CHNLS | \
                                         L2CAP_EXTFEA_NO_CRC )

#define RFCOMM_INCLUDED                 TRUE
#define RFCOMM_USE_EXTERNAL_SCN         TRUE
#define RFCOMM_CMD_POOL_ID              GKI_POOL_ID_1
#define RFCOMM_DATA_POOL_ID             GKI_POOL_ID_2
#define MAX_RFC_PORTS                   (p_btm_cfg_settings->rfcomm_cfg.max_ports)
#define MAX_BD_CONNECTIONS              (p_btm_cfg_settings->rfcomm_cfg.max_links)
#define PORT_RX_CRITICAL_WM             ((UINT32)(L2CAP_MTU_SIZE-L2CAP_MIN_OFFSET-RFCOMM_DATA_OVERHEAD)*PORT_RX_BUF_CRITICAL_WM)
#define PORT_RX_LOW_WM                  ((UINT32)(L2CAP_MTU_SIZE-L2CAP_MIN_OFFSET-RFCOMM_DATA_OVERHEAD)*PORT_RX_BUF_LOW_WM)
#define PORT_RX_HIGH_WM                 ((UINT32)(L2CAP_MTU_SIZE-L2CAP_MIN_OFFSET-RFCOMM_DATA_OVERHEAD)*PORT_RX_BUF_HIGH_WM)
#define PORT_RX_BUF_LOW_WM              2
#define PORT_RX_BUF_HIGH_WM             3
#define PORT_RX_BUF_CRITICAL_WM         5
#define PORT_TX_HIGH_WM                 ((UINT32)(L2CAP_MTU_SIZE-L2CAP_MIN_OFFSET-RFCOMM_DATA_OVERHEAD)*PORT_TX_BUF_HIGH_WM)
#define PORT_TX_CRITICAL_WM             ((UINT32)(L2CAP_MTU_SIZE-L2CAP_MIN_OFFSET-RFCOMM_DATA_OVERHEAD)*PORT_TX_BUF_CRITICAL_WM)
#define PORT_TX_BUF_HIGH_WM             3
#define PORT_TX_BUF_CRITICAL_WM         5
#define PORT_CREDIT_RX_LOW              2
#define PORT_CREDIT_RX_MAX              3

/* HID definitions */
#define HID_DEV_INCLUDED                TRUE
#define HID_DEV_MAX_DESCRIPTOR_SIZE     200
#define HID_DEV_SET_CONN_MODE           FALSE

/* AVDT/A2DP/AVRC definitions */
#define A2D_INCLUDED                    TRUE
#define A2D_SBC_INCLUDED                TRUE
#define A2D_M12_INCLUDED                TRUE
#define A2D_M24_INCLUDED                TRUE
#define AVDT_INCLUDED                   TRUE
#define AVDT_REPORTING                  FALSE
#define AVDT_MULTIPLEXING               FALSE
#define AVDT_NUM_LINKS                  (p_btm_cfg_settings->avdt_cfg.max_links)
#define AVDT_CMD_POOL_ID                GKI_POOL_ID_1
#define AVDT_DATA_POOL_ID               GKI_POOL_ID_3
#define AVDT_DATA_POOL_SIZE             GKI_BUF3_SIZE

#define AVRC_INCLUDED                   TRUE
#define AVCT_INCLUDED                   TRUE
#define AVCT_NUM_LINKS                  (p_btm_cfg_settings->avrc_cfg.max_links)
#define AVCT_NUM_CONN                   (avct_cb.num_conn)
#define AVRC_SEC_MASK                   (p_btm_cfg_settings->security_requirement_mask)
#define AVRC_CONTROL_MTU                (L2CAP_MTU_SIZE)
#define AVRC_BROWSE_MTU                 (L2CAP_MTU_SIZE)

#define BT_USE_TRACES       FALSE


#define GATT_FIXED_DB                   TRUE
#define GATTS_APPU_USE_GATT_TRACE       TRUE
#define GATT_MAX_APPS                   3
#define GATT_MAX_SR_PROFILES            3
#define GATT_MAX_PHY_CHANNEL            (GATT_CL_MAX_LCB + GATT_MAX_SCCB)
#define GATT_MAX_ATTR_LEN               (p_btm_cfg_settings->gatt_cfg.max_attr_len)
#define GATT_MAX_MTU_SIZE               517
#define GATT_CL_MAX_LCB                 (p_btm_cfg_settings->gatt_cfg.client_max_links)
#define GATT_MAX_SCCB                   (p_btm_cfg_settings->gatt_cfg.server_max_links)
#define GATTP_TRANSPORT_SUPPORTED       GATT_TRANSPORT_LE_BR_EDR
#define GATTC_NOTIF_TIMEOUT             3

#define SIM_ACCESS_INCLUDED             FALSE
#define SAP_SERVER_INCLUDED             FALSE
#define SAP_CLIENT_INCLUDED             FALSE

#define BLE_INCLUDED                    TRUE
#define SMP_INCLUDED                    TRUE
#define GAP_INCLUDED                    TRUE
#define SMP_HOST_ENCRYPT_INCLUDED       TRUE
#define SMP_LE_SC_INCLUDED              TRUE
#define SMP_LE_SC_OOB_INCLUDED          TRUE
#define ATT_DEBUG                       TRUE
#define SMP_DEBUG                       TRUE
#define BLE_BRCM_INCLUDED               TRUE
#define BLE_DATA_LEN_EXT_INCLUDED       TRUE

#define AMP_INCLUDED                    FALSE
#define GPS_INCLUDED                    FALSE

#define GKI_NUM_FIXED_BUF_POOLS         WICED_BT_CFG_NUM_BUF_POOLS
#define GKI_NUM_TOTAL_BUF_POOLS         WICED_BT_CFG_NUM_BUF_POOLS
#define GKI_BUF0_SIZE                   (p_btm_cfg_buf_pools[GKI_POOL_ID_0].buf_size)
#define GKI_BUF0_MAX                    (p_btm_cfg_buf_pools[GKI_POOL_ID_0].buf_count)
#define GKI_BUF1_SIZE                   (p_btm_cfg_buf_pools[GKI_POOL_ID_1].buf_size)
#define GKI_BUF1_MAX                    (p_btm_cfg_buf_pools[GKI_POOL_ID_1].buf_count)
#define GKI_BUF2_SIZE                   (p_btm_cfg_buf_pools[GKI_POOL_ID_2].buf_size)
#define GKI_BUF2_MAX                    (p_btm_cfg_buf_pools[GKI_POOL_ID_2].buf_count)
#define GKI_BUF3_SIZE                   (p_btm_cfg_buf_pools[GKI_POOL_ID_3].buf_size)
#define GKI_BUF3_MAX                    (p_btm_cfg_buf_pools[GKI_POOL_ID_3].buf_count)
#define GKI_DYNAMIC_POOL_CFG            TRUE
#define GKI_DYNAMIC_MEMORY              FALSE
#define GKI_USE_DYNAMIC_BUFFERS         TRUE



#define HCIC_INCLUDED                   TRUE
#define HCI_CMD_POOL_ID                 GKI_POOL_ID_1
#define HCI_CMD_POOL_BUF_SIZE           (p_btm_cfg_buf_pools[HCI_CMD_POOL_ID].buf_size)
#define HCI_ACL_POOL_ID                 GKI_POOL_ID_2
#define HCI_ACL_POOL_BUF_SIZE           (p_btm_cfg_buf_pools[HCI_ACL_POOL_ID].buf_size)
#define HCI_SCO_POOL_ID                 GKI_POOL_ID_0
#define HCI_SCO_POOL_BUF_SIZE           (p_btm_cfg_buf_pools[HCI_SCO_POOL_ID].buf_size)
#define HCI_USE_VARIABLE_SIZE_CMD_BUF   TRUE

#define HCISU_H4_INCLUDED               TRUE
#define HCILP_INCLUDED                  TRUE
#define H4IBSS_INCLUDED                 FALSE
#define H4IBSS_DEBUG                    FALSE

#define SLIP_INCLUDED                   FALSE
#define SLIP_STATIS_INCLUDED            FALSE
#define SLIP_SW_FLOW_CTRL               FALSE
#define BT_TRACE_SLIP                   FALSE
#define SLIP_SLEEP_TO                   5000
#define SLIP_HOST_SLIDING_WINDOW_SIZE   7

#define BTM_INQ_DB_SIZE                 1
#define BTM_SEC_MAX_DEVICE_RECORDS      (p_btm_cfg_settings->max_simultaneous_links)
#define BTM_SEC_HOST_PRIVACY_ADDR_RESOLUTION_TABLE_SIZE    (p_btm_cfg_settings->addr_resolution_db_size)

#define BTM_SEC_MAX_SERVICE_RECORDS     4
#define BTM_SEC_SERVICE_NAME_LEN        0
#define BTM_MAX_LOC_BD_NAME_LEN         0
#define BTM_MAX_PM_RECORDS              1
#define BTM_MAX_VSE_CALLBACKS           1
#define BTM_BLE_MAX_BG_CONN_DEV_NUM     2
#define BTM_OOB_INCLUDED                TRUE
#define BTM_BR_SC_INCLUDED              TRUE
#define BTM_CROSS_TRANSP_KEY_DERIVATION TRUE
#define BTM_PWR_MGR_INCLUDED            FALSE
#define BT_BRCM_VS_INCLUDED             TRUE

#define BTTRC_INCLUDED                  FALSE
#define BTTRC_PARSER_INCLUDED           FALSE
#define MAX_TRACE_RAM_SIZE              10

#define SDP_CLIENT_ENABLED              TRUE
#define SDP_POOL_ID                     GKI_POOL_ID_2
#define SDP_MAX_CONNECTIONS             1
#define SDP_MAX_RECORDS                 3
#define SDP_MAX_REC_ATTR                8
#define SDP_MAX_UUID_FILTERS            3
#define SDP_MAX_ATTR_FILTERS            12
#define SDP_MAX_PROTOCOL_PARAMS         2
#define SDP_RAW_DATA_SERVER             TRUE

#if (defined(BTU_DYNAMIC_CB_INCLUDED)  && (BTU_DYNAMIC_CB_INCLUDED == TRUE))
#define MAX_L2CAP_CLIENTS               (btu_cb.l2c_cfg_max_clients)
#define MAX_L2CAP_LINKS                 (btu_cb.l2c_cfg_max_links)
#define MAX_L2CAP_CHANNELS              (btu_cb.l2c_cfg_max_channels)
#define BTM_INIT_CLASS_OF_DEVICE        (p_btm_cfg_settings->device_class)

/* Connection Oriented Channel configuration */
#define MAX_L2CAP_BLE_CLIENTS           (p_btm_cfg_settings->l2cap_application.max_le_psm)
#define MAX_L2CAP_BLE_CHANNELS          (p_btm_cfg_settings->l2cap_application.max_le_channels)

#else /* BTU_DYNAMIC_CB_INCLUDED  */
#define MAX_L2CAP_CLIENTS               3
#define MAX_L2CAP_LINKS                 1
#define MAX_L2CAP_CHANNELS              4
#endif /* BTU_DYNAMIC_CB_INCLUDED */

#define GAP_CONN_INCLUDED               FALSE


#define BTU_MUTEX_INCLUDED              TRUE

#define BTE_IDLE_TASK_INCLUDED          FALSE
#define BTE_IDLE_TASK_SIZE              0xA0
#define BTE_HCI_TASK_SIZE               0x100
#define BTE_BTU_TASK_SIZE               0x400

#define HCISU_TASK                      0
#define BTU_TASK                        1
#define BTE_APPL_TASK                   2
#define GKI_MAX_TASKS                   3
/* Miscellaneous application configuration */
#define _MAX_PATH                       16
#define TICKS_PER_SEC                   1000
#define USERIAL_HCI_PORT                USERIAL_PORT_1
#define GKI_SHUTDOWN_EVT                APPL_EVT_7
#define THREAD_EVT_QUEUE_MSG_SIZE       4
#define THREAD_EVT_QUEUE_NUM_MSG        10

#define A2D_VERSION                     0x0102

#ifdef __cplusplus
} /*extern "C" */
#endif

/*
 * Copyright (c) 2014-2016 Cesanta Software Limited
 * All rights reserved
 */

#ifndef CS_FW_PLATFORMS_ESP8266_USER_V7_ESP_FEATURES_H_
#define CS_FW_PLATFORMS_ESP8266_USER_V7_ESP_FEATURES_H_

#include "fw/src/mg_features.h"

#ifndef ESP_ENABLE_HEAP_LOG
#define ESP_ENABLE_HEAP_LOG 0
#endif

#ifndef ESP_ENABLE_HW_WATCHDOG
#define ESP_ENABLE_HW_WATCHDOG 0
#endif

#ifndef ESP_ENABLE_MALLOC_TRACES
#define ESP_ENABLE_MALLOC_TRACES 0
#endif

#endif /* CS_FW_PLATFORMS_ESP8266_USER_V7_ESP_FEATURES_H_ */

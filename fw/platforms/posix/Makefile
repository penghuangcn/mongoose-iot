# -*- mode: makefile -*-
#
# Mongoose IoT for POSIX

MAKEFLAGS += --warn-undefined-variables
.DEFAULT_GOAL := all

REPO_PATH ?= ../../..
COMMON_PATH ?= $(REPO_PATH)/common
MIOT_PATH ?= $(REPO_PATH)/fw
MIOT_SRC_PATH ?= $(MIOT_PATH)/src
SRC_PATH ?= ../../src
V7_PATH ?= ../../../v7
MONGOOSE_PATH ?= ../../../mongoose
BIN_DIR ?= ./bin
BUILD_DIR ?= ./.build
KRYPTON_PATH ?= $(REPO_PATH)/krypton
FROZEN_PATH ?= $(REPO_PATH)/frozen
OUTAPP ?= mongoose-iot
VERBOSE ?= 0
VPATH = $(SRC_PATH) $(V7_PATH) $(MONGOOSE_PATH) $(FROZEN_PATH) $(COMMON_PATH) $(COMMON_PATH)/clubby
SSL ?= Krypton
DEBUG ?= 0
CC_WRAPPER ?=

# Explicitly disable updater, it's not supported on POSIX build yet.
MG_ENABLE_UPDATER_POST = 0
MG_ENABLE_UPDATER_CLUBBY = 0
MG_ENABLE_DNS_SD = 0  # Not supported on POSIX

# For FW_VERSION, COMMON_V7_FEATURES, MG_FEATURES_TINY
include $(REPO_PATH)/fw/common.mk

LDFLAGS ?=

V7_FEATURES ?= $(COMMON_V7_FEATURES) \
              -DV7_BUILD_PROFILE=3 -DV7_ENABLE__Memory__stats \
              -DV7_ENABLE_COMPACTING_GC \
              -DV7_ENABLE_FILE -DV7_MAIN -DV7_ALLOW_ARGLESS_MAIN \
              -DV7_ENABLE_ENTITY_IDS

MIOT_POSIX_FEATURES ?= -DMG_PROMPT_DISABLE_ECHO
MONGOOSE_FEATURES = \
  -DMG_USE_READ_WRITE -DMG_ENABLE_THREADS -DMG_ENABLE_THREADS \
  -DMG_ENABLE_HTTP_STREAMING_MULTIPART -DMG_DISABLE_DAV


INCLUDES = $(REPO_PATH) $(SRC_PATH) $(BUILD_DIR)
MIOT_SRCS = $(notdir $(wildcard *.c)) mg_init.c  \
            mongoose.c frozen.c mg_mongoose.c \
            mg_prompt.c mg_timers_mongoose.c mg_wifi.c \
            mg_config.c mg_sys_config.c \
            $(notdir $(SYS_CONFIG_C)) $(notdir $(SYS_RO_VARS_C)) \
            json_utils.c cs_rbuf.c mg_uart.c \
            mg_utils.c mg_console.c

ifeq "$(MG_ENABLE_JS)" "1"
MIOT_SRCS += v7.c \
            mg_adc_js.c \
            mg_clubby_js.c \
            mg_config_js.c \
            mg_console_js.c \
            mg_debug_js.c \
            mg_gpio_js.c \
            mg_http_js.c \
            mg_i2c_js.c \
            mg_init_js.c \
            mg_mqtt_js.c \
            mg_pwm_js.c \
            mg_spi_js.c \
            mg_sys_config_js.c \
            mg_tcp_udp_js.c \
            mg_timers_js.c \
            mg_v7_ext.c \
            mg_uart_js.c \
            mg_wifi_js.c \
            mg_ws_client_js.c

$(BUILD_DIR)/mongoose.o: CFLAGS += -DEXCLUDE_COMMON

MIOT_POSIX_FEATURES += -DMG_ENABLE_JS

else
  MIOT_SRCS += cs_file.c
endif

include $(MIOT_PATH)/src/features.mk

# inline causes crashes in the compacting GC
# TODO(mkm) figure out which functions are inline sensitive and annotate them
CFLAGS_EXTRA =
CFLAGS ?= -std=c99 -fno-inline -W -Wall -Wundef -Werror -g -Wno-unused-function \
          -Wno-missing-field-initializers \
          -D_DEFAULT_SOURCE \
          -D_GNU_SOURCE \
          $(V7_FEATURES) \
          $(MIOT_FEATURES) $(MIOT_POSIX_FEATURES) \
          $(MONGOOSE_FEATURES) \
          -DFW_ARCHITECTURE=\"$(BUILD_PLATFORM)\" \
          $(CFLAGS_EXTRA)

ifeq "$(DEBUG)" "1"
CFLAGS += -O0
else
CFLAGS += -O2
endif

include $(COMMON_PATH)/scripts/platform.mk

ifeq ($(OS),Windows_NT)
  BUILD_PLATFORM="WIN"
else
  UNAME_S := $(shell uname -s)
  ifeq ($(UNAME_S),Linux)
    BUILD_PLATFORM="LINUX"
  else
    BUILD_PLATFORM="POSIX"
  endif
endif

ifeq "$(SSL)" "OpenSSL"
  MONGOOSE_FEATURES += -DMG_ENABLE_SSL
  ADD_LIBS += ssl crypto
  ifeq ($(BUILD_PLATFORM), "WIN")
    ADD_LIBS += gdi32
  endif
endif

# Non Windows
ifneq ($(BUILD_PLATFORM), "WIN")
  ADD_LIBS += m pthread
endif

# Linux
ifeq ($(BUILD_PLATFORM), "LINUX")
  ADD_LIBS += rt
  MIOT_FEATURES += -DMG_ENABLE_GPIO_API \
                   -DMG_ENABLE_I2C_API
endif

# Windows
ifeq ($(BUILD_PLATFORM), "WIN")
  CFLAGS += -D_WIN32_WINNT=0x0500
  ADD_LIBS += ws2_32
endif

ifeq "$(SSL)" "Krypton"
INCLUDES += $(KRYPTON_PATH)
VPATH += $(KRYPTON_PATH)
MIOT_SRCS += krypton.c
CFLAGS += -DMG_ENABLE_SSL -DMG_DISABLE_PFS -DSSL_KRYPTON
endif

GC_CHECK ?=
ifeq ($(GC_CHECK),1)
CFLAGS += -fno-optimize-sibling-calls -fno-omit-frame-pointer -fno-inline -finstrument-functions -DV7_ENABLE_GC_CHECK
endif

ASAN ?=
ifeq ($(ASAN),1)
  CFLAGS += -fsanitize=address -fcolor-diagnostics -fno-common
  LDFLAGS += -fsanitize=address
  CC = clang-3.6
endif

INCDIRS = $(addprefix -I,$(INCLUDES))
LIBS = $(addprefix -l,$(ADD_LIBS))

APP_OBJS = $(patsubst %.c,$(BUILD_DIR)/%.o,$(MIOT_SRCS))
SYS_CONFIG_C = $(BUILD_DIR)/sys_config.c
SYS_CONFIG_DEFAULTS_JSON = $(BUILD_DIR)/sys_config_defaults.json
SYS_RO_VARS_C = $(BUILD_DIR)/sys_ro_vars.c
BUILD_INFO_C = $(BUILD_DIR)/build_info.c
OBJS = $(APP_OBJS) $(BUILD_DIR)/build_info.o
GENFILES_FLAG = $(BUILD_DIR)/genfiles.done
GENFILES_LIST = $(SYS_CONFIG_C) $(SYS_RO_VARS_C) $(V7_PATH)/v7.c

FS_FILES = $(wildcard $(SRC_PATH)/js/* $(REPO_PATH)/fw/skeleton/filesystem/*)

# Custom conf.json : by default, it is empty, so that the one from `fs` is
# used. NOTE: if the custom one is provided, the file should anyway be named
# `conf.json`
CONF_JSON ?= ""

# If custom conf.json was provided, use it
ifneq ($(CONF_JSON),"")
  # Note: instead of substituting one with another one, we separately remove
  # the old one, and add a new one, to make it work even if `fs` does not
  # contain `conf.json`.
  FS_FILES := $(subst $(SRC_PATH)/fs/conf.json,,$(FS_FILES))
  FS_FILES += $(CONF_JSON)
endif

.PHONY: all clean fs

define compile
$(vecho) "CC    $< -> $@"
$(Q) $(CC_WRAPPER) $(CC) -MD $(INCDIRS) $(CFLAGS) $1 -c $< -o $@
endef

all: $(BIN_DIR) $(BUILD_DIR) $(BIN_DIR)/$(OUTAPP) fs

include $(REPO_PATH)/fw/src/sys_config.mk

$(BIN_DIR) $(BUILD_DIR):
	$(vecho) MKDIR $@
	$(Q) mkdir -p $@

# This rule is for normal (pre-existing) sources from VPATH.
$(BUILD_DIR)/%.o: %.c $(GENFILES_FLAG)
	$(call compile,)

# This one is for generated sources in build directory.
$(BUILD_DIR)/%.o: $(BUILD_DIR)/%.c $(GENFILES_FLAG)
	$(call compile,)

$(BUILD_DIR)/build_info.o: $(BUILD_INFO_C)
	$(call compile,)

# Common gathering point for all generated files.
# Except build info, which is special because it depends on objects.
$(GENFILES_FLAG): $(GENFILES_LIST)
	$(Q) touch $@

# Generate build info. Only regenerates if there are changes in objects.
include $(REPO_PATH)/common/scripts/build_info.mk

$(BUILD_INFO_C): $(OBJS)
	$(call gen_build_info,$@,,,,,$(BUILD_INFO_C),)

-include $(wildcard $(BUILD_DIR)/*.d)

$(V7_PATH)/v7.c:
	@make -C $(V7_PATH) v7.c

$(BIN_DIR)/$(OUTAPP): $(BIN_DIR) $(V7_PATH)/v7.c $(OBJS)
	@echo "LD $@"
	$(Q) $(CC_WRAPPER) $(CC) $(OBJS) $(LIBS) $(LDFLAGS) -o $@

# After file copy, change default HTTP port to 9080
fs: $(BIN_DIR) $(FS_FILES) $(SYS_CONFIG_C)
	@echo "MKFS $@"
	$(Q) cp $(FS_FILES) $(BIN_DIR)
	$(Q) cp $(SYS_CONFIG_DEFAULTS_JSON) $(BIN_DIR)/conf_defaults.json
	$(Q) perl -pi"" -e 's/"80"/"9080"/' $(BIN_DIR)/conf_defaults.json

clean:
	$(Q) rm -rf $(BIN_DIR) $(BUILD_DIR)

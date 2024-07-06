# ----------------------------
# Makefile Options
# ----------------------------

NAME = Expanse
ICON = icon.png
DESCRIPTION = Real-time Space Combat Simulator

APP_NAME = Expanse
APP_VERSION = 0

CFLAGS = -Wall -Wextra -Oz -I src/include
CXXFLAGS = -Wall -Wextra -Oz -Isrc/include
OUTPUT_MAP = YES

# ----------------------------

include app_tools/makefile

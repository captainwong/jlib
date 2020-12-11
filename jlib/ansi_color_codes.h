#pragma once

#define CSI                 "\x1B["     // Control Sequence Introducer (ANSI spec name)
#define CLEAR               CSI "0m"    // all attributes off
#define BRIGHT_BLACK_S      CSI "0;90m" // somewhat MD.GRAY
#define BLACK_S             CSI "0;30m"
#define BLACK_BOLD_S        CSI "1;30m" // another name for GRAY
#define RED_S               CSI "0;31m"
#define RED_BOLD_S          CSI "1;31m"
#define GREEN_S             CSI "0;32m"
#define GREEN_BOLD_S        CSI "1;32m"
#define YELLOW_S            CSI "0;33m"
#define YELLOW_BOLD_S       CSI "1;33m"
#define BLUE_S              CSI "0;34m"
#define BLUE_BOLD_S         CSI "1;34m"
#define MAGENTA_S           CSI "0;35m"
#define MAGENTA_BOLD_S      CSI "1;35m"
#define CYAN_S              CSI "0;36m"
#define CYAN_BOLD_S         CSI "1;36m"
#define WHITE_S             CSI "0;37m" // another name for LT.GRAY
#define WHITE_BOLD_S        CSI "1;37m" // actually white

#define GREEN_BG_BOLD_S     CSI "42;1m"
#define YELLOW_BG_BOLD_S    CSI "43;1m"
#define BLUE_BG_S           CSI "44m"
#define BLUE_BG_BOLD_S      CSI "44;1m"
#define MAGENTA_BG_S        CSI "45m"
#define MAGENTA_BG_BOLD_S   CSI "45;1m"
#define CYAN_BG_S           CSI "46m"
#define CYAN_BG_BOLD_S      CSI "46;1m"

//color wrappings
#define BLACK(x)            BLACK_S x CLEAR
#define BLACK_BOLD(x)       BLACK_BOLD_S x CLEAR
#define RED(x)              RED_S x CLEAR
#define RED_BOLD(x)         RED_BOLD_S x CLEAR
#define GREEN(x)            GREEN_S x CLEAR
#define GREEN_BOLD(x)       GREEN_BOLD_S x CLEAR
#define YELLOW(x)           YELLOW_S x CLEAR
#define YELLOW_BOLD(x)      YELLOW_BOLD_S x CLEAR
#define BLUE(x)             BLUE_S x CLEAR
#define BLUE_BOLD(x)        BLUE_BOLD_S x CLEAR
#define MAGENTA(x)          MAGENTA_S x CLEAR
#define MAGENTA_BOLD(x)     MAGENTA_BOLD_S x CLEAR
#define CYAN(x)             CYAN_S x CLEAR
#define CYAN_BOLD(x)        CYAN_BOLD_S x CLEAR
#define WHITE(x)            WHITE_S x CLEAR
#define WHITE_BOLD(x)       WHITE_BOLD_S x CLEAR

#define GREEN_BG_BOLD(x)    GREEN_BG_BOLD_S x CLEAR
#define YELLOW_BG_BOLD(x)   YELLOW_BG_BOLD_S x CLEAR
#define BLUE_BG(x)          BLUE_BG_S x CLEAR
#define BLUE_BG_BOLD(x)     BLUE_BG_BOLD_S x CLEAR
#define MAGENTA_BG(x)       MAGENTA_BG_S x CLEAR
#define MAGENTA_BG_BOLD(x)  MAGENTA_BG_BOLD_S x CLEAR
#define CYAN_BG(x)          CYAN_BG_S x CLEAR
#define CYAN_BG_BOLD(x)     CYAN_BG_BOLD_S x CLEAR

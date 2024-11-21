"""

Program history
24.07.2024    V. 01.00    init Version printFormating modul

printFormating modul with
class used as enumerator classes
  - Base - Base coloring
  - ANSI_Compatible - ANSI combatible coloring
  - Formatting - Text Formatting
  - GColor - Gnome Color
  - Color - normal Coloring
and functions
  - printCol - alternate print function with color or formating text
"""

class Base:
    """
    Base enum class
      - Base Color enumations
    """
    # Foreground:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    # Formatting
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'    
    # End colored text
    END = '\033[0m'
    NC ='\x1b[0m' # No Color

class ANSI_Compatible:
    """
    ANSI compatible class
      - use function Color
    """
    # If Foreground is False that means color effect on Background
    def Color(ColorNo, Foreground=True): # 0 - 255
        """
        create string from color number
        Color(ColorNo, Foreground=True)
          - input value
            ColorNo - Color number : int
            Foreground - : bool
        """
        FB_G = 48 # Effect on background
        if Foreground:
            FB_G = 38 # Effect on foreground
        return '\x1b[' + str(FB_G) + ';5;' + str(ColorNo) + 'm'

class Formatting:
    """
    Formatting enum class
      - Formatting enumations with
        on and off strings
    """
    Bold = "\x1b[1m"
    Dim = "\x1b[2m"
    Italic = "\x1b[3m"
    Underlined = "\x1b[4m"
    Blink = "\x1b[5m"
    Reverse = "\x1b[7m"
    Hidden = "\x1b[8m"
    # Reset part
    Reset = "\x1b[0m"
    Reset_Bold = "\x1b[21m"
    Reset_Dim = "\x1b[22m"
    Reset_Italic = "\x1b[23m"
    Reset_Underlined = "\x1b[24"
    Reset_Blink = "\x1b[25m"
    Reset_Reverse = "\x1b[27m"
    Reset_Hidden = "\x1b[28m"

class GColor: # Gnome supported
    """
    GColor class
      - use function RGB
    """
    # If Foreground is False that means color effect on Background
    def RGB(R, G, B, Foreground=True): # R: 0-255  ,  G: 0-255  ,  B: 0-255
        """
        create string from RGB color number
        RGB(R,G,B, Foreground=True)
          - input value
            R - Red Color number : int [0..255]
            G - Green Color number : int [0..255]
            B - Blue Color number : int [0..255]
            Foreground : bool
        """

        FB_G = 48 # Effect on background
        if Foreground:
            FB_G = 38 # Effect on foreground
        return "\x1b[" + str(FB_G) + ";2;" + str(R) + ";" + str(G) + ";" + str(B) + "m"

class Color:
    """
    Color enum class
      - Color enumations with Fore- and
        Background colors
    """
    # Foreground
    F_Default = "\x1b[39m"
    F_Black = "\x1b[30m"
    F_Red = "\x1b[31m"
    F_Green = "\x1b[32m"
    F_Yellow = "\x1b[33m"
    F_Blue = "\x1b[34m"
    F_Magenta = "\x1b[35m"
    F_Cyan = "\x1b[36m"
    F_LightGray = "\x1b[37m"
    F_DarkGray = "\x1b[90m"
    F_LightRed = "\x1b[91m"
    F_LightGreen = "\x1b[92m"
    F_LightYellow = "\x1b[93m"
    F_LightBlue = "\x1b[94m"
    F_LightMagenta = "\x1b[95m"
    F_LightCyan = "\x1b[96m"
    F_White = "\x1b[97m"
    # Background
    B_Default = "\x1b[49m"
    B_Black = "\x1b[40m"
    B_Red = "\x1b[41m"
    B_Green = "\x1b[42m"
    B_Yellow = "\x1b[43m"
    B_Blue = "\x1b[44m"
    B_Magenta = "\x1b[45m"
    B_Cyan = "\x1b[46m"
    B_LightGray = "\x1b[47m"
    B_DarkGray = "\x1b[100m"
    B_LightRed = "\x1b[101m"
    B_LightGreen = "\x1b[102m"
    B_LightYellow = "\x1b[103m"
    B_LightBlue = "\x1b[104m"
    B_LightMagenta = "\x1b[105m"
    B_LightCyan = "\x1b[106m"
    B_White = "\x1b[107m"
    
def printCol(strIn:str,Code:str):
    """
    print a string with a formating code
    printCol(strIn:str,Code:str)
      - input value
        R - Red Color number : int [0..255]
        G - Green Color number : int [0..255]
        B - Blue Color number : int [0..255]
        Foreground : bool
    """
    END = '\033[0m'
    print(Code+strIn+END)
    
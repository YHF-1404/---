SET PATH=E:\Keil_v5\ARM\ARMCC\Bin;C:\Program Files (x86)\Common Files\Oracle\Java\javapath;C:\WINDOWS\system32;C:\WINDOWS;C:\WINDOWS\System32\Wbem;C:\WINDOWS\System32\WindowsPowerShell\v1.0\;C:\WINDOWS\System32\OpenSSH\;C:\Program Files (x86)\STMicroelectronics\STM32 ST-LINK Utility\ST-LINK Utility;G:\Programs\Python\Python37-32\Scripts\;G:\Programs\Python\Python37-32\;E:\Programs\Microsoft VS Code\bin;C:\MinGW\bin;C:\Program Files (x86)\GNU Tools Arm Embedded\7 2018-q2-update\bin;C:\Users\YHF\AppData\Roaming\Python\Python37\Scripts;E:\Espressif\Esp32\msys32\mingw32\bin\;D:\Program Files\MATLAB\R2021a\bin;E:\Program Files (x86)\GNU Tools ARM Embedded\5.4 2016q3\bin;G:\Programs\Python\Python37-32\Scripts\;G:\Programs\Python\Python37-32\;C:\Users\YHF\AppData\Local\Microsoft\WindowsApps;E:\Programs\Microsoft VS Code\bin;C:\MinGW\bin;C:\Program Files (x86)\GNU Tools Arm Embedded\7 2018-q2-update\bin;C:\Users\YHF\AppData\Roaming\Python\Python37\Scripts;E:\Espressif\Esp32\msys32\mingw32\bin;E:\Espressif\Esp32\msys32\opt\xtensa-lx106-elf\bin;E:\Program Files\Git\bin;
SET CPU_TYPE=STM32H750VBTx
SET CPU_VENDOR=STMicroelectronics
SET UV2_TARGET=TCB_V1
SET CPU_CLOCK=0x0E4E1C00
"E:\Keil_v5\ARM\ARMCC\Bin\ArmAsm" --Via ".\tcb_v1\startup_stm32h750xx._ia"
"E:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\tcb_v1\lcd_rgb.__i"
"E:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\tcb_v1\touch_480x272.__i"
"E:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\tcb_v1\touch_iic.__i"
"E:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\tcb_v1\uart.__i"
"E:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\tcb_v1\main.__i"
"E:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\tcb_v1\gpio.__i"
"E:\Keil_v5\ARM\ARMCC\Bin\ArmLink" --Via ".\TCB_V1\out.lnp"
"E:\Keil_v5\ARM\ARMCC\Bin\fromelf.exe" ".\TCB_V1\out.axf" --i32combined --output ".\TCB_V1\out.hex"

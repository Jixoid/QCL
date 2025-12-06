#
#  This file is part of QAOS.
#
#  This file is licensed under the GNU Lesser General Public License version 3 (LGPLv3).
#
#  You should have received a copy of GNU Lesser General Public License
#  along with QAOS. If not, see <https://www.gnu.org/licenses/>.
#
#  Copyright (c) 2025 by Kadir Aydın.
#




# Colors

GRAY = \e[1;30m
GREEN = \e[1;32m
BLUE = \e[1;34m
ORANGE = \e[1;33m
RED = \e[1;91m
RESET = \e[0m


# Compile

_O_Level = -O2

CONFIG_Arch_X64=y

ifeq ($(CONFIG_Arch_X64), y)
  Arch = x64
  CTarget = x86_64
  PTarget = x86_64
else ifeq ($(CONFIG_Arch_X32), y)
  Arch = x32
  CTarget = i386
  PTarget = i386

else ifeq ($(CONFIG_Arch_A64), y)
  Arch = a64
  CTarget = aarch64
  PTarget = aarch64
else ifeq ($(CONFIG_Arch_A32), y)
  Arch = a32
  CTarget = arm
  PTarget = arm

else ifeq ($(CONFIG_Arch_R64), y)
  Arch = r64
  CTarget = riscv64
  PTarget = riscv64
else ifeq ($(CONFIG_Arch_R32), y)
  Arch = r32
  CTarget = riscv32
  PTarget = riscv32

else ifeq ($(CONFIG_Arch_P64), y)
  Arch = p64
  CTarget = powerpc64le
  PTarget = powerpc64le
endif



# Params

Locate = /home/alforce/Projeler/QCL

GOut = !Out
Out = $(GOut)/$(shell echo -n "linux-gnu-$(Arch)-$(_O_Level)" | sha256sum | cut -d' ' -f1 | cut -c1-10)
Tmp = $(Out)/Tmp
Bin = $(Out)/Bin
Lib = $(Out)/Lib
Nuc = $(Out)/Nuc


CArg = -fPIC \
	-Wno-unused-result \
  -Wno-unused-command-line-argument \


CC = clang    $(CArg) $(CONFIG_CC_Params) -std=c23
CX = clang++  $(CArg) $(CONFIG_CX_Params) -std=c++23

AR = $(CTarget)-linux-gnu-ar $(CONFIG_AR_Params)
LD = $(CTarget)-linux-gnu-ld $(CONFIG_LD_Params)

MQ  = mksquashfs
MQ2 = $(Out)/Base.moq -quiet -comp xz -noappend; echo -ne "\033[A\033[2K"

SHA = openssl dgst -sha512 -binary $(Out)/Base.moq > $(Out)/Base.moq.sha512
SIG = openssl pkeyutl -sign -in $(Out)/Base.moq.sha512 -inkey $(Locate)/!Dev/Key_priv.pem -out $(Out)/Base.sig


L_CC = "    ⚙️ $(GRAY)CC$(RESET)"
L_CX = "    ⚙️ $(GRAY)CX$(RESET)"

L_AR = "    🔗 $(GRAY)AR$(RESET)"
L_LD = "    🔗 $(GRAY)LD$(RESET)"

L_MQ = "    📦️ $(GRAY)MQ$(RESET)"
L_SG = "    ✒️ $(GRAY)SG$(RESET)"

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



include ../Make.inc


# Config

Module = $(notdir $(CURDIR))

Srcs = \
	Dev/Main.cc

Dirs = \
	../lib.qaos.basis.dev/Inc \
	../lib.qaos.qcl.dev/Inc \
	../lib.qaos.qcl.qstd.dev/Inc \
	../lib.qaos.jconf.dev/Inc


Links = \
	../lib.qaos.qcl.qstd/$(Lib)/libQCL_qstd.a \
	../lib.qaos.qcl.opengl/$(Lib)/libQCL_qgl.a \
	../lib.qaos.qcl.x11/$(Lib)/libQCL_platform.a \
	../lib.qaos.qcl/$(Lib)/libQCL.a \
	../lib.qaos.jconf/$(Lib)/libJConf.a \
	../lib.qaos.qcl.x11/Lib/libresvg.a \
	-lcairo -lX11 -lGL -lGLU -lGLEW


Incs = $(foreach dir,$(Dirs),-I$(dir))

Deps = $(foreach dir,$(Dirs),$(wildcard $(dir)/*.hh))

Objs = $(addprefix $(Tmp)/, $(basename $(notdir $(Srcs))))
Objs:= $(addsuffix .o, $(Objs))




# Targets

Build: Basis $(Out)/Base.moqs


Basis:
	@mkdir -p $(Out)
	@mkdir -p $(Tmp)
	@mkdir -p $(Bin)


Clean:
	@rm -rf $(Out)



# Files
$(Out)/Base.moqs: $(Out)/Base.moq
	@echo -e $(L_SG) "Base.moqs $(GRAY)(Base.moq Base.sig)$(RESET)"
	@$(SHA)
	@$(SIG)
	@rm -f $(Out)/Base.moqs
	@tar -cf $(Out)/Base.moqs  -C $(Out) Base.moq Base.sig


$(Out)/Base.moq: Package.conf $(Bin)/Main.elf
	@echo -e $(L_MQ) "Base.moq"
	@$(MQ) Package.conf $(Bin) \
		$(MQ2)



$(Bin)/Main.elf: $(Objs) $(Links)
	@echo -e $(L_LD) "Main.elf $(GRAY)($(notdir $^))$(RESET)"
	@$(CX) $(Objs) $(Links) -o $@



$(Tmp)/%.o: */%.cc $(Deps)
	@echo -e $(L_CX) "$(notdir $@) $(GRAY)($(notdir $<))$(RESET)"
	@$(CX) -c -DModule=\"$(Module)\" $(Incs) $< \
		-o $@


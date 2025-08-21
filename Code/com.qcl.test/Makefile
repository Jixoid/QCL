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



include ../../Make.inc


# Config

Module = $(notdir $(CURDIR))

Srcs = \
	Dev/Main.cc

Dirs = \
	$(DEV)/lib.qaos.basis.dev/Inc \
	$(DEV)/lib.qaos.qcl.dev/Inc \
	$(DEV)/lib.qaos.qcl.qstd.dev/Inc \
	$(DEV)/lib.qaos.jconf.dev/Inc


Links = \
	$(COD)/lib.qaos.qcl/$(Lib)/libQCL.a \
	$(COD)/lib.qaos.qcl.qstd/$(Lib)/libQCL_qstd.a \
	$(COD)/lib.qaos.qcl.x11/$(Lib)/libQCL_x11.a \
	$(COD)/lib.qaos.jconf/$(Lib)/libJConf.a \
	-lcairo -lX11


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



$(Bin)/Main.elf: $(Objs)
	@echo -e $(L_LD) "Main.elf $(GRAY)($(notdir $^))$(RESET)"
	@$(CP) -o $(Bin)/Main.elf \
		$(Objs) $(Links)



$(Tmp)/%.o: */%.cc $(Deps)
	@echo -e $(L_CP) "$(notdir $@) $(GRAY)($(notdir $<))$(RESET)"
	@$(CP) -c -DModule=\"$(Module)\" $(Incs) $< \
		-o $@


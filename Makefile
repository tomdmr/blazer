#
#
MYIP        = 192.168.1.13

ESPOTA   = python3 /home/tom/.arduino15/packages/esp32/hardware/esp32/1.0.6/tools/espota.py
MKSPIFFS = ~/gits/mkspiffs/mkspiffs
HASH     = ~/c/djb2
OTABIN   = arduinio/sketch_web_sockets/sketch_web_sockets.ino.esp32.bin 



OTASRC      = $(wildcard arduinio/sketch_web_sockets/*.ino arduinio/sketch_web_sockets/config.h)

JSC_SRC     = $(wildcard www/*.js)
CSS_SRC     = $(wildcard www/*.css)
HTM_SRC     = $(wildcard www/*.html)

ALL_WWW_SRC = $(JSC_SRC) $(CSS_SRC) $(HTM_SRC)
CSS_HASH  = $(foreach name, $(CSS_SRC), $(shell $(HASH) /$(patsubst www/%,%,$(name))).css)
JSC_HASH  = $(foreach name, $(JSC_SRC), $(shell $(HASH) /$(patsubst www/%,%,$(name))).js)
HTM_HASH  = $(foreach name, $(HTM_SRC), $(shell $(HASH) /$(patsubst www/%,%,$(name))).html)
ALL_WWW_HASH = $(CSS_HASH) $(JSC_HASH) $(HTM_HASH)

SPIFFS    = spiffs.bin
SPIFFSDIR = data


## $(warning input is $(1) end)
## $(warning dst   is $(SPIFFSDIR)/$(shell $(HASH) /$(patsubst www/%,%,$(1)))$(suffix $(1)) end)

define MKHASHES_template

ifeq ($(suffix $(1)), .css)
$(SPIFFSDIR)/$(shell $(HASH) /$(patsubst www/%,%,$(1)))$(suffix $(1)): $(1)
	@echo css $$^ to $$@
	curl -X POST -s --data-urlencode 'input@$$^' https://cssminifier.com/raw |gzip -9c > $$@
endif

ifeq ($(suffix $(1)), .js)
$(SPIFFSDIR)/$(shell $(HASH) /$(patsubst www/%,%,$(1)))$(suffix $(1)): $(1)
	@echo js $$^ to $$@
	curl -X POST -s --data-urlencode 'input@$$^' https://javascript-minifier.com/raw |gzip -9c > $$@
endif

ifeq ($(suffix $(1)), .html)
$(SPIFFSDIR)/$(shell $(HASH) /$(patsubst www/%,%,$(1)))$(suffix $(1)): $(1)
	@echo html $$^ to $$@
	@tmp=`mktemp` && ./doHTML $$^ > $$tmp ; \
	curl -X POST -s --data-urlencode "input@$$tmp" https://html-minifier.com/raw |gzip -9c > $$@; \
	rm $$tmp
endif

endef
$(foreach dep, $(ALL_WWW_SRC), $(eval $(call MKHASHES_template,$(dep))))

#
# $(warning dst   is $(SPIFFSDIR)/$(shell $(HASH) /$(1))$(suffix $(1)))
# $(SPIFFSDIR)/$(shell $(HASH) /$(1))$(suffix $(1)): $(addprefix tmp/,$(addsuffix .gz, $(1)))
#	cp $$^ $$@
#
#
#


$(SPIFFS): $(addprefix data/, $(ALL_WWW_HASH))
	$(MKSPIFFS) -c $(SPIFFSDIR) -b 4096 -p 256 -s 1507328  $@

$(OTABIN): $(OTASRC)
	echo recompile!

all: $(OTABIN) $(SPIFFS)

esp00: $(OTABIN)
	$(ESPOTA) -i $@ -I $(MYIP) -p 3232 -f $(OTABIN)
esp01: $(OTABIN)
	$(ESPOTA) -i $@ -I $(MYIP) -p 3232 -f $(OTABIN)
esp02: $(OTABIN)
	$(ESPOTA) -i $@ -I $(MYIP) -p 3232 -f $(OTABIN)
esp03: $(OTABIN)
	$(ESPOTA) -i $@ -I $(MYIP) -p 3232 -f $(OTABIN)
esp04: $(OTABIN)
	$(ESPOTA) -i $@ -I $(MYIP) -p 3232 -f $(OTABIN)

esp00.spif: $(SPIFFS)
	$(ESPOTA) -i $(basename $@) -I $(MYIP) -p 3232 -s -f $(SPIFFS)
esp01.spif: $(SPIFFS)
	$(ESPOTA) -i $(basename $@) -I $(MYIP) -p 3232 -s -f $(SPIFFS)
esp02.spif: $(SPIFFS)
	$(ESPOTA) -i $(basename $@) -I $(MYIP) -p 3232 -s -f $(SPIFFS)
esp03.spif: $(SPIFFS)
	$(ESPOTA) -i $(basename $@) -I $(MYIP) -p 3232 -s -f $(SPIFFS)
esp04.spif: $(SPIFFS)
	$(ESPOTA) -i $(basename $@) -I $(MYIP) -p 3232 -s -f $(SPIFFS)


$(DST): $(GZ_SRC_CSS) $(GZ_SRC_JS) $(GZ_SRC_HTM)
	@echo rm $(DST)
	@echo "#ifndef _HTTP_GZ_H_\n#define _HTTP_GZ_H_" > $(DST)
	@for f in $^; do \
		xxd -i $$f | \
		perl -pe 's/tmp_//;s/unsigned char/const uint8_t/;s/ =/ PROGMEM=/;s/unsigned int/const uint16_t/' >> $(DST); \
	done
	@echo "#endif\n"  >> $(DST)

# 	@echo "const dict[] = \n"
#	@for f in $^; do \
#		echo $$f ;\
#	done
#
clean:
	rm -f $(DST) $(GZ_SRC_CSS) $(GZ_SRC_JS) $(GZ_SRC_HTM)
	rm -f .depend .all
	find . -name \*~\* -print0 | xargs -0 rm -f

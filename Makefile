#
# Minifier:
# Javascript:
# curl -X POST -s --data-urlencode 'input@ready.js' https://javascript-minifier.com/raw > ready.min.js
# HTML:
# curl -X POST -s --data-urlencode 'input@index.html' https://html-minifier.com/raw > html/index.html
# CSS:
# curl -X POST -s --data-urlencode 'input@style.css' https://cssminifier.com/raw > style.min.css
#
DST = arduinio/sketch_web_sockets/http_gz.h
all: $(DST) 

SRC_CSS  = blazer.css
SRC_JS   = blazer2.js
SRC_HTM  = portal.html demo01.html demo02.html demo03.html

GZ_SRC_CSS = $(addprefix tmp/,$(addsuffix .gz, $(SRC_CSS)))
GZ_SRC_JS  = $(addprefix tmp/,$(addsuffix .gz, $(SRC_JS)))
GZ_SRC_HTM = $(addprefix tmp/,$(addsuffix .gz, $(SRC_HTM)))

GZ_LIST=$(addsuffix _gz, $(subst .,_,$(SRC_CSS) $(SRC_JS) $(SRC_HTM)))

check:
	echo $(GZ_LIST)

ESPOTA = python3 /home/tom/.arduino15/packages/esp32/hardware/esp32/1.0.6/tools/espota.py
OTABIN = arduinio/sketch_web_sockets/sketch_web_sockets.ino.firebeetle32.bin

esp00:
	$(ESPOTA) -i $@ -I 192.168.178.13 -p 3232 -P 3232 -f $(OTABIN)
esp01:
	$(ESPOTA) -i $@ -I 192.168.178.13 -p 3232 -P 3232 -f $(OTABIN)
esp02:
	$(ESPOTA) -i $@ -I 192.168.178.13 -p 3232 -P 3232 -f $(OTABIN)
esp03:
	$(ESPOTA) -i $@ -I 192.168.178.13 -p 3232 -P 3232 -f $(OTABIN)
esp04:
	$(ESPOTA) -i $@ -I 192.168.178.13 -p 3232 -P 3232 -f $(OTABIN)

tmp/%.css.gz: %.css
	curl -X POST -s --data-urlencode 'input@$^' https://cssminifier.com/raw |gzip -9c > $@

tmp/%.js.gz: %.js
	curl -X POST -s --data-urlencode 'input@$^' https://javascript-minifier.com/raw |gzip -9c > $@
#
# Cut <script> </script>
# Send script to javamin
#
tmp/%.html.gz: %.html
	@tmp=`mktemp` && ./doHTML $^ > $$tmp ; \
	curl -X POST -s --data-urlencode "input@$$tmp" https://html-minifier.com/raw |gzip -9c > $@; \
	rm $$tmp

$(DST): $(GZ_SRC_CSS) $(GZ_SRC_JS) $(GZ_SRC_HTM)
	@echo rm $(DST)
	@echo "#ifndef _HTTP_GZ_H_\n#define _HTTP_GZ_H_" > $(DST)
	@for f in $^; do \
		xxd -i $$f | \
		perl -pe 's/tmp_//;s/unsigned char/const uint8_t/;s/ =/ PROGMEM=/;s/unsigned int/const uint16_t/' >> $(DST); \
	done
	@echo "const dict[] = \n"
	@for f in $^; do \
		echo $$f ;\
	done
	@echo "#endif\n"  >> $(DST)

clean:
	rm -f $(DST) $(GZ_SRC_CSS) $(GZ_SRC_JS) $(GZ_SRC_HTM)
	rm -f .depend .all
	find . -name \*~\* -print0 | xargs -0 rm -f

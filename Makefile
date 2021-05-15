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
default: $(DST) 

SRC_CSS  = blazer.css
SRC_JS   = blazer2.js
SRC_HTM  = portal.html demo01.html demo02.html

GZ_SRC_CSS = $(addprefix tmp/,$(addsuffix .gz, $(SRC_CSS)))
GZ_SRC_JS  = $(addprefix tmp/,$(addsuffix .gz, $(SRC_JS)))
GZ_SRC_HTM = $(addprefix tmp/,$(addsuffix .gz, $(SRC_HTM)))

$(GZ_SRC_CSS): $(SRC_CSS)
	curl -X POST -s --data-urlencode 'input@$(notdir $(@:.gz=))' https://cssminifier.com/raw |gzip -9c > $@

$(GZ_SRC_JS): $(SRC_JS)
	curl -X POST -s --data-urlencode 'input@$(notdir $(@:.gz=))' https://javascript-minifier.com/raw |gzip -9c > $@
$(GZ_SRC_HTM): $(SRC_HTM)
	curl -X POST -s --data-urlencode 'input@$(notdir $(@:.gz=))' https://html-minifier.com/raw |gzip -9c > $@


$(DST): $(GZ_SRC_CSS) $(GZ_SRC_JS) $(GZ_SRC_HTM)
	@echo rm $(DST)
	@echo "#ifndef _HTTP_GZ_H_\n#define _HTTP_GZ_H_" > $(DST)
	@for f in $^; do \
		xxd -i $$f | \
		perl -pe 's/tmp_//;s/unsigned char/const uint8_t/;s/ =/ PROGMEM=/;s/unsigned int/const uint16_t/' >> $(DST); \
	done
	@echo "#endif\n"  >> $(DST)

clean:
	rm -f $(DST) $(GZ_SRC_CSS) $(GZ_SRC_JS) $(GZ_SRC_HTM)
	rm -f .depend .all
	find . -name \*~\* -print0 | xargs -0 rm -f

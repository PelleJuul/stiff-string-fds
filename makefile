HEADERS=$(wildcard pal-fds/*.h)
DOCS=$(patsubst pal-fds/%.h, docs/%.h.md, $(HEADERS))

.PHONY: clean examples documentation

examples:
	make -C examples/ all

documentation: $(DOCS) docs/

docs/:
	mkdir docs

docs/%.h.md : pal-fds/%.h docs/
	python literalize.py $< > $@

clean:
	$(MAKE) -C examples clean
	rm -f $(DOCS)
HEADERS=$(wildcard *.h)
DOCS=$(patsubst %.h, docs/%.h.md, $(HEADERS))

.PHONY: clean examples documentation

examples:
	make -C examples/ all

documentation: $(DOCS) docs/

docs/:
	mkdir docs

docs/%.h.md : %.h docs/
	python literalize.py $< > $@

clean:
	$(MAKE) -C examples clean
	rm -f $(DOCS)
SAIO_DOTS = \
	$(wildcard saio-move-*.dot) \
	$(wildcard saio-pivot-*.dot) \
	$(wildcard saio-recalc-*.dot)

SAIO_PNGS = $(patsubst %.dot,%.png,$(SAIO_DOTS))

.SECONDARY : $(SAIO_PNGS)

EXTRA_CLEAN = \
	saio.toc saio.aux saio.pdf saio.log \
	saio.out saio.nav saio.snm saio*.vrb \
	$(SAIO_PNGS)

all : saio.pdf

clean :
	rm -f $(EXTRA_CLEAN)

%.png : %.dot
	dot -Tpng -o $@ $<

%.pdf: %.tex $(SAIO_PNGS)
	pdflatex $<
	pdflatex $<

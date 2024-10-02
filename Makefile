all: naismul

%.d: %.c
	@set -e; rm -f $@; \
	$(COMPILE.c) -MM $(CPPFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

sources=toto.c tata.c titi.c naismul.c

-include $(sources:.c=.d)

naismul: $(sources:.c=.o)

clean:
	$(RM) *.d *.o naismul

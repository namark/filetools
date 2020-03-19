
override CPPFLAGS	+= --std=c++1z
override CPPFLAGS	+= -MMD -MP
override CPPFLAGS	+= -I./include
override CPPFLAGS	+= $(shell cat .cxxflags 2> /dev/null | xargs)

PREFIX	:= $(DESTDIR)/usr/local
BINDIR	:= $(PREFIX)/bin

SOURCES	:= $(shell echo *.cpp)
TEMPDIR	:= temp
DISTDIR := out
OUT		:= $(SOURCES:%.cpp=$(DISTDIR)/f%)
TARGET	:= $(OUT:$(DISTDIR)/%=$(BINDIR)/%)
OBJECTS	:= $(SOURCES:%.cpp=$(TEMPDIR)/%.o)
DEPENDS	:= $(OBJECTS:.o=.d)

build: $(OUT)

test: build
	./tests/basic.sh

$(DISTDIR)/f%: $(TEMPDIR)/%.o | $(DISTDIR)
	$(CXX) $(LDFLAGS) $< $(LDLIBS) -o $@

$(TEMPDIR)/%.o: %.cpp | $(TEMPDIR)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ -c $<

$(TEMPDIR):
	@mkdir $@

$(DISTDIR):
	@mkdir $@

clean:
	@rm $(DEPENDS) 2> /dev/null || true
	@rm $(OBJECTS) 2> /dev/null || true
	@rmdir $(TEMPDIR) 2> /dev/null || true
	@echo Temporaries cleaned!

distclean: clean
	@rm $(OUT) 2> /dev/null || true
	@rmdir $(DISTDIR) 2> /dev/null || true
	@echo All clean!

install: $(TARGET)
	@echo Install complete!

$(BINDIR)/%: $(DISTDIR)/% | $(BINDIR)
	install --strip $< $@

$(BINDIR):
	@mkdir $@

uninstall:
	-rm $(TARGET)
	@rmdir -p $(BINDIR) 2> /dev/null || true
	@echo Uninstall complete!

-include $(DEPENDS)

.PRECIOUS : $(OBJECTS)
.PHONY : test clean distclean uninstall


# Set the file name of your jar package:
JAR_PKG = disassemble.jar
# Set your entry point of your java app:
ENTRY_POINT = disassemble
RES_DIR = no

SOURCE_FILES = \
disassemble.java


# Set your java compiler here:
JAVAC = javac

JFLAGS = -g 




vpath %.class bin
vpath %.java src

# show help message by default
Default:
	@echo "make new: new project, create bin."
	@echo "make build: build project."
	@echo "make clean: clear classes generated."
	@echo "make rebuild: rebuild project."
	@echo "make run: run your app."
	@echo "make jar: package your project into a executable jar."

build: $(SOURCE_FILES:.java=.class)

%.class: %.java
	$(JAVAC) -cp bin -d bin $(JFLAGS) $<

rebuild: clean build

.PHONY: new clean run jar

new:
ifeq ($(RES_DIR),yes)
	mkdir -pv src bin res
else
	mkdir -pv bin
endif

clean:
	rm -frv bin/*

run:
	java -cp bin $(ENTRY_POINT)

jar:
ifeq ($(RES_DIR),yes)
	jar cvfe $(JAR_PKG) $(ENTRY_POINT)  -C bin . res
else
	jar cvfe $(JAR_PKG) $(ENTRY_POINT) -C bin .
endif

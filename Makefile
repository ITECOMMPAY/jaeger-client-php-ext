#
#	Makefile template
#
#	This is an example Makefile that can be used by anyone who is building
#	his or her own PHP extensions using the PHP-CPP library. 
#
#	In the top part of this file we have included variables that can be
#	altered to fit your configuration, near the bottom the instructions and
#	dependencies for the compiler are defined. The deeper you get into this
#	file, the less likely it is that you will have to change anything in it.
#

#
#	Name of your extension
#
#	This is the name of your extension. Based on this extension name, the
#	name of the library file (name.so) and the name of the config file (name.ini)
#	are automatically generated
#
NAME				=	jaeger-client

#
#	Php services
#
PHP_SERVICES		=	$(shell systemctl list-unit-files | grep -o 'php.*.service' | rev | cut -d. -f1 --complement | rev)

#
#	The extension dirs
#
#	This is normally a directory like /usr/lib/php5/20121221 (based on the 
#	PHP version that you use. We make use of the command line 'php-config' 
#	instruction to find out what the extension directory is, you can override
#	this with a different fixed directory
#
EXTENSION_DIR		=	$(shell php-config --extension-dir)

#
#	OS Release
#
OS_RELEASE			=	$(shell awk -F= '/^NAME/{print $$2}' /etc/os-release)

#
#	The name of the extension and the name of the .ini file
#
#	These two variables are based on the name of the extension. We simply add
#	a certain extension to them (.so or .ini)
#
EXTENSION			=	${NAME}.so
INI					=	${NAME}.ini
LINK_INI			=	20-${NAME}.ini

#
#	Compiler
#
#	By default, the GNU C++ compiler is used. If you want to use a different
#	compiler, you can change that here. You can change this for both the 
#	compiler (the program that turns the c++ files into object files) and for
#	the linker (the program that links all object files into the single .so
#	library file. By default, g++ (the GNU C++ compiler) is used for both.
#
COMPILER			=	g++
LINKER				=	g++

#
#	Compiler and linker flags
#
#	This variable holds the flags that are passed to the compiler. By default, 
# 	we include the -O2 flag. This flag tells the compiler to optimize the code, 
#	but it makes debugging more difficult. So if you're debugging your application, 
#	you probably want to remove this -O2 flag. At the same time, you can then 
#	add the -g flag to instruct the compiler to include debug information in
#	the library (but this will make the final libphpcpp.so file much bigger, so
#	you want to leave that flag out on production servers).
#
#	If your extension depends on other libraries (and it does at least depend on
#	one: the PHP-CPP library), you should update the LINKER_DEPENDENCIES variable
#	with a list of all flags that should be passed to the linker.
#
COMPILER_FLAGS		=	-Wall -c -I"src" -I"PHP-CPP" -I"/opt/remi/php73/root/usr/include" -O2 -std=c++11 -fpic -o
LINKER_FLAGS		=	-shared -L"PHP-CPP" -L"/opt/remi/php73/root/usr/lib64" -fpic -Wl,--whole-archive
LINKER_DEPENDENCIES	=	-l:libphpcpp.a -Wl,--no-whole-archive

#
#	Command to remove files, copy files and create directories.
#
#	I've never encountered a *nix environment in which these commands do not work. 
#	So you can probably leave this as it is
#
RM					=	rm -f
CP					=	cp -f
LN					=	ln -f -s
MKDIR				=	mkdir -p
LDCONFIG			=	ldconfig
SERVICE				=	service
CHMOD				=	chmod

#
#	All source files are simply all *.cpp files found in the current directory
#
#	A builtin Makefile macro is used to scan the current directory and find 
#	all source files. The object files are all compiled versions of the source
#	file, with the .cpp extension being replaced by .o.
#
SOURCES				=	\
						src/main.cpp \
						src/IReporter.cpp \
						src/ISampler.cpp \
						src/ISpan.cpp \
						src/ITracer.cpp \
						src/FileReporter.cpp \
						src/Helper.cpp \
						src/JaegerSpan.cpp \
						src/JaegerTracer.cpp \
						src/Log.cpp \
						src/NoopSpan.cpp \
						src/NoopTracer.cpp \
						src/PageViewHandler.cpp \
						src/PercentageSampler.cpp \
						src/Printer.cpp \
						src/Process.cpp \
						src/SpanContext.cpp \
						src/Tag.cpp \
						src/TextCarrier.cpp \
						src/Tracer.cpp \
						src/UdpReporter.cpp \
						src/thrift-gen/Agent.cpp \
						src/thrift-gen/jaeger_types.cpp \
						src/thrift-gen/zipkincore_types.cpp \
						src/thrift-lib/protocol/TProtocol.cpp \
						src/thrift-lib/transport/TBufferTransports.cpp
OBJECTS				=	$(SOURCES:%.cpp=%.o)

#
#	From here the build instructions start
#
all:					${OBJECTS} ${EXTENSION}

${EXTENSION}:			${OBJECTS}
						${LINKER} ${LINKER_FLAGS} -o $@ ${OBJECTS} ${LINKER_DEPENDENCIES}

${OBJECTS}:
						${COMPILER} ${COMPILER_FLAGS} $@ ${@:%.o=%.cpp}

fetch-php-cpp:
						git submodule update --init --recursive
						sed -i 's/.*INSTALL_PREFIX.*=.*/  INSTALL_PREFIX		=	\/usr\/local/g' ./PHP-CPP/Makefile

install-php-cpp:
						cd ./PHP-CPP && sudo make clean
						cd ./PHP-CPP && sudo make
						cd ./PHP-CPP && sudo make install

install:
						${CP} ${EXTENSION} ${EXTENSION_DIR}

						@if [ ${OS_RELEASE} = "CentOS Linux" ]; then \
							for dir in $(shell ls /etc/ | grep 'php.*.d' 2>/dev/null); do \
								INI_DIR="/etc/$$dir"; \
								if [ -d $$INI_DIR ]; then \
									${CP} ${INI} $$INI_DIR/${LINK_INI}; \
									${CHMOD} 0644 $$INI_DIR/${LINK_INI}; \
								fi; \
							done; \
						elif [ ${OS_RELEASE} = "Ubuntu" ]; then \
							for dir in $(shell ls /etc/php 2>/dev/null); do \
								INI_DIR="/etc/php/$$dir/mods-available"; \
								if [ -d $$INI_DIR ]; then \
									${CP} ${INI} $$INI_DIR; \
								fi; \
								LINK_INI_DIR_FPM="/etc/php/$$dir/fpm/conf.d"; \
								if [ -d $$LINK_INI_DIR_FPM ]; then \
									${LN} $$INI_DIR/${INI} $$LINK_INI_DIR_FPM/${LINK_INI}; \
								fi; \
								LINK_INI_DIR_CLI="/etc/php/$$dir/cli/conf.d"; \
								if [ -d $$LINK_INI_DIR_CLI ]; then \
									${LN} $$INI_DIR/${INI} $$LINK_INI_DIR_CLI/${LINK_INI}; \
								fi; \
							done \
						else \
							echo "unknown os"; \
						fi
						@if `which ldconfig`; then \
							${LDCONFIG}; \
						fi

clean:
						${RM} ${EXTENSION} ${OBJECTS}

uninstall:
						${RM} ${EXTENSION_DIR}/${EXTENSION}
						@if [ ${OS_RELEASE} = "CentOS Linux" ]; then \
							for dir in $(shell ls /etc/ | grep 'php.*.d' 2>/dev/null); do \
								INI_DIR="/etc/$$dir"; \
								if [ -d $$INI_DIR ]; then \
									${RM} $$INI_DIR/${LINK_INI}; \
								fi; \
							done; \
						elif [ ${OS_RELEASE} = "Ubuntu" ]; then \
							for dir in $(shell ls /etc/php 2>/dev/null); do \
								INI_DIR="/etc/php/$$dir/mods-available"; \
								if [ -d $$INI_DIR ]; then \
									${RM} $$INI_DIR/${INI}; \
								fi; \
								LINK_INI_DIR_FPM="/etc/php/$$dir/fpm/conf.d"; \
								if [ -d $$LINK_INI_DIR_FPM ]; then \
									${RM} $$LINK_INI_DIR_FPM/${LINK_INI}; \
								fi; \
								LINK_INI_DIR_CLI="/etc/php/$$dir/cli/conf.d"; \
								if [ -d $$LINK_INI_DIR_CLI ]; then \
									${RM} $$LINK_INI_DIR_CLI/${LINK_INI}; \
								fi; \
							done \
						else \
							echo "unknown os"; \
						fi

restart:
						@for service in $(PHP_SERVICES); do \
							service $$service restart; \
						done
						@php -m | grep $(NAME) 2>/dev/null

# ICP - projekt Solitaire Makefile (xjerab21, xjudap00)

all: make

make:
	cd src; qmake; make; make -f Makefile-cli; cd ../;

merlin:
	cd src; \
	export LD_LIBRARY_PATH:=/usr/local/share/Qt-5.5.1/5.5/gcc_64/lib:${LD_LIBRARY_PATH}; \
	export QT_PLUGIN_PATH:=/usr/local/share/Qt-5.5.1/5.5/gcc_64/plugins:${QT_PLUGIN_PATH};\
	/usr/local/share/Qt-5.5.1/5.5/gcc_64/bin/qmake; make; make -f Makefile-cli; cd ../; 

run:
	make; cd src/; ./hra2017 & ./hra2017-cli; cd ../;

rungui:
	make; cd src/; ./hra2017; cd ../;

runcli:
	make; cd src/; ./hra2017-cli; cd ../;

clean: 
	rm -rf doc/* src/*.o src/Makefile src/hra* src/moc*

pack:
	zip -r xjerab21-xjudap00.zip *

doxygen:
	doxygen doxygen.txt ./src
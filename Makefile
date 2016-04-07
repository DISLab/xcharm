all: charmxi libs

charmxi:
	make -C src/xlat-i charmxi OPTS=""
	ln -sf $(CHARM_HOME)/bin/charmc ./bin

libs:
	make -C src/libs/NDMeshStreamer all
	make -C src/libs/ucharelib all
	make -C src/libs/graphrt all

test:
	make -C t test

bench:
	make -C bench all

clean:
	rm -f ./lib/* ./bin/* ./include/*
	make -C src/xlat-i xi-clean
	make -C src/libs/NDMeshStreamer clean
	make -C src/libs/ucharelib clean
	make -C src/libs/graphrt clean
	make -C t clean
	make -C bench clean

SOURCES := $(wildcard *.c)

all: comp

comp: prj1
	docker compose -f docker-compose.yml up

prj1: Dockerfile $(SOURCES)
	docker build . -t $@

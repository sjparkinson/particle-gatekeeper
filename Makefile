SHELL := /bin/bash

TARGET := build/gatekeeper.bin
SOURCE := src/gatekeeper.ino
PLATFORM := photon
DEVICE := gatekeeper

EXECUTABLES := particle
CHECK := $(foreach executable,$(EXECUTABLES),\
	$(if $(shell which $(executable)),"",$(error "No executable found for $(executable).")))

PARTICLE := $(shell which particle)

.PHONY: default flash clean help

.SILENT: help

default: $(TARGET)

$(TARGET): ## Build the firmware 🚀.
	${PARTICLE} compile ${PLATFORM} ${SOURCE}

flash: ## Deploy the firmware 🚀.
flash: $(TARGET)
	${PARTICLE} flash ${DEVICE} ${TARGET}

clean: ## Clenup the project.
	rm -f ${TARGET}

help: ## Show this help message.
	echo "usage: make [target] ..."
	echo ""
	echo "targets:"
	fgrep --no-filename "##" ${MAKEFILE_LIST} | fgrep --invert-match $$'\t' | sed -e 's/: ## / - /'

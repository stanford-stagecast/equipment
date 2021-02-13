.PHONY: all
all: help

.PHONY: help
help:
	@echo -e "Targets:"
	@echo -e "\tsetup\tdownload build scripts and dependencies"
	@echo -e "\trun\trun the development server with live-reloading"
	@echo -e "\tbuild\tbuild the production version of the frontend"
	@echo -e "\tcheck\tcheck for syntax errors"

.PHONY: setup
setup: npm
	@stat node_modules > /dev/null || npm install

.PHONY: run
run: setup
	npm run dev

.PHONY: build
build: setup
	npm run build

.PHONY: check
check: setup
	npm run validate

.PHONY: npm
npm:
	@which npm > /dev/null || (echo -e "\nPlease install NPM (https://www.npmjs.com) and try again.\n" && exit 1)


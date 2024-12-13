# Main Makefile

# Detect environment
ENV ?= dev

# Include makefiles
include make/Makefile.common
ifeq ($(ENV),prod)
	include make/Makefile.prod
else
	include make/Makefile.dev
endif

# Install prefix based on environment
INSTALL_PREFIX = $(if $(filter prod,$(ENV)),$(PROD_PREFIX),$(DEV_PREFIX))

# Set paths based on environment
SSL_CERT = $(if $(filter prod,$(ENV)),$(PROD_SSL_CERT),$(DEV_SSL_CERT))
SSL_KEY = $(if $(filter prod,$(ENV)),$(PROD_SSL_KEY),$(DEV_SSL_KEY))
CONFIG_PATH = $(if $(filter prod,$(ENV)),$(PROD_CONFIG),$(DEV_CONFIG))
AUTH_PATH = $(if $(filter prod,$(ENV)),$(PROD_AUTH),$(DEV_AUTH))
DB_PATH = $(if $(filter prod,$(ENV)),$(PROD_DB),$(DEV_DB))
LOG_PATH = $(if $(filter prod,$(ENV)),$(PROD_LOG),$(DEV_LOG))
AUDIT_PATH = $(if $(filter prod,$(ENV)),$(PROD_AUDIT),$(DEV_AUDIT))

# Default target
all: dirs $(TARGET)

# Installation selector
install:
	@if [ "$(ENV)" = "prod" ]; then \
		$(MAKE) install-prod; \
	else \
		$(MAKE) install-dev; \
	fi

# Rebuild targets
rebuild: clean all

rebuild-dev: clean-dev all
	$(MAKE) dev-install

rebuild-prod: clean-prod all
	$(MAKE) ENV=prod install-prod

# Clean target
clean: clean-common
	@if [ "$(ENV)" = "prod" ]; then \
		$(MAKE) clean-prod; \
	else \
		$(MAKE) clean-dev; \
	fi

# Add after clean target
uninstall:
	@if [ "$(ENV)" = "prod" ]; then \
		sudo rm -rf $(PROD_PREFIX)/bin/app; \
		sudo rm -rf $(PROD_PREFIX)/etc/app; \
		sudo rm -rf $(PROD_PREFIX)/var/log/app.*; \
		sudo rm -rf $(PROD_PREFIX)/var/lib/records.*; \
	else \
		rm -rf $(DEV_PREFIX)/*; \
	fi

# Package target
package: all
	@echo "Creating distribution package..."
	@mkdir -p dist/bin dist/etc/ssl dist/etc/env dist/etc/auth dist/setup
	@cp $(TARGET) dist/bin/
	@cp config/cert.pem.example dist/etc/ssl/
	@cp config/privkey.pem.example dist/etc/ssl/
	@cp config/.env.example dist/etc/env/
	@cp config/passwd.example dist/etc/auth/
	@cp scripts/setup.sh dist/setup/
	@chmod +x dist/setup/setup.sh
	@tar -czf app.tar.gz -C dist .
	@rm -rf dist

# Update PHONY targets
.PHONY: all dirs clean clean-dev clean-prod uninstall distclean \
		rebuild rebuild-dev rebuild-prod install install-dev install-prod package \
		dev-setup prod-setup-help

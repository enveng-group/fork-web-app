#!/bin/sh

# Check arguments
if [ $# -ne 5 ]; then
    echo "Usage: $0 PREFIX USER GROUP UID GID" >&2
    exit 1
fi

PREFIX="$1"
USER="$2"
GROUP="$3"
UID="$4"
GID="$5"

# Create directory structure
install -d -m 755 "${PREFIX}/bin" || exit 1
install -d -m 750 "${PREFIX}/etc/app/ssl" || exit 1
install -d -m 750 "${PREFIX}/etc/app/auth" || exit 1
install -d -m 750 "${PREFIX}/etc/app/env" || exit 1
install -d -m 750 "${PREFIX}/var/log" || exit 1
install -d -m 750 "${PREFIX}/var/lib" || exit 1

# Copy development files
cp /devcontainer/web-app/etc/ssl/cert.pem "${PREFIX}/etc/app/ssl/" || exit 1
cp /devcontainer/web-app/etc/ssl/privkey.pem "${PREFIX}/etc/app/ssl/" || exit 1
cp /devcontainer/web-app/etc/env/.env "${PREFIX}/etc/app/env/" || exit 1
cp /devcontainer/web-app/etc/auth/passwd "${PREFIX}/etc/app/auth/" || exit 1

# Set ownership and permissions
chown -R "${USER}:${GROUP}" "${PREFIX}" || exit 1
find "${PREFIX}" -type f -exec chmod 644 {} \;
find "${PREFIX}" -type d -exec chmod 755 {} \;
chmod 600 "${PREFIX}/etc/app/ssl/privkey.pem" || exit 1
chmod 600 "${PREFIX}/etc/app/auth/passwd" || exit 1
chmod 600 "${PREFIX}/etc/app/env/.env" || exit 1

exit 0

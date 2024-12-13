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

# Create service user/group if they don't exist
if ! getent group "${GROUP}"; then
    groupadd -r -g "${GID}" "${GROUP}" || exit 1
fi

if ! getent passwd "${USER}"; then
    useradd -r -u "${UID}" -g "${GROUP}" \
        -d /nonexistent -s /sbin/nologin \
        -c "EnvEng Web Service" "${USER}" || exit 1
fi

# Create directory structure
install -d -m 755 "${PREFIX}/bin" || exit 1
install -d -m 750 -o "${USER}" -g "${GROUP}" "${PREFIX}/etc/app/ssl" || exit 1
install -d -m 750 -o "${USER}" -g "${GROUP}" "${PREFIX}/etc/app/auth" || exit 1
install -d -m 750 -o "${USER}" -g "${GROUP}" "${PREFIX}/etc/app/env" || exit 1
install -d -m 750 -o "${USER}" -g "${GROUP}" "${PREFIX}/var/log" || exit 1
install -d -m 750 -o "${USER}" -g "${GROUP}" "${PREFIX}/var/lib" || exit 1

# Create empty files with correct permissions
install -m 600 -o "${USER}" -g "${GROUP}" /dev/null "${PREFIX}/etc/app/ssl/privkey.pem" || exit 1
install -m 644 -o "${USER}" -g "${GROUP}" /dev/null "${PREFIX}/etc/app/ssl/cert.pem" || exit 1
install -m 600 -o "${USER}" -g "${GROUP}" /dev/null "${PREFIX}/etc/app/env/.env" || exit 1
install -m 600 -o "${USER}" -g "${GROUP}" /dev/null "${PREFIX}/etc/app/auth/passwd" || exit 1
install -m 600 -o "${USER}" -g "${GROUP}" /dev/null "${PREFIX}/var/lib/records.rec" || exit 1
install -m 644 -o "${USER}" -g "${GROUP}" /dev/null "${PREFIX}/var/log/app.log" || exit 1
install -m 600 -o "${USER}" -g "${GROUP}" /dev/null "${PREFIX}/var/log/audit.log" || exit 1

exit 0

# Stage 1: Build tools in Alpine for amd64
FROM --platform=linux/amd64 docker.io/alpine:latest AS builder

# Enable the testing repository and install necessary packages
RUN echo "http://dl-cdn.alpinelinux.org/alpine/edge/testing" >> /etc/apk/repositories && \
    apk update && \
    apk add --no-cache \
    tcc \
    musl-dev \
    git \
    dropbear \
    gnupg \
    tree \
    nodejs \
    gcompat \
    dropbear-dbclient \
    dropbear-scp \
    dropbear-ssh \
    npm

# Stage 2: Create final image with BusyBox for amd64
FROM --platform=linux/amd64 docker.io/busybox:musl

# Copy necessary binaries from the builder stage
COPY --from=builder /usr/bin/tcc /usr/bin/tcc
COPY --from=builder /usr/bin/c_rehash /usr/bin/c_rehash
COPY --from=builder /usr/bin/getconf /usr/bin/getconf
COPY --from=builder /usr/bin/getent /usr/bin/getent
COPY --from=builder /usr/bin/git /usr/bin/git
COPY --from=builder /usr/bin/git-receive-pack /usr/bin/git-receive-pack
COPY --from=builder /usr/bin/git-shell /usr/bin/git-shell
COPY --from=builder /usr/bin/git-upload-archive /usr/bin/git-upload-archive
COPY --from=builder /usr/bin/git-upload-pack /usr/bin/git-upload-pack
COPY --from=builder /usr/sbin/dropbear /usr/sbin/dropbear
COPY --from=builder /usr/bin/dropbearkey /usr/bin/dropbearkey
COPY --from=builder /usr/bin/tree /usr/bin/tree
COPY --from=builder /usr/bin/node /usr/bin/node
COPY --from=builder /usr/bin/npm /usr/bin/npm
COPY --from=builder /usr/bin/gpg-agent /usr/bin/gpg-agent
COPY --from=builder /usr/bin/gpg2 /usr/bin/gpg2
COPY --from=builder /usr/bin/dbclient /usr/bin/dbclient
COPY --from=builder /usr/bin/scp /usr/bin/scp
COPY --from=builder /usr/bin/ssh /usr/bin/ssh
COPY --from=builder /usr/bin/pinentry-curses /usr/bin/pinentry-curses

# Copy musl headers and libraries necessary for tcc
COPY --from=builder /usr/include /usr/include
COPY --from=builder /lib /lib
COPY --from=builder /lib64 /lib64
COPY --from=builder /usr/lib /usr/lib

# Create symbolic links for the compiler
RUN ln -sf /usr/bin/tcc /usr/bin/cc && ln -sf /usr/bin/tcc /usr/bin/ld

# Create group and user with specific UID and GID
# Consider removing if user data is managed by the volume
RUN addgroup -g 20 devgroup && \
    adduser -D -s /bin/ash -u 501 -G devgroup devuser && \
    mkdir -p /home/devuser && \
    chown -R 501:20 /home/devuser

# Switch to non-root user for development
USER devuser

# Set working directory for the user
WORKDIR /home/devuser

# Set up environment variables for the user
ENV PATH="/home/devuser/bin:$PATH"

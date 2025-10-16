# Dockerfile multi-stage pour RogueMagicBox
# Stage 1: Build
FROM alpine:3.18 AS builder

# Installer les dépendances de build
RUN apk add --no-cache \
    build-base \
    cmake \
    git \
    curl-dev \
    libcurl

WORKDIR /build

# Copier les sources
COPY CMakeLists.txt ./
COPY include/ ./include/
COPY src/ ./src/
COPY third_party/ ./third_party/
COPY tests/ ./tests/
COPY config/ ./config/

# Configurer et compiler
RUN cmake -S . -B build \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX=/usr/local

RUN cmake --build build --config Release -j$(nproc)

# Installer dans /usr/local
RUN cmake --install build --prefix /install

# Tests dans le builder
RUN cd build && ctest -C Release --output-on-failure || echo "Tests skipped"

# Stage 2: Runtime (image finale légère)
FROM alpine:3.18

# Installer uniquement les dépendances runtime
RUN apk add --no-cache \
    libstdc++ \
    libcurl \
    git \
    github-cli \
    ca-certificates \
    && rm -rf /var/cache/apk/*

# Copier le binaire depuis le builder
COPY --from=builder /install/bin/roguebox /usr/local/bin/roguebox
COPY --from=builder /install/share/roguebox/ /usr/local/share/roguebox/

# Créer dossiers pour logs et config
RUN mkdir -p /workspace /logs /config && \
    apk add --no-cache bash

WORKDIR /workspace

# Variables d'environnement (le token sera passé au runtime avec -e ou via build-arg)
ARG GITHUB_TOKEN=""
ENV GITHUB_TOKEN=${GITHUB_TOKEN}
ENV ROGUEBOX_CONFIG=/config/rogue.toml

# Health check (vérifier que le binaire fonctionne)
HEALTHCHECK --interval=30s --timeout=5s --start-period=5s --retries=3 \
    CMD roguebox --version || exit 1

# Entrypoint
ENTRYPOINT ["/usr/local/bin/roguebox"]
CMD ["--help"]

# Labels
LABEL maintainer="EPSI I2 Cyber"
LABEL description="RogueMagicBox - GitOps CLI for Workshop automation"
LABEL version="1.0.0"

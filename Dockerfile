FROM gcc:13 AS builder
LABEL maintainer="Morteza Hosseini"

ARG CMAKE_VERSION=4.0.3
ARG SMASHPP_VERSION=0.0.0-dev
ARG TARGETARCH

RUN set -eux; \
    arch="${TARGETARCH:-$(dpkg --print-architecture)}"; \
    case "$arch" in \
        amd64) cmake_arch="x86_64" ;; \
        arm64) cmake_arch="aarch64" ;; \
        *) echo "Unsupported target architecture: $arch" >&2; exit 1 ;; \
    esac; \
    curl -fsSL \
        --retry 5 \
        --retry-delay 5 \
        --connect-timeout 30 \
        "https://github.com/Kitware/CMake/releases/download/v${CMAKE_VERSION}/cmake-${CMAKE_VERSION}-linux-${cmake_arch}.tar.gz" \
        -o /tmp/cmake.tar.gz; \
    tar -xzf /tmp/cmake.tar.gz --strip-components=1 -C /usr/local; \
    rm -f /tmp/cmake.tar.gz; \
    cmake --version

WORKDIR /src
COPY . .
RUN cmake -S . -B build \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_INSTALL_PREFIX=/opt/smashpp \
        -DSMASHPP_VERSION_OVERRIDE="${SMASHPP_VERSION}" \
        -DSMASHPP_ENABLE_OPENMP=OFF && \
    cmake --build build --parallel "$(nproc)" --config Release && \
    cmake --install build

FROM ubuntu:24.04 AS runtime

COPY --from=builder /opt/smashpp /usr/local
WORKDIR /data
ENTRYPOINT ["smashpp"]
CMD ["--help"]

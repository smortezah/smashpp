FROM ubuntu:24.04 AS build
LABEL maintainer="Morteza Hosseini"

ENV DEBIAN_FRONTEND=noninteractive
ENV PATH="/opt/cmake-venv/bin:${PATH}"
RUN apt-get update && \
    apt-get install -y --no-install-recommends build-essential ca-certificates python3-venv && \
    python3 -m venv /opt/cmake-venv && \
    /opt/cmake-venv/bin/pip install --no-cache-dir --upgrade pip && \
    /opt/cmake-venv/bin/pip install --no-cache-dir "cmake~=4.0.0" && \
    rm -rf /var/lib/apt/lists/*

WORKDIR /src
COPY . .
RUN cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/opt/smashpp && \
    cmake --build build --parallel && \
    cmake --install build

FROM ubuntu:24.04 AS runtime
ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get update && \
    apt-get install -y --no-install-recommends ca-certificates libgomp1 && \
    rm -rf /var/lib/apt/lists/*

COPY --from=build /opt/smashpp /usr/local
WORKDIR /data
ENTRYPOINT ["smashpp"]
CMD ["--help"]

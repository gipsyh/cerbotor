FROM alpine:3.21

RUN apk update && \
    apk add --no-cache \
        bash \
        build-base \
        cmake \
        git \
        meson \
        m4 \
        ca-certificates \
        coreutils

COPY . /cerbotor
WORKDIR /cerbotor
RUN \
  rm -rf build && \
  cmake -DCMAKE_BUILD_TYPE=Release -DTOOLS=ON -B build && \
  make -j$(nproc) -C build

ENTRYPOINT ["/cerbotor/build/bin/check"]

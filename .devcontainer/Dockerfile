FROM ubuntu:latest
ARG DEBIAN_FRONTEND=noninteractive

# Set locale
ENV LANG C.uft8

# Install core packages
RUN apt-get update && apt-get install -y \
    build-essential \
    git \
    software-properties-common \
    sudo \
    && rm -rf /var/lib/apt/lists/*

# Install Go
# https://github.com/golang/go/wiki/Ubuntu#using-ppa
# https://go.dev/doc/install
RUN add-apt-repository ppa:longsleep/golang-backports \
    && apt-get update && apt-get install -y golang-go \
    && rm -rf /var/lib/apt/lists/*
ENV PATH ${PATH}:/root/go/bin

# Install Bazelisk
# https://github.com/bazelbuild/bazelisk#requirements
RUN go install github.com/bazelbuild/bazelisk@latest

# Install Buildifier
# https://github.com/bazelbuild/buildtools/tree/master/buildifier
RUN go install github.com/bazelbuild/buildtools/buildifier@latest

# Install other useful tools
RUN apt-get update && apt-get install -y \
    clang-format \
    clang-tidy \
    clangd \
    && rm -rf /var/lib/apt/lists/*

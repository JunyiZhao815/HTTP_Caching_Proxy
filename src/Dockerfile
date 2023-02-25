FROM ubuntu:22.04

RUN apt-get update && apt-get -y --no-install-recommends install \
    build-essential \
    libboost-all-dev

WORKDIR /http_proxy

COPY . .

RUN chmod +x /http_proxy/main_run.sh

EXPOSE 4000
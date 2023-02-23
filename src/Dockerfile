FROM ubuntu:22.04

RUN apt-get update && apt-get -y --no-install-recommends install \
    build-essential \
    libboost-all-dev

WORKDIR /http_proxy

COPY . .

EXPOSE 4000

CMD ["make"]

CMD ["./http_proxy"]
FROM ubuntu:22.04


ENV DEBIAN_FRONTEND=noninteractive
ENV TZ=Europe/Moscow


RUN apt-get update && apt-get install -y \
    cmake \
    g++ \
    make \
    libpq-dev \
    libqt5sql5-psql \
    libqt5core5a \
    libqt5gui5 \
    libqt5widgets5 \
    qtbase5-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY . .
RUN rm -rf build && mkdir -p build

WORKDIR /app/build
RUN cmake /app && cmake --build . \
&& chmod +x SmartHouseSystemServer
WORKDIR /app/build

EXPOSE 1234

RUN ls
CMD ["./SmartHouseSystemServer"]


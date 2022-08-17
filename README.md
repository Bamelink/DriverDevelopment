# DriverDevelopment

Before getting started, make sure you have a proper Zephyr development
environment. You can follow the official
[Zephyr Getting Started Guide](https://docs.zephyrproject.org/latest/getting_started/index.html).

## Initialization

The first step is to initialize the workspace folder (``my-workspace``) where
the ``driver-dev`` and all Zephyr modules will be cloned. You can do
that by running:

```shell
# initialize my-workspace for the example-application (main branch)
west init -m https://github.com/Bamelink/DriverDevelopment --mr main my-workspace
# update Zephyr modules
cd my-workspace
west update
```

## Build & Run

The application can be built by running:

```shell
cd driver-dev
west build -b $BOARD app
```
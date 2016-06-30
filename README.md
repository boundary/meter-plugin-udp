# TrueSight Pulse UDP Metric Plugin 

Example plugin written using the meter plugin SDK for C.

Collects meterics from example application that presents metrics via a UDP interface called
[metric-daemon](https://github.com/BMCTrueSight/metric-daemon)

### Prerequisites

|     OS    | Linux | Windows | SmartOS | OS X |
|:----------|:-----:|:-------:|:-------:|:----:|
| Supported |   v   |    v    |    v    |  v   |

- [TrueSight Pulse Meter Plugin SDK for C](https://github.com/boundary/meter-plugin-sdk-c)
- [metric-daemon](https://github.com/BMCTrueSight/metric-daemon) running on a host that is reachable from the host
that is running the plugin.

### Plugin Setup

#### Metric Daemon

Install the Metric Daemon according to the instructions [here](https://github.com/BMCTrueSight/metric-daemon).

#### Meter Plugin SDK
For this plugin to run the [TrueSight Pulse Meter Plugin SDK for C](https://github.com/boundary/meter-plugin-sdk-c) must
be installed on the same host where the plugin is running.

#### Plugin Installation

1. Download plugin distribution to the target platform:

    ```bash
    $ wget https://github.com/boundary/meter-plugin-random-c/releases/download/v1.0.0/meter-plugin-udp-1.0.0.tar.gz
    ```
2. Extract distribution:

   ```bash
   $ tar xvf meter-plugin-udp-1.0.0.tar.gz
   ```

3. Change directory to extracted plugin distribution:

   ```bash
   $ cd meter-plugin-udp-1.0.0
   ```

4. Configure plugin for compilation:

   ```bash
   $ ./configure
   ```

5. Compile meter plugin executable:

   ```bash
   $ make
   ```

5. Install meter plugin executable:

   ```bash
   $ sudo make install
   ```

### Plugin Configuration Fields

|Field Name|Description                                        |
|:---------|:--------------------------------------------------|
|Host      |Host running the metric daemon                     |
|Port      |Listening port of the metric daemon                |
|Source    |Label to display in the legend for the measurement.|
|Interval  |How often should the plugin poll for metrics.      |

### Metrics Collected

|Metric Name        |Description                           |
|:------------------|:-------------------------------------|
| EXAMPLE_BYTECOUNT | A metric to track counts of bytes.   |
| EXAMPLE_DURATION  | A metric to track a duration of time |
| EXAMPLE_NUMBER    | A metric to track a number value     |
| EXAMPLE_PERCENT   | A metric to track a percentage value |

### Dashboards

- Example Count

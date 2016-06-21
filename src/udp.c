//
// Copyright 2016 BMC Software, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
#include "plugin.h"
#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>


#define PLUGIN_PARAM_HOST "host"
#define PLUGIN_PARAM_PORT "port"
#define PLUGIN_PARAM_METRIC "metric"
#define PLUGIN_PARAM_SOURCE "source"
#define PLUGIN_PARAM_INTERVAL "interval"

// Define a type to store hostname information
typedef char hostname_t[_POSIX_HOST_NAME_MAX + 1];


// Collector specific data for Random Collector
struct udp_collector_data {
    hostname_t host;
    in_port_t port;
    measurement_metric_t metric;
    char request[MEASUREMENT_METRIC_SIZE+1];
    int interval;
    measurement_source_t source;
    int sockfd;
    socklen_t serverlen;
    struct sockaddr_in serveraddr;
};

typedef struct udp_collector_data udp_collector_data_t;

/** \brief Random plugin initialization method
 *
 */
plugin_result_t udp_plugin_initialize(meter_plugin_t *plugin) {
    plugin_result_t result = PLUGIN_SUCCEED;

    strcpy(plugin->name, "udp");

    return result;
}

plugin_result_t udp_get_measurement(collector_t *collector, double *measurement) {
    plugin_result_t result = PLUGIN_SUCCEED;
    char response[1024];
    udp_collector_data_t *data = collector->data;
    fprintf(stderr, "request: %s\n", data->request);

    // Send the request
    int n = sendto(data->sockfd, data->request, strlen(data->request), 0,
                   (struct sockaddr *) &data->serveraddr, data->serverlen);
    if (n < 0) {
        fprintf(stderr, "Error sending request");
        result = PLUGIN_FAIL;
    } else {
        // Extract the reply
        n = recvfrom(data->sockfd, response, sizeof(response), 0,
                     (struct sockaddr *) &data->serveraddr, &data->serverlen);

        if (n < 0) {
            result = PLUGIN_FAIL;;
            fprintf(stderr, "Error receiving response");
        } else {
            fprintf(stderr, "scan measurement\n");
            fprintf(stderr, "response: %s\n", response);
            sscanf(response, "%lf", measurement);
        }

    }
    return result;
}

plugin_result_t udp_collector_collect(collector_t *collector) {
    plugin_result_t result = PLUGIN_SUCCEED;
    double value = 0;

    udp_get_measurement(collector, &value);

    udp_collector_data_t *data = collector->data;
    measurement_timestamp_t timestamp = time(NULL);
    collector->send_measurement(data->metric, value, data->source, &timestamp);

    return result;
}

/** \brief Random plugin collector initialize method
 *
 */
plugin_result_t udp_collector_initialize(collector_t *collector) {
    plugin_result_t result = PLUGIN_SUCCEED;
    socklen_t serverlen;
    struct sockaddr_in serveraddr;
    struct hostent *server;
    udp_collector_data_t *data = collector->data;

    // Create socket to communicate with the service
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        fprintf(stderr, "ERROR opening socket");
        result = PLUGIN_FAIL;
    } else {
        data->sockfd = sockfd;
        // Get the hostname
        server = gethostbyname(data->host);
        if (server == NULL) {
            fprintf(stderr, "ERROR, no such host as %s\n", data->host);
            result = PLUGIN_FAIL;
        } else {

            // build the server's Internet address
            memset(&serveraddr, '\0', sizeof(serveraddr));
            serveraddr.sin_family = AF_INET;
            memcpy(server->h_addr, &serveraddr.sin_addr.s_addr, server->h_length);
            serveraddr.sin_port = htons(data->port);
            data->serveraddr = serveraddr;
            data->serverlen = serverlen = sizeof(serveraddr);
        }
    }

    return result;
}

plugin_result_t udp_plugin_start(meter_plugin_t *plugin) {
    plugin_result_t result = PLUGIN_SUCCEED;
    return result;
}

/** \brief Random plugin collector start method
 *
 */
plugin_result_t udp_collector_start(collector_t *collector) {
    plugin_result_t result = PLUGIN_SUCCEED;
    return result;
}

plugin_result_t udp_plugin_collector_initialize(meter_plugin_t *plugin, collector_t *collector) {
    plugin_result_t result = PLUGIN_SUCCEED;

    udp_collector_data_t *data = malloc(sizeof(udp_collector_data_t));
    assert(data);
    memset(data, '\0', sizeof(udp_collector_data_t));

    parameter_item_t *item = collector->item;

    // Get the parameters for this collector
    strcpy(data->host, parameter_get_string(item, PLUGIN_PARAM_HOST));
    data->port = parameter_get_integer(item, PLUGIN_PARAM_PORT);
    strcpy(data->metric, parameter_get_string(item, PLUGIN_PARAM_METRIC));
    data->interval = parameter_get_integer(item, PLUGIN_PARAM_INTERVAL);
    const char *source = parameter_get_string(item, PLUGIN_PARAM_SOURCE);
    strcpy(data->source, source);

    fprintf(stderr, "metric: %s\n", data->metric);
    char *s = strstr(data->metric, "_");
    s++;
    fprintf(stderr, "s: %s\n", s);
    strcpy(data->request, s);

    // Assign the random collector data to the collector
    collector->data = data;

    // Use the source for the name of the collector
    strcpy(collector->name, source);

    // Assign our collector functions
    collector->initialize_cb = udp_collector_initialize;
    collector->start_cb = udp_collector_start;
    collector->collect_cb = udp_collector_collect;

    return result;
}

/** \brief Random plugin collector start method
 *
 */
plugin_result_t udp_plugin_parameters(meter_plugin_t *plugin) {
    plugin_result_t result = PLUGIN_SUCCEED;
    plugin_parameters_t *parameters = plugin->parameters;

    size_t size = parameters->size;
    for (int i = 0; i < size; i++) {
        param_string_t host = parameter_get_string(parameters->items[i], PLUGIN_PARAM_HOST);
        fprintf(stderr, "host: %s, ", host);
        param_integer_t port = parameter_get_integer(parameters->items[i], PLUGIN_PARAM_PORT);
        fprintf(stderr, "port: %lld, ", port);
        param_string_t metric = parameter_get_string(parameters->items[i], PLUGIN_PARAM_METRIC);
        fprintf(stderr, "metric: %s, ", metric);
        param_string_t source = parameter_get_string(parameters->items[i], PLUGIN_PARAM_SOURCE);
        fprintf(stderr, "source: %s, ", source);
        param_integer_t interval = parameter_get_integer(parameters->items[i], PLUGIN_PARAM_INTERVAL);
        fprintf(stderr, "interval: %lld\n", interval);
    }

    return result;
}





/** \brief Plugin main() entry point
 *
 */
int main(int argc, char *argv[]) {
    // Create an instance of a plugin
    meter_plugin_t *plugin = plugin_create();

    // Assign function pointers that get called by the framework
    //    initialize_cb - Initial function called when plugin_run() is called with the meter plugin instance
    //    collector_init - Called to initialize the collectors from the plugin parameter items
    //    start_cb - Called just before starting the collectors
    plugin->initialize_cb = udp_plugin_initialize;
    plugin->collector_initialize_cb = udp_plugin_collector_initialize;
    plugin->start_cb = udp_plugin_start;

    return plugin_run(plugin);
}

#include "telemetry.hpp"
#include <opentelemetry/exporters/otlp/otlp_http_metric_exporter_factory.h>
#include <opentelemetry/metrics/provider.h>
#include <opentelemetry/sdk/metrics/aggregation/default_aggregation.h>
#include <opentelemetry/sdk/metrics/export/periodic_exporting_metric_reader.h>
#include <opentelemetry/sdk/metrics/meter.h>
#include <opentelemetry/sdk/metrics/meter_provider.h>

namespace metric_sdk    = opentelemetry::sdk::metrics;
namespace common        = opentelemetry::common;
namespace metrics_api   = opentelemetry::metrics;
namespace otlp_exporter = opentelemetry::exporter::otlp;

void initialise_telemetry()
{
    otlp_exporter::OtlpHttpMetricExporterOptions exporter_options;

    auto exporter = otlp_exporter::OtlpHttpMetricExporterFactory::Create(exporter_options);

    std::string version{"1.2.0"};
    std::string schema{"https://opentelemetry.io/schemas/1.2.0"};

    metric_sdk::PeriodicExportingMetricReaderOptions reader_options;

    reader_options.export_interval_millis = std::chrono::milliseconds(1000);
    reader_options.export_timeout_millis  = std::chrono::milliseconds(500);
    
    std::unique_ptr<metric_sdk::MetricReader> reader{new metric_sdk::PeriodicExportingMetricReader(std::move(exporter), reader_options)};
    auto provider = std::shared_ptr<metrics_api::MeterProvider>(new metric_sdk::MeterProvider());
    auto p        = std::static_pointer_cast<metric_sdk::MeterProvider>(provider);
    p->AddMetricReader(std::move(reader));

    metrics_api::Provider::SetMeterProvider(provider);
}
#include "telemetry.hpp"
#include <opentelemetry/exporters/otlp/otlp_http_metric_exporter_factory.h>
#include <opentelemetry/exporters/ostream/metric_exporter.h>
#include <opentelemetry/metrics/provider.h>
#include <opentelemetry/sdk/metrics/aggregation/default_aggregation.h>
#include <opentelemetry/sdk/metrics/export/periodic_exporting_metric_reader.h>
#include <opentelemetry/sdk/metrics/meter.h>
#include <opentelemetry/sdk/metrics/meter_provider.h>

namespace metrics::fixsed
{

const char* initiator_fix_message_in = "fixsed.initiator.fix.message.in";
const char* initiator_fix_message_out = "fixsed.initiator.fix.message.out";
const char* acceptor_fix_message_in = "fixsed.acceptor.fix.message.in";
const char* acceptor_fix_message_out = "fixsed.acceptor.fix.message.out";

}

namespace metric_sdk    = opentelemetry::sdk::metrics;
namespace common        = opentelemetry::common;
namespace metrics_api   = opentelemetry::metrics;
namespace otlp_exporter = opentelemetry::exporter::otlp;
namespace exporter_metrics = opentelemetry::exporter::metrics;

namespace {
    static std::string version{"1.2.0"};
    static std::string schema{"https://opentelemetry.io/schemas/1.2.0"};
}

void add_counter(const std::shared_ptr<metric_sdk::MeterProvider> provider, const std::string& name)
{
    std::string counter_name = name + "_counter";
    std::unique_ptr<metric_sdk::InstrumentSelector> instrument_selector{new metric_sdk::InstrumentSelector(metric_sdk::InstrumentType::kCounter, counter_name)};
    std::unique_ptr<metric_sdk::MeterSelector> meter_selector{new metric_sdk::MeterSelector(name, version, schema)};
    std::unique_ptr<metric_sdk::View> sum_view{new metric_sdk::View{name, "description", metric_sdk::AggregationType::kSum}};
    provider->AddView(std::move(instrument_selector), std::move(meter_selector), std::move(sum_view));
}

void initialise_telemetry()
{
    // otlp_exporter::OtlpHttpMetricExporterOptions exporter_options;
    // auto exporter = otlp_exporter::OtlpHttpMetricExporterFactory::Create(exporter_options);

    std::unique_ptr<metric_sdk::PushMetricExporter> exporter{new exporter_metrics::OStreamMetricExporter};

    metric_sdk::PeriodicExportingMetricReaderOptions reader_options;

    reader_options.export_interval_millis = std::chrono::milliseconds(1000);
    reader_options.export_timeout_millis  = std::chrono::milliseconds(500);
    
    std::unique_ptr<metric_sdk::MetricReader> reader{new metric_sdk::PeriodicExportingMetricReader(std::move(exporter), reader_options)};
    auto provider = std::shared_ptr<metrics_api::MeterProvider>(new metric_sdk::MeterProvider());
    auto meterProvider = std::static_pointer_cast<metric_sdk::MeterProvider>(provider);
    meterProvider->AddMetricReader(std::move(reader));

    // initiator messages in/out/processing time
    // acceptor messages in/out/processing time
    add_counter(meterProvider, "fixsed.initiator.fix.message.in");
    add_counter(meterProvider, "fixsed.initiator.fix.message.out");
    add_counter(meterProvider, "fixsed.acceptor.fix.message.in");
    add_counter(meterProvider, "fixsed.acceptor.fix.message.out");

    metrics_api::Provider::SetMeterProvider(provider);
}
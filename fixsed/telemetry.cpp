#include "telemetry.hpp"
#include <opentelemetry/exporters/otlp/otlp_grpc_metric_exporter_factory.h>
#include <opentelemetry/exporters/ostream/metric_exporter.h>
#include <opentelemetry/metrics/provider.h>
#include <opentelemetry/sdk/metrics/aggregation/default_aggregation.h>
#include <opentelemetry/sdk/metrics/export/periodic_exporting_metric_reader.h>
#include <opentelemetry/sdk/metrics/meter.h>
#include <opentelemetry/sdk/metrics/meter_provider.h>
//#include <opentelemetry/sdk/common/global_log_handler.h>

namespace metrics::fixsed
{

const char* initiator_fix_message_in = "fixsed.initiator.fix.message.in";
const char* initiator_fix_message_out = "fixsed.initiator.fix.message.out";
const char* acceptor_fix_message_in = "fixsed.acceptor.fix.message.in";
const char* acceptor_fix_message_out = "fixsed.acceptor.fix.message.out";

const char* initiator_fix_message_process = "fixsed.initiator.fix.message.process";
const char* acceptor_fix_message_process = "fixsed.acceptor.fix.message.process";

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

void add_counter(const std::string& name)
{
    std::string counter_name = name + "_counter";
    auto provider = metrics_api::Provider::GetMeterProvider();
    auto meter = provider->GetMeter(name, "1.2.0");
    auto counter = meter->CreateUInt64Counter(counter_name);
}

void add_histogram(const std::string& name)
{
    std::string histogram_name = name + "_histogram";
    auto provider = metrics_api::Provider::GetMeterProvider();
    auto meter = provider->GetMeter(name, "1.2.0");
    auto histogram = meter->CreateUInt64Histogram(histogram_name);    
}

void initialise_telemetry()
{
    // TODO - can we push this to the boost log?
    opentelemetry::sdk::common::internal_log::GlobalLogHandler::SetLogLevel(opentelemetry::sdk::common::internal_log::LogLevel::Debug);

    otlp_exporter::OtlpGrpcMetricExporterOptions exporter_options;
    exporter_options.endpoint = otlp_exporter::GetOtlpDefaultMetricsEndpoint();
    exporter_options.metadata = otlp_exporter::GetOtlpDefaultMetricsHeaders();
    exporter_options.aggregation_temporality = metric_sdk::AggregationTemporality::kDelta;
    auto exporter = otlp_exporter::OtlpGrpcMetricExporterFactory::Create(exporter_options);

    // std::unique_ptr<metric_sdk::PushMetricExporter> exporter{new exporter_metrics::OStreamMetricExporter(std::cout, metric_sdk::AggregationTemporality::kDelta)};

    metric_sdk::PeriodicExportingMetricReaderOptions reader_options;

    reader_options.export_interval_millis = std::chrono::milliseconds(1000);
    reader_options.export_timeout_millis  = std::chrono::milliseconds(500);
   
    std::unique_ptr<metric_sdk::MetricReader> reader{new metric_sdk::PeriodicExportingMetricReader(std::move(exporter), reader_options)};

    auto provider = std::shared_ptr<metrics_api::MeterProvider>(new metric_sdk::MeterProvider());
    auto meterProvider = std::static_pointer_cast<metric_sdk::MeterProvider>(provider);
    meterProvider->AddMetricReader(std::move(reader));
    metrics_api::Provider::SetMeterProvider(provider);

    add_counter(metrics::fixsed::initiator_fix_message_in);
    add_counter(metrics::fixsed::initiator_fix_message_out);
    add_counter(metrics::fixsed::acceptor_fix_message_in);
    add_counter(metrics::fixsed::acceptor_fix_message_out);

    add_histogram(metrics::fixsed::initiator_fix_message_process);
    add_histogram(metrics::fixsed::acceptor_fix_message_process);
}
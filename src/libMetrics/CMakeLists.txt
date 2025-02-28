find_package(CURL REQUIRED)
find_package(opentelemetry-cpp REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)
find_package(re2 CONFIG REQUIRED)



set_target_properties(opentelemetry-cpp::opentelemetry_exporter_prometheus PROPERTIES
        INTERFACE_LINK_LIBRARIES "opentelemetry-cpp::metrics"
        )

add_library(Metrics Metrics.cpp Tracing.cpp Api.h Metrics.h Tracing.h Common.h internal/mixins.h)

target_include_directories(Metrics PUBLIC ${PROJECT_SOURCE_DIR}/src ${CMAKE_BINARY_DIR}/src ${CURL_INCLUDE_DIRS})
target_link_libraries(Metrics
        INTERFACE
        Threads::Threads
        CURL::libcurl
        PUBLIC
        g3log
        Constants
        protobuf::libprotobuf
        opentelemetry-cpp::api
        opentelemetry-cpp::sdk
        opentelemetry-cpp::logs
        opentelemetry-cpp::trace
        opentelemetry-cpp::ostream_span_exporter
        opentelemetry-cpp::ostream_metrics_exporter
        opentelemetry-cpp::otlp_http_metric_exporter
        opentelemetry-cpp::otlp_http_exporter
        opentelemetry-cpp::opentelemetry_exporter_prometheus
        opentelemetry-cpp::otlp_grpc_metrics_exporter
        opentelemetry-cpp::otlp_grpc_exporter
        PRIVATE
        ${g3sinks_LOGROTATE_LIBRARY}
        Boost::filesystem)

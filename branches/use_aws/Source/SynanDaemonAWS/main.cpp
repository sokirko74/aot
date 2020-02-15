// https://aws.amazon.com/ru/blogs/compute/introducing-the-c-lambda-runtime/
// aws iam create-role --role-name synan-daemon-invoker-role --assume-role-policy-document file://trust-policy.json
// "Arn": "arn:aws:iam::516891605423:role/synan-daemon-invoker-role",
// $ make -C ~/RML/cmake-build-debug/ aws-lambda-package-SynanDaemonAWS
// aws lambda delete-function --function-name invoke-synan
// aws lambda create-function --function-name invoke-synan --role arn:aws:iam::516891605423:role/synan-daemon-invoker-role --runtime provided --timeout 15 --memory-size 256 --handler synan-daemon --zip-file fileb:///home/sokirko/RML/cmake-build-debug/Source/SynanDaemonAWS/SynanDaemonAWS.zip
// aws lambda invoke --function-name invoke-synan  --payload '{"action":"morph" }' output.txt

#include <aws/core/Aws.h>
#include <aws/core/utils/logging/LogLevel.h>
#include <aws/core/utils/logging/ConsoleLogSystem.h>
#include <aws/core/utils/logging/LogMacros.h>
#include <aws/core/utils/json/JsonSerializer.h>
#include <aws/core/utils/HashingUtils.h>
#include <aws/core/platform/Environment.h>
#include <aws/core/client/ClientConfiguration.h>
#include <aws/core/auth/AWSCredentialsProvider.h>
#include <aws/s3/S3Client.h>
#include <aws/s3/model/GetObjectRequest.h>
#include <aws/lambda-runtime/runtime.h>
#include <iostream>
#include <fstream>

using namespace aws::lambda_runtime;

invocation_response my_handler(invocation_request const& request)
{
    using namespace Aws::Utils::Json;
    JsonValue json(request.payload);
    if (!json.WasParseSuccessful()) {
        return invocation_response::failure("Failed to parse input JSON", "InvalidJSON");
    }
    auto v = json.View();
    auto action = v.GetString("action");
    return invocation_response::success(action, "application/json");
}

char const TAG[] = "LAMBDA_ALLOC";

void download_and_save_file(
        Aws::S3::S3Client const& client,
        Aws::String const& bucket,
        Aws::String const& key,
        const char* fileName)
{
    using namespace Aws;
    AWS_LOGSTREAM_INFO(TAG, "try download dicts");

    S3::Model::GetObjectRequest request;
    request.WithBucket(bucket).WithKey(key);

    auto outcome = client.GetObject(request);
    if (outcome.IsSuccess()) {
        AWS_LOGSTREAM_INFO(TAG, "Download completed!");
        auto &retrieved_file = outcome.GetResultWithOwnership().GetBody();

        std::ofstream output_file(fileName, std::ios::binary);
        output_file << retrieved_file.rdbuf();
    }
    else {
        AWS_LOGSTREAM_ERROR(TAG, "Failed with error: " << outcome.GetError());
        return;
    }
}

int main()
{
    using namespace Aws;
    Aws::Client::ClientConfiguration config;
    config.region = Aws::Environment::GetEnv("AWS_REGION");
    config.caFile = "/etc/pki/tls/certs/ca-bundle.crt";

    auto credentialsProvider = Aws::MakeShared<Aws::Auth::EnvironmentAWSCredentialsProvider>(TAG);
    S3::S3Client client(credentialsProvider, config);
    download_and_save_file(client, "morph.aot.ru", "morph.tar.gz", "/tmp/morph.tar.gz");

    run_handler(my_handler);
    return 0;
}

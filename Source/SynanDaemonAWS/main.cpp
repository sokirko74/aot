// https://aws.amazon.com/ru/blogs/compute/introducing-the-c-lambda-runtime/
// aws iam create-role --role-name synan-daemon-invoker-role --assume-role-policy-document file://trust-policy.json
// "Arn": "arn:aws:iam::516891605423:role/synan-daemon-invoker-role",
// $ make -C ~/RML/cmake-build-debug/ aws-lambda-package-SynanDaemonAWS
// aws lambda delete-function --function-name invoke-synan
// aws lambda create-function --function-name invoke-synan --role arn:aws:iam::516891605423:role/synan-daemon-invoker-role --runtime provided --timeout 15 --memory-size 128 --handler synan-daemon --zip-file fileb:///home/sokirko/RML/cmake-build-debug/Source/SynanDaemonAWS/SynanDaemonAWS.zip
// aws lambda invoke --function-name hello-world --payload '{ }' output.txt

#include <aws/lambda-runtime/runtime.h>
#include <aws/core/utils/json/JsonSerializer.h>

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

int main()
{
   run_handler(my_handler);
   return 0;
}

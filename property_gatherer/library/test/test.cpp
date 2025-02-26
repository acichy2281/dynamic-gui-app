#include "property_consumer_api.h"

namespace PropertyGatherer
{
    void TestSerializeGetValueReq() {
        GetValueReq_T message = {{0x01, 0x02, 0x1234}, 100, 2, {10, 20}};
        std::vector<uint8_t> outBuff;

        PropertyGathererMessageSerializer serializer;
        uint16_t size = serializer.Serialize(message, outBuff);

        // Manually check the serialized buffer
        std::vector<uint8_t> expected = {0x01, 0x02, 0x34, 0x12, 100, 0, 2, 0, 10, 0, 20, 0};
        if (outBuff == expected) {
            std::cout << "SerializeGetValueReq passed.\n";
        } else {
            std::cout << "SerializeGetValueReq failed.\n";
        }
    }

    void TestSerializeGetValueReply() {
        GetValueReply_T message = { {0x01, 0x02, 0x1234}, {PropertyStorageVariant{uint8_t(1)}, PropertyStorageVariant{std::string("Test")}}, 0 };
        std::vector<uint8_t> outBuff;

        PropertyGathererMessageSerializer serializer;
        uint16_t size = serializer.Serialize(message, outBuff);

        // You would manually need to construct the expected serialized data based on the variant types
        std::vector<uint8_t> expected = {0x01, 0x02, 0x34, 0x12, 0, 1, 1, 'T', 'e', 's', 't', 0, 0, 0};
        if (outBuff == expected) {
            std::cout << "SerializeGetValueReply passed.\n";
        } else {
            std::cout << "SerializeGetValueReply failed.\n";
        }
    }

    void TestDeserializeGetValueReq() {
        std::vector<uint8_t> inBuff = {0x01, 0x02, 0x34, 0x12, 100, 0, 2, 0, 10, 0, 20, 0};
        GetValueReq_T message;

        PropertyGathererMessageSerializer serializer;
        bool success = serializer.Deserialize(message, inBuff);

        if (success && message.header.protocolIdHigh == 0x01 && message.header.protocolIdLow == 0x02 &&
            message.header.messageId == 0x1234 && message.maxRespLen == 100 && message.numReqProp == 2 &&
            message.propIds == std::vector<uint16_t>{10, 20}) {
            std::cout << "DeserializeGetValueReq passed.\n";
        } else {
            std::cout << "DeserializeGetValueReq failed.\n";
        }
    }

    void TestDeserializeGetValueReply() {
        std::vector<uint8_t> inBuff = { 0x01, 0x02, 0x34, 0x12, 0, 1, 1, 'T', 'e', 's', 't', 0, 0, 0};
        GetValueReply_T message;

        PropertyGathererMessageSerializer serializer;
        bool success = serializer.Deserialize(message, inBuff);
        std::vector<PropertyStorageVariant> expectedPropList = { PropertyStorageVariant{uint8_t(1)}, PropertyStorageVariant{std::string("Test")} };

        if (success && message.header.protocolIdHigh == 0x01 && message.header.protocolIdLow == 0x02 &&
            message.header.messageId == 0x1234 && message.status == 0 && message.propValues == expectedPropList) {
            std::cout << "DeserializeGetValueReply passed.\n";
        } else {
            std::cout << "DeserializeGetValueReply failed.\n";
        }
    }
}


int main(int argc, char** argv)
{
    PropertyGatherer::TestSerializeGetValueReq();
    PropertyGatherer::TestSerializeGetValueReply();
    PropertyGatherer::TestDeserializeGetValueReq();
    PropertyGatherer::TestDeserializeGetValueReply();

    std::cout << "Test Complete\n";
    return 0;
}
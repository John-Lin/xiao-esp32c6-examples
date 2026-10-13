#include <cstdint>
#include <cstdio>

#include "../MQTTLed/LedCommand.h"

int main() {
  // Wrong polarity, prefix matching, or treating payload as a C string must fail.
  const uint8_t on[] = {'O', 'N'};
  const uint8_t off[] = {'O', 'F', 'F'};
  const uint8_t onWithNull[] = {'O', 'N', 0};
  const uint8_t offWithSuffix[] = {'O', 'F', 'F', 'X'};
  const uint8_t lowercase[] = {'o', 'n'};
  const uint8_t whitespace[] = {'O', 'N', '\n'};
  const uint8_t unknown[] = {'1'};

  struct TestCase {
    const char* name;
    const char* topic;
    const uint8_t* payload;
    unsigned int length;
    int expected;
  };

  const TestCase cases[] = {
      {"ON drives active-low LED low", "xiao/led/set", on, 2, 0},
      {"OFF drives active-low LED high", "xiao/led/set", off, 3, 1},
      {"other topic is ignored", "other/led/set", on, 2, -1},
      {"topic suffix is ignored", "xiao/led/set/extra", on, 2, -1},
      {"empty payload is ignored", "xiao/led/set", nullptr, 0, -1},
      {"truncated ON is ignored", "xiao/led/set", on, 1, -1},
      {"truncated OFF is ignored", "xiao/led/set", off, 2, -1},
      {"embedded null is ignored", "xiao/led/set", onWithNull, 3, -1},
      {"suffix is ignored", "xiao/led/set", offWithSuffix, 4, -1},
      {"lowercase is ignored", "xiao/led/set", lowercase, 2, -1},
      {"whitespace is ignored", "xiao/led/set", whitespace, 3, -1},
      {"unknown command is ignored", "xiao/led/set", unknown, 1, -1},
  };

  int failures = 0;
  for (const TestCase& test : cases) {
    int actual = ledLevelForMessage(test.topic, test.payload, test.length);
    if (actual != test.expected) {
      std::fprintf(stderr, "FAIL: %s: expected %d, got %d\n", test.name,
                   test.expected, actual);
      ++failures;
    }
  }

  if (failures != 0) {
    return 1;
  }
  std::puts("PASS: 12 MQTT LED command cases");
}

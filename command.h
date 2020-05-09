#pragma once
#define CALL(name, ...)	   ___##name##_main(__VA_ARGS__)
#define COMMAND(name, ...) int CALL(name, __VA_ARGS__)
#define DECLARE(name, ...) COMMAND(name, );

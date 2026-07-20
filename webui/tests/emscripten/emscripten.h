#pragma once

// Native smoke tests use this no-op definition. Real web builds include the
// Emscripten SDK header instead because this directory is not on their path.
#define EMSCRIPTEN_KEEPALIVE

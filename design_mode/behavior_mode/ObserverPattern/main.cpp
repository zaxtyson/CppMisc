#include "Sync/sync.h"
#include "Async/async1.h"
#include <fmt/format.h>

int main(){
    sync::test();
    fmt::print("\n\n\n");
    async::test_async1();
}
import crocofix.utility;

#include <catch2/catch_all.hpp>
#include <thread>
#include <chrono>

using namespace crocofix;

namespace {
constexpr auto dequeue_timeout = std::chrono::milliseconds(200);
constexpr auto short_dequeue_timeout = std::chrono::milliseconds(50);
}

TEST_CASE("blocking queue", "[blocking queue]") {

    SECTION("enqueue and dequeue from the same thread") {
        blocking_queue<int> queue;
        const auto expected = 55;
        queue.enqueue(expected);
        auto actual = queue.dequeue();
        REQUIRE(actual == expected);
    }

    SECTION("enqueue and dequeue from different threads") {
        blocking_queue<int> queue;
        const auto expected = 55;
        auto thread = std::thread([&](){ queue.enqueue(expected); });
        auto actual = queue.dequeue();
        REQUIRE(actual == expected);
        thread.join();
    }

    SECTION("try_dequeue fails on empty queue") {
        blocking_queue<int> queue;
        int value = 0;
        REQUIRE(!queue.try_dequeue(value, dequeue_timeout));
    }

     SECTION("enqueue and try_dequeue from the same thread") {
        blocking_queue<int> queue;
        const auto expected = 55;
        queue.enqueue(expected);
        int actual = 0;
        REQUIRE(queue.try_dequeue(actual, dequeue_timeout));
        REQUIRE(actual == expected);
    }

    SECTION("enqueue and try_dequeue from different threads") {
        blocking_queue<int> queue;
        const auto expected = 55;
        auto thread = std::thread([&](){ queue.enqueue(expected); });
        int actual = 0;
        REQUIRE(queue.try_dequeue(actual, dequeue_timeout));
        REQUIRE(actual == expected);
        thread.join();
    }

    SECTION("slow enqueue and fast try_dequeue from different threads") {
        blocking_queue<int> queue;
        const auto expected = 55;
        auto thread = std::thread([&](){ 
            std::this_thread::sleep_for(dequeue_timeout);
            queue.enqueue(expected);
        });
        int actual = 0;
        REQUIRE(!queue.try_dequeue(actual, short_dequeue_timeout));
        REQUIRE(actual == 0);
        thread.join();
    }
    
}
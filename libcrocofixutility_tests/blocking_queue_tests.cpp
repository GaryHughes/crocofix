#include <catch.hpp>
#include <libcrocofixutility/blocking_queue.hpp>
#include <thread>
#include <chrono>

using namespace crocofix;

TEST_CASE("blocking queue", "[blocking queue]") {

/*
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
        REQUIRE(!queue.try_dequeue(value, std::chrono::milliseconds(200))); // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
    }

     SECTION("enqueue and try_dequeue from the same thread") {
        blocking_queue<int> queue;
        const auto expected = 55;
        queue.enqueue(expected);
        int actual = 0;
        REQUIRE(queue.try_dequeue(actual, std::chrono::milliseconds(200))); // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
        REQUIRE(actual == expected);
    }

    SECTION("enqueue and try_dequeue from different threads") {
        blocking_queue<int> queue;
        const auto expected = 55;
        auto thread = std::thread([&](){ queue.enqueue(expected); });
        int actual = 0;
        REQUIRE(queue.try_dequeue(actual, std::chrono::milliseconds(200))); // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
        REQUIRE(actual == expected);
        thread.join();
    }

    SECTION("slow enqueue and fast try_dequeue from different threads") {
        blocking_queue<int> queue;
        const auto expected = 55;
        auto thread = std::thread([&](){ 
            std::this_thread::sleep_for(std::chrono::milliseconds(200)); // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
            queue.enqueue(expected); 
        });
        int actual = 0;
        REQUIRE(!queue.try_dequeue(actual, std::chrono::milliseconds(50))); // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
        REQUIRE(actual == 0);
        thread.join();
    }
    */
}
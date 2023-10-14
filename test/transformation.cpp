import <stl>;
import <glm>; 
import resource;
import input;
import context;

#include <snitch_all.hpp>

TEST_CASE("transformation", "[transformation]") {
    auto r1 = glm::rotate(glm::radians(10.0f), glm::vec3{1.0f, 0.0f, 0.0f});
    auto r2 = glm::rotate(glm::radians(20.0f), glm::vec3{0.0f, 1.0f, 0.0f});
    auto r3 = glm::rotate(glm::radians(30.0f), glm::vec3{0.0f, 1.0f, 1.0f});
    // 证明旋转矩阵之间同样不满足交换律
    REQUIRE(r1 * r2 * r3 != r1 * r3 * r2);
    REQUIRE(r1 * r2 * r3 != r2 * r1 * r3);
    REQUIRE(r1 * r2 * r3 != r2 * r3 * r1);
    REQUIRE(r1 * r2 * r3 != r3 * r1 * r2);
    REQUIRE(r1 * r2 * r3 != r3 * r2 * r1);
}
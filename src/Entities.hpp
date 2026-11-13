#pragma once
#include <SFML/Graphics.hpp>
#include <cmath>
#include <string>
#include <vector>

inline float length(sf::Vector2f v) { return std::sqrt(v.x * v.x + v.y * v.y); }
inline sf::Vector2f normalize(sf::Vector2f v) {
    float l = length(v);
    return l > 0.f ? v / l : sf::Vector2f(0.f, 0.f);
}

// Kael Draven, o Cavaleiro Errante
struct Player {
    sf::Vector2f pos{800.f, 600.f};
    sf::Vector2f facing{0.f, 1.f};
    float speed = 180.f;
    int hp = 100;
    int maxHp = 100;
    float attackCooldown = 0.f;
    float attackTimer = 0.f;
    float hurtTimer = 0.f;
};

// Criatura de Malakar
struct Enemy {
    sf::Vector2f pos;
    int hp = 30;
    float speed = 70.f;
    float hitFlash = 0.f;
    bool alive = true;
};

struct Npc {
    sf::Vector2f pos;
    std::string name;
    std::vector<std::string> lines;
    sf::Color color;
};

// Faisca, a Raposa-Espirito
struct Companion {
    sf::Vector2f pos;
    float t = 0.f;
};

struct House {
    sf::FloatRect rect;
};

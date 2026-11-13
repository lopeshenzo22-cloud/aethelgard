#pragma once
#include "Entities.hpp"

enum class State { Title, Playing, Dialogue, GameOver, Victory };

class Game {
public:
    Game();
    void run();

private:
    void setupWorld();
    void handleEvent(const sf::Event& e);
    void update(float dt);
    void render();
    void renderWorld();
    void renderHud();

    void attack();
    void spawnWave();
    void startDialogue(const std::string& speaker, const std::vector<std::string>& lines);
    void endDialogue();
    void drawText(const std::string& s, float x, float y, unsigned size,
                  sf::Color c = sf::Color::White, bool centered = false);

    sf::RenderWindow window;
    sf::View camera;
    sf::Font font;
    bool hasFont = false;

    State state = State::Title;
    int stage = 0;   // 0 = antes do ataque, 1 = invasao, 2 = juramento
    int wave = 0;
    int kills = 0;
    float time = 0.f;
    std::string objective;

    Player player;
    Companion faisca;
    std::vector<Enemy> enemies;
    std::vector<Npc> npcs;
    std::vector<House> houses;

    std::string dlgSpeaker;
    std::vector<std::string> dlgLines;
    std::size_t dlgIndex = 0;

    const sf::Vector2f worldSize{1600.f, 1200.f};
    static constexpr int kTotalWaves = 3;
};

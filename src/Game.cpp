#include "Game.hpp"
#include <algorithm>
#include <cstdlib>
#include <ctime>

namespace {
const char* kFontPaths[] = {
    "assets/fonts/font.ttf",
    "C:/Windows/Fonts/arial.ttf",
    "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
    "/usr/share/fonts/TTF/DejaVuSans.ttf",
    "/System/Library/Fonts/Supplemental/Arial.ttf",
    "/Library/Fonts/Arial.ttf",
};

float clampf(float v, float lo, float hi) { return std::max(lo, std::min(v, hi)); }

float randf(float lo, float hi) {
    return lo + static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX) * (hi - lo);
}

sf::CircleShape circle(float r, sf::Vector2f pos, sf::Color c) {
    sf::CircleShape s(r);
    s.setOrigin(r, r);
    s.setPosition(pos);
    s.setFillColor(c);
    return s;
}
}  // namespace

Game::Game() : window(sf::VideoMode(960, 540), "Cronicas de Aethelgard") {
    window.setFramerateLimit(60);
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    for (const char* p : kFontPaths) {
        if (font.loadFromFile(p)) { hasFont = true; break; }
    }
    camera = window.getDefaultView();
    setupWorld();
    state = State::Title;
}

void Game::setupWorld() {
    player = Player{};
    faisca.pos = player.pos + sf::Vector2f(30.f, 0.f);
    enemies.clear();
    npcs.clear();
    houses.clear();
    stage = 0;
    wave = 0;
    kills = 0;

    houses = {
        {{600.f, 380.f, 120.f, 90.f}}, {{900.f, 360.f, 140.f, 100.f}},
        {{560.f, 720.f, 110.f, 90.f}}, {{960.f, 740.f, 130.f, 95.f}},
        {{760.f, 860.f, 100.f, 80.f}},
    };

    npcs.push_back({{860.f, 540.f}, "Ancião de Pedrabranca",
                    {"Kael! Que bom que voltou da patrulha.",
                     "Ouvi uivos estranhos vindos da floresta... algo ronda Aethelgard.",
                     "Dizem que Malakar, o Devorador de Reinos, despertou.",
                     "Nenhuma arma comum pode feri-lo. Só a lendária Aurelion...",
                     "Fique atento, filho. Se algo acontecer, proteja a aldeia!"},
                    sf::Color(210, 190, 130)});

    objective = "Fale com o Ancião (chegue perto e aperte E)";
    state = State::Playing;
}

void Game::run() {
    sf::Clock clock;
    while (window.isOpen()) {
        sf::Event e;
        while (window.pollEvent(e)) handleEvent(e);
        float dt = std::min(clock.restart().asSeconds(), 0.05f);
        update(dt);
        render();
    }
}

void Game::handleEvent(const sf::Event& e) {
    if (e.type == sf::Event::Closed) { window.close(); return; }
    if (e.type != sf::Event::KeyPressed) return;
    const auto k = e.key.code;
    if (k == sf::Keyboard::Escape) { window.close(); return; }

    switch (state) {
        case State::Title:
            if (k == sf::Keyboard::Enter) state = State::Playing;
            break;
        case State::Dialogue:
            if (k == sf::Keyboard::E || k == sf::Keyboard::Enter || k == sf::Keyboard::Space) {
                if (++dlgIndex >= dlgLines.size()) endDialogue();
            }
            break;
        case State::Playing:
            if (k == sf::Keyboard::E && stage == 0) {
                for (const auto& n : npcs) {
                    if (length(n.pos - player.pos) < 70.f) { startDialogue(n.name, n.lines); break; }
                }
            }
            if ((k == sf::Keyboard::Space || k == sf::Keyboard::J) && player.attackCooldown <= 0.f) {
                player.attackTimer = 0.15f;
                player.attackCooldown = 0.35f;
                attack();
            }
            break;
        case State::GameOver:
        case State::Victory:
            if (k == sf::Keyboard::Enter) setupWorld();
            break;
    }
}

void Game::startDialogue(const std::string& speaker, const std::vector<std::string>& lines) {
    dlgSpeaker = speaker;
    dlgLines = lines;
    dlgIndex = 0;
    state = State::Dialogue;
}

void Game::endDialogue() {
    state = State::Playing;
    if (stage == 0) {
        stage = 1;
        spawnWave();
    } else if (stage == 2) {
        state = State::Victory;
    }
}

void Game::spawnWave() {
    ++wave;
    objective = "Criaturas de Malakar atacam! Defenda Pedrabranca (onda " +
                std::to_string(wave) + "/" + std::to_string(kTotalWaves) + ")";
    const int count = 2 + wave * 2;
    for (int i = 0; i < count; ++i) {
        Enemy en;
        switch (std::rand() % 4) {
            case 0: en.pos = {randf(0.f, worldSize.x), 20.f}; break;
            case 1: en.pos = {randf(0.f, worldSize.x), worldSize.y - 20.f}; break;
            case 2: en.pos = {20.f, randf(0.f, worldSize.y)}; break;
            default: en.pos = {worldSize.x - 20.f, randf(0.f, worldSize.y)}; break;
        }
        en.hp = 20 + wave * 10;
        en.speed = 60.f + wave * 12.f;
        enemies.push_back(en);
    }
}

void Game::attack() {
    const sf::Vector2f hit = player.pos + player.facing * 28.f;
    for (auto& en : enemies) {
        if (!en.alive || length(en.pos - hit) > 36.f) continue;
        en.hp -= 15;
        en.hitFlash = 0.12f;
        en.pos += normalize(en.pos - player.pos) * 24.f;
        if (en.hp <= 0) { en.alive = false; ++kills; }
    }
}

void Game::update(float dt) {
    time += dt;
    if (state != State::Playing) return;

    // Movimento de Kael (WASD ou setas)
    sf::Vector2f dir{0.f, 0.f};
    using K = sf::Keyboard;
    if (K::isKeyPressed(K::W) || K::isKeyPressed(K::Up)) dir.y -= 1.f;
    if (K::isKeyPressed(K::S) || K::isKeyPressed(K::Down)) dir.y += 1.f;
    if (K::isKeyPressed(K::A) || K::isKeyPressed(K::Left)) dir.x -= 1.f;
    if (K::isKeyPressed(K::D) || K::isKeyPressed(K::Right)) dir.x += 1.f;
    if (length(dir) > 0.f) {
        dir = normalize(dir);
        player.facing = dir;
    }
    player.pos += dir * player.speed * dt;
    player.pos.x = clampf(player.pos.x, 16.f, worldSize.x - 16.f);
    player.pos.y = clampf(player.pos.y, 16.f, worldSize.y - 16.f);

    player.attackCooldown -= dt;
    player.attackTimer -= dt;
    player.hurtTimer -= dt;

    // Faisca segue Kael
    faisca.t += dt;
    sf::Vector2f target = player.pos - player.facing * 34.f +
                          sf::Vector2f(std::cos(faisca.t * 2.f) * 10.f, std::sin(faisca.t * 3.f) * 8.f);
    faisca.pos += (target - faisca.pos) * std::min(1.f, 4.f * dt);

    // Inimigos perseguem Kael
    for (auto& en : enemies) {
        en.hitFlash -= dt;
        sf::Vector2f toPlayer = player.pos - en.pos;
        en.pos += normalize(toPlayer) * en.speed * dt;
        if (length(toPlayer) < 24.f && player.hurtTimer <= 0.f) {
            player.hp -= 10;
            player.hurtTimer = 0.6f;
            player.pos += normalize(toPlayer) * 20.f;
        }
    }
    enemies.erase(std::remove_if(enemies.begin(), enemies.end(),
                                 [](const Enemy& e) { return !e.alive; }),
                  enemies.end());

    if (stage == 1 && enemies.empty()) {
        if (wave < kTotalWaves) {
            spawnWave();
        } else {
            stage = 2;
            objective = "Pedrabranca está em ruínas...";
            startDialogue("Kael Draven",
                          {"Pedrabranca... reduzida a cinzas.",
                           "Faísca, você está bem? Ainda bem que você ficou comigo.",
                           "Malakar fez isso. E nenhuma arma comum pode detê-lo.",
                           "Então eu vou encontrar Eldrion, o Último Guardião.",
                           "Eu juro: vou reunir os 5 Selos da Aurora e empunhar Aurelion!"});
        }
    }

    if (player.hp <= 0) state = State::GameOver;

    // Camera segue Kael sem sair do mapa
    sf::Vector2f half = camera.getSize() / 2.f;
    camera.setCenter(clampf(player.pos.x, half.x, worldSize.x - half.x),
                     clampf(player.pos.y, half.y, worldSize.y - half.y));
}

void Game::drawText(const std::string& s, float x, float y, unsigned size, sf::Color c, bool centered) {
    if (!hasFont) return;
    sf::Text t(sf::String::fromUtf8(s.begin(), s.end()), font, size);
    t.setFillColor(c);
    if (centered) {
        auto b = t.getLocalBounds();
        t.setOrigin(b.left + b.width / 2.f, 0.f);
    }
    t.setPosition(x, y);
    window.draw(t);
}

void Game::renderWorld() {
    window.setView(camera);

    sf::RectangleShape ground(worldSize);
    ground.setFillColor(stage == 0 ? sf::Color(70, 120, 60) : sf::Color(60, 80, 50));
    window.draw(ground);

    // Estrada da aldeia
    sf::RectangleShape road({worldSize.x, 60.f});
    road.setPosition(0.f, 580.f);
    road.setFillColor(sf::Color(150, 130, 95));
    window.draw(road);

    for (const auto& h : houses) {
        sf::RectangleShape r({h.rect.width, h.rect.height});
        r.setPosition(h.rect.left, h.rect.top);
        r.setFillColor(sf::Color(225, 220, 205));  // casas de pedra branca
        r.setOutlineThickness(3.f);
        r.setOutlineColor(sf::Color(110, 70, 40));
        window.draw(r);
        if (stage >= 1) {  // aldeia em chamas
            float flicker = 6.f + std::sin(time * 12.f + h.rect.left) * 4.f;
            window.draw(circle(14.f + flicker, {h.rect.left + h.rect.width / 2.f, h.rect.top},
                               sf::Color(255, 120, 20, 200)));
        }
    }

    for (const auto& n : npcs) {
        window.draw(circle(15.f, n.pos, n.color));
        if (stage == 0 && length(n.pos - player.pos) < 70.f)
            drawText("[E] Falar", n.pos.x, n.pos.y - 42.f, 14, sf::Color::Yellow, true);
    }

    for (const auto& en : enemies) {
        sf::Color c = en.hitFlash > 0.f ? sf::Color::White : sf::Color(120, 30, 140);
        window.draw(circle(13.f, en.pos, c));
        window.draw(circle(3.f, en.pos + sf::Vector2f(-4.f, -3.f), sf::Color::Red));
        window.draw(circle(3.f, en.pos + sf::Vector2f(4.f, -3.f), sf::Color::Red));
    }

    // Faisca com brilho
    window.draw(circle(14.f, faisca.pos, sf::Color(255, 170, 60, 70)));
    window.draw(circle(7.f, faisca.pos, sf::Color(255, 150, 40)));

    // Kael
    sf::Color kc = (player.hurtTimer > 0.f && static_cast<int>(time * 20.f) % 2 == 0)
                       ? sf::Color(255, 120, 120) : sf::Color(60, 110, 220);
    window.draw(circle(16.f, player.pos, kc));
    if (player.attackTimer > 0.f) {
        window.draw(circle(18.f, player.pos + player.facing * 28.f, sf::Color(230, 230, 255, 160)));
    } else {
        window.draw(circle(5.f, player.pos + player.facing * 14.f, sf::Color(200, 200, 210)));
    }
}

void Game::renderHud() {
    window.setView(window.getDefaultView());

    sf::RectangleShape back({204.f, 20.f});
    back.setPosition(16.f, 16.f);
    back.setFillColor(sf::Color(40, 0, 0));
    window.draw(back);
    sf::RectangleShape bar({200.f * std::max(0, player.hp) / player.maxHp, 16.f});
    bar.setPosition(18.f, 18.f);
    bar.setFillColor(sf::Color(200, 30, 30));
    window.draw(bar);
    drawText("Kael Draven  " + std::to_string(std::max(0, player.hp)) + "/" +
             std::to_string(player.maxHp), 230.f, 16.f, 16);
    drawText("Objetivo: " + objective, 16.f, 44.f, 16, sf::Color(255, 230, 150));
    drawText("Mover: WASD/Setas   Atacar: Espaço/J   Interagir: E", 16.f, 512.f, 14,
             sf::Color(200, 200, 200));

    if (state == State::Dialogue && dlgIndex < dlgLines.size()) {
        sf::RectangleShape box({900.f, 120.f});
        box.setPosition(30.f, 400.f);
        box.setFillColor(sf::Color(15, 15, 30, 230));
        box.setOutlineThickness(2.f);
        box.setOutlineColor(sf::Color(200, 170, 90));
        window.draw(box);
        drawText(dlgSpeaker, 50.f, 410.f, 18, sf::Color(255, 210, 110));
        drawText(dlgLines[dlgIndex], 50.f, 445.f, 18);
        drawText("[E] continuar", 820.f, 490.f, 14, sf::Color(180, 180, 180));
    }
}

void Game::render() {
    window.clear(sf::Color(10, 10, 20));

    if (state == State::Title) {
        window.setView(window.getDefaultView());
        drawText("Crônicas de Aethelgard", 480.f, 150.f, 44, sf::Color(255, 210, 110), true);
        drawText("A Espada do Último Guardião", 480.f, 210.f, 24, sf::Color::White, true);
        drawText("Prólogo: A Queda de Pedrabranca", 480.f, 280.f, 20, sf::Color(200, 200, 200), true);
        drawText("Aperte ENTER para começar", 480.f, 380.f, 18, sf::Color::Yellow, true);
        window.display();
        return;
    }

    renderWorld();
    renderHud();

    if (state == State::GameOver || state == State::Victory) {
        sf::RectangleShape shade(sf::Vector2f(window.getSize()));
        shade.setFillColor(sf::Color(0, 0, 0, 180));
        window.draw(shade);
        if (state == State::GameOver) {
            drawText("Kael caiu...", 480.f, 200.f, 40, sf::Color(220, 60, 60), true);
        } else {
            drawText("Fim do Prólogo", 480.f, 170.f, 40, sf::Color(255, 210, 110), true);
            drawText("Kael e Faísca partem rumo à Cidadela de Halvorn.", 480.f, 240.f, 20,
                     sf::Color::White, true);
            drawText("Criaturas derrotadas: " + std::to_string(kills), 480.f, 280.f, 18,
                     sf::Color(200, 200, 200), true);
        }
        drawText("ENTER para recomeçar  |  ESC para sair", 480.f, 360.f, 18, sf::Color::Yellow, true);
    }

    window.display();
}

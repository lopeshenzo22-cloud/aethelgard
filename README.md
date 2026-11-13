# Crônicas de Aethelgard: A Espada do Último Guardião

RPG de aventura 2D em **C++17 + SFML 2.6**, visão de cima.

O mundo de Aethelgard está sendo devorado por **Malakar, o Devorador de Reinos**. Só **Aurelion, a Lâmina da Aurora**, guardada por **Eldrion, o Último Guardião**, pode feri-lo. Reúna os **5 Selos da Aurora** para encontrá-lo.

## Estado atual

**Prólogo jogável — A Queda de Pedrabranca.** Fale com o Ancião, defenda a aldeia de 3 ondas de criaturas de Malakar com Kael Draven e a raposa-espírito Faísca, e assista ao juramento de Kael. Veja o [ROADMAP](ROADMAP.md).

## Controles

| Ação | Tecla |
|---|---|
| Mover | WASD / Setas |
| Atacar | Espaço / J |
| Falar / avançar diálogo | E |
| Sair | ESC |

## Como compilar

Precisa de **CMake 3.16+**, um compilador C++17 e **git**. O SFML é baixado automaticamente.

```bash
git clone https://github.com/lopeshenzo22-cloud/aethelgard.git
cd aethelgard
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

Rodar:
- Linux/macOS: `./build/aethelgard`
- Windows: `build\Release\aethelgard.exe`

No Linux, instale antes as dependências do SFML (Ubuntu/Debian):

```bash
sudo apt install libx11-dev libxrandr-dev libxcursor-dev libxi-dev libudev-dev libgl1-mesa-dev libfreetype-dev
```

## Estrutura

```
src/
  main.cpp      ponto de entrada
  Game.hpp/cpp  loop, estados, prólogo, render e HUD
  Entities.hpp  Kael, inimigos, NPCs, Faísca
assets/         fontes e (futuramente) sprites e mapas
```

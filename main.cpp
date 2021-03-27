#include <SFML/Graphics.hpp>
#include <math.h>
#include <vector>
#include <random>
#include <string>
#include <iostream>

int gridSize = 48;

enum blockType
{
    wall, food, snake
};

class block
{
private:
    sf::Vector2f pos;
    sf::RectangleShape shape;
    blockType type;
    bool isFood;

public:
    sf::Vector2f getPos()
    {
        return this->pos;
    }

    void setPos(sf::Vector2f pos)
    {
        this->pos = pos;
        shape.setPosition(this->pos);
    }

    sf::Color color()
    {
        switch (this->type)
        {
        case wall:
            return sf::Color::White;
        case food:
            return sf::Color::Red;
        case snake:
            return sf::Color::Green;
        }
        return sf::Color::Black; // Default
    }

    sf::RectangleShape getShape()
    {
        return shape;
    }

    block(sf::Vector2f pos, blockType type)
    {
        this->pos = pos;
        this->type = type;

        shape.setFillColor(color());
        shape.setSize(sf::Vector2f((float)gridSize, (float)gridSize));
        shape.setPosition(this->pos);
    }
};

sf::Vector2f mulVec(sf::Vector2f& a, sf::Vector2f& b)
{
    return sf::Vector2f(a.x * b.x, a.y * b.y);
}

int main()
{
    sf::RenderWindow window(sf::VideoMode(1920, 1056), "SFML snake");
    window.setFramerateLimit(60);

    sf::Vector2f jumpLen = sf::Vector2f((float)gridSize, (float)gridSize); // Grid
    sf::Vector2f currPos = sf::Vector2f(window.getSize().x / 2.f, window.getSize().y / 2.f);

    sf::Vector2f rightVec = sf::Vector2f( 1.f,  0.f);
    sf::Vector2f leftVec  = sf::Vector2f(-1.f,  0.f);
    sf::Vector2f upVec    = sf::Vector2f( 0.f, -1.f);
    sf::Vector2f downVec  = sf::Vector2f( 0.f,  1.f);

    sf::Vector2f nextDir = rightVec; // Default direction
    sf::Vector2f lastDir = nextDir;

    int nScore = 0;
    bool gameOver = false;

    sf::Font font;
    sf::Text text;
    //font.loadFromFile("Roboto-Bold.ttf");
    font.loadFromFile("epic-font.ttf");

    text.setCharacterSize((unsigned int)gridSize * 2);
    text.setFillColor(sf::Color::White);
    text.setFont(font);
    text.setPosition(sf::Vector2f((float)gridSize * 2, (float)gridSize));
    text.setString("Score : " + std::to_string(nScore));

    std::vector<block> snake;
    snake.push_back(block(currPos, blockType::snake));

    // Random generator
    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<int> nRanX(0, window.getSize().x / gridSize - 1);
    std::uniform_int_distribution<int> nRanY(0, window.getSize().y / gridSize - 1);

    block food(sf::Vector2f((float)(nRanX(gen) * gridSize), (float)(nRanY(gen) * gridSize)), blockType::food);

    sf::Clock gameClock;
    float gameSpeed = 0.2f;

    int lenght = 3;

    //
    //  Game Loop
    //
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        if (!gameOver)
        {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) && lastDir != leftVec)  nextDir = rightVec;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)  && lastDir != rightVec) nextDir = leftVec;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)    && lastDir != downVec)  nextDir = upVec;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)  && lastDir != upVec)    nextDir = downVec;

            if (gameClock.getElapsedTime().asSeconds() > gameSpeed)
            {
                gameClock.restart();

                currPos += mulVec(jumpLen, nextDir);
                snake.push_back(block(currPos, blockType::snake));

                while ((int)snake.size() > lenght)
                    snake.erase(snake.begin());

                lastDir = nextDir;

                // Collision with itself
                for (int i = 0; i < (int)snake.size() - 1; i ++)
                   if (snake.back().getPos().x == snake.at(i).getPos().x &&
                       snake.back().getPos().y == snake.at(i).getPos().y && lenght > 3)
                       gameOver = true;

                // Wall collisions
                if (snake.back().getPos().x > window.getSize().x ||
                       snake.back().getPos().y > window.getSize().y)
                       gameOver = true;

                if (snake.back().getPos().x < 0 ||
                       snake.back().getPos().y < 0)
                       gameOver = true;

                if (snake.back().getPos().x == food.getPos().x && snake.back().getPos().y == food.getPos().y)
                {
                    food.setPos(sf::Vector2f((float)(nRanX(gen) * gridSize), (float)(nRanY(gen) * gridSize)));

                    if (gameSpeed > 0.075f)
                        gameSpeed /= 1.01f;

                    lenght ++;
                    nScore ++;
                }

                //std::cout << snake.size() << std::endl;
            }
        } else
        {
            nScore = 0;
            lenght = 3;

            gameSpeed = 0.2f;

            currPos = sf::Vector2f(window.getSize().x / 2.f, window.getSize().y / 2.f);
            nextDir = rightVec;

            food.setPos(sf::Vector2f((float)(nRanX(gen) * gridSize), (float)(nRanY(gen) * gridSize)));

            snake.clear();
            snake.shrink_to_fit();
            snake.push_back(block(currPos, blockType::snake));

            gameOver = false;
        }

        window.clear(sf::Color(32, 32, 32, 255));

        for (int i = 0; i < (int)snake.size(); i ++)
            window.draw(snake.at(i).getShape());

        window.draw(food.getShape());

        text.setString("Score : " + std::to_string(nScore));
        window.draw(text);

        window.display();
    }
    return 0;
}

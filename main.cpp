#include <SFML/Graphics.hpp>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <vector>

class Ball {
public:
    sf::CircleShape shape;
    float xVelocity = 5.f;
    float yVelocity = -5.f;

    Ball(float radius, sf::Color color) {
        shape.setRadius(radius);
        shape.setFillColor(color);
        shape.setOrigin(radius, radius);
    }

    void update() {
        shape.move(xVelocity, yVelocity);
        shape.setFillColor(sf::Color::White);
    }

    bool isCollidingWithPaddle(sf::RectangleShape paddle) {
        if (shape.getGlobalBounds().intersects(paddle.getGlobalBounds())) {
            float ballX = shape.getPosition().x;
            float paddleX = paddle.getPosition().x;
            float relativeIntersectX = ballX - paddleX;
            float angle = (relativeIntersectX / paddle.getSize().x) * 180.f;
            xVelocity = -5.f * cos(angle * 3.14 / 180.f);
            yVelocity = -5.f * sin(angle * 3.14 / 180.f);
            return true;
        }
        return false;
    }

    bool isCollidingWithBlock(sf::RectangleShape block) {
        if (shape.getGlobalBounds().intersects(block.getGlobalBounds())) {
            sf::Vector2f ballPos = shape.getPosition();
            sf::Vector2f blockPos = block.getPosition();

            sf::Vector2f blockHalfSize = block.getSize() / 2.f;
            sf::Vector2f ballRelativePos = ballPos - (blockPos + blockHalfSize);

            float angle = atan2(ballRelativePos.y, ballRelativePos.x);
            xVelocity = -5.f * cos(angle);
            yVelocity = -5.f * sin(angle);

            return true;
        }
        return false;
    }
};

class Paddle {
public:
    sf::RectangleShape shape;
    float speed = 65.f;

    Paddle(float x, float y) {
        shape.setPosition(x, y);
        shape.setSize(sf::Vector2f(100.f, 20.f));
        shape.setFillColor(sf::Color::White);
        shape.setOutlineThickness(1.f);
        shape.setOutlineColor(sf::Color::Black);
    }

    void draw(sf::RenderWindow& window) {
        window.draw(shape);
    }

    void moveLeft() {
        shape.move(-speed, 0.f);
    }

    void moveRight() {
        shape.move(speed, 0.f);
    }

    bool isCollidingWithBall(Ball& ball) {
        if (ball.shape.getGlobalBounds().intersects(shape.getGlobalBounds())) {
            ball.isCollidingWithPaddle(shape);
            return true;
        }
        return false;
    }
};

class Block {
public:
    sf::RectangleShape shape;
    bool destroyed = false;

    Block(float x, float y, sf::Color color) {
        shape.setPosition(x, y);
        shape.setSize(sf::Vector2f(60.f, 20.f));
        shape.setFillColor(color);
        shape.setOutlineThickness(1.f);
        shape.setOutlineColor(sf::Color::Black);
    }

    void draw(sf::RenderWindow& window) {
        if (!destroyed) {
            window.draw(shape);
        }
    }

    bool isCollidingWithBall(Ball& ball) {
        if (!destroyed && ball.shape.getGlobalBounds().intersects(shape.getGlobalBounds())) {
            destroyed = true;
            return true;
        }
        return false;
    }
};

class Game {
public:
sf::RenderWindow window;
Paddle paddle;
std::vector<Block> blocks;
Ball ball;
Game() : window(sf::VideoMode(800, 600), "Breakout"),
         paddle(window.getSize().x / 2.f - 50.f, window.getSize().y - 50.f),
         ball(10.f, sf::Color::Red)
{
    window.setFramerateLimit(60);

    ball.shape.setPosition(window.getSize().x / 2.f, window.getSize().y / 2.f);

    sf::Color colors[] = { sf::Color::Red, sf::Color::Green, sf::Color::Blue };
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 10; j++) {
            blocks.push_back(Block(j * 70.f + 30.f, i * 30.f + 50.f, colors[i]));
        }
    }
}


void run() {
    while (window.isOpen()) {
        processEvents();
        update();
        render();
    }
}

void processEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        switch (event.type) {
        case sf::Event::Closed:
            window.close();
            break;
        case sf::Event::KeyPressed:
            if (event.key.code == sf::Keyboard::Left) {
                paddle.moveLeft();
            }
            else if (event.key.code == sf::Keyboard::Right) {
                paddle.moveRight();
            }
            break;
        }
    }
}

void update() {
    ball.update();
    if (paddle.isCollidingWithBall(ball)) {
        ball.shape.setFillColor(sf::Color::Green);
    }
    for (auto& block : blocks) {
        if (block.isCollidingWithBall(ball)) {
            ball.shape.setFillColor(sf::Color::Blue);
        }
    }
    if (ball.shape.getPosition().y + ball.shape.getRadius() > window.getSize().y) {
        window.close();
    }
}

void render() {
    window.clear();
    paddle.draw(window);
    ball.shape.setFillColor(sf::Color::Red);
    ball.shape.setOutlineThickness(1.f);
    ball.shape.setOutlineColor(sf::Color::Black);
    window.draw(ball.shape);
    for (auto& block : blocks) {
        block.draw(window);
    }
    window.display();
}
};
int main() {
Game game;
game.run();
return 0;
}

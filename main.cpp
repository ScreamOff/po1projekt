#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <string>

using namespace std;


class Ball {
public:
    sf::CircleShape shape;
    float xVelocity = 5.f;
    float yVelocity = 5.f;
    sf::SoundBuffer soundBuffer;
    sf::Sound sound;

    Ball(float radius, sf::Color color) {
        shape.setRadius(radius);
        shape.setFillColor(color);
        shape.setOrigin(radius, radius);
        soundBuffer.loadFromFile("ballSound.wav");
        sound.setBuffer(soundBuffer);
    }

    void update() {
        shape.move(xVelocity, yVelocity);
        shape.setFillColor(sf::Color::White);
    }

    bool isCollidingWithPaddle(sf::RectangleShape paddle) {
        if (shape.getGlobalBounds().intersects(paddle.getGlobalBounds())) {
            yVelocity = yVelocity * -1+0.05;
            return true;
        }
        return false;
    }

    bool isCollidingWithBlock(sf::RectangleShape block) {
        if (shape.getGlobalBounds().intersects(block.getGlobalBounds())) {
            yVelocity = yVelocity * -1 + 0.05;
            sound.play();

            return true;
        }
        return false;
    }
};

class Paddle {
public:

    sf::RectangleShape shape;
    float speed = 65.f;
    sf::RenderWindow* window;

    Paddle(float x, float y,sf::RenderWindow& window) {
        this->window = &window;
        shape.setPosition(x, y);
        shape.setSize(sf::Vector2f(100.f, 12.f));
        shape.setFillColor(sf::Color::White);
        shape.setOutlineThickness(1.f);
        shape.setOutlineColor(sf::Color::Black);
    }

    void draw(sf::RenderWindow& window) {
        window.draw(shape);
    }

    void moveLeft() {
        if (shape.getPosition().x > 0.f) {  // sprawdzenie, czy nowa pozycja paletki nie wyjdzie poza obszar okna programu
            shape.move(-speed, 0.f);
        }
}

    void moveRight() {
        if (shape.getPosition().x + shape.getSize().x < window->getSize().x) {  // sprawdzenie, czy nowa pozycja paletki nie wyjdzie poza obszar okna programu
            shape.move(speed, 0.f);
        }
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
    int points;

    Block(float x, float y, sf::Color color, int points) {
        shape.setPosition(x, y);
        shape.setSize(sf::Vector2f(60.f, 20.f));
        shape.setFillColor(color);
        shape.setOutlineThickness(1.f);
        shape.setOutlineColor(sf::Color::Black);
        this -> points=points;
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
int score=0;
sf::Font font;
int k=7;
int i=0;
int j=0;
unsigned short licznik=0;
unsigned short mnoznik=1;
unsigned short rozmiar=100;

Game() : window(sf::VideoMode(860, 600), "Breakout"),
         paddle(window.getSize().x / 2.f - 50.f, window.getSize().y - 50.f,window),
         ball(10.f, sf::Color::Red)
{
    window.setFramerateLimit(60);

    ball.shape.setPosition(window.getSize().x / 2.f, window.getSize().y / 2.f);
    font.loadFromFile("ARCADECLASSIC.ttf");



}
void generuj(){
  sf::Color colors[] = { sf::Color::Magenta, sf::Color::Red, sf::Color::Yellow, sf::Color::Green, sf::Color::Cyan, sf::Color::Blue};
  for (int i = 0; i <6; i++) {
      k -= 1;
      for (int j = 0; j < 12; j++) {
          blocks.push_back(Block(j * 70.f + 20.f, i * 30.f + 50.f, colors[i],k*mnoznik));

      }
  }
}
void writePoints(){


  sf::Text text(to_string(score), font, 24);
  text.setFillColor(sf::Color::White);
  text.setPosition(window.getSize().x / 12 - text.getLocalBounds().width / 2,
      window.getSize().y / 1.10 - text.getLocalBounds().height / 2);
      window.draw(text);
}
void waitForEnter()
{
    bool enterPressed = false;
    sf::Text text("Press Enter to start", font, 24);
    text.setFillColor(sf::Color::White);
    text.setPosition(window.getSize().x / 2 - text.getLocalBounds().width / 2,
        window.getSize().y / 2 - text.getLocalBounds().height / 2);

    while (!enterPressed)
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
                exit(0);
            }

            if (event.type == sf::Event::KeyPressed)
            {
                if (event.key.code == sf::Keyboard::Enter)
                {
                    enterPressed = true;
                }
            }
        }

        window.clear();
        window.draw(text);
        window.display();
    }
}


void run() {
  sf::Music music;
  music.setVolume(1.f);
  music.openFromFile("soundTrack.wav");
  music.play();
  music.setLoop(true);
  waitForEnter();
  generuj();
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

    // Check collision with paddle
    if (paddle.isCollidingWithBall(ball)) {
        ball.shape.setFillColor(sf::Color::Green);
    }

    // Check collision with blocks
    for (auto& block : blocks) {
        if (block.isCollidingWithBall(ball)) {
            score += block.points;
            licznik++;
            ball.shape.setFillColor(sf::Color::Blue);
            ball.isCollidingWithBlock(block.shape);

        }
    }
    if(licznik==70){
      blocks.clear();
      mnoznik++;
      k=7;
      if(rozmiar >20)
      {
          rozmiar -= 20;
      }
      paddle.shape.setSize(sf::Vector2f(rozmiar, 12.f));
      generuj();
      licznik=0;

    }

    // Check collision with window edges
    sf::Vector2f ballPosition = ball.shape.getPosition();
    float ballRadius = ball.shape.getRadius();
    float windowWidth = window.getSize().x;
    float windowHeight = window.getSize().y;

    // Collision with left and right edges
    if (ballPosition.x - ballRadius < 0.f) {
        ball.xVelocity = std::abs(ball.xVelocity);
    } else if (ballPosition.x + ballRadius > windowWidth) {
        ball.xVelocity = -std::abs(ball.xVelocity);
    }

    // Collision with top edge
    if (ballPosition.y - ballRadius < 0.f) {
        ball.yVelocity = std::abs(ball.yVelocity);
    }

    // Collision with bottom edge ending
    if (ballPosition.y + ballRadius > windowHeight) {
        window.close();
    }

}


void render() {
    window.clear();
    paddle.draw(window);
    writePoints();
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

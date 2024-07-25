#include <SFML/Graphics.hpp>
#include <iostream>
#include <cstdlib>
#include <ctime>

int main()
{
    sf::RenderWindow window(sf::VideoMode(800, 600), "Air Hockey Game", sf::Style::Close | sf::Style::Resize);

    sf::RectangleShape player1(sf::Vector2f(100.0f, 100.0f));
    player1.setOrigin(50.0f, 50.0f);
    player1.setPosition(200.0f, 200.0f);
    sf::Texture texture;
    if (!texture.loadFromFile("skibidi.png")) {
        std::cerr << "Failed to load texture" << std::endl;
        return -1;
    }
    player1.setTexture(&texture);

    sf::RectangleShape player2(sf::Vector2f(100.0f, 100.0f));
    player2.setOrigin(50.0f, 50.0f);
    player2.setPosition(600.0f, 200.0f);
    player2.setTexture(&texture);

    sf::RectangleShape topGoal(sf::Vector2f(300, 50)); // width: 300px, height: 50px
    topGoal.setPosition(sf::Vector2f(260, 30)); // x: 350px (centered), y: 30px from the top
    topGoal.setFillColor(sf::Color::Red);

    sf::RectangleShape bottomGoal(sf::Vector2f(300, 50)); // width: 300px, height: 50px
    bottomGoal.setPosition(sf::Vector2f(260, 600 - 80)); // x: 350px (centered), y: 600px - 50px - 30px
    bottomGoal.setFillColor(sf::Color::Blue);

    // Create the middle line
    sf::RectangleShape line(sf::Vector2f(800, 5)); // width: 800px, height: 5px
    line.setPosition(0, 297.5); // x: 0px (starting from the left), y: 300px (centered vertically)
    line.setFillColor(sf::Color::White);

    // Create the ball
    sf::CircleShape ball(25.0f);
    ball.setOrigin(25.0f, 25.0f);
    ball.setPosition(400.0f, 300.0f);
    ball.setFillColor(sf::Color::Cyan);

    float player2Speed = 1.0f;
    float ballSpeed = 1.0f;
    sf::Vector2f ballVelocity(0.0f, 0.0f);

    // Seed the random number generator
    std::srand(static_cast<unsigned int>(std::time(0)));

    while (window.isOpen())
    {
        sf::Event evnt;
        while (window.pollEvent(evnt)) {
            switch (evnt.type)
            {
            case sf::Event::Closed:
                window.close();
                break;
            case sf::Event::Resized:
                std::cout << "Height is: " << evnt.size.height << " Width is: " << evnt.size.width << std::endl;
                break;
            }
        }

        if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
            sf::Vector2i mousePos = sf::Mouse::getPosition(window);
            float newX = static_cast<float>(mousePos.x);
            float newY = static_cast<float>(mousePos.y);

            // Ensure the newY position is within the boundaries for player1
            float upperBoundary = line.getPosition().y + line.getSize().y / 2 + player1.getSize().y / 2;
            float lowerBoundary = bottomGoal.getPosition().y - player1.getSize().y / 2;

            if (newY < upperBoundary) {
                newY = upperBoundary;
            }
            if (newY > lowerBoundary) {
                newY = lowerBoundary;
            }
            if (newX < player1.getSize().x / 2) {
                newX = player1.getSize().x / 2;
            }
            if (newX > window.getSize().x - player1.getSize().x / 2) {
                newX = window.getSize().x - player1.getSize().x / 2;
            }

            player1.setPosition(newX, newY);
        }

        // Check for collision between player1 and the ball
        if (player1.getGlobalBounds().intersects(ball.getGlobalBounds())) {
            // Calculate the velocity vector based on player1's movement direction
            sf::Vector2f direction = ball.getPosition() - player1.getPosition();
            float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
            direction /= length; // Normalize the direction vector
            ballVelocity = direction * ballSpeed;
        }

        // Check for collision between player2 and the ball
        if (player2.getGlobalBounds().intersects(ball.getGlobalBounds())) {
            // Calculate a random target point within the bottom goal
            float targetX = static_cast<float>(std::rand() % static_cast<int>(bottomGoal.getSize().x) + bottomGoal.getPosition().x);
            sf::Vector2f goalPosition(targetX, 600.0f);
            sf::Vector2f direction = goalPosition - ball.getPosition();
            float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
            direction /= length; // Normalize the direction vector
            ballVelocity = direction * ballSpeed;
        }

        // Update ball position
        ball.move(ballVelocity);

        // Boundary checks for the ball
        sf::Vector2f ballPos = ball.getPosition();
        sf::Vector2f ballRadius(ball.getRadius(), ball.getRadius());

        // Left boundary
        if (ballPos.x - ballRadius.x < 0) {
            ballVelocity.x = -ballVelocity.x;
            ball.setPosition(ballRadius.x, ballPos.y);
        }

        // Right boundary
        if (ballPos.x + ballRadius.x > window.getSize().x) {
            ballVelocity.x = -ballVelocity.x;
            ball.setPosition(window.getSize().x - ballRadius.x, ballPos.y);
        }

        // Top boundary
        if (ballPos.y - ballRadius.y < 0) {
            ballVelocity.y = -ballVelocity.y;
            ball.setPosition(ballPos.x, ballRadius.y);
        }

        // Bottom boundary
        if (ballPos.y + ballRadius.y > window.getSize().y) {
            ballVelocity.y = -ballVelocity.y;
            ball.setPosition(ballPos.x, window.getSize().y - ballRadius.y);
        }

        // Basic AI for player2 to follow the ball
        sf::Vector2f player2Pos = player2.getPosition();
        if (player2Pos.x < ballPos.x) {
            player2Pos.x += player2Speed;
        }
        else if (player2Pos.x > ballPos.x) {
            player2Pos.x -= player2Speed;
        }

        // Ensure the player2 position is within the boundaries of its half of the field
        float player2UpperBoundary = topGoal.getPosition().y + line.getSize().y / 2;
        float player2LowerBoundary = line.getPosition().y + line.getSize().y / 2 + player2.getSize().y / 2;

        if (player2Pos.y < player2UpperBoundary) {
            player2Pos.y = player2UpperBoundary;
        }
        if (player2Pos.y > player2LowerBoundary) {
            player2Pos.y = player2LowerBoundary;
        }

        player2.setPosition(player2Pos);
  

        window.clear();
        window.draw(player1);
        window.draw(player2);
        window.draw(topGoal);
        window.draw(bottomGoal);
        window.draw(line);
        window.draw(ball);
        window.display();
    }

    return 0;
}
